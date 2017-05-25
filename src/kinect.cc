#include "kinect.h"

namespace kinect {

    using v8::Exception;
    using v8::Function;
    using v8::FunctionCallbackInfo;
    using v8::FunctionTemplate;
    using v8::Local;
    using v8::Isolate;
    using v8::Persistent;
    using v8::String;
    using v8::Value;
    using v8::Object;
    using v8::Handle;

    Persistent<Function> Kinect::constructor;

    Kinect::Kinect(Isolate* isolate) {
        context_ = NULL;
        device_ = NULL;
        running_ = false;

        if (freenect_init(&context_, NULL) < 0) {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Error initializing freenect context")));
            return;
        }

        freenect_set_log_level(context_, FREENECT_LOG_DEBUG);
        freenect_select_subdevices(context_, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));

        int nr_devices = freenect_num_devices (context_);
        if (nr_devices < 1) {
            Close(isolate);
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Plug a kinect device")));
            return;
        }

        if (freenect_open_device(context_, &device_, 0) < 0) {
            Close(isolate);
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Could not open device")));
            return;
        }

        freenect_set_user(device_, this);
    }

    Kinect::~Kinect() {}

    void Kinect::NewInstance(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        const unsigned argc = 1;
        Local<Value> argv[argc] = { args[0] };
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        Local<v8::Context> context = isolate->GetCurrentContext();
        Local<Object> instance =
            cons->NewInstance(context, argc, argv).ToLocalChecked();

        args.GetReturnValue().Set(instance);
    }

    Kinect* Kinect::GetContext(const FunctionCallbackInfo<Value>& args) {
        return ObjectWrap::Unwrap<Kinect>(args.This());
    }

    void Kinect::Close(Isolate* isolate) {

        running_ = false;

        if (device_ != NULL) {
            if (freenect_close_device(device_) < 0) {
                isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Error closing device")));
                return;
            }

             device_ = NULL;
        }

        if (context_ != NULL) {
            if (freenect_shutdown(context_) < 0) {
                isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Error shutting down")));
                return;
            }

            context_ = NULL;
        }
    }

    void Kinect::Init(Isolate* isolate) {

        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
        tpl->SetClassName(String::NewFromUtf8(isolate, "Kinect"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "led", Led);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getAngle", GetAngle);
        NODE_SET_PROTOTYPE_METHOD(tpl, "setAngle", SetAngle);

        constructor.Reset(isolate, tpl->GetFunction());
    }

    void Kinect::New(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.IsConstructCall()) {
            Kinect* obj = new Kinect(isolate);
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        } else {
            const int argc = 1;
            Local<Value> argv[argc] = { args[0] };
            Local<Function> cons = Local<Function>::New(isolate, constructor);
            Local<v8::Context> context = isolate->GetCurrentContext();
            Local<Object> instance =
                cons->NewInstance(context, argc, argv).ToLocalChecked();
            args.GetReturnValue().Set(instance);
        }
    }

    void Kinect::Led(const std::string color, Isolate* isolate) {

        freenect_led_options ledCode;

        if (color.compare("off") == 0) {
            ledCode = LED_OFF;
        } else if (color.compare("green") == 0) {
            ledCode = LED_GREEN;
        } else if (color.compare("red") == 0) {
            ledCode = LED_RED;
        } else if (color.compare("yellow") == 0) {
            ledCode = LED_YELLOW;
        } else if (color.compare("blink green") == 0) {
            ledCode = LED_BLINK_GREEN;
        } else if (color.compare("blink red yellow") == 0) {
            ledCode = LED_BLINK_RED_YELLOW;
        } else {
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Did not recognize given led code")));
            return;
        }

        if (freenect_set_led(device_, ledCode) < 0) {
            isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Error setting led")));
            return;
        }
    }

    void Kinect::Led(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (args.Length() == 1) {
            if (!args[0]->IsString()) {
                isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Led argument must be a string")));
                return;
            }
        }
        else {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        std::string color(*v8::String::Utf8Value(args[0]->ToString()));

        GetContext(args)->Led(color, isolate);
    }

    double Kinect::GetAngle(v8::Isolate* isolate) {
        return freenect_get_tilt_degs(freenect_get_tilt_state(device_));
    }

    void Kinect::GetAngle(const v8::FunctionCallbackInfo<v8::Value>& args) {

        Isolate* isolate = args.GetIsolate();

        args.GetReturnValue().Set(v8::Number::New(isolate, GetContext(args)->GetAngle(isolate)));
    }

    void Kinect::SetAngle(double angle, v8::Isolate* isolate) {
        angle = (angle > 20) ? 20 : angle;
        angle = (angle < -20) ? -20 : angle;

        freenect_set_tilt_degs(device_, angle);
    }

    void Kinect::SetAngle(const v8::FunctionCallbackInfo<v8::Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (args.Length() == 1) {
            if (!args[0]->IsNumber()) {
                isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Tilt argument must be a number")));
                return;
            }
        } else {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Expecting at least one argument with the led status")));
            return;
        }

        double angle = args[0]->NumberValue();

        GetContext(args)->SetAngle(angle, isolate);
    }
}
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
    using v8::Undefined;
    using v8::Handle;

    Persistent<Function> Context::constructor;

    Context::Context(Isolate* isolate) {
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

    Context::~Context() {}

    Context* Context::GetContext(const FunctionCallbackInfo<Value>& args) {
        return ObjectWrap::Unwrap<Context>(args.This());
    }

    void Context::Close(Isolate* isolate) {

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

    void Context::Init(Isolate* isolate) {

        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
        tpl->SetClassName(String::NewFromUtf8(isolate, "Kinect"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "led", Led);

        constructor.Reset(isolate, tpl->GetFunction());
    }

    void Context::New(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.IsConstructCall()) {
            Context* obj = new Context(isolate);
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        } else {
            // TODO
        }
    }

    void Context::Led(const char* color, Isolate* isolate) {

        freenect_led_options ledCode;

        if (strcmp(color, "off") == 0) {
            ledCode = LED_OFF;
        } else if (strcmp(color, "green") == 0) {
            ledCode = LED_GREEN;
        } else if (strcmp(color, "red") == 0) {
            ledCode = LED_RED;
        } else if (strcmp(color, "yellow") == 0) {
            ledCode = LED_YELLOW;
        } else if (strcmp(color, "blink green") == 0) {
            ledCode = LED_BLINK_GREEN;
        } else if (strcmp(color, "blink red yellow") == 0) {
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

    void Context::Led(const FunctionCallbackInfo<Value>& args) {

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

        char* color = *String::Utf8Value(args[0]->ToString());

        GetContext(args)->Led(color, isolate);
    }
}
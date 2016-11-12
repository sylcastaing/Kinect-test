#include <node.h>
#include "kinect.h"

namespace kinect {

    using v8::Context;
    using v8::Function;
    using v8::FunctionCallbackInfo;
    using v8::FunctionTemplate;
    using v8::Local;
    using v8::Isolate;
    using v8::Persistent;
    using v8::String;

    Persistent<Function> Kinect::constructor;

    Kinect::Kinect() {}

    Kinect::~Kinect() {}

    void Kinect::Init(Isolate* isolate) {

        Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
        tpl->SetClassName(String::NewFromUtf8(isolate, "Kinect"));
        tpl->InstanceTemplate()->setInternalFieldCount(1);

        NODE_SET_METHOD(tpl, "led", Led);

        constructor.Reset(isolate, tpl->GetFunction());
    }

    void Kinect::New(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();

        if (args.IsConstructCall()) {
            Kinect* obj = new Kinect();
            obj->Wrap(args.This());
            args.GetReturnValue().Set(args.This());
        } else {
            // TODO
        }
    }

    void Kinect::Led(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();
        freenect_led_options ledCode;

        if (args.Length() != 1) {
            isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        char* color = *String::Utf8Value(args[0]->ToString());

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
}
#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

#include <libfreenect.h>

#include <string.h>

namespace kinect {

    using v8::Exception;
    using v8::FunctionTemplate;
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Number;
    using v8::Object;
    using v8::String;
    using v8::Value;

    class Kinect : public node::ObjectWrap {
        public:
            static void Initialize(v8::Handle<v8::Object> target);
            virtual ~Kinect();

        private: 
            Kinect(int user_device_number);

            void Led(const FunctionCallbackInfo<Value>& args);

            freenect_device* device_;
    };

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

    void Kinect::Initialize(Local<Object> exports) {
        Isolate* isolate = exports->GetIsolate();
        
        Local<FunctionTemplate> t = FunctionTemplate::New(isolate, New);
        t->SetClassNam(String::NewFromUtf8(isolate, "Kinect"));
        t->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(t, "led", Led);

        exports->Set(String::NewFromUtf8(isolate, "Kinect"), t->GetFunction());

    }

    void Init(Local<Object> exports, Local<Object> module) {
        Kinect::Initialize(target);
    }

    NODE_MODULE(kinect, init)
}


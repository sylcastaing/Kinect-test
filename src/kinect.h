#ifndef KINECT_H
#define KINECT_H

#include <node.h>
#include <node_object_wrap.h>

#include <libfreenect.h>
#include <string.h>

namespace kinect {

    class Context : public node::ObjectWrap {
        public:
            static void             Init        (v8::Isolate* isolate);
            static void             NewInstance (const v8::FunctionCallbackInfo<v8::Value>& args);
            
            bool                    running_;
            freenect_context*       context_;
        private:
            explicit Context(v8::Isolate* isolate);
            ~Context();

            static Context*         GetContext  (const v8::FunctionCallbackInfo<v8::Value>& args);

            void                    Close       (v8::Isolate* isolate);

            static void             New         (const v8::FunctionCallbackInfo<v8::Value>& args);

            void                    Led         (const char* option, v8::Isolate* isolate);
            static void             Led         (const v8::FunctionCallbackInfo<v8::Value>& args);

            static                  v8::Persistent<v8::Function> constructor;

            freenect_device*        device_;
    };
}

#endif // !KINECT_H#define KINECT_H
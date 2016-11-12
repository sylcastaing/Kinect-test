#ifdef KINECT_H
#define KINECT_H

#include <node.h>
#include <node_object_wrap.h>

namespace kinect {

    class Kinect : public node::ObjectWrap {
        public:
            static void Init(v8::Isolate* isolate);
            static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args);

        private:
            explicit Kinect();
            ~Kinect();

            static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

            static void Led(const v8::FunctionCallbackInfo<v8::Value>& args);

            static v8::Persistent<v8::Function> constructor;
    };
}

#endif // !KINECT_H#define KINECT_H
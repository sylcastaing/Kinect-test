#include <node.h>
#include "kinect.h"

namespace kinect {

    using v8::Local;
    using v8::Object;

    void InitAll(Local<Object> exports) {
        Kinect::Init(exports);
    }

    NODE_MODULE(kinect, InitAll);
}
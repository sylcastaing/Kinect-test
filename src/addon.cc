#include <node.h>
#include "kinect.h"

namespace kinect {

    using v8::Local;
    using v8::Object;

    void initAll(Local<Object> exports) {
        Kinect::Init(exports);
    }

    NODE_MODULE(kinect, initAll);
}
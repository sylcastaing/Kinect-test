const kinect = require('./build/Debug/kinect');

var test = kinect();

try {
  test.led('green');

  test.setAngle(1);
  console.log(test.getAngle());
}
catch(err) {
  console.log(err);
}

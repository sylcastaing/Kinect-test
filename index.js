const kinect = require('./build/Debug/kinect');

var test = kinect();

try {
  test.led('green');

  test.setAngle(6, () => {
    console.log(test.getAngle());
  });
}
catch(err) {
  console.log(err);
}

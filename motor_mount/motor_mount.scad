use <mattwach/util.scad>
include <focus_rail.scad>
include <motor.scad>

module stepper_motor() {
  motor_xoffset = KNOB_HEIGHT + 2;
  tx(-motor_xoffset) rx(180) ry(90) motor();
}

$fa=2.0;
$fs=0.5;
focus_rail();
stepper_motor();


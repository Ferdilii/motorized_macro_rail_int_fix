use <lib/mattwach/util.scad>
include <focus_rail.scad>
include <motor.scad>
use <flange.scad>
use <motor_bracket.scad>

module stepper_motor() {
  motor_xoffset = KNOB_HEIGHT + 2;
  tx(-motor_xoffset) rx(135) ry(90) motor();
}

$fa=2.0;
$fs=0.5;
focus_rail();
stepper_motor();
motor_bracket(RAIL_BODY_WIDTH, RAIL_BODY_HEIGHT, RAIL_Z);
tx(-5) ry(-90) flange();


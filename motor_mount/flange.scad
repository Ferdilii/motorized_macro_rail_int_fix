use <mattwach/util.scad>
include <knob.scad>
include <motor.scad>

FLANGE_DIAMETER = 20;

module flange() {
  flange_knob_inset = 10;
  flange_knob_gap = 0.3;
  flange_shaft_inset = 10;
  flange_shaft_gap = 0.15;
  flange_pad = 0.5;
  flange_height = flange_knob_inset + flange_shaft_inset + flange_pad;
  module body() {
    cylinder(d=FLANGE_DIAMETER, h=flange_height);
  }

  module knob_cutout() {
    finger_knob(
        KNOB_DIAMETER + flange_knob_gap,
        KNOB_INSET_DIAMETER + flange_knob_gap,
        KNOB_FINGER_HEIGHT);
  }

  color("red") difference() {
    body();
    tz(-KNOB_FINGER_HEIGHT + flange_knob_inset) knob_cutout();
    tz(flange_height + SHAFT_LENGTH - flange_shaft_inset - 1) ry(180) motor_shaft(flange_shaft_gap);
  }

  //tz(-KNOB_HEIGHT + flange_knob_inset) knob();
  //tz(flange_height + SHAFT_LENGTH - flange_shaft_inset) ry(180) motor_shaft(0);
}

/*
flange();
$fa=2.0;
$fs=0.5;
*/

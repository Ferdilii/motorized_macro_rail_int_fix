
overlap = 0.01;

KNOB_DIAMETER = 15.1;
KNOB_INSET_DIAMETER = 13.2;
KNOB_FINGER_HEIGHT = 14.6;
KNOB_HEIGHT = 17.6;

module finger_knob(knob_diameter, knob_inset_diameter, knob_finger_height) {
  finger_cutout_diameter = 4;
  finger_cutout_count = 8;

  module outer_knob() {
    cylinder(d=knob_diameter, h=knob_finger_height);
  }
  module cutout() {
    translate([
        (finger_cutout_diameter + knob_inset_diameter) / 2,
        0,
        -overlap
    ]) cylinder(d=finger_cutout_diameter, h=knob_finger_height+overlap*2);
  }
  module cutouts() {
    for (i = [0:7]) {
      rz(i * (360 / finger_cutout_count)) cutout();
    }
  }
  difference() {
    outer_knob();
    cutouts();
  }
}

module knob() {
  knob_inner_diameter = 10;
  knob_inner_height = KNOB_HEIGHT - KNOB_FINGER_HEIGHT;
  module base() {
    cylinder(
        d1=knob_inner_diameter,
        d2=KNOB_INSET_DIAMETER,
        knob_inner_height + overlap);
  }

  color("#ddd") union() {
    base();
    tz(knob_inner_height) finger_knob(
        KNOB_DIAMETER, KNOB_INSET_DIAMETER, KNOB_FINGER_HEIGHT);
  }
}

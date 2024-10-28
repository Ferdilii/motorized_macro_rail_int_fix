
overlap = 0.01;

module knob() {
  knob_diameter = 15.1;
  knob_inset_diameter = 13.2;
  knob_height = 17.6;
  knob_finger_height = 14.6;
  knob_inner_diameter = 10;
  knob_inner_height = knob_height - knob_finger_height;
  finger_cutout_diameter = 4;
  finger_cutout_count = 8;
  module base() {
    cylinder(
        d1=knob_inner_diameter,
        d2=knob_inset_diameter,
        knob_inner_height + overlap);
  }
  module finger_knob() {
    module outer_knob() {
      tz(knob_inner_height) cylinder(d=knob_diameter, h=knob_finger_height);
    }
    module cutout() {
      translate([
          (finger_cutout_diameter + knob_inset_diameter) / 2,
          0,
          knob_inner_height-overlap
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

  color("#ddd") union() {
    base();
    finger_knob();
  }
}

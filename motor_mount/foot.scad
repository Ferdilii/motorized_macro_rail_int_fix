overlap = 0.01;

FOOT_TOP_DIAMETER = 5.6;

module foot() {
  foot_height = 18;
  foot_base_height = 2.7;
  finger_grab_height = 4;
  chamfer_height = 8;
  module foot_base() {
    foot_base_diameter_bottom = 8;
    foot_base_diameter_top = 10;
    cylinder(
        d1=foot_base_diameter_bottom,
        d2=foot_base_diameter_top,
        h=foot_base_height + overlap);
  }
  module finger_grab() {
    finger_grab_diameter = 11;
    tz(foot_base_height) cylinder(
        d=finger_grab_diameter, h=finger_grab_height + overlap);
  }
  module chamfer() {
    chamfer_base_diameter = 9.9;
    tz(foot_base_height + finger_grab_height) cylinder(
        d1=chamfer_base_diameter, d2=FOOT_TOP_DIAMETER, h=chamfer_height+overlap);
  }
  module top() {
    top_height = foot_height - chamfer_height - finger_grab_height - foot_base_height;
    tz(chamfer_height + finger_grab_height + foot_base_height) cylinder(d=FOOT_TOP_DIAMETER, h=top_height);
  }

  tz(-foot_height) color("#ddd") union() {
    foot_base();
    finger_grab();
    chamfer();
    top();
  }
}


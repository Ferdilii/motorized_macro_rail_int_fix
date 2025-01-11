use <lib/mattwach/shapes.scad>
include <foot.scad> 
include <knob.scad>

overlap = 0.01;

RAIL_BODY_WIDTH = 37.8;
RAIL_BODY_HEIGHT = 20;
RAIL_Z = -RAIL_BODY_HEIGHT + 7;

module focus_rail() {
  rail_body_length = 200;

  foot_w_span = 33 - FOOT_TOP_DIAMETER;
  foot_l_offset = 8.8 - FOOT_TOP_DIAMETER / 2;
  foot_w_offset = (RAIL_BODY_WIDTH - foot_w_span) / 2;

  module outer_body() {
    corner_radius = 5;
    rounded_cube([rail_body_length, RAIL_BODY_WIDTH, RAIL_BODY_HEIGHT], 5);
  }

  module main_cutout() {
    length_pad = 8;
    width_pad = 4.8;
    height_pad = 2;
    cutout_length = rail_body_length - length_pad * 2;
    cutout_width = RAIL_BODY_WIDTH - width_pad * 2;
    cutout_height = RAIL_BODY_HEIGHT - height_pad;
    corner_radius = 5;

    translate([
        length_pad,
        width_pad,
        height_pad]) rounded_cube([
          cutout_length,
          cutout_width,
          cutout_height + overlap], corner_radius);
  }
  translate([
      0,
      -RAIL_BODY_WIDTH / 2,
      RAIL_Z]) {
    color("#777") difference() {
      outer_body();
      main_cutout();
    }
    txy(foot_l_offset, foot_w_offset) foot();
    txy(foot_l_offset, RAIL_BODY_WIDTH - foot_w_offset) foot();
    txy(rail_body_length - foot_l_offset, foot_w_offset) foot();
    txy(rail_body_length - foot_l_offset, RAIL_BODY_WIDTH - foot_w_offset) foot();
  }
  ry(-90) knob();
}

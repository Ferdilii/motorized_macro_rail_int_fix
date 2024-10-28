use <mattwach/shapes.scad>
include <foot.scad> 
include <knob.scad>

overlap = 0.01;

module focus_rail() {
  rail_body_length = 200;
  rail_body_width = 37.8;
  rail_body_height = 20;

  foot_w_span = 33 - FOOT_TOP_DIAMETER;
  foot_l_offset = 8.8 - FOOT_TOP_DIAMETER / 2;
  foot_w_offset = (rail_body_width - foot_w_span) / 2;

  module outer_body() {
    corner_radius = 5;
    rounded_cube([rail_body_length, rail_body_width, rail_body_height], 5);
  }

  module main_cutout() {
    length_pad = 8;
    width_pad = 4.8;
    height_pad = 2;
    cutout_length = rail_body_length - length_pad * 2;
    cutout_width = rail_body_width - width_pad * 2;
    cutout_height = rail_body_height - height_pad;
    corner_radius = 5;

    translate([
        length_pad,
        width_pad,
        height_pad]) rounded_cube([
          cutout_length,
          cutout_width,
          cutout_height + overlap], corner_radius);
  }
  *translate([
      0,
      -rail_body_width / 2,
      -rail_body_height / 2]) {
    color("#777") difference() {
      outer_body();
      main_cutout();
    }
    txy(foot_l_offset, foot_w_offset) foot();
    txy(foot_l_offset, rail_body_width - foot_w_offset) foot();
    txy(rail_body_length - foot_l_offset, foot_w_offset) foot();
    txy(rail_body_length - foot_l_offset, rail_body_width - foot_w_offset) foot();
  }
  knob();
}

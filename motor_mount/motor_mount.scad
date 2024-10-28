use <mattwach/util.scad>
use <mattwach/shapes.scad>

overlap = 0.01;

module rail_body() {
  rail_body_length = 200;
  rail_body_width = 37.8;
  rail_body_height = 20;
  foot_base_height = 2.7;
  finger_grab_height = 4;
  chamfer_height = 8;
  top_diameter = 5.6;

  module foot() {
    foot_height = 18;
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
          d1=chamfer_base_diameter, d2=top_diameter, h=chamfer_height+overlap);
    }
    module top() {
      top_height = foot_height - chamfer_height - finger_grab_height - foot_base_height;
      tz(chamfer_height + finger_grab_height + foot_base_height) cylinder(d=top_diameter, h=top_height);
    }

    color("#ddd") union() {
      foot_base();
      finger_grab();
      chamfer();
      top();
    }
  }

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
/*
  difference() {
    outer_body();
    main_cutout();
  }
*/
  foot();
}

//module motor_mount() { }


$fa=2.0;
$fs=0.5;
//motor_mount();
rail_body();


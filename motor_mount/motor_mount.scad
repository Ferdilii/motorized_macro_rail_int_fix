use <mattwach/util.scad>
use <mattwach/shapes.scad>

overlap = 0.01;

module rail_body() {
  rail_body_length = 200;
  rail_body_width = 37.8;
  rail_body_height = 20;

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

  difference() {
    outer_body();
    main_cutout();
  }
}

//module motor_mount() { }


$fa=2.0;
$fs=0.5;
//motor_mount();
rail_body();


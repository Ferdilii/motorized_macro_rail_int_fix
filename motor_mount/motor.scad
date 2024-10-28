use <mattwach/util.scad>
include <focus_rail.scad>

overlap = 0.01;

module motor() {
  body_height = 30;

  module body() {
    body_width = 42.3;
    body_chamfer = 4.2;

    module chamfer() {
      txy(-body_width/2, body_width/2 - body_chamfer)
        rz(45) tz(-overlap) cube([body_chamfer * 2, body_chamfer, body_height + overlap * 2]);
    }

    color("#666") difference() {
      txy(-body_width/2, -body_width/2) cube([body_width, body_width, body_height]);
      chamfer();
      rz(90) chamfer();
      rz(180) chamfer();
      rz(270) chamfer();
    }
  }

  module mounting_holes() {
    hole_spacing = 31;

    module hole() {
      hole_diameter = 3;
      hole_depth = 4.5;
      
      color("#eee") tz(body_height - hole_depth)
        cylinder(d=hole_diameter, h=hole_depth + overlap);
    }

    txy(hole_spacing/2, hole_spacing/2) hole();
    txy(hole_spacing/2, -hole_spacing/2) hole();
    txy(-hole_spacing/2, hole_spacing/2) hole();
    txy(-hole_spacing/2, -hole_spacing/2) hole();
  }

  difference() {
    body();
    mounting_holes();
  }
}

$fa=2.0;
$fs=0.5;
motor();


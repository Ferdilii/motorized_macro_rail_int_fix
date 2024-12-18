use <mattwach/util.scad>
include <focus_rail.scad>

overlap = 0.01;

SHAFT_LENGTH = 20;

module motor_shaft(pad) {
  shaft_diameter = 5 + pad;

  module flat() {
    flat_length = 16;
    flat_inset = 0.5;
    translate([
        -shaft_diameter / 2,
        shaft_diameter / 2 - flat_inset,
        SHAFT_LENGTH - flat_length]) cube(
          [shaft_diameter,
          shaft_diameter,
          flat_length + overlap]);
  }

  color("#bbb") difference() {
    cylinder(d=shaft_diameter, h=SHAFT_LENGTH);
    flat();
  }
}

module motor() {
  body_width = 42.3;
  body_height = 30;
  front_plate_depth = 2;

  module motor_main() {
    module body() {
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

  module front_plate() {
    front_plate_diamter = 22;
    color("#ddd") tz(body_height) cylinder(d=front_plate_diamter, h=front_plate_depth);
  }

  module connector() {
    connector_width = 16;
    connector_height = 6.5;
    connector_depth = 6.5;
    color("white") translate([-connector_width / 2, body_width / 2, 0])
      cube([connector_width, connector_height, connector_depth]);
  }

  tz(-body_height - front_plate_depth - SHAFT_LENGTH) {
    motor_main();
    front_plate();
    tz(body_height + front_plate_depth) motor_shaft(0);
    connector();
  }
}

/*
$fa=2.0;
$fs=0.5;
motor();
*/


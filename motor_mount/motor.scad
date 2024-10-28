use <mattwach/util.scad>
include <focus_rail.scad>

overlap = 0.01;

module motor() {
  body_width = 42.3;
  body_height = 30;
  front_plate_depth = 2;
  shaft_length = 20;

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

  module shaft() {
    shaft_diameter = 5;

    module flat() {
      flat_length = 16;
      flat_inset = 0.5;
      translate([
          -shaft_diameter / 2,
          shaft_diameter / 2 - flat_inset,
          shaft_length - flat_length]) cube([shaft_diameter, shaft_diameter, flat_length + overlap]);
    }

    color("#bbb") tz(body_height + front_plate_depth) difference() {
      cylinder(d=shaft_diameter, h=shaft_length);
      flat();
    }
  }

  module connector() {
    connector_width = 16;
    connector_height = 6.5;
    connector_depth = 6.5;
    color("white") translate([-connector_width / 2, body_width / 2, 0])
      cube([connector_width, connector_height, connector_depth]);
  }

  tz(-body_height - front_plate_depth - shaft_length) {
    motor_main();
    front_plate();
    shaft();
    connector();
  }
}

$fa=2.0;
$fs=0.5;
motor();


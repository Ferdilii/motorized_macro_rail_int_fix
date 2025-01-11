use <lib/mattwach/util.scad>
use <lib/mattwach/shapes.scad>
include <flange.scad>

overlap = 0.01;

module motor_bracket(rail_body_width, rail_body_height, rail_z) {
  motor_gap = 41.5;
  rail_ypad = 7;
  rail_zpad = 16;
  rail_overlap = 30;
  bracket_x = motor_gap + rail_overlap;
  bracket_y = rail_body_width + rail_ypad * 2;
  bracket_z = rail_body_height + rail_zpad * 2;
  top_slot_xpad = 8;
  top_slot_ypad = 10;
  top_slot_fillet = 5;
  tighten_hole_offset = 12;
  tighten_hole_offset_top = 14;
  tighten_hole_offset_bottom = 8;

  module rail_interface() {
    bracket_fillet = 5;
    module main() {
      translate([
          -motor_gap,
          -bracket_y / 2,
          -bracket_z / 2]) rounded_cube_x([
            bracket_x, bracket_y, bracket_z], bracket_fillet);
    }
    module foot() {
      foot_height = 5;
      translate([
          -motor_gap,
          -bracket_y / 2,
          -bracket_z / 2 - foot_height
      ]) rounded_cube_x([5, bracket_y, foot_height + bracket_fillet * 2], bracket_fillet);
    }
    union() {
      main();
      foot();
    }
  }

  module top_slot() {
    top_slot_y = bracket_y - top_slot_ypad * 2;
    top_slot_z = bracket_z;
    translate([
        -motor_gap + top_slot_xpad,
        -top_slot_y / 2,
        -top_slot_z / 2 - overlap]) rounded_cube(
          [bracket_x,
          top_slot_y,
          top_slot_z + overlap * 2], top_slot_fillet);
  }

  module rail_cutout() {
    cutout_pad = 0.25;
    cutout_y = rail_body_width + cutout_pad * 2;
    cutout_z = rail_body_height + cutout_pad * 4;
    translate([
        -cutout_pad,
        -cutout_y / 2,
        rail_z - cutout_pad * 2]) cube([50, cutout_y, cutout_z]);
  }

  module flange_cutout() {
    flange_cutout_diameter = FLANGE_DIAMETER + 4;
    tx(-motor_gap - overlap) ry(90) cylinder(
        d=flange_cutout_diameter,
        h=top_slot_xpad + 10);
  }

  module side_slot() {
    side_slot_x = 40;
    side_slot_y = bracket_y + overlap * 2;
    side_slot_zpad = 10;
    side_slot_z = bracket_z - side_slot_zpad * 2;
    translate([
        -motor_gap + top_slot_xpad,
        -side_slot_y / 2,
        -side_slot_z / 2]) rounded_cube_y(
          [side_slot_x, side_slot_y, side_slot_z], top_slot_fillet);
  }

  module motor_holes() {
    module motor_hole() {
      motor_hole_span = 31;
      motor_hole_diameter = 3.1;
      motor_hole_length = top_slot_xpad + overlap * 2;
      translate([
          -motor_gap - overlap,
          motor_hole_span / 2,
          motor_hole_span / 2
      ]) ry(90) cylinder(d=motor_hole_diameter, h=motor_hole_length);
    }

    rx(45) motor_hole();
    rx(135) motor_hole();
    rx(225) motor_hole();
    rx(315) motor_hole();
  }

  module tighten_holes() {
    module insert() {
      insert_diameter = 5;
      ty(-bracket_y / 2 + top_slot_ypad + overlap) rx(90) cylinder(d=insert_diameter, h=top_slot_ypad + overlap * 2);
    }

    module bolt_hole() {
      hole_diameter = 3.2;
      inset_diameter = 7;
      inset_pad = 8;

      ty(bracket_y / 2 + overlap) rx(90) union() {
        cylinder(d=inset_diameter, h=top_slot_ypad - inset_pad + overlap);
        tz(top_slot_ypad - inset_pad) cylinder(d=hole_diameter, h=inset_pad + overlap * 2);
      }
    }

    tx(rail_overlap - tighten_hole_offset) {
      insert();
      bolt_hole();
    }
  }

  color("green") difference() {
    rail_interface();
    top_slot();
    side_slot();
    rail_cutout();
    flange_cutout();
    motor_holes();
    tz(bracket_z / 2 - tighten_hole_offset_top) tighten_holes();
    tz(-bracket_z / 2 + tighten_hole_offset_bottom) tighten_holes();
  }
}

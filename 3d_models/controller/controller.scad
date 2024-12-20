use <mattwach/util.scad>
include <pcb.scad>
include <gimbal.scad>

pcb_x = 9;
gimbal_xpad = 13;
gimbal_ypad_top = 6;
gimbal_ypad_bottom = 12;
pcb_ypad = 11;
pcb_y = GIMBAL_HOLE_YSPAN + gimbal_ypad_top + gimbal_ypad_bottom;
controller_fillet = 10;
controller_base_thickness = 2;
controller_xsize = PCB_XSIZE + pcb_x * 2;
controller_ysize = PCB_YSIZE + pcb_y + pcb_ypad;
pcb_top_clearance = 7;
gimbal_z = controller_base_thickness + 20;
pcb_z = controller_base_thickness + gimbal_z - pcb_top_clearance;

module controller_slice(inset, zsize) {
  fillet = controller_fillet - inset / 2;
  module corner() {
    cylinder(r=fillet, h=zsize);
  }

  module upper() {
    hull() {
      txy(fillet + inset, controller_ysize - fillet - inset) corner();
      txy(controller_xsize - fillet - inset,
          controller_ysize - fillet - inset) corner();
      txy(fillet + inset, pcb_y + fillet - pcb_ypad + inset) corner();
      txy(controller_xsize - 1 - inset,
          pcb_y - pcb_ypad + inset) cube([1,1,zsize]);
    }
  }

  module lower() {
    hull() {
      txy(controller_xsize - fillet - inset, fillet + inset) corner();
      txy(controller_xsize + fillet - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2 + inset,
          fillet + inset) corner();
      txy(controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2 + inset,
          pcb_y - pcb_ypad + inset) cube([
            GIMBAL_HOLE_XSPAN + gimbal_xpad * 2 - inset * 2, 1, zsize]);
    }
  }

  module inner() {
    translate([
        controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2 - fillet + inset,
        pcb_y - pcb_ypad - fillet + inset,
        0]) difference() {
          cube([fillet + 1, fillet + 1, zsize]);
          tz(-overlap) cylinder(r=fillet, h=zsize + overlap * 2);
    }
  }

  union() {
    upper();
    lower();
    inner();
  }
}

module controller() {
  zsize = pcb_z + pcb_top_clearance + 5;
  wall_thickness = 4;
  hole_inset = 6.2;

  module top_plate_mounting_posts() {
    post_diameter = 9;
    module post() {
      cylinder(d=post_diameter, h=zsize);
    }
    txy(hole_inset, controller_ysize - hole_inset) post();
    txy(controller_xsize - hole_inset, controller_ysize - hole_inset) post();
    txy(hole_inset, pcb_y - pcb_ypad + hole_inset) post();
    txy(controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2, pcb_y - pcb_ypad + 1) post();
    txy(controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2 + hole_inset, hole_inset) post();
    txy(controller_xsize - hole_inset, hole_inset) post();
  }

  module top_place_mouting_holes() {
    module hole() {
      hole_diameter = 5;
      hole_depth = 10;
      tz(zsize - hole_depth) cylinder(d=hole_diameter, h=hole_depth + overlap);
    }

    txy(hole_inset, controller_ysize - hole_inset) hole();
    txy(controller_xsize - hole_inset, controller_ysize - hole_inset) hole();
    txy(hole_inset, pcb_y - pcb_ypad + hole_inset) hole();
    txy(controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2, pcb_y - pcb_ypad + 1) hole();
    txy(controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2 + hole_inset, hole_inset) hole();
    txy(controller_xsize - hole_inset, hole_inset) hole();
  }

  color("#700") difference() {
    union() {
      difference() {
        controller_slice(0, zsize);
        tz(controller_base_thickness) controller_slice(wall_thickness, zsize);
      }
      top_plate_mounting_posts();
    }
    top_place_mouting_holes();
  }
}

module placed_pcb() {

  translate([
      pcb_x,
      pcb_y,
      pcb_z]) pcb();
}

module placed_gimbal() {
  gimbal_x = controller_xsize - GIMBAL_HOLE_XSPAN / 2 - gimbal_xpad;
  gimbal_y = GIMBAL_HOLE_YSPAN / 2 + gimbal_ypad_bottom;
  translate([
      gimbal_x,
      gimbal_y,
      gimbal_z]) gimbal();
}

$fa=2.0;
$fs=0.5;
controller();
placed_pcb();
placed_gimbal();

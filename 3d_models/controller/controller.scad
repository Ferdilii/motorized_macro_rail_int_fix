use <mattwach/util.scad>
include <pcb.scad>
include <gimbal.scad>

pcb_x = 8;
gimbal_xpad = 10;
gimbal_ypad_top = 10;
gimbal_ypad_bottom = 10;

pcb_ypad = 8;
pcb_y = GIMBAL_HOLE_YSPAN + gimbal_ypad_top + gimbal_ypad_bottom;
controller_fillet = 10;
controller_base_thickness = 2;
controller_xsize = PCB_XSIZE + pcb_x * 2;
controller_ysize = PCB_YSIZE + pcb_y + pcb_ypad;
pcb_top_clearance = 7;
gimbal_z = controller_base_thickness + 20;
gimbal_x = controller_xsize - GIMBAL_HOLE_XSPAN / 2 - gimbal_xpad;
gimbal_y = GIMBAL_HOLE_YSPAN / 2 + gimbal_ypad_bottom;
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

module controller(open_view=false) {
  zsize = open_view ? 2 : pcb_z + pcb_top_clearance + 5;
  wall_thickness = 2.5;
  hole_inset = 4.9;

  module top_plate_mounting_posts() {
    post_diameter = 5.5;
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
      hole_diameter = 3.5;
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

  module pcb_mounts() {
    post_diameter = 9;
    module post() {
      cylinder(d=post_diameter, h=pcb_z);
    }

    txy(pcb_x + 3, pcb_y + 3) post();
    txy(pcb_x + 6.35, pcb_y + 50.8) post();
    txy(pcb_x + 115.55, pcb_y + 50.8) post();
    txy(pcb_x + 115.55, pcb_y + 3.75) post();
  }

  module pcb_mount_holes() {
    module hole() {
      hole_diameter = 5;
      hole_depth = 10;
      tz(pcb_z - hole_depth) cylinder(d=hole_diameter, h=hole_depth + overlap);
    }

    txy(pcb_x + 6.35, pcb_y + 50.8) hole();
    txy(pcb_x + 115.55, pcb_y + 50.8) hole();
    txy(pcb_x + 115.55, pcb_y + 3.75) hole();
  }

  module gimbal_mounting_posts() {
    post_diameter = 7;
    module post() {
      cylinder(d=post_diameter, h=gimbal_z);
    }
    txy(gimbal_x - GIMBAL_HOLE_XSPAN / 2, gimbal_y - GIMBAL_HOLE_YSPAN / 2) post();
    txy(gimbal_x + GIMBAL_HOLE_XSPAN / 2, gimbal_y - GIMBAL_HOLE_YSPAN / 2) post();
    txy(gimbal_x - GIMBAL_HOLE_XSPAN / 2, gimbal_y + GIMBAL_HOLE_YSPAN / 2) post();
    txy(gimbal_x + GIMBAL_HOLE_XSPAN / 2, gimbal_y + GIMBAL_HOLE_YSPAN / 2) post();
  }

  module gimbal_mounting_holes() {
    module hole() {
      hole_diameter = 3.5;
      hole_depth = 10;
      tz(gimbal_z - hole_depth) cylinder(d=hole_diameter, h=hole_depth + overlap);
    }
    txy(gimbal_x - GIMBAL_HOLE_XSPAN / 2, gimbal_y - GIMBAL_HOLE_YSPAN / 2) hole();
    txy(gimbal_x + GIMBAL_HOLE_XSPAN / 2, gimbal_y - GIMBAL_HOLE_YSPAN / 2) hole();
    txy(gimbal_x - GIMBAL_HOLE_XSPAN / 2, gimbal_y + GIMBAL_HOLE_YSPAN / 2) hole();
    txy(gimbal_x + GIMBAL_HOLE_XSPAN / 2, gimbal_y + GIMBAL_HOLE_YSPAN / 2) hole();
  }

  color("#700") difference() {
    union() {
      difference() {
        controller_slice(0, zsize);
        tz(controller_base_thickness) controller_slice(wall_thickness, zsize);
      }
      top_plate_mounting_posts();
      pcb_mounts();
      gimbal_mounting_posts();
    }
    top_place_mouting_holes();
    pcb_mount_holes();
    gimbal_mounting_holes();
  }
}

module placed_pcb() {
  translate([
      pcb_x,
      pcb_y,
      pcb_z]) pcb();
}

module placed_gimbal() {
  translate([
      gimbal_x,
      gimbal_y,
      gimbal_z]) gimbal();
}

$fa=2.0;
$fs=0.5;
controller(false);
*placed_pcb();
*placed_gimbal();

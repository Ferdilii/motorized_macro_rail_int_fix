use <mattwach/util.scad>
include <pcb.scad>
include <gimbal.scad>

pcb_x = 8;
gimbal_xpad = 10;
gimbal_ypad_top = 10;
gimbal_ypad_bottom = 10;
gimbal_y = GIMBAL_HOLE_YSPAN / 2 + gimbal_ypad_bottom;

pcb_ypad = 8;
controller_base_thickness = 2;
pcb_top_clearance = 7;
gimbal_z = controller_base_thickness + 20;
pcb_y = GIMBAL_HOLE_YSPAN + gimbal_ypad_top + gimbal_ypad_bottom;
pcb_z = controller_base_thickness + gimbal_z - pcb_top_clearance;
controller_fillet = 10;
controller_xsize = PCB_XSIZE + pcb_x * 2;
controller_ysize = PCB_YSIZE + pcb_y + pcb_ypad;
controller_zsize = pcb_z + pcb_top_clearance + 5;
gimbal_x = controller_xsize - GIMBAL_HOLE_XSPAN / 2 - gimbal_xpad;
wall_thickness = 2.5;
hole_inset = 4.9;

plate_mounting_holes = [
  [hole_inset, controller_ysize - hole_inset],
  [controller_xsize - hole_inset, controller_ysize - hole_inset],
  [hole_inset, pcb_y - pcb_ypad + hole_inset],
  [controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2, pcb_y - pcb_ypad + 1],
  [controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2 + hole_inset, hole_inset],
  [controller_xsize - hole_inset, hole_inset],
];

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
  zsize = open_view ? 2 : controller_zsize;

  module top_plate_mounting_posts() {
    post_diameter = 5.5;
    module post() {
      cylinder(d=post_diameter, h=zsize);
    }
    for (i=[0:len(plate_mounting_holes)-1]) {
      txy(plate_mounting_holes[i][0], plate_mounting_holes[i][1]) post();
    }
  }

  module top_place_mouting_holes() {
    module hole() {
      hole_diameter = 3.5;
      hole_depth = 10;
      tz(zsize - hole_depth) cylinder(d=hole_diameter, h=hole_depth + overlap);
    }

    for (i=[0:len(plate_mounting_holes)-1]) {
      txy(plate_mounting_holes[i][0], plate_mounting_holes[i][1]) hole();
    }
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

  module motor_pin_port() {
    port_x = pcb_x + 66;
    port_z = pcb_z + 2.2;
    port_xsize = 13;
    port_zsize = 5;
    translate([
        port_x,
        controller_ysize - wall_thickness - overlap,
        port_z]) cube([
          port_xsize,
          wall_thickness + overlap * 2,
          port_zsize]);
  }

  module xt60_port() {
    port_x = pcb_x + 100;
    port_xsize = 15.7;
    port_zsize = 8.3;
    port_z = 10;
    hole_span = 20.5;
    translate([
        port_x - port_xsize / 2,
        controller_ysize - wall_thickness - overlap,
        port_z - port_zsize / 2]) cube([
          port_xsize,
          wall_thickness + overlap * 2,
          port_zsize]);
    module hole() {
      hole_size = 2.5;
      translate([
          port_x,
          controller_ysize - wall_thickness - overlap,
          port_z]) rx(-90) cylinder(d=hole_size, h=wall_thickness + overlap * 2);
    }
    tx(hole_span / 2) hole();
    tx(-hole_span / 2) hole();
  }

  module remote_trigger_port() {
    port_diameter = 4;
    port_x = 64;
    port_z = 10;
    translate([
        port_x,
        controller_ysize - wall_thickness - overlap,
        port_z]) rx(-90) cylinder(d=port_diameter, h=wall_thickness + overlap * 2);
  }

  module usb_port() {
    port_y = pcb_y + 19;
    port_z = pcb_z + 5.5;
    port_ysize = 16;
    port_zsize = 10;
    translate([
        controller_xsize - wall_thickness - overlap,
        port_y - port_ysize / 2,
        port_z - port_zsize / 2]) #cube([
          wall_thickness + overlap * 2,
          port_ysize,
          port_zsize]);
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
    motor_pin_port();
    xt60_port();
    remote_trigger_port();
    usb_port();
  }
}

module cover() {
  cover_thickness = 3;
  port_width = 16;

  module mounting_holes() {
    module hole() {
      hole_diameter = 2.1;
      tz(-overlap) cylinder(d=hole_diameter, h=cover_thickness + overlap * 2);
    }
    for (i=[0:len(plate_mounting_holes)-1]) {
      txy(plate_mounting_holes[i][0], plate_mounting_holes[i][1]) hole();
    }
  }

  module left_buttons_port() {
    port_x = pcb_x + 8.35;
    port_y = pcb_y + 23.1;
    port_span = 25.4;
    module port_end() {
      translate([
          port_x,
          port_y,
          -overlap]) cylinder(d=port_width, h=cover_thickness + overlap * 2);
    }
    hull() {
      ty(-port_span / 2) port_end();
      ty(port_span / 2) port_end();
    }
  }

  module right_button_port() {
    port_x = pcb_x + 56.6;
    port_y = pcb_y + 10.4;
    translate([
        port_x,
        port_y,
        -overlap]) cylinder(d=port_width, h=cover_thickness + overlap * 2);
  }

  module boot_button_port() {
    port_x = pcb_x + 102.95;
    port_y = pcb_y + 22.2;
    boot_port_width = 8;
    translate([
        port_x,
        port_y,
        -overlap]) cylinder(d=boot_port_width, h=cover_thickness + overlap * 2);
  }

  module a9488_port() {
    fillet = 3;
    port_x = pcb_x + 62.45;
    port_y = pcb_y + 34.5;
    port_xsize = 19.8;
    port_ysize = 14.7;
    module corner() {
      translate([
          port_x,
          port_y,
          -overlap]) cylinder(r = fillet, h=cover_thickness + overlap * 2);
    }

    hull() {
      txy(fillet, fillet) corner();
      txy(port_xsize - fillet, fillet) corner();
      txy(port_xsize - fillet, port_ysize - fillet) corner();
      txy(fillet, port_ysize - fillet) corner();
    }
  }

  module gimbal_port() {
    port_diameter = 20;
    translate([
        gimbal_x,
        gimbal_y,
        -overlap]) cylinder(r = port_diameter, h=cover_thickness + overlap * 2);
  }

  color("#ddd", 0.3) tz(controller_zsize) difference() {
    controller_slice(0, cover_thickness);
    mounting_holes();
    left_buttons_port();
    right_button_port();
    boot_button_port();
    a9488_port();
    gimbal_port();
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

module cover_projection() {
  projection() cover();
}

$fa=2.0;
$fs=0.5;
// comment out everything but controller for the 3d print model
controller(false);
placed_pcb();
placed_gimbal();
cover();
// comment out everything but this to create a dxf projection
//cover_projection();


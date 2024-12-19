use <mattwach/util.scad>
include <pcb.scad>
include <gimbal.scad>

pcb_x = 5;
gimbal_xpad = 7;
gimbal_ypad = gimbal_xpad;
pcb_ypad = 5;
pcb_y = GIMBAL_HOLE_YSPAN + gimbal_ypad * 2;
controller_fillet = 7;
controller_base_thickness = 2;
controller_xsize = PCB_XSIZE + pcb_x * 2;
controller_ysize = PCB_YSIZE + pcb_y + pcb_ypad;
gimbal_z = controller_base_thickness + 20;
pcb_top_clearance = 5;
pcb_z = controller_base_thickness + gimbal_z - pcb_top_clearance;

module controller_slice(zsize) {
  module corner() {
    cylinder(r=controller_fillet, h=zsize);
  }

  #cube([
      controller_xsize,
      controller_ysize,
      zsize]);

  module upper() {
    hull() {
      txy(controller_fillet, controller_ysize - controller_fillet) corner();
      txy(controller_xsize - controller_fillet, controller_ysize - controller_fillet) corner();
      txy(controller_fillet, pcb_y + controller_fillet - pcb_ypad) corner();
      txy(controller_xsize - 1, pcb_y - pcb_ypad) cube([1,1,zsize]);
    }
  }

  module lower() {
    hull() {
      txy(controller_xsize - controller_fillet, controller_fillet) corner();
      txy(controller_xsize + controller_fillet - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2, controller_fillet) corner();
      txy(controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2, pcb_y - pcb_ypad)
        cube([GIMBAL_HOLE_XSPAN + gimbal_xpad * 2,1,zsize]);
    }
  }

  module inner() {
    translate([
        controller_xsize - GIMBAL_HOLE_XSPAN - gimbal_xpad * 2 - controller_fillet,
        pcb_y - pcb_ypad - controller_fillet,
        0]) difference() {
          cube([controller_fillet + 1, controller_fillet + 1, zsize]);
          tz(-overlap) cylinder(r=controller_fillet, h=zsize + overlap * 2);
    }
  }

  union() {
    upper();
    lower();
    inner();
  }
}

module controller() {

  controller_slice(controller_base_thickness);
}

module placed_pcb() {

  translate([
      pcb_x,
      pcb_y,
      pcb_z]) pcb();
}

module placed_gimbal() {
  gimbal_x = controller_xsize - GIMBAL_HOLE_XSPAN / 2 - gimbal_xpad;
  gimbal_y = GIMBAL_HOLE_YSPAN / 2 + gimbal_ypad;
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

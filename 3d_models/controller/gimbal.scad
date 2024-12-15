use <mattwach/util.scad>

GIMBAL_HOLE_XSPAN = 35; 
GIMBAL_HOLE_YSPAN = 42; 

overlap = 0.01;

module gimbal() {
  base_plate_height = 1.5;
  bottom_parts_zsize = 17;
  fit_cylinder_height = 5;
  gimbal_cutout_xsize = 26.3;
  gimbal_cutout_ysize = 18.8;
  gimbal_cylinder_diameter = gimbal_cutout_ysize;
  mount_hole_outer_diameter = 6.7;
  mount_hole_height = 5.4;

  module mount() {
    mount_hole_inner_diameter = 4.5;
    mount_hole_diameter = 2.8;
    mount_hole_top_inset = 1.6;
    mount_hole_bottom_inset = 2.4;

    difference() {
      cylinder(d=mount_hole_outer_diameter, h=mount_hole_height);
      tz(mount_hole_height - mount_hole_top_inset) cylinder(
          d=mount_hole_inner_diameter, h=mount_hole_top_inset + overlap);
      tz(-overlap) cylinder(
          d=mount_hole_inner_diameter,
          h=mount_hole_bottom_inset + overlap);
      cylinder(d=mount_hole_diameter, h=mount_hole_height);
    }
  }

  module mount_hole() {
    mount_hole_diameter = 2.8;
    cylinder(d=mount_hole_diameter, h=mount_hole_height);
  }

  module base_plate() {
    base_plate_inset = 2;
    base_plate_xsize = (
        GIMBAL_HOLE_XSPAN + mount_hole_outer_diameter - base_plate_inset);
    base_plate_ysize = (
        GIMBAL_HOLE_YSPAN + mount_hole_outer_diameter - base_plate_inset);
    txy(-base_plate_xsize / 2, -base_plate_ysize / 2)
      cube([base_plate_xsize, base_plate_ysize, base_plate_height]);
  }

  module fit_cylinder() {
    fit_cylinder_diameter = 37.6;
    tz(base_plate_height - overlap) cylinder(
        d=fit_cylinder_diameter, h=fit_cylinder_height + overlap);
  }

  module bottom_parts() {
    bottom_parts_xsize = 41.2;
    bottom_parts_ysize = 36.3;

    translate([
        -bottom_parts_xsize / 2,
        -bottom_parts_ysize / 2,
        -bottom_parts_zsize]) cube([
          bottom_parts_xsize,
          bottom_parts_ysize,
          bottom_parts_zsize + overlap]);
  }

  module gimbal_cutout() {
    gimbal_cutout_zsize = (bottom_parts_zsize + fit_cylinder_height +
        base_plate_height + overlap * 2);
    translate([
        -gimbal_cutout_xsize / 2,
        -gimbal_cutout_ysize / 2,
        -bottom_parts_zsize - overlap]) cube([
          gimbal_cutout_xsize,
          gimbal_cutout_ysize,
          gimbal_cutout_zsize]);
  }

  module gimbal_cylinder() {
    gimbal_cylinder_length = gimbal_cutout_xsize;
    translate([
        -gimbal_cylinder_length / 2,
        0,
        -gimbal_cylinder_diameter / 2 + base_plate_height + fit_cylinder_height]) ry(90) cylinder(
          d=gimbal_cylinder_diameter, h=gimbal_cylinder_length);
  }

  module gimbal_stick() {
    gimbal_stick_height = gimbal_cylinder_diameter / 2 + 22;
    finger_grip_height = 14.8;
    stick_base_height = gimbal_stick_height - finger_grip_height;
    module stick_base() {
      stick_base_diameter = 4;
      cylinder(
          d=stick_base_diameter,
          h=gimbal_stick_height - finger_grip_height + overlap);
    }
    module finger_grip() {
      finger_grip_diameter = 8;
      tz(gimbal_stick_height - finger_grip_height) cylinder(
          d=finger_grip_diameter,
          h=finger_grip_height);
    }
    stick_base();
    finger_grip();
  }

  color("#777") difference() {
    union() {
      txy(GIMBAL_HOLE_XSPAN / 2, GIMBAL_HOLE_YSPAN / 2) mount();
      txy(GIMBAL_HOLE_XSPAN / 2, -GIMBAL_HOLE_YSPAN / 2) mount();
      txy(-GIMBAL_HOLE_XSPAN / 2, GIMBAL_HOLE_YSPAN / 2) mount();
      txy(-GIMBAL_HOLE_XSPAN / 2, -GIMBAL_HOLE_YSPAN / 2) mount();
      base_plate();
      fit_cylinder();
      bottom_parts();
    }
    txy(GIMBAL_HOLE_XSPAN / 2, GIMBAL_HOLE_YSPAN / 2) mount_hole();
    txy(GIMBAL_HOLE_XSPAN / 2, -GIMBAL_HOLE_YSPAN / 2) mount_hole();
    txy(-GIMBAL_HOLE_XSPAN / 2, GIMBAL_HOLE_YSPAN / 2) mount_hole();
    txy(-GIMBAL_HOLE_XSPAN / 2, -GIMBAL_HOLE_YSPAN / 2) mount_hole();
    gimbal_cutout();
  }
  color("#777") gimbal_cylinder();
  gimbal_stick();
}

$fa=2.0;
$fs=0.5;
gimbal();

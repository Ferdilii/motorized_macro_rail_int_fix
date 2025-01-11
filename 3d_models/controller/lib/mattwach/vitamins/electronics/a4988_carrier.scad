// Adafruit INA260 Breakout Board
use <../../util.scad>

include <../../../NopSCADlib/utils/core/core.scad>
include <../../../NopSCADlib/vitamins/pin_headers.scad>

module a4988_carrier(add_pin_header = true, add_heat_sink=true) {
  overlap = 0.01;

  pcb_xsize = 20.3;
  pcb_ysize = 15.2;
  pcb_thickness = 1.6;

  hole_yoffset = 1.25;
  hole_xoffset = (pcb_xsize - (7 * 2.54)) / 2;
  hole_diameter = 0.7;
  hole_yspan = 12.7;
  pad_thickness = 0.1;
  pad_pad = 0.5;

  module pcb() {
    color("#900") cube([pcb_xsize, pcb_ysize, pcb_thickness]);

    module pin_hole_pad() {
      color("#ddd") translate([
          hole_xoffset,
          hole_yoffset,
          -pad_thickness
      ]) cylinder(d=hole_diameter + pad_pad * 2, h=pcb_thickness + pad_thickness * 2);
    }

    for (i=[0:7]) {
      tx(i * 2.54) pin_hole_pad();
      ty(hole_yspan) tx(i * 2.54) pin_hole_pad();
    }

  }

  module pin_holes() {
    module hole() {
      translate([
          hole_xoffset,
          hole_yoffset,
          -overlap - pcb_thickness
      ]) cylinder(
        d=hole_diameter,
        h=pcb_thickness + pcb_thickness * 2 + overlap * 2);
    }

    for (i=[0:7]) {
      tx(i * 2.54) hole();
    }
  }

  module pins() {
    txy(pcb_xsize / 2, hole_yoffset) ry(180) pin_header(2p54header, 8, 1);
  }

  module pot() {
    pot_xsize = 2.5;
    pot_ysize = 2.5;
    pot_zsize = 1.5;
    color("#ddd") translate([
        1.5,
        5,
        pcb_thickness]) cube([pot_xsize, pot_ysize, pot_zsize]);
  }

  module labels(lbl, align) {
    label_spacing = 2.54;
    xoffset = 1.25;
    label_text_size = 1.0;

    module label(label) {
      label_pad_thickness = 0.1;
      translate([
          0,
          -label_text_size / 2,
          pcb_thickness - label_pad_thickness + overlap])
        color("white")
        linear_extrude(label_pad_thickness)
        text(label, size=label_text_size, halign=align);
    }


    for (i = [0:len(lbl)-1]) {
      tx(xoffset + i * label_spacing) rz(-90) label(lbl[i]);
    }
  }

  module heat_sink() {
    heat_sink_xsize = 8.6;
    heat_sink_ysize = 9;
    heat_sink_zsize = 5.4;
    heat_sink_slot_width = 1;
    heat_sink_slot_count = 4;
    heat_sink_slot_depth = 3.7;
    heat_sink_tab_width = (
        (heat_sink_xsize - (heat_sink_slot_width * heat_sink_slot_count)) /
        (heat_sink_slot_count + 1));

    module block() {
      cube([
        heat_sink_xsize,
        heat_sink_ysize,
        heat_sink_zsize]);
    }

    module slot() {
      translate([
        heat_sink_tab_width,
        -overlap,
        heat_sink_zsize - heat_sink_slot_depth]) cube([
          heat_sink_slot_width,
          heat_sink_ysize + overlap * 2,
          heat_sink_slot_depth + overlap]);
    }

    difference() {
      block();
      for (i = [0:heat_sink_slot_count-1]) {
        tx(i * (heat_sink_tab_width + heat_sink_slot_width)) slot();
      }
    }
  }

  txy(-hole_xoffset, -hole_yoffset) {
    difference() {
      pcb();
      pin_holes();
      ty(hole_yspan) pin_holes();
    }
    pot();
    ty(2.2) labels(["GND", "VDD", "1B", "1A", "2A", "2B", "GND", "VMOT"], "right");
    ty(13) labels(["DIR", "STEP", "SLP", "RST", "MS3", "MS2", "MS1", "EN"], "left");
    if (add_pin_header) {
      pins();
      ty(hole_yspan) pins();
    }
  }

  translate([
      4,
      2,
      pcb_thickness + 1]) heat_sink();
}

/*
$fa=2;
$fs=0.5;
a4988_carrier();
*/


use <mattwach/util.scad>
include <mattwach/vitamins/electronics/pi_pico.scad>
include <NopSCADlib/core.scad>
include <NopSCADlib/vitamins/pin_headers.scad>

PCB_XSIZE = 119.4;
PCB_YSIZE = 57.2;
PCB_ZSIZE = 1.6;

module pcb() {
  header_zsize = 2.54;
  pin_spacing = 2.54;

  module board() {
    translate([
        -91.45,
        127,
        0]) import("macro_rail_kicad_PCB.stl");
  }

  module pico() {
    offset_x = 90.2;
    offset_y = 10.1;
    module header() {
      translate([
          offset_x,
          offset_y,
          PCB_ZSIZE + header_zsize]) ry(180) pin_header(2p54header, 20, 1);
    }
    header();
    ty(pin_spacing * 7) header();
    translate([
        offset_x + pin_spacing * 10 + 0.1,
        offset_y + pin_spacing * 7.6,
        PCB_ZSIZE + header_zsize]) rz(180) pi_pico();
  }

  board();
  pico();
}

$fa=2.0;
$fs=0.5;
pcb();

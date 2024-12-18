use <mattwach/util.scad>
include <mattwach/vitamins/electronics/pi_pico.scad>
include <NopSCADlib/core.scad>
include <NopSCADlib/vitamins/pin_headers.scad>
include <NopSCADlib/vitamins/buttons.scad>
include <NopSCADlib/vitamins/components.scad>
include <NopSCADlib/vitamins/dip.scad>
include <NopSCADlib/vitamins/axials.scad> 


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

  module button() {
    translate([
        8.35,
        10.4,
        PCB_ZSIZE]) square_button(button_6mm);
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

  module vreg() {
    translate([
        108.5,
        38.15,
        PCB_ZSIZE + 1]) rz(-90) TO220("7805", leads = 3, lead_length = 5);
  }

  module opto() {
    translate([
        54.5,
        36.8,
        PCB_ZSIZE]) rz(90) pdip(6, "OPTO");
  }

  module resistor() {
    translate([
        53.5,
        28,
        PCB_ZSIZE]) ax_res(res1_8, 100);
  }

  module gimbal_header() {
    translate([
        95.2,
        3.2,
        PCB_ZSIZE]) jst_xh_header(jst_xh_header, 4);
  }

  board();
  pico();
  button();
  ty(pin_spacing * 5) button();
  ty(pin_spacing * 10) button();
  tx(pin_spacing * 19) button();
  vreg();
  opto();
  resistor();
  gimbal_header();
}

$fa=2.0;
$fs=0.5;
pcb();

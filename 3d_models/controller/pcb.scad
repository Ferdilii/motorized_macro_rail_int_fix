use <mattwach/util.scad>

PCB_XSIZE = 119.4;
PCB_YSIZE = 57.2;
PCB_ZSIZE = 1.6;

module pcb() {
  module board() {
    translate([
        -91.45,
        127,
        0]) import("macro_rail_kicad_PCB.stl");
  }

  board();
}

$fa=2.0;
$fs=0.5;
pcb();

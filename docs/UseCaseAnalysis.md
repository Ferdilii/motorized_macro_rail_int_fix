The purpose of this doc is to help design the user interface, especiall in
terms of streamlining the primary usecase.

## Primary Usecase

Bob would like a set of images for a focus stacking program.

### Step 1: Bob step up his subject, macro rail, and camera.

Bob also connects the shutter release cable from the module to the camera

### Step 2: Power up

Bob connects either a 3S lipo battery or a 12V DC Adapter for power. The unit
activates and asks Bob to press a button to continue.  The unit will only
continue if the control stick has been idle for 500ms.

### Step 3: End Point Selection

Bob can freely move the camera with the control stick and hit next to go to the
next step.

* From this point forward, Bob can also go back.
* Also activated is the manual shutter release button that bob can press
  at any time during setup.
* The unit helps bob with backlash management by showing a scale.  e.g.  |....|...|
* The unit shows coordinates in steps and mm.  It shows motor in steps only
  and plate position (backlash compensated) in both mm and steps.
* The control stick offers fowward and back for normal movement and side to side for
  finer tuning.

 |012345689012345
 +---------------+
0|End Pt Select  |
1|               |
2|Plate:  34.60mm|
3|          55360|
4|Motor:    55000|
5|               |
6|Backlash    -12|
7||      |      ||
 +---------------+

### Step 4: Start point selection

At the start of this step, the unit is considered "zeroed".  The positive direction
for the 1d coordinate system is distance from subject.  So if you are behind the camera
then the number is increasing when the camera is moving toward you.

Other steps are similar to end point selection

### Step 5: Shot parameters

Bob is shown something like this

 |012345689012345
 +---------------+
0|Parameters     |
1|               |
2|Shots:     010 |
3|Delay:   04.1s |
4|               |
5|Step:   1.01mm |
6|          1616 |
7|Time: 00:00:00 |
 +---------------+


### Step 6: Start

 |012345689012345
 +---------------+
0|               |
1|               |
2|Press Shutter  |
3|Button To      |
4|Begin          |
5|               |
6|               |
7|               |
 +---------------+

### Step 7: Process

Bob can press the next button to pause the process
Bob can press the shutter button to continuue
Bob can press the prev to cancel


 |012345689012345
 +---------------+
0|Running        |
1|               |
2|Shot:   002/010|
3|Time:  00:00:08|
4|Left:  00:01:12|
5|               |
6|[next] > pause |
7|[prev] > cancel|
 +---------------+

## Settings

Pressing the setting button offers a selection

 |012345689012345
 +---------------+
0|Settings       |
1|               |
2|Backlash       |
3|Speed          |
4|Calibrate mm   |
6|Min Step       |
7|               |
 +---------------+

 |012345689012345
 +---------------+
0|Backlash       |
1|               |
2|               |
3|Steps:    12345|
4|               |
5|               |
6|[next] > test  |
7|[prev] > back  |
 +---------------+

 |012345689012345
 +---------------+
0|Speed          |
1|               |
2|Accel:    12345|
3|Max Vel: 123455|
4|        steps/s|
5|Settle:   00.5s|
6|               |
7|[prev] > back  |
 +---------------+

 |012345689012345
 +---------------+
0|Calibrate mm   |
1|               |
2|               |
3|Steps/mm: 12345|
4|               |
5|[up/dn] > 10mm |
6|[lt/rt] > 1mm  |
7|[prev] > back  |
 +---------------+

 |012345689012345
 +---------------+
0|Min Step       |
2|            1  |
3|            2  |
4|            4  |
5|            8  |
5|           16  |
6|               |
7|[prev] > back  |
 +---------------+

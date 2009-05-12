[14:34] <syd> jentron do I need any dependancies ? I just did a make and tried to execute the binary but nothing happens , no warnings , error messages
[14:35] <jentron> Oh, feed it a .dcm file from datcom
[14:35] <AndersG> syd: Feed it a .dcm file

Usage: ./datcom-modeler myairplane.dcm >myairplane.ac

Usage: datcom-modeler [-n b|e|f|h|v|w] [-v] [-q] [-o ac-file] datcom-file

The fuselage body is colored green.  The un-exposed spans of the wings and fins are also green.  The outboard panels (if present) are red.


What doesn't work:
2. TVTPAN isn't implemented.
3. Oops, missed .VERTUP.
6. jet pods aren't there.
7. Supersonic body features aren't there.
8. 4 digit NACA airfoils are present.
   Airfoils defined by XORD, YUPPER and YLOWER are also drawn correctly.
   Other NACA airfoil types default to a NACA 4 digit shape.  NACA 2414 for wings and NACA 0014 for fins and tails.
   NACA 5 airfoils are drawn badly.

Ron Jensen, 
Sun May  3 18:07:25 MDT 2009
Mon May 11 20:39:32 MDT 2009

ron (at) jentronics.com



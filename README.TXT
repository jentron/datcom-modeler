[14:34] <syd> jentron do I need any dependancies ? I just did a make and tried to execute the binary but nothing happens , no warnings , error messages
[14:35] <jentron> Oh, feed it a .dcm file from datcom
[14:35] <AndersG> syd: Feed it a .dcm file

Usage: ./datcom-modeler myairplane.dcm >myairplane.ac

The fuselage body is colored green.  The un-exposed spans of the wings and fins are also green.  The outboard panels (if present) are light red.


What doesn't work:
1. All elements, WGPLNF, HTPLNF, BODY, VTPLNF must be present to win.  Current code isn't smart enough to skip what isn't there.
2. VFPLNF and TVTPAN aren't implemented.  VFPLNF will be easy but see bug 1.
3. Oops, missed .VERTUP.
4. <AndersG> It can process all .dcm files I have except for the one with EXPR01 and EXPR02 name lists.
5. <jentron> The ones I have with numbers in the PROPWR namelist fail as well.
6. propdisk and jet pods aren't there.
7. Supersonic body features aren't there.
8. 4 digit NACA airfoils are present.
   Airfoils defined by XORD, YUPPER and YLOWER are also drawn correctly.
   Other NACA airfoil types default to a NACA 4 digit shape.  NACA 2414 for wings and NACA 0014 for fins and tails.

Ron Jensen, 
Sun May  3 18:07:25 MDT 2009
ron (at) jentronics.com



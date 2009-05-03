/* modeler.h */
/*
 *=============================================================================
 *  
 *
 *  Copyright (C) 2009  Ronald Jensen
 *  ron(at)jentronics.com
 *  http://www.jentronics.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *=============================================================================
 */

#ifndef MODELER_H
/* Flight Conditions */
struct OPTINS {
  char pad[4];
} ;

/* SYNTHS Reference Parameters*/
struct SYNTHS {
  double XCG, ZCG,
         XW, ZW, ALIW,
         XH, ZH, ALIH,
         XV, ZV,
         XVF, ZVF,
         SCALE,
         HINAX;
  int    VERTUP;
/*  char[4] pad; */
} ;

/* Body Geometric Data */
struct BODY {
  int    NX;      /* Number of longitudinal body stations at which data is specified, max of 20 */
  double X[20];   /* Array(20) Longitudinal distance measured from arbitray location */
  double S[20];   /* Array(20) Cross sectional area at station. See note above. */
  double P[20];   /* Array(20) Periphery at station Xi. See note above. */
  double R[20];   /* Array(20) Planform half width at station Xi. See note above. */
  double ZU[20];  /* Array(20) Z-coordinate at upper body surface at station Xi 
                   *           (positive when above centerline)
                   *           [Only required for subsonic asymmetric bodies] */
  double ZL[20];  /* Array(20) Z-coordinate at lower body surface at station Xi
                   *           (negative when below centerline)
                   *           [Only required for subsonic asymmetric bodies] */
  double BNOSE;   /* Nosecone type  1.0 = conical (rounded), 2.0 = ogive (sharp point)
                   *           [Not required in subsonic speed regime] */
  double BTAIL;   /* Tailcone type  1.0 = conical, 2.0 = ogive, omit for lbt = 0
                   *           [Not required in subsonic speed regime] */
  double BLN;     /* Length of body nose
                   *           Not required in subsonic speed regime */
  double BLA;     /* Length of cylindrical afterbody segment, =0.0 for nose alone
                   *           or nose-tail configuration
                   *           Not required in subsonic speed regime */
  double DS;      /* Nose bluntness diameter, zero for sharp nosebodies
                   *           [Hypersonic speed regime only] */
  int ITYPE;      /* 1.0 = straight wing, no area rule
                   * 2.0 = swept wing, no area rule (default)
                   * 3.0 = swept wing, area rule */
  int METHOD;     /* 1.0 = Use existing methods (default)
                   * 2.0 = Use Jorgensen method */
} ;

/* Wing Planform Variables */
struct WGPLNF {
  double   CHRDR,  /* Chord root */
           CHRDBP, /* Chord at breakpoint. Not required for straight tapered planform. */
           CHRDTP, /* Tip chord */
           SSPN,   /* Semi-span theoretical panel from theoretical root chord */
           SSPNE,  /* Semi-span exposed panel, */
           SSPNOP, /* Semi-span outboard panel. Not required for straight tapered planform. */
           SAVSI,  /* Inboard panel sweep angle */
           SAVSO,  /* Outboard panel sweep angle */
           CHSTAT, /* Reference chord station for inboard and outboard panel sweep angles, fraction of chord */
           TWISTA, /* Twist angle, negative leading edge rotated down (from exposed root to tip) */
           SSPNDD, /* Semi-span of outboard panel with dihedral */
           DHDADI, /* Dihedral angle of inboard panel */
           DHDADO; /* Dihedral angle of outboard panel. If DHDADI=DHDADO only input DHDADI */
  int      TYPE;   /* 1.0 - Straight tapered planform
                      2.0 - Double delta planform (aspect ratio <= 3)
                      3.0 - Cranked planform (aspect ratio > 3) */
/*  char[4] pad; */
} ;

/* Horizontal Tail Planform Variables */
struct HTPLNF {
  char pad[4];
} ;

/* Vertical Tail Planform Variables */
struct VTPLNF {
  char pad[4];
} ;

/* Vertical Fin Planform Variables */
struct VFPLNF {
  char pad[4];
} ;

/* Jet Power Parameters */
struct JETPWR{
  char pad[4];
} ;

/* Propeller Power Parameters */
struct PROPWR{
  char pad[4];
} ;

/* Twin Vertical Panels */
struct TVTPAN{
  char pad[4];
} ;

/* Airfoil */
/*************************
 *
 * This structure contains data
 * calculated from the input NACA
 * number or airfoil geometric data. 
 *
 *************************/
struct AIRFOIL{
	int COUNT;    /* Number of entries */
	double *DATAX; /* data elements */
	double *DATAY;
};

#define MODELER_H
#endif


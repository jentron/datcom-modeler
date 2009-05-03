/* modeler.h */

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
  char   VERTUP;
/*  char[4] pad; */
} ;

/* Body Geometric Data */
struct BODY {
  char pad[4];
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

int InitAC(FILE *ofp, int kids);
int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, struct SYNTHS *synths);

#define MODELER_H
#endif


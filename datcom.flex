/*
 *=============================================================================
 *  flex based parser for DATCOM input cards.
 *
 *  Copyright (C) 2009  Anders Gidenstam (anders(at)gidenstam.org)
 *  Copyright (C) 2009  Ronald Jensen    (ron(at)jentronics.com)
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

%{
    #include <stdio.h>
    #include <string.h>
    #include <math.h>
    #include "datcom-parser.h"
    extern int verbose;

    /* Internal global variables. */
    static AIRCRAFT* current_aircraft;

    static int current_namelist;
    #define NL_NONE    0
    #define NL_UNKNOWN 1
    #define NL_SYNTHS  2
    #define NL_BODY    3
    #define NL_WGPLNF  4
    #define NL_HTPLNF  5
    #define NL_VTPLNF  6
    #define NL_VFPLNF  7
    #define NL_WGSCHR  8
    #define NL_HTSCHR  9
    #define NL_VTSCHR  10
    #define NL_VFSCHR  11
    #define NL_PROPWR  12
    #define NL_SYMFLP  13
    #define NL_ASYFLP  14
    #define NL_OPTINS  15
    #define NL_MAX     16 /* Keep this entry last! */

    static int line_number;

    /* For variables and array variables. */
    static double* next_double;
    static int     num_doubles;

    static int*    next_int;
    static int*    next_bool;

    static const char* NL_NAME[] = {
                                       "",
                                       "",
                                       "$SYNTHS",
                                       "$BODY",
                                       "$WGPLNF",
                                       "$HTPLNF",
                                       "$VTPLNF",
                                       "$VFPLNF",
                                       "$WGSCHR",
                                       "$HTSCHR",
                                       "$VTSCHR",
                                       "$VFSCHR",
                                       "$PROPWR",
                                       "$SYMFLP",
                                       "$ASYFLP",
                                       "$OPTINS",
                                   };

    /* Internal functions. */
    static void InitializeParser(AIRCRAFT* aircraft);
    static void Fail();
    static void BeginNameList(char* name);
    static void EndNameList();
    static void ReadVariable(char* var);
    static void ReadNumber(char* str); /* Reads a double or an int */ 

    static void SYNTHSReadVariable(char* var);
    static void BODYReadVariable(char* var);
    static void PLNFReadVariable(char* var, struct WGPLNF* surface);
    static void SCHRReadVariable(char* var, DATCOM_AIRFOIL* airfoil);
    static void PROPWRReadVariable(char* var);

    static void NACARead(char* str);
    /* Added by Bill Galbraith 10-13-10 */
    static void ClearLineRead();
    static void AppendToLineRead(char* str);
    char LineRead[300];
%}

/* This tells flex to read only one input file */
%option noyywrap

/* Parser conditions/"states" */
%s namelist

DIGIT           [0-9]
ALPHA           [a-zA-Z]
ALPHANUM        [A-Za-z0-9]
ID              [A-Z][A-Z0-9]*
WS              [ \t]
EOL             "\n"|"\n\r"|"\r\n"
NEOL            [^\n\r]

/* Bill Galbraith  10-12-10
   Redefined what a DOUBLE was, to :
      digit(s), a decimal point, and digit(s)
      digit(s) and a decimal point
      a decimal point and digit(s)
   and of which may or may not have a leading minus sign and
   trailing exponent, which is made up of an optional minus sign
   and digit(s)
   * IMPORTANT NOTE * The DOUBLE is also defined as ending with a comma.

   Old definition
DOUBLE          "-"?{DIGIT}*"."{DIGIT}*("E""-"?{DIGIT}+)?   */

/* This works, except it doesn't account for not having
   the numbers separated by commas. If you fix this, also have BOOLEAN
   separated by commas.
*/
DOUBLE          ("-"?{DIGIT}+"."{DIGIT}+("E""-"?{DIGIT}+)?|"-"?{DIGIT}+"."("E""-"?{DIGIT}+)?|"-"?"."{DIGIT}+("E""-"?{DIGIT}+)?)

BOOL            \.TRUE\.|\.FALSE\.
COMMAND         ^(DIM|PART|DERIV|DUMP|DAMP|SAVE|NEXT|CASEID|BUILD|PLOT|TRIM)
NAMELIST        "$"{ID}
VAR             {ID}({WS}*"=")
ARRVAR          {ID}"(1)"{WS}*"="
NACAAIRFOIL     ^"NACA"({WS}|"-"){NEOL}*{LENDCOMMENT}?
LINECOMMENT     ^("*"|"#"){NEOL}*
LENDCOMMENT     "!"{NEOL}*

/*******************************************************************************
 * Rules
 */
%%

<namelist>{DOUBLE} {
    if (verbose > 2) fprintf(stderr,"datcom-parser:    DOUBLE: %s = %g\n", yytext, atof(yytext));
    /* Bill Galbraith 10-13-10  This is a more accurate output */
    /* Ron Jensen 10-13-10 The conversion is done with atof() so perhaps both are interesting */
    //fprintf(stderr,"    DOUBLE: %s\n", yytext);
    ReadNumber(yytext);
    AppendToLineRead(yytext);
}

<namelist>{BOOL} {
    if (verbose > 2) fprintf(stderr,"datcom-parser:    BOOL: %s\n", yytext);
    ReadNumber(yytext);
    AppendToLineRead(yytext);
}

<INITIAL>{NAMELIST} {
    if (verbose > 2) fprintf(stderr,"datcom-parser:NAMELIST: %s\n", yytext);
    BEGIN(namelist);
    BeginNameList(yytext);
    AppendToLineRead(yytext);
}

<namelist>"$" {
    if (verbose > 2) fprintf(stderr,"datcom-parser:END OF NAMELIST\n");
    BEGIN(INITIAL);
    EndNameList();
    AppendToLineRead(yytext);
}

<namelist>{ARRVAR} {
    char* e;
    AppendToLineRead(yytext);
    /* Strip trailing garbage. I hope this is safe.. */
    e = strchr(yytext, '(');
    if (e != NULL) {
        *e = '\0';
    }
    if (verbose > 2) fprintf(stderr,"datcom-parser:  ARRAYVARIABLE: %s\n", yytext);

    ReadVariable(yytext);
}

<namelist>{VAR} {
    char* s;
    char* e;

    /* Strip trailing garbage. I hope this is safe.. */
    s = strchr(yytext, ' ');
    e = strchr(yytext, '=');
    e = (e != NULL) ? e : s;
    s = (s != NULL) ? s : e;
    e = (e < s) ? e : s;
    if (e != NULL) {
        *e = '\0';
    }
    AppendToLineRead(yytext);
    if (verbose > 2) fprintf(stderr,"datcom-parser:  VARIABLE: %s\n", yytext);

    ReadVariable(yytext);
}

{NACAAIRFOIL} {
    if (verbose > 2) fprintf(stderr,"datcom-parser:AIRFOIL: %s\n", yytext);
    NACARead(yytext);
    ClearLineRead();
}

{COMMAND}{NEOL}* {
	if (verbose > 2) fprintf(stderr,"datcom-parser:Command: %s\n", yytext);

    /* Drop uninteresting commands */
    ClearLineRead();
}

{LINECOMMENT} {
    /* Drop comment lines */
   ClearLineRead();
}

{LENDCOMMENT} {
    /* Drop comment lines */
    ClearLineRead();
}

{EOL} {
    line_number++;
   ClearLineRead();
}

"," {
    /* Eat ',' */
    AppendToLineRead(yytext);
}

{WS} {
    /* Eat up whitespace */
}

. {
    Fail();
}

%%
/*******************************************************************************
 * Verbatim code section
 */

/* Main entry point. */
void ReadDatcom(char* filename, AIRCRAFT* aircraft)
{
    yyin = fopen(filename, "r");

    if(!yyin){
      fprintf(stderr,"Failed to open input file %s\n", filename);
      exit(EXIT_FAILURE);
    }

    InitializeParser(aircraft);
    yylex();
    fclose(yyin);
}

static void InitializeParser(AIRCRAFT* aircraft)
{
    /* Bill Galbraith  10-12-10
       Changed the initial value from 0 to 1, so that the line numbers
       output actually matches that in the file. */
    
    line_number = 1;

    current_aircraft = aircraft;
    current_namelist = NL_NONE;
    next_double      = NULL;
    num_doubles      = 0;
    next_int         = NULL;

    bzero(current_aircraft, sizeof(AIRCRAFT));
}

static void Fail()
{
    if (verbose > 0) fprintf(stderr, "datcom-parser: Error: Unrecognized or misplaced character '%s' in line %d\n",
            yytext, line_number);
    fprintf(stderr, "Errant line up to the error is:\n%s\n", LineRead );
    exit(-1);
}

static void BeginNameList(char* name)
{
    int i;
    if (current_namelist != NL_NONE) {
        if (verbose > 0) fprintf(stderr,
               "datcom-parser: Unterminated NAMELIST in line %d\n",
                line_number);
        exit(-1);
    }
    current_namelist = NL_UNKNOWN;
    for (i = 0; i < NL_MAX; i++) {
        if (strcmp(name, NL_NAME[i]) == 0) {
            current_namelist = i;
        }
    }

    if (current_namelist == NL_UNKNOWN) {
        if (verbose > 1) fprintf(stderr,
               "datcom-parser: Unknown NAMELIST %s in line %d\n",
                name,
                line_number);
    }
 }

static void EndNameList()
{
    if (current_namelist == NL_NONE) {
        if (verbose > 0) fprintf(stderr,
                "datcom-parser: Unbegun NAMELIST terminated in line %d\n",
                line_number);
        exit(-1);
    }
    current_namelist = NL_NONE;
}

static void ReadVariable(char* var)
{
    if(verbose > 0) //FIXME: Raise verbosity level later 
    {
        switch (current_namelist) {
        case NL_NONE:
            fprintf(stderr,"datcom-parser: NL_NONE\n");
            break;
        case NL_UNKNOWN: // This case will print elsewhere
//            fprintf(stderr,"datcom-parser: NL_UNKNOWN\n");
            break;
        default:
            if(current_namelist < NL_MAX) fprintf(stderr,"datcom-parser: %s\n", NL_NAME[current_namelist]);
        }
    }
    switch (current_namelist) {
    case NL_NONE:
        if (verbose > 0) fputs("datcom-parser: Variable outside of NAMELIST", stderr);
        exit(-1);
        break;
    case NL_SYNTHS:
        SYNTHSReadVariable(var);
        break;
    case NL_BODY:
        BODYReadVariable(var);
        break;
    case NL_WGPLNF:
        PLNFReadVariable(var, &current_aircraft->wing);
        break;
    case NL_HTPLNF:
        PLNFReadVariable(var, &current_aircraft->htail);
        break;
    case NL_VTPLNF:
        PLNFReadVariable(var, &current_aircraft->vtail);
        break;
    case NL_VFPLNF:
        PLNFReadVariable(var, &current_aircraft->vfin);
        break;
    case NL_WGSCHR:
        SCHRReadVariable(var, &current_aircraft->wingfoil);
        break;
    case NL_HTSCHR:
        SCHRReadVariable(var, &current_aircraft->htailfoil);
        break;
    case NL_VTSCHR:
        SCHRReadVariable(var, &current_aircraft->vtailfoil);
        break;
    case NL_VFSCHR:
        SCHRReadVariable(var, &current_aircraft->vfinfoil);
        break;
    case NL_PROPWR:
        PROPWRReadVariable(var);
        break;
    case NL_SYMFLP:
// FIXME: Do Something!
        break;
    case NL_ASYFLP:
// FIXME: Do Something!
        break;
    case NL_OPTINS:
// FIXME: Do Something!
        break;
    default:
        break;
    }
}

static void ReadNumber(char* str)
{
    if (num_doubles-- && next_double != NULL) {
        *next_double = atof(yytext); /* Add error detection here! */
        next_double = (num_doubles > 0) ? next_double + 1 : NULL;
    } else if (next_int) {
        *next_int = (int)atof(yytext); /* Add error detection here! */
        next_int = NULL;
    } else if (next_bool) {
        if (strcmp(yytext, ".TRUE.") == 0) {
            *next_bool = 1;
        } else {
            *next_bool = 0;
        }
        next_bool=NULL;
    }
}

static void PROPWRReadVariable(char* var)
{
    num_doubles = 1;
    if (strcmp(var, "AIETLP") == 0) {
        next_double = &current_aircraft->propwr.AIETLP;
    } else if (strcmp(var, "NENGSP") == 0) {
        next_double = &current_aircraft->propwr.NENGSP;
    } else if (strcmp(var, "THSTCP") == 0) {
        next_double = &current_aircraft->propwr.THSTCP;
    } else if (strcmp(var, "PHALOC") == 0) {
        next_double = &current_aircraft->propwr.PHALOC;
    } else if (strcmp(var, "PHVLOC") == 0) {
        next_double = &current_aircraft->propwr.PHVLOC;
    } else if (strcmp(var, "PRPRAD") == 0) {
        next_double = &current_aircraft->propwr.PRPRAD;
    } else if (strcmp(var, "BWAPR3") == 0) {
        next_double = &current_aircraft->propwr.BWAPR3;
    } else if (strcmp(var, "BWAPR6") == 0) {
        next_double = &current_aircraft->propwr.BWAPR6;
    } else if (strcmp(var, "BWAPR9") == 0) {
        next_double = &current_aircraft->propwr.BWAPR9;
    } else if (strcmp(var, "NOPBPE") == 0) {
        next_double = &current_aircraft->propwr.NOPBPE;
    } else if (strcmp(var, "BAPR75") == 0) {
        next_double = &current_aircraft->propwr.BAPR75;
    } else if (strcmp(var, "YP") == 0) {
        next_double = &current_aircraft->propwr.YP;
    } else if (strcmp(var, "CROT") == 0) {
        num_doubles = 0;
        next_int = &current_aircraft->propwr.CROT;
    } else {
       if (verbose > 1) fprintf(stderr,
                "datcom-parser: Unknown variable %s in PROPWR NAMELIST close to line %d\n",
                var, line_number);
    }
}

static void SYNTHSReadVariable(char* var)
{
    /*printf("SYNTHS: %s\n", var);*/
    num_doubles = 1;
    if (strcmp(var, "XCG") == 0) {
        next_double = &current_aircraft->synths.XCG;
    } else if (strcmp(var, "ZCG") == 0) {
        next_double = &current_aircraft->synths.ZCG;
    } else if (strcmp(var, "XW") == 0) {
        next_double = &current_aircraft->synths.XW;
    } else if (strcmp(var, "ZW") == 0) {
        next_double = &current_aircraft->synths.ZW;
    } else if (strcmp(var, "ALIW") == 0) {
        next_double = &current_aircraft->synths.ALIW;
    } else if (strcmp(var, "XH") == 0) {
        next_double = &current_aircraft->synths.XH;
    } else if (strcmp(var, "ZH") == 0) {
        next_double = &current_aircraft->synths.ZH;
    } else if (strcmp(var, "ALIH") == 0) {
        next_double = &current_aircraft->synths.ALIH;
    } else if (strcmp(var, "XV") == 0) {
        next_double = &current_aircraft->synths.XV;
    } else if (strcmp(var, "ZV") == 0) {
        next_double = &current_aircraft->synths.ZV;
    } else if (strcmp(var, "XVF") == 0) {
        next_double = &current_aircraft->synths.XVF;
    } else if (strcmp(var, "ZVF") == 0) {
        next_double = &current_aircraft->synths.ZVF;
    } else if (strcmp(var, "SCALE") == 0) {
        next_double = &current_aircraft->synths.SCALE;
    } else if (strcmp(var, "HINAX") == 0) {
        next_double = &current_aircraft->synths.HINAX;
    } else if (strcmp(var, "VERTUP") == 0) {
        num_doubles = 0;
        next_int = NULL;
        next_bool = &current_aircraft->synths.VERTUP;
    } else {
       if (verbose > 1) fprintf(stderr,
                "datcom-parser: Unknown variable %s in SYNTHS NAMELIST in line %d\n",
                var, line_number);
    }
}

static void BODYReadVariable(char* var)
{
    if (strcmp(var, "NX") == 0) {
        next_int = &current_aircraft->body.NX;
    } else if (strcmp(var, "X") == 0) {
        num_doubles = current_aircraft->body.NX;
        next_double = &current_aircraft->body.X[0];
    } else if (strcmp(var, "S") == 0) {
        num_doubles = current_aircraft->body.NX;
        next_double = &current_aircraft->body.S[0];
    } else if (strcmp(var, "P") == 0) {
        num_doubles = current_aircraft->body.NX;
        next_double = &current_aircraft->body.P[0];
    } else if (strcmp(var, "R") == 0) {
        num_doubles = current_aircraft->body.NX;
        next_double = &current_aircraft->body.R[0];
    } else if (strcmp(var, "ZU") == 0) {
        num_doubles = current_aircraft->body.NX;
        next_double = &current_aircraft->body.ZU[0];
    } else if (strcmp(var, "ZL") == 0) {
        num_doubles = current_aircraft->body.NX;
        next_double = &current_aircraft->body.ZL[0];
    } else if (strcmp(var, "BNOSE") == 0) {
        num_doubles = 1;
        next_double = &current_aircraft->body.BNOSE;
    } else if (strcmp(var, "BTAIL") == 0) {
        num_doubles = 1;
        next_double = &current_aircraft->body.BTAIL;
    } else if (strcmp(var, "BLN") == 0) {
        num_doubles = 1;
        next_double = &current_aircraft->body.BLN;
    } else if (strcmp(var, "BLA") == 0) {
        num_doubles = 1;
        next_double = &current_aircraft->body.BLA;
    } else if (strcmp(var, "DS") == 0) {
        num_doubles = 1;
        next_double = &current_aircraft->body.DS;
    } else if (strcmp(var, "ITYPE") == 0) {
        num_doubles = 0;
        next_int = &current_aircraft->body.ITYPE;
    } else if (strcmp(var, "METHOD") == 0) {
        num_doubles = 0;
        next_int = &current_aircraft->body.METHOD;
    } else {
        if (verbose > 1) fprintf(stderr,
                "datcom-parser: Unknown variable %s in BODY NAMELIST in line %d\n",
                var, line_number);
    }
}

static void PLNFReadVariable(char* var, struct WGPLNF* surface)
{
    num_doubles = 1;
    if (strcmp(var, "CHRDR") == 0) {
        next_double = &surface->CHRDR;
    } else if (strcmp(var, "CHRDBP") == 0) {
        next_double = &surface->CHRDBP;
    } else if (strcmp(var, "CHRDTP") == 0) {
        next_double = &surface->CHRDTP;
    } else if (strcmp(var, "SSPN") == 0) {
        next_double = &surface->SSPN;
    } else if (strcmp(var, "SSPNE") == 0) {
        next_double = &surface->SSPNE;
    } else if (strcmp(var, "SSPNOP") == 0) {
        next_double = &surface->SSPNOP;
    } else if (strcmp(var, "SAVSI") == 0) {
        next_double = &surface->SAVSI;
    } else if (strcmp(var, "SAVSO") == 0) {
        next_double = &surface->SAVSO;
    } else if (strcmp(var, "CHSTAT") == 0) {
        next_double = &surface->CHSTAT;
    } else if (strcmp(var, "TWISTA") == 0) {
        next_double = &surface->TWISTA;
    } else if (strcmp(var, "SSPNDD") == 0) {
        next_double = &surface->SSPNDD;
    } else if (strcmp(var, "DHDADI") == 0) {
        next_double = &surface->DHDADI;
    } else if (strcmp(var, "DHDADO") == 0) {
        next_double = &surface->DHDADO;
    } else if (strcmp(var, "TYPE") == 0) {
        num_doubles = 0;
        next_int    = &surface->TYPE;
    } else {
       if (verbose > 1) fprintf(stderr,
                "datcom-parser: Unknown variable %s in PLNF NAMELIST around line %d\n",
                var, line_number);
    }
}

static void SCHRReadVariable(char* var, DATCOM_AIRFOIL *airfoil)
{
    if (strcmp(var, "TYPEIN") == 0) {
        next_int = &airfoil->TYPEIN;
    } else if (strcmp(var, "NPTS") == 0) {
        next_int = &airfoil->NPTS;
    } else if (strcmp(var, "XCORD") == 0) {
        num_doubles = airfoil->NPTS;
        airfoil->XCORD = calloc(num_doubles, sizeof(double));
        next_double = &airfoil->XCORD[0];
    } else if (strcmp(var, "YUPPER") == 0) {
        num_doubles = airfoil->NPTS;
        airfoil->YUPPER = calloc(num_doubles, sizeof(double));
        next_double = &airfoil->YUPPER[0];
    } else if (strcmp(var, "YLOWER") == 0) {
        num_doubles = airfoil->NPTS;
        airfoil->YLOWER = calloc(num_doubles, sizeof(double));
        next_double = &airfoil->YLOWER[0];
    } else {
       if (verbose > 1) fprintf(stderr,
                "datcom-parser: Unknown variable %s in SCHR NAMELIST in line %d\n",
                var, line_number);
    } 
}

static void NACARead(char* str)
{
    ClearLineRead();
    switch (str[5]) {
    case 'W':
        current_aircraft->wingfoil.NACA_DESCR =
            (char *)malloc(strlen(str) + 1);
        strcpy(current_aircraft->wingfoil.NACA_DESCR, str);
        break;
    case 'H':
        current_aircraft->htailfoil.NACA_DESCR =
            (char *)malloc(strlen(str) + 1);
        strcpy(current_aircraft->htailfoil.NACA_DESCR, str);
        break;
    case 'V':
        current_aircraft->vtailfoil.NACA_DESCR =
            (char *)malloc(strlen(str) + 1);
        strcpy(current_aircraft->vtailfoil.NACA_DESCR, str);
        break;
    case 'F':
        current_aircraft->vfinfoil.NACA_DESCR =
            (char *)malloc(strlen(str) + 1);
        strcpy(current_aircraft->vfinfoil.NACA_DESCR, str);
        break;
    default:
       if (verbose > 1) fprintf(stderr, "datcom-parser: Unknow surface in NACA airfoil %s in line %d\n",
                str, line_number);
        break;
    }
}

static void ClearLineRead()
{
    strcpy( LineRead,"" );
}

static void AppendToLineRead(char* str)
{
    strcat( LineRead, str );
    if ( strlen(LineRead) > 80 )
    {
       fprintf(stderr,"datcom-parser: Line is too long at line %d. Keep it under 80 characters\n%s\n", 
          line_number, LineRead );
//       exit(-1);
    }
}

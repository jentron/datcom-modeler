/*
 *=============================================================================
 *  flex based scanner for DATCOM input cards.
 *
 *  Copyright (C) 2009  Anders Gidenstam
 *  anders(at)gidenstam.org
 *  http://www.gidenstam.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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

    /* Internal global variables. Hard to avoid with flex. */
    static AIRCRAFT* current_aircraft;

    static int current_namelist;
    #define NL_NONE    0
    #define NL_UNKNOWN 1
    #define NL_SYNTHS  2
    #define NL_BODY    3
    #define NL_WGPLNF  4

    /* For variables and array variables. */
    static double* next_double;
    static int     num_doubles;

    static int*    next_int;

    static const char* NL_NAME[] = {
                                       "",
                                       "",
                                       "$SYNTHS",
                                       "$BODY",
                                       "$WGPLNF"
                                   };

    /* Internal functions. */
    static void InitializeParser(AIRCRAFT* aircraft);
    static void BeginNameList(char* name);
    static void EndNameList();
    static void ReadVariable(char* var);
    static void ReadDouble(char* str);

    static void SYNTHSReadVariable(char* var);
    static void WGPLNFReadVariable(char* var);
%}

/* This tells flex to read only one input file */
%option noyywrap

DIGIT    [0-9]
ALPHA    [a-zA-Z]
ALPHANUM [A-Za-z0-9]
ID       {ALPHA}+
WS       [ \t\n]
DOUBLE   "-"?{DIGIT}*"."{DIGIT}+("E""-"?{DIGIT}+)?
BOOL     \.TRUE\.|\.FALSE\.
COMMAND  ^(DIM|PART|DERIV|DUMP|DAMP|SAVE|NEXT|CASEID)
NAMELIST "$"{ID}
ARRVAR   {ID}"(1)"{WS}*"="
VAR      {ID}{WS}*"="
AIRFOIL  "NACA"{WS}?{ALPHA}{WS}?{DIGIT}{WS}?{DIGIT}+

/* Rules */
%%

{DOUBLE} {
    fprintf(stderr,"    DOUBLE: %g\n", atof(yytext));
    ReadDouble(yytext);
}

{BOOL} {
    fprintf(stderr,"    BOOL: %s\n", yytext);
}

{NAMELIST} {
    fprintf(stderr,"NAMELIST: %s\n", yytext);
    BeginNameList(yytext);
}

"$"\n {
    fprintf(stderr,"END OF NAMELIST\n");
    EndNameList();
}

{ARRVAR} {
    fprintf(stderr,"  ARRAYVARIABLE: %s\n", yytext);
}

{VAR} {
    fprintf(stderr,"  VARIABLE: %s\n", yytext);
    ReadVariable(yytext);
}

{AIRFOIL} {
    fprintf(stderr,"AIRFOIL: %s\n", yytext);
}

{COMMAND}[^\n]*
    /* Drop uninteresting commands */

^"*"[^\n]*
    /* Drop comments */

"!"[^\n]*
    /* Apparently "!" marks an inline comment? */

[,\n]
    /* Eat ',' and newline */

{WS}+
    /* Eat up whitespace */

. {
    fprintf(stderr, "Unrecognized character: %s\n", yytext );
}

%%

/* Main entry point. */
void ReadDatcom(char* filename, AIRCRAFT* aircraft)
{
    yyin = fopen(filename, "r");

    InitializeParser(aircraft);
    yylex();
    fclose(yyin);
}

static void InitializeParser(AIRCRAFT* aircraft)
{
    current_aircraft = aircraft;
    current_namelist = NL_NONE;
    next_double      = NULL;
    num_doubles      = 0;
    next_int         = NULL;
}

static void BeginNameList(char* name)
{
    int i;
    if (current_namelist != NL_NONE) {
        fputs("datcom-parser: Unterminated NAMELIST.",stderr);
        exit(-1);
    }
    current_namelist = NL_UNKNOWN;
    for (i = 0; i <= NL_WGPLNF; i++) {
        if (strcmp(name, NL_NAME[i]) == 0) {
            current_namelist = i;
        }
    }
}

static void EndNameList()
{
    if (current_namelist == NL_NONE) {
        fputs("datcom-parser: Unbegun NAMELIST terminated.",stderr);
        exit(-1);
    }
    current_namelist = NL_NONE;
}

static void ReadVariable(char* var)
{
    switch (current_namelist) {
    case NL_NONE:
        fputs("datcom-parser: Variable outside of NAMELIST.",stderr);
        exit(-1);
        break;
    case NL_SYNTHS:
        SYNTHSReadVariable(var);
        break;
    case NL_WGPLNF:
        WGPLNFReadVariable(var);
        break;
    default:
        break;
    }
}

static void ReadDouble(char* str)
{
    if (num_doubles-- && next_double != NULL) {
        *next_double = atof(yytext); /* Add error detection here! */
        next_double  = NULL;
    } else if (next_int) {
        *next_int = (int)atof(yytext); /* Add error detection here! */
        next_int = NULL;
    }
}

static void SYNTHSReadVariable(char* var)
{
    /*printf("SYNTHS: %s\n", var);*/
    num_doubles = 1;
    if (strncmp(var, "XCG", 3) == 0) {
        next_double = &current_aircraft->synths.XCG;
    } else if (strncmp(var, "ZCG=", 4) == 0) {
        next_double = &current_aircraft->synths.ZCG;
    } else if (strncmp(var, "XW=", 3) == 0) {
        next_double = &current_aircraft->synths.XW;
    } else if (strncmp(var, "ZW=", 3) == 0) {
        next_double = &current_aircraft->synths.ZW;
    } else if (strncmp(var, "ALIW=", 5) == 0) {
        next_double = &current_aircraft->synths.ALIW;
    } else if (strncmp(var, "XH=", 3) == 0) {
        next_double = &current_aircraft->synths.XH;
    } else if (strncmp(var, "ZH=", 3) == 0) {
        next_double = &current_aircraft->synths.ZH;
    } else if (strncmp(var, "ALIH=", 5) == 0) {
        next_double = &current_aircraft->synths.ALIH;
    } else if (strncmp(var, "XV=", 3) == 0) {
        next_double = &current_aircraft->synths.XV;
    } else if (strncmp(var, "ZV=", 3) == 0) {
        next_double = &current_aircraft->synths.ZV;
    } else if (strncmp(var, "XVF=", 4) == 0) {
        next_double = &current_aircraft->synths.XVF;
    } else if (strncmp(var, "ZVF=", 4) == 0) {
        next_double = &current_aircraft->synths.ZVF;
    } else if (strncmp(var, "SCALE=", 6) == 0) {
        next_double = &current_aircraft->synths.SCALE;
    } else if (strncmp(var, "HINAX=", 6) == 0) {
        next_double = &current_aircraft->synths.HINAX;
    } else if (strncmp(var, "VERTUP=", 7) == 0) {
        num_doubles = 0;
        next_int = &current_aircraft->synths.VERTUP;
    } else {
        fprintf(stderr,
                "datcom-parser: Unknown variable %s in SYNTHS NAMELIST.\n",
                var);
    }
}

static void WGPLNFReadVariable(char* var)
{
    num_doubles = 1;
    if (strncmp(var, "CHRDR=", 6) == 0) {
        next_double = &current_aircraft->wing.CHRDR;
    } else if (strncmp(var, "CHRDBP=", 7) == 0) {
        next_double = &current_aircraft->wing.CHRDBP;
    } else if (strncmp(var, "CHRDTP=", 7) == 0) {
        next_double = &current_aircraft->wing.CHRDTP;
    } else if (strncmp(var, "SSPN=", 5) == 0) {
        next_double = &current_aircraft->wing.SSPN;
    } else if (strncmp(var, "SSPNE=", 6) == 0) {
        next_double = &current_aircraft->wing.SSPNE;
    } else if (strncmp(var, "SSPNOP=", 7) == 0) {
        next_double = &current_aircraft->wing.SSPNOP;
    } else if (strncmp(var, "SAVSI=", 6) == 0) {
        next_double = &current_aircraft->wing.SAVSI;
    } else if (strncmp(var, "SAVSO=", 6) == 0) {
        next_double = &current_aircraft->wing.SAVSO;
    } else if (strncmp(var, "CHSTAT=", 7) == 0) {
        next_double = &current_aircraft->wing.CHSTAT;
    } else if (strncmp(var, "TWISTA=", 7) == 0) {
        next_double = &current_aircraft->wing.TWISTA;
    } else if (strncmp(var, "SSPNDD=", 7) == 0) {
        next_double = &current_aircraft->wing.SSPNDD;
    } else if (strncmp(var, "DHDADI=", 7) == 0) {
        next_double = &current_aircraft->wing.DHDADI;
    } else if (strncmp(var, "DHDADO=", 7) == 0) {
        next_double = &current_aircraft->wing.DHDADO;
    } else if (strncmp(var, "TYPE=", 5) == 0) {
        num_doubles = 0;
        next_int    = &current_aircraft->wing.TYPE;
    } else {
        fprintf(stderr,
                "datcom-parser: Unknown variable %s in PLNF NAMELIST.\n",
                var);
    }
}

/* Test main */
/*
int main(int argc, char** argv) {
    ++argv, --argc;
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    yylex();
}*/

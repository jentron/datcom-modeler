/*
 *=============================================================================
 *  Main program for DATCOM-MODELER
 *
 *  Copyright (C) 2009  Anders Gidenstam (anders(at)gidenstam.org)
 *  Copyright (C) 2009  Ronald Jensen    (ron(at)jentronics.com)
 *  http://www.gidenstam.org
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
#include "getopt.h"     //  Windows
/*#include <unistd.h>*/ //  Unix
#include <stdio.h>
#include <stdlib.h>
#include "datcom-parser.h"
#include "modeler_proto.h"

int verbose=1; /* External to all other modules */
/* Verbose Levels:
 *
 * 0. Quiet: No output on stderr
 * 1. Default: Print errors
 * 2. Chatty: Print interesting trivia, warnings
 * 3. Verbose: Print everything
 *
 */
int dofoils(DATCOM_AIRFOIL *datcomfoil, struct AIRFOIL *foil, char * defaultfoil);


void PrintAC(struct AIRCRAFT *ac)
{
    return;
}

void Usage(char *name)
{
    fprintf(stderr, "Usage: %s [-n b|e|f|h|v|w] [-v] [-q] [-Q] [-o ac-file] datcom-file\n", name);
}

int main(int argc, char *argv[])
{
    AIRCRAFT ac;

/* We read these on the command line */
    int wing=1;
    int htail=1;
    int vtail=1;
    int vfin=1;
    int body=1;
    int engines=1;
    int opt;
    int quads=0;
    FILE *ofp = stdout;
    char *outputfile=NULL;


/* internal variables */
    int objects = 0;
    int propellers = 0;
    int shiny = 0;
    struct AIRFOIL wingfoil;
    struct AIRFOIL htailfoil;
    struct AIRFOIL vtailfoil;
    struct AIRFOIL vfinfoil;

    while ((opt = getopt(argc, argv, "n:o:qvQ")) != -1)
    {
        switch (opt)
        {
            case 'n':
                switch(optarg[0])
                {
                    case 'b':
                        body=0;
                        break;
                    case 'e':
                        engines = 0;
                        break;
                    case 'f':
                        vfin = 0;
                        break;
                    case 'h':
                        htail=0;
                        break;
                    case 'v':
                        vtail=0;
                        break;
                    case 'w':
                        wing=0;
                        break;
                    default:
                        Usage(argv[0]);
                        exit(EXIT_FAILURE);
                }
                break;
            case 'o':
                outputfile=optarg;
                break;
            case 'v':
                verbose++;
                break;
            case 'q':
                verbose = 0;
                break;
            case 'Q':
                quads=1;
                break;
            default: /* '?' */
                Usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

//  fprintf(stderr, "verbose=%d; optind=%d\n", verbose, optind);

    if (optind >= argc) {
        Usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    ReadDatcom(argv[optind], &ac);

/* ReadDatcom will exit() on error so don't create the output file until after it is called */
    if(outputfile)
    {
        fprintf(stderr, "Creating file: %s\n",  outputfile);
        if( ( ofp = fopen(outputfile, "w")) == NULL)
        {
            fprintf(stderr,"Unable to open %s for writing\n", outputfile);
            exit(EXIT_FAILURE);
        }
    }

    PrintAC(&ac);

/* Process wingfoil */
    if (wing) wing = ac.wing.CHRDR > 0. ? 1 : 0; // only render if root chord exists
    if (wing) wing = dofoils(&ac.wingfoil, &wingfoil, "NACA-W-4-2414");
    if (wing) objects += 2;

/* Process htailfoil */
    if (htail) htail = ac.htail.CHRDR > 0. ? 1 : 0; // only render if root chord exists
    if (htail) htail =  dofoils(&ac.htailfoil, &htailfoil, "NACA-H-4-0014");
    if (htail) objects += 2;

/* Process vtailfoil */
    if (vtail) vtail = ac.vtail.CHRDR > 0. ? 1 : 0; // only render if root chord exists
        if (vtail) vtail =  dofoils(&ac.vtailfoil, &vtailfoil, "NACA-V-4-0012");
    if (vtail) objects += 1;

//* Process vfinfoil */
    if (vfin) vfin = ac.vfin.CHRDR > 0. ? 1 : 0; // only render if root chord exists
        if (vfin) vfin =  dofoils(&ac.vfinfoil, &vfinfoil, "NACA-F-4-0012");
    if (vfin) objects += 1;

/* Process Body */
    if (body) body=ac.body.NX > 0. ? 1 : 0;
    if (body) objects +=1;

/* Process Engines */
    if (engines)
    {
        propellers=ac.propwr.NENGSP;
        objects += propellers;
    }
      if(ac.optins.ROUGFC == 0.0) shiny=65;
      else shiny = GetShine(ac.optins.ROUGFC);

      InitAC(ofp, objects, shiny);
      if (wing) WriteWing(ofp, &ac.wing, &wingfoil, "Wing", ac.synths.ALIW, ac.synths.XW, ac.synths.ZW, quads);
      if (htail) WriteWing(ofp, &ac.htail, &htailfoil, "H-Tail", ac.synths.ALIH, ac.synths.XH, ac.synths.ZH, quads);
      if (vtail) WriteFin(ofp, &ac.vtail, &vtailfoil, "V-Tail", ac.synths.XV, ac.synths.ZV, 1, quads);
      if (vfin) WriteFin(ofp, &ac.vfin, &vfinfoil, "V-Fin", ac.synths.XVF, ac.synths.ZVF, ac.synths.VERTUP, quads);
      if (body) WriteBody(ofp, &ac.body, &ac.synths, quads);
      if (propellers) WritePropellers(ofp, &ac.propwr);
    return(0);
}

int dofoils(DATCOM_AIRFOIL *datcomfoil, struct AIRFOIL *foil, char * defaultfoil)
{
        if (datcomfoil->NPTS && datcomfoil->YUPPER && datcomfoil->YLOWER)
                DatcomFoil(datcomfoil, foil);
        else if(datcomfoil->NACA_DESCR)
    {
              NacaFoil(datcomfoil->NACA_DESCR, foil, 20);
              foil->COUNT--;
    }
    else if(defaultfoil)
    {
        NacaFoil(defaultfoil, foil, 20);
        foil->COUNT--;
    }
        else return(0);


    return(1);
}

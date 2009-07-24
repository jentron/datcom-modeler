/*
 *=============================================================================
 *
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

#ifndef MODELER_PROTO_H

/* Funtions in modeler.c */
int InitAC(FILE *ofp, int kids);
int WriteBody(FILE *ofp, struct BODY *body, struct SYNTHS *synths);
int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, char *name, double X, double Z);
int WriteFin(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, char *name, double X, double Z, int vertup);
int WritePropellers(FILE *ofp, struct PROPWR *propwr);

/* Functions in naca.c */
int NacaFoil(char *name, struct AIRFOIL *foil);


/* Functions in body.c */
/******
 *
 * S = Area of rib,  P = Perimiter of rib
 * XR = X Radius, ZR = Z Radius
 * XC = X Center,  ZC = Z Center
 *
 ******/
RIB3D *GetRib(double S, double P, double XR, double ZR, double XC, double YC, double ZC, int points, FILE *ofp);

#define MODELER_PROTO_H
#endif


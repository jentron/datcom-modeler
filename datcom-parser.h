/*
 *=============================================================================
 *  flex based parser for DATCOM input cards.
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

#ifndef DATCOM_PARSER_H
#define DATCOM_PARSER_H

#include "modeler.h"

typedef struct DATCOM_AIRFOIL {
  int     TYPEIN;
  int     DWASH;
  int     NPTS;
  double* XCORD;
  double* YUPPER;
  double* YLOWER;
  char*   NACA_DESCR;
} DATCOM_AIRFOIL;

typedef struct AIRCRAFT {
  struct BODY    body;
  struct SYNTHS  synths;
  struct WGPLNF  wing;
  struct WGPLNF  htail;
  struct WGPLNF  vtail;
  struct WGPLNF  vfin;
  struct PROPWR  propwr;
  DATCOM_AIRFOIL wingfoil;
  DATCOM_AIRFOIL htailfoil;
  DATCOM_AIRFOIL vtailfoil;
  DATCOM_AIRFOIL vfinfoil;
} AIRCRAFT;

void ReadDatcom(char* filename, AIRCRAFT* aircraft);
int DatcomFoil(DATCOM_AIRFOIL *dfoil, struct AIRFOIL *foil);

#endif

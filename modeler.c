/*
 *=============================================================================
 *  modeler.c - routines to generate AC3D file output
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

#include <stdio.h>
#include <math.h>

#include "modeler.h"
#include "modeler_proto.h"
extern int verbose;

/* Tube surface joins two sets of points, starting with offset a and b with triangle surfaces */
int tubesurface(FILE *ofp, int a, int b, int count, int type, int color);
/* Tube surface quad joins two sets of points, starting with offset a and b with quad surfaces */
int tubesurfaceQuads(FILE *ofp, int a, int b, int count, int type, int color);
/* skinsurface joins a set of points in a zig-zag pattern */
int skinsurface(FILE *ofp, int a, int count, int type, int color, int reverse);
/* skinsurface2 joins a set of points in a ray fashion always from point a */
int skinsurface2(FILE *ofp, int a, int count, int type, int color, int reverse);


int InitAC(FILE *ofp, int kids, int shiny)
{
    double r,g,b,sr,sg,sb;

    fprintf(ofp,"AC3Db\n");
    r=0.455;g=0.722;b=0.169;
    GetSpecular(r,g,b,shiny,&sr,&sg,&sb);
    fprintf(ofp,"MATERIAL \"zinc-chromate\" rgb %0.3f %0.3f %0.3f amb %0.3f %0.3f %0.3f  emis 0 0 0  spec %0.3f %0.3f %0.3f shi %d  trans 0\n", r, g, b, r, g, b, sr, sg, sb, shiny);
    r=0.788;g=0.788;b=0.788;
    GetSpecular(r,g,b,shiny,&sr,&sg,&sb);
    fprintf(ofp,"MATERIAL \"white\"  rgb %0.3f %0.3f %0.3f amb %0.3f %0.3f %0.3f  emis 0 0 0  spec %0.3f %0.3f %0.3f shi %d  trans 0\n", r, g, b, r, g, b, sr, sg, sb, shiny);
    r=0.906;g=0.369;b=0.31;
    GetSpecular(r,g,b,shiny,&sr,&sg,&sb);
    fprintf(ofp,"MATERIAL \"red\"  rgb %0.3f %0.3f %0.3f amb %0.3f %0.3f %0.3f  emis 0 0 0  spec %0.3f %0.3f %0.3f shi %d  trans 0\n", r, g, b, r, g, b, sr, sg, sb, shiny);
    fprintf(ofp,"OBJECT world\nkids %d\n", kids);

    return(1);
}

int WritePropellers(FILE *ofp, struct PROPWR *propwr)
{
    int i, j;
    double Y = propwr->YP;
    for(i=0;i < propwr->NENGSP; i++)
    {
         fprintf(ofp,"OBJECT polyline\nname \"Prop %d\"\ncrease 89.0\nnumvert %d\n", i+1, 32);
         GetRib(propwr->PRPRAD*propwr->PRPRAD*3.14159, propwr->PRPRAD*3.2832, propwr->PRPRAD, propwr->PRPRAD, propwr->PHALOC, Y, propwr->PHVLOC, 32, ofp);
         fprintf(ofp,"numsurf 1\nSURF 0x31\nmat 0\nrefs %d\n", 32);
         for (j = 31; j>=0;j--)
         {
            fprintf(ofp,"%d 0 0\n", j);
         }
         fprintf(ofp,"kids 0\n");
         Y *= -1;
     }
    return(1);
}


int WriteBody(FILE *ofp, struct BODY *body, struct SYNTHS *synths, int Quads)
{
    int i, j, points=32, surface_count;
    double XR, ZR, ZC;
    double p=0.,r=0.,s=0.,z=0.;
    int good=0;
/* first we look at the data structure too see what information we've been provided */
    for(i=0; i < body->NX; i++)
        {
        p += body->P[i]; // P(erimiter) should always be positve!
        r += body->R[i]; // R(adius) should always be positve!
        s += body->S[i]; // S(urface area) should always be positve!
        z += fabs(body->ZU[i]) + fabs(body->ZL[i]) ; // Upper and lower bounds can have sign
    }
/* Synthesize "R" if needed */
  if(r) good=1;
  else if (s)
  {
    for(i=0; i < body->NX; i++)
    {
        body->R[i] = sqrt(body->S[i]/3.14159);
    }
    good=1;
  } else if (p) {
    for(i=0; i < body->NX; i++)
    {
        body->R[i] = (body->P[i]/6.2831853);
    }
    good=1;
  } else {
    if (verbose > 0) fprintf(stderr,"Not enough data found to draw a body, sorry. P=%f, R=%f, S=%f, Z=%f\n", p, r, s, z);
    return 0; //not enough data to proceed
  }

/* Synthesize "S" if needed */
  if(!s)
  {
        for(i=0; i < body->NX; i++)
        {
        if(z) body->S[i] = body->R[i]*((body->ZU[i]-body->ZL[i])/2)*3.14159;
        else body->S[i] = body->R[i]*body->R[i]*3.14159 ;
        }

  }

/* Suggest "P" if needed */
/* We interpolate between the circumfrence of a diamond and the
   circumference of a square based on the area */
    if( s && !p )
    {
        if (verbose > 0) fprintf(stderr,"If S(1) is given, P(1) is generally required by DATCOM for $BODY\n");
        if (verbose > 1)
        {
            fprintf(stderr,"* Perimeter estimation follows:\n P(1) = ");
            for(i=0; i < body->NX; i++)
            {
                if(z) ZR = (body->ZU[i]-body->ZL[i])/2;
                else ZR = body->R[i];
                XR = body->R[i];
                double ref = (body->S[i] / (ZR * XR)-2)/2;
                ref=pow(ref, 0.5);
                double s_area = 4.0 * (XR + ZR);
                double d_area = 4.0 * pow( (XR * XR + ZR * ZR), 0.5);
                double area = s_area * ref + d_area * (1.0 - ref);
                fprintf(stderr,"%0.3f, ", area);
            }
            fprintf(stderr,"\n");
        }
    }




/* then we might be able to make use of it */
        fprintf(ofp,"OBJECT poly\nname \"Body\"\ncrease 89.0\nnumvert %d\n", body->NX * points); //
   if( z )
   {
    for(i=0; i < body->NX; i++)
    {
        ZR = (body->ZU[i]-body->ZL[i])/2;
        ZC = body->ZU[i]-ZR;
        GetRib(body->S[i], body->P[i], body->R[i], ZR, body->X[i], 0.0, ZC, points, ofp);
    }
   } else {
    for(i=0; i < body->NX; i++)
    {
        GetRib(body->S[i], body->P[i], body->R[i], body->R[i], body->X[i], 0.0, 0.0, points, ofp);
    }

  }

  surface_count = points * ( body->NX -1 );
  if( !Quads ) surface_count *= 2;
  fprintf(ofp,"numsurf %d\n",  surface_count );

  for(i=0;i<body->NX - 1;i++)
  {
     if(!Quads) tubesurface(ofp, (i+1) * points, (i) * points , points, 0x30, 0);
     else  tubesurfaceQuads(ofp, (i+1) * points, (i) * points , points, 0x30, 0);
  }
  fprintf(ofp,"kids 0\n");
  return(1);
}

int WriteWing(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, char *name, double ALI, double X, double Z, int Quads)
{
    int i, ribs, sections, current_rib, current_section, surface_count;
    double chord[4],span[4], offset_x[4], offset_z[4], twist[4];

    if(wing->SSPNOP > 0.0)
    {
        ribs=4;
        sections=3;
        chord[0]=wing->CHRDR;
        chord[1]=wing->CHRDBP+(wing->CHRDR-wing->CHRDBP)*((wing->SSPNE-wing->SSPNOP)/(wing->SSPN-wing->SSPNOP));
        chord[2]=wing->CHRDBP;
        chord[3]=wing->CHRDTP;
        span[0]=0;
        span[1]=wing->SSPN-wing->SSPNE;
        span[2]=wing->SSPN-wing->SSPNOP;
        span[3]=wing->SSPN;
        offset_x[0]=X+wing->CHSTAT*wing->CHRDR;
        offset_x[1]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[1];
        offset_x[2]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2];
        offset_x[3]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2] + sin(wing->SAVSO * 0.017453293)*wing->SSPNOP;
        twist[0] = 0.0;
        twist[1] = 0.0;
        twist[2] = wing->TWISTA * ((wing->SSPNE - wing->SSPNOP) / (wing->SSPN - wing->SSPNOP));
        twist[3] = wing->TWISTA;
// SSPNDD  * cos (dihedral) = SSPNOP ??
        offset_z[0]=Z;
        offset_z[1]=Z + sin(wing->DHDADI * 0.017453293) * (wing->SSPN - wing->SSPNE);
        offset_z[2]=Z + sin(wing->DHDADI * 0.017453293) * (wing->SSPN -wing->SSPNOP);
        offset_z[3]=Z + sin(wing->DHDADI * 0.017453293) * (wing->SSPN -wing->SSPNOP)
                              + sin(wing->DHDADO * 0.017453293) * (wing->SSPNOP) ;
    } else {
        ribs=3;
        sections=2;
        chord[0]=wing->CHRDR;
        chord[1]=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
        chord[2]=wing->CHRDTP;
        span[0]=0;
        span[1]=wing->SSPN-wing->SSPNE;
        span[2]=wing->SSPN;
        offset_x[0]=X+wing->CHSTAT*wing->CHRDR;
        offset_x[1]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[1];
        offset_x[2]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2];
        offset_z[0]=Z;
        offset_z[1]=Z + sin(wing->DHDADI * 0.017453293) * (wing->SSPN - wing->SSPNE);
        offset_z[2]=Z + sin(wing->DHDADI * 0.017453293) * (wing->SSPN);
        twist[0] = 0.0;
        twist[1] = 0.0;
        twist[2] = wing->TWISTA;
    }

    fprintf(ofp,"OBJECT poly\nname \"Left %s\"\ncrease 45.0\nnumvert %d\n", name, airfoil->COUNT * ribs); // three or four based on type
    printf("ALI %f    ", ALI);
    printf("TWISTA %f   ", wing->TWISTA);
    for(current_rib=0;current_rib<ribs;current_rib++)
    {
        printf("twist %f  \n", twist[current_rib]);
        for (i=0;i<airfoil->COUNT;i++)
        {
            double Dx = airfoil->DATAX[i];
            double ANG = ALI + twist[current_rib];
            double Dy = airfoil->DATAY[i] - airfoil->DATAX[i] * sin(ANG/ 57.3);
            fprintf(ofp,"%f %f %f\n",
                offset_x[current_rib] + ((Dx - wing->CHSTAT) * chord[current_rib]),
                offset_z[current_rib] + (Dy * chord[current_rib]),
                span[current_rib]);
        }
    }

    surface_count = airfoil->COUNT * sections;
    if( !Quads ) surface_count *= 2;
    surface_count += airfoil->COUNT - 2;
    fprintf(ofp,"numsurf %d\n",  surface_count );

    for(current_section=0;current_section<sections;current_section++)
    {
        if(!Quads) tubesurface(ofp, current_section * airfoil->COUNT, (current_section + 1) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
        else  tubesurfaceQuads(ofp, current_section * airfoil->COUNT, (current_section + 1) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
    }
    skinsurface(ofp, (sections)*airfoil->COUNT, airfoil->COUNT, 0x30, sections-1, 0);

    fprintf(ofp,"kids 0\n");

    fprintf(ofp,"OBJECT poly\nname \"Right %s\"\ncrease 45.0\nnumvert %d\n", name, airfoil->COUNT * ribs); // three or four based on type
    for (current_rib = 0; current_rib < ribs; current_rib++)
    {
        printf("twist %f  \n", twist[current_rib]);
        for (i=0;i<airfoil->COUNT;i++)
        {
            double Dx = airfoil->DATAX[i];
            double ANG = ALI + twist[current_rib];
            double Dy = airfoil->DATAY[i] - airfoil->DATAX[i] * sin(ANG / 57.3);
            fprintf(ofp,"%f %f %f\n",
                offset_x[current_rib] + ((Dx - wing->CHSTAT) * chord[current_rib]),
                offset_z[current_rib] + (Dy * chord[current_rib]),
                -span[current_rib]);
        }
    }


    fprintf(ofp,"numsurf %d\n", surface_count);
    for(current_section=0;current_section<sections;current_section++)
    {
        if(!Quads) tubesurface(ofp, (current_section + 1) * airfoil->COUNT, (current_section ) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
        else  tubesurfaceQuads(ofp, (current_section + 1) * airfoil->COUNT, (current_section ) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
    }
    skinsurface(ofp, (sections)*airfoil->COUNT, airfoil->COUNT, 0x30, sections-1, 1);
    fprintf(ofp,"kids 0\n");

    return(1);
}

int WriteFin(FILE *ofp, struct WGPLNF *wing, struct AIRFOIL *airfoil, char *name, double X, double Z, int vertup, int Quads)
{
    int i, ribs, sections, current_rib, current_section, count, surface_count;
    double chord[4],span[4], offset_x[4], offset_z[4],sign=1.0;

    if(!vertup) sign= -1.0;
    if (verbose > 0) fprintf(stderr,"VERTUP is %d, sign is %0.1f\n", vertup, sign);

    if(wing->SSPNOP > 0.0)
    {
        ribs=4;
        sections=3;
        chord[0]=wing->CHRDR;
        chord[1]=wing->CHRDBP+(wing->CHRDR-wing->CHRDBP)*((wing->SSPNE-wing->SSPNOP)/(wing->SSPN-wing->SSPNOP));
        chord[2]=wing->CHRDBP;
        chord[3]=wing->CHRDTP;
        span[0]=0;
        span[1]=wing->SSPN-wing->SSPNE;
        span[2]=wing->SSPN-wing->SSPNOP;
        span[3]=wing->SSPN;
        offset_x[0]=X+wing->CHSTAT*wing->CHRDR;
        offset_x[1]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[1];
        offset_x[2]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2];
        offset_x[3]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2] + tan(wing->SAVSO * 0.017453293)*wing->SSPNOP;
// SSPNDD  * cos (dihedral) = SSPNOP ??
        offset_z[0]=Z;
        offset_z[1]=Z;
        offset_z[2]=Z + tan(wing->DHDADI * 0.017453293) * (wing->SSPNE-wing->SSPNOP);
        offset_z[3]=Z + tan(wing->DHDADI * 0.017453293) * (wing->SSPNE-wing->SSPNOP)
                              + tan(wing->DHDADO * 0.017453293) * (wing->SSPNOP) ;
    } else {
        ribs=3;
        sections=2;
        chord[0]=wing->CHRDR;
        chord[1]=wing->CHRDTP+(wing->CHRDR-wing->CHRDTP)*(wing->SSPNE/wing->SSPN);
        chord[2]=wing->CHRDTP;
        span[0]=0;
        span[1]=wing->SSPN-wing->SSPNE;
        span[2]=wing->SSPN;
        offset_x[0]=X+wing->CHSTAT*wing->CHRDR;
        offset_x[1]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[1];
        offset_x[2]=offset_x[0]+tan(wing->SAVSI * 0.017453293)*span[2];
        offset_z[0]=Z;
        offset_z[1]=Z;
        offset_z[2]=Z + tan(wing->DHDADI * 0.017453293) * (wing->SSPNE);
    }

    fprintf(ofp,"OBJECT poly\nname \"Vertical %s\"\ncrease 45.0\nnumvert %d\n", name, airfoil->COUNT * ribs); // three or four based on type
    for(current_rib=0;current_rib<ribs;current_rib++)
    {
        if(vertup)
        {
            for (i=0;i<airfoil->COUNT;i++)
            {
                fprintf(ofp,"%f %f %f\n",
                    offset_x[current_rib] + ((airfoil->DATAX[i] - wing->CHSTAT) * chord[current_rib]),
                    (offset_z[current_rib] + span[current_rib])*sign,
                    (airfoil->DATAY[i] * chord[current_rib]));
            }
        } else {
            for (i=airfoil->COUNT-1;i>=0;i--)
            {
                fprintf(ofp,"%f %f %f\n",
                    offset_x[current_rib] + ((airfoil->DATAX[i] - wing->CHSTAT) * chord[current_rib]),
                    (offset_z[current_rib] + span[current_rib])*sign,
                    (airfoil->DATAY[i] * chord[current_rib]));
            }

        }
    }

    surface_count = airfoil->COUNT * sections;
    if( !Quads ) surface_count *= 2;
    surface_count += airfoil->COUNT - 2;
    fprintf(ofp,"numsurf %d\n",  surface_count );
    for(current_section=0;current_section<sections;current_section++)
    {
        if(!Quads) tubesurface(ofp, (current_section + 1) * airfoil->COUNT, (current_section) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
        else  tubesurfaceQuads(ofp, (current_section + 1) * airfoil->COUNT, (current_section) * airfoil->COUNT , airfoil->COUNT, 0x30, current_section);
    }
    skinsurface(ofp, (sections)*airfoil->COUNT, airfoil->COUNT, 0x30, sections-1, vertup );

    fprintf(ofp,"kids 0\n");

    return(1);
}



int tubesurface(FILE *ofp, int a, int b, int count, int type, int color)
{
    int i;
    for(i=0;i<count;i++)
    {
        fprintf(ofp, "SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, a+i, b+i, a+(i+1)%count);
        fprintf(ofp, "SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, b+i, b+(i+1)%count, a+(i+1)%count);
    }
    return(1);
}

int tubesurfaceQuads(FILE *ofp, int a, int b, int count, int type, int color)
{
    int i;
    for(i=0;i<count;i++)
    {
        fprintf(ofp, "SURF 0x%02x\nmat %d\nrefs 4\n%d 0 0\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, a+i, b+i, b+(i+1)%count, a+(i+1)%count);
    }
    return(1);
}

int skinsurface(FILE *ofp, int a, int count, int type, int color, int reverse)
{
    int i,j,c;
    if(reverse){
        for(i=a,j=a+count-1, c=count-2;c>1;i++,j--,c-=2)
        {
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, i, i+1, j);
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, i+1, j-1, j);
        }
        if(c)
        {
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, i, i+1, j);
        }
    } else {
        for(i=a,j=a+count-1, c=count-2;c>1;i++,j--,c-=2)
        {   
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, i+1, i, j);
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, i+1, j, j-1);
        }
        if(c)
        {
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, i+1, i, j);
        }
    }

    return(1);
}

int skinsurface2(FILE *ofp, int a, int count, int type, int color, int reverse)
{
    int i;
    if(reverse){
        for(i=1;i<count;i++)
        {
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, a, a+i, a+i+1);
        }
    } else {
        for(i=1;i<count;i++)
        {
            fprintf(ofp,"SURF 0x%02x\nmat %d\nrefs 3\n%d 0 0\n%d 0 0\n%d 0 0\n", type, color, a, a+i+1, a+i);
        }
    }

    return(1);
}

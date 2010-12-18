#include "threeD.h"

int main(int argc, char *argv)
{
	Object3D World, *foo;
	int i;
	
	World.child=NULL;
	World.next=NULL;
	World.type=0;
	World.kids=1;
	World.name="World";
	World.texture=NULL;
	World.data=NULL;
	World.datac=0;
	World.texrep_u=0.;
	World.texrep_v=0.;
	World.crease = 80.;
	World.rot = NULL;
	World.loc.element[0]=0.;
	World.loc.element[1]=0.;
	World.loc.element[2]=0.;
	World.url=NULL;
	World.numverts=0;
	World.vert=NULL;
	World.numsurf=0;
	World.surf=NULL;

	World.child=CreateObject(39, 78, "Left Wing", NULL, 2);
	foo=World.child;
	foo->numverts=foo->allocverts;
	foo->numsurf=foo->allocsurf;
	for(i=0;i<foo->numsurf;i++)
	{
		foo->surf[i].refs=3;
	}

	WriteObject(&World, stdout);
	WriteObject(foo, stdout);
	

}
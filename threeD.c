#include "threeD.h"
#include <stdlib.h>
#include <string.h>

Object3D *CreateObject(unsigned int numvert, unsigned int numsurf, char *name, char *texture, int type)
{
	Object3D *result;
	size_t i;

	result=malloc(sizeof(Object3D));
	if(!result) return NULL;
	if(numvert){
		i=sizeof(Vector3)*numvert;
		result->allocverts=numvert;
		result->vert=malloc(i);
		if(!result->vert){
			fprintf(stderr,"Could not allocate %d bytes for vertices\n", i);
			free(result);
			return NULL;
		}
	}
	if(numsurf){
		i=sizeof(Surface3)*numsurf;
		result->surf=malloc(i);
		result->allocsurf=numsurf;
		if(!result->surf){
			fprintf(stderr,"Could not allocate %d bytes for surfaces\n", i);
			free(result->vert);
			free(result);
			return NULL;
		}
	}

	if(name){
		i=strlen(name);
		result->name=malloc(i);
		if(!result->name){
			fprintf(stderr,"Could not allocate %d bytes for object name\n", i);
		}
		strncpy(result->name, name, i);
	}

	if(texture){
		i=strlen(texture);
		result->texture=malloc(i);
		if(!result->texture){
			fprintf(stderr,"Could not allocate %d bytes for object texture\n", i);
		}
		strncpy(result->texture, texture, i);
	}
	result->type=type;

	return(result);
}

void DestroyObject(Object3D *obj);

int  WriteObject(Object3D *obj, FILE *fp)
{
	char *type[]={"WORLD","GROUP","POLY"};
	int len, i, j;

	if(!fp) return(0);

	fprintf(fp,"OBJECT %s\n", type[obj->type]);
	fprintf(fp,"name \"%s\"\n", obj->name);
	if(obj->datac)  //the trailing newline excluded from the length of the datablock
	{
		len=obj->datac;
		fprintf(fp,"data %d\n", obj->datac);
		fwrite(obj->data, 1, obj->datac, fp);
		fprintf(fp,"\n");
	}

	if(obj->texture)
	{
		fprintf(fp,"texture \"%s\"\n", obj->texture);
		fprintf(fp,"textrep %d %d\n", obj->texrep_u, obj->texrep_v);
	}
	if(obj->rot){  //fixme: check order here!
		fprintf(fp,"rot %f %f %f  %f %f %f  %f %f %f\n",
			obj->rot->element[0][0], obj->rot->element[0][1], obj->rot->element[0][2], 
			obj->rot->element[1][0], obj->rot->element[1][1], obj->rot->element[1][2], 
			obj->rot->element[2][0], obj->rot->element[2][1], obj->rot->element[2][2]);
	}
	fprintf(fp,"loc %f %f %f\n", obj->loc.element[0], obj->loc.element[1], obj->loc.element[2]);
	if(obj->url)
	{
		fprintf(fp,"url \"%s\"\n", obj->url);
	}
	fprintf(fp,"crease %f\n", obj->crease);
	fprintf(fp,"numvert %d\n", obj->numverts);
	for(i=0;i<obj->numverts;i++)
	{
		fprintf(fp,"%f %f %f\n", obj->vert[i].element[0], obj->vert[i].element[1], obj->vert[i].element[2]);
	}
	fprintf(fp,"numsurf %d\n", obj->numsurf);
	for(i=0;i<obj->numsurf;i++)
	{
		fprintf(fp,"SURF %x\n", obj->surf[i].type);
		fprintf(fp,"mat %d\n", obj->surf[i].mat);
		fprintf(fp,"refs %d\n", obj->surf[i].refs);
		for (j=0;j<obj->surf[i].refs;j++)
		{
			fprintf(fp,"%d %f %f\n", obj->surf[i].point[j], obj->surf[i].u[j], obj->surf[i].v[j]);
		}
	}
	fprintf(fp,"kids %d\n", obj->kids);
}

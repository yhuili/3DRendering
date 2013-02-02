/*   CS580 HW   */
// 3D_Graphics_&_Rendering_HW5.cpp:
// Name: Hui Li
#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include <vector>
#include <algorithm>

#define PI 3.141592653; //define PI

using namespace std;

//additional function declaration for render
void setVert(GzPointer *valueList, vector<Vertex>& vert);
void sortVertsByY(vector<Vertex>& vert);
void setupTri(GzRender *render, vector<Edge>& edge, vector<Vertex>& vert, int &oriented);
void scanLineRast(GzRender *render, vector<Edge>& edge, vector<Vertex>& vert, int &oriented);
short	ctoi(float color);

//additional function declaration for transform
float degree2Radian(float degree);
void matrixMul(const GzMatrix matrix_top, const GzMatrix matrix_new, GzMatrix matrix_out);
int setXsp(GzRender *render);
int setXpi(GzRender *render);
Vertex normalize(const Vertex& v);
float dotProduct(const Vertex& a, const Vertex& b);
Vertex crossProduct(const Vertex& a, const Vertex& b);
int setXiw(GzRender *render);
void transVert(vector<Vertex>& vertW, GzMatrix matrix, vector<Vertex>& vertS);
int checkTri(GzDisplay *display, vector<Vertex>& v);

//additional function declaration for shading
void normalizeCoord(GzCoord& v);
void normalizeNormal(Vertex& v);
void setNorm(GzPointer *valueList, vector<Vertex>& vert);
void transNorm(vector<Vertex>& vertW, GzMatrix matrix, vector<Vertex>& vertS);
void assignPos(vector<Vertex>& vertPos, vector<Vertex>& vert);
void assignNorm(vector<Vertex>& vertNorm, vector<Vertex>& vert);
int getColor(GzRender *render, const Vertex v, GzColor color);

//additional function declaration for texture
void setTexture(GzPointer *valueList, vector<Vertex>& vert);
void assignTexture(vector<Vertex>& vertTex, vector<Vertex>& vert);
void warp2Perspective(vector<Vertex> &v);
void unwarp(Vertex &v, GzTextureIndex tex);

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
//Rotation Matrix along X axis:
//1,  0,   0,    0
//0,  cos, -sin, 0
//0,  sin, cos,  0
//0,  0,   0,    1
	float radian = degree2Radian(degree);
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = cos(radian);
	mat[1][2] = -sin(radian);
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = sin(radian);
	mat[2][2] = cos(radian);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
//Rotation Matrix along Y axis:
//cos,  0,  sin, 0
//0,    1,  0,   0
//-sin, 0,  cos, 0
//0,    0,  0,   1
	float radian = degree2Radian(degree);
	mat[0][0] = cos(radian);
	mat[0][1] = 0;
	mat[0][2] = sin(radian);
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = -sin(radian);
	mat[2][1] = 0;
	mat[2][2] = cos(radian);
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
//Rotation Matrix along Z axis:
//cos,  -sin, 0,  0
//sin,  cos,  0,  0
//0,    0,    1,  0
//0,    0,    0,  1
	float radian = degree2Radian(degree);
	mat[0][0] = cos(radian);
	mat[0][1] = -sin(radian);
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = sin(radian);
	mat[1][1] = cos(radian);
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value
//Translantion Matrix:
//1,  0,  0,  tx
//0,  1,  0,  ty
//0,  0,  1,  tz
//0,  0,  0,  1
	mat[0][0] = 1;
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = translate[0];

	mat[1][0] = 0;
	mat[1][1] = 1;
	mat[1][2] = 0;
	mat[1][3] = translate[1];

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = 1;
	mat[2][3] = translate[2];

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value
//Scaling Matrix:
//sx,  0,   0,   0
//0,   sy,  0,   0
//0,   0,   sz,  0
//0,   0,   0,   1
	mat[0][0] = scale[0];
	mat[0][1] = 0;
	mat[0][2] = 0;
	mat[0][3] = 0;

	mat[1][0] = 0;
	mat[1][1] = scale[1];
	mat[1][2] = 0;
	mat[1][3] = 0;

	mat[2][0] = 0;
	mat[2][1] = 0;
	mat[2][2] = scale[2];
	mat[2][3] = 0;

	mat[3][0] = 0;
	mat[3][1] = 0;
	mat[3][2] = 0;
	mat[3][3] = 1;
	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- keep closed until all inits are done 
- setup Xsp and anything only done once 
- span interpolator needs pointer to display 
- check for legal class GZ_Z_BUFFER_RENDER 
- init default camera 
*/ 
	*render = new GzRender;
	if (*render == NULL) 
	{
		return GZ_FAILURE;
	}
	(*render)->renderClass = renderClass;
	if ((*render)->renderClass != GZ_Z_BUFFER_RENDER)
	{ 
		return GZ_FAILURE;
	}
	(*render)->display = display;
	//initialize the render flag as close
	(*render)->open = 0;
	//initialize the texture function as null
	(*render)->tex_fun = NULL;
	//initialize the stack as empty
	(*render)->matlevel = 0;
	//set transform matrix
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			(*render)->Xsp[i][j] = 0;
			(*render)->camera.Xiw[i][j] = 0;
			(*render)->camera.Xpi[i][j] = 0;
		}
	}
	//set default camera
	(*render)->camera.FOV = DEFAULT_FOV;
	
	(*render)->camera.lookat[0] = 0;
	(*render)->camera.lookat[1] = 0;
	(*render)->camera.lookat[2] = 0;

	(*render)->camera.position[0] = DEFAULT_IM_X;
	(*render)->camera.position[1] = DEFAULT_IM_Y;
	(*render)->camera.position[2] = DEFAULT_IM_Z;

	(*render)->camera.worldup[0] = 0;
	(*render)->camera.worldup[1] = 1;
	(*render)->camera.worldup[2] = 0;

	(*render)->interp_mode = GZ_RGB_COLOR;
	(*render)->numlights = 0;
	GzColor Ka = DEFAULT_AMBIENT;
	GzColor Kd = DEFAULT_DIFFUSE;
	GzColor Ks = DEFAULT_SPECULAR;
	//initialize Ka
	(*render)->Ka[0] = Ka[0];
	(*render)->Ka[1] = Ka[1];
	(*render)->Ka[2] = Ka[2];
	//initialize Kd
	(*render)->Kd[0] = Kd[0];
	(*render)->Kd[1] = Kd[1];
	(*render)->Kd[2] = Kd[2];
	//initialize Ks
	(*render)->Ks[0] = Ks[0];
	(*render)->Ks[1] = Ks[1];
	(*render)->Ks[2] = Ks[2];
	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/
	delete render;
	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- set up for start of each frame - clear frame buffer 
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms if it want to. 
*/ 
	//set up frame
	if (GzInitDisplay(render->display) != GZ_SUCCESS)
	{
		return GZ_FAILURE;
	}
	//set Xsp
	setXsp(render);
	//push Xsp
	GzPushMatrix(render, render->Xsp);
	//set Xpi
	setXpi(render);
	//push Xpi
	GzPushMatrix(render, render->camera.Xpi);
	//set Xiw
	setXiw(render);
	//push Xiw
	GzPushMatrix(render, render->camera.Xiw);
	render->open = 1;
	return GZ_SUCCESS;
}

int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	render->camera.FOV = camera->FOV;

	render->camera.position[0] = camera->position[0];
	render->camera.position[1] = camera->position[1];
	render->camera.position[2] = camera->position[2];

	render->camera.lookat[0] = camera->lookat[0];
	render->camera.lookat[1] = camera->lookat[1];
	render->camera.lookat[2] = camera->lookat[2];

	render->camera.worldup[0] = camera->worldup[0];
	render->camera.worldup[1] = camera->worldup[1];
	render->camera.worldup[2] = camera->worldup[2];
	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	//for Ximage
	if (render->matlevel >= MATLEVELS) //overflow
	{	
		return GZ_FAILURE;
	}
	
	if (render->matlevel == 0) //empty stack
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				render->Ximage[render->matlevel][i][j] = matrix[i][j];
				render->Xnorm[render->matlevel][i][j] = 0;
			}
		}
		render->Xnorm[render->matlevel][0][0] = 1;
		render->Xnorm[render->matlevel][1][1] = 1;
		render->Xnorm[render->matlevel][2][2] = 1;
		render->Xnorm[render->matlevel][3][3] = 1;
	}
	else //there is matrix in the stack
	{
		matrixMul(render->Ximage[render->matlevel-1], matrix, render->Ximage[render->matlevel]);
		if (render->matlevel == 1)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					render->Xnorm[render->matlevel][i][j] = 0;
				}
			}
			render->Xnorm[render->matlevel][0][0] = 1;
			render->Xnorm[render->matlevel][1][1] = 1;
			render->Xnorm[render->matlevel][2][2] = 1;
			render->Xnorm[render->matlevel][3][3] = 1;
		}
		else
		{
			float k;
			GzMatrix R;
			//K = 1 / (a^2 + b^2 + c^2)^1/2
			k = 1 / sqrt(matrix[0][0] * matrix[0][0] + matrix[1][0] * matrix[1][0] + matrix[2][0] * matrix[2][0]);
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					R[i][j] = matrix[i][j] * k;
				}
				R[i][3] = 0;
				R[3][i] = 0;
			}
			R[3][3] = 1;
			matrixMul(render->Xnorm[render->matlevel-1], R, render->Xnorm[render->matlevel]);
		}
	}
	render->matlevel++;
	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render->matlevel <= 0)
	{
		return GZ_FAILURE;
	}
	render->matlevel--;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++)
	{
		//set renderer attribute states
		if (nameList[i] == GZ_RGB_COLOR)
		{
			GzColor* colorPointer = (GzColor*)(valueList[i]);
			render->flatcolor[0] = (*colorPointer)[0];
			render->flatcolor[1] = (*colorPointer)[1];
			render->flatcolor[2] = (*colorPointer)[2];
		}
		else if (nameList[i] == GZ_INTERPOLATE)
		{
			int* is = (int*)(valueList[i]);
			render->interp_mode = (*is);
		}
		else if (nameList[i] == GZ_DIRECTIONAL_LIGHT)
		{
			if (render->numlights >= MAX_LIGHTS) 
			{
				return GZ_FAILURE;
			}
			GzLight* directLight = (GzLight*)(valueList[i]);
			render->lights[render->numlights].color[0] = directLight->color[0];
			render->lights[render->numlights].color[1] = directLight->color[1];
			render->lights[render->numlights].color[2] = directLight->color[2];
			render->lights[render->numlights].direction[0] = directLight->direction[0];
			render->lights[render->numlights].direction[1] = directLight->direction[1];
			render->lights[render->numlights].direction[2] = directLight->direction[2];
			normalizeCoord(render->lights[render->numlights].direction);
			render->numlights++;
		}
		else if (nameList[i] == GZ_AMBIENT_LIGHT)
		{
			GzLight* ambientLight = (GzLight*)(valueList[i]);
			render->ambientlight.color[0] = ambientLight->color[0];
			render->ambientlight.color[1] = ambientLight->color[1];
			render->ambientlight.color[2] = ambientLight->color[2];
		}
		else if (nameList[i] == GZ_AMBIENT_COEFFICIENT)
		{
			GzColor* colorPointer = (GzColor*)(valueList[i]);
			render->Ka[0] = (*colorPointer)[0];
			render->Ka[1] = (*colorPointer)[1];
			render->Ka[2] = (*colorPointer)[2];	
		}
		else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT)
		{
			GzColor* colorPointer = (GzColor*)(valueList[i]);
			render->Kd[0] = (*colorPointer)[0];
			render->Kd[1] = (*colorPointer)[1];
			render->Kd[2] = (*colorPointer)[2];
		}
		else if (nameList[i] == GZ_SPECULAR_COEFFICIENT)
		{
			GzColor* colorPointer = (GzColor*)(valueList[i]);
			render->Ks[0] = (*colorPointer)[0];
			render->Ks[1] = (*colorPointer)[1];
			render->Ks[2] = (*colorPointer)[2];
		}
		else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT)
		{
			float* sp = (float*)(valueList[i]);
			render->spec = (*sp);
		}
		else if (nameList[i] == GZ_TEXTURE_MAP)
		{
			GzTexture texture = (GzTexture)(valueList[i]);
			render->tex_fun = texture;
		}
	}
	return GZ_SUCCESS;
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, 
				  GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts  
- Clip - just discard any triangle with verts behind view plane 
       - test for triangles with all three verts off-screen 
- invoke triangle rasterizer  
*/ 
	//set vertPos for initialization
	vector<Vertex> vertPos(3);
	//set vertNorm for initialization
	vector<Vertex> vertNorm(3);
	//set vertTex for initialization
	vector<Vertex> vertTex(3);
	//result vert vp for screen space position
	vector<Vertex> vp(3);
	//result vert vni for image space normal
	vector<Vertex> vni(3);
	//result vert v
	vector<Vertex> v(3);
	vector<Edge> edge(3);
	int oriented = 0;

	for (int i = 0; i < numParts; i++)
	{
		//set renderer attribute states
		if (nameList[i] == GZ_POSITION)
		{
			//set Vertex
			setVert(valueList, vertPos);
			//transform vertex from world space to screen space
			transVert(vertPos, render->Ximage[render->matlevel-1], vp);
			//check the position of the triangles, and discard the useless triangle
			if ((checkTri(render->display, vp)) == 1)
			{
				continue;
			}
			assignPos(vp, v);
		}
		else if (nameList[i] == GZ_NORMAL)
		{
			//set Normal
			setNorm(valueList, vertNorm);
			//transform vertex from world space to image space
			transNorm(vertNorm, render->Xnorm[render->matlevel-1], vni);
			//normalize the normal for calculation
			normalizeNormal(vni[0]);
			normalizeNormal(vni[1]);
			normalizeNormal(vni[2]);
			assignNorm(vni, v);
		}
		else if (nameList[i] == GZ_TEXTURE_INDEX)
		{
			//set texture
			setTexture(valueList, vertTex);
			//assign texture
			assignTexture(vertTex, v);
		}
		else if (nameList[i] == GZ_NULL_TOKEN)
		{
			//do nothing
		}
	}
	//sort Vertex
	sortVertsByY(v);
	setupTri(render, edge, v, oriented);
	scanLineRast(render, edge, v, oriented);
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */
//HW2
short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}

void setVert(GzPointer *valueList, vector<Vertex>& vert)
{
	GzCoord* coordPointer = (GzCoord*)(valueList[0]);
	Vertex v;
	vert.clear();
	//store the x, y, z value of three vertexs using vector
	for (int i = 0; i < 3; i++)
	{
		v.x = (*(coordPointer + i))[0];
		v.y = (*(coordPointer + i))[1];
		v.z = (*(coordPointer + i))[2];
		vert.push_back(v);	
	}
}

void setNorm(GzPointer *valueList, vector<Vertex>& vert)
{
	GzCoord* normalPointer = (GzCoord*)(valueList[1]);
	Vertex n;
	vert.clear();
	//store the x, y, z normal of three vertexs using vector
	for (int i = 0; i < 3; i++)
	{
		n.nx = (*(normalPointer + i))[0];
		n.ny = (*(normalPointer + i))[1];
		n.nz = (*(normalPointer + i))[2];
		vert.push_back(n);	
	}
}

void setTexture(GzPointer *valueList, vector<Vertex>& vert)
{
	GzTextureIndex* texturePointer = (GzTextureIndex*)(valueList[2]);
	Vertex v;
	vert.clear();
	//store the x, y, z value of three vertexs using vector
	for (int i = 0; i < 3; i++)
	{
		v.tx = (*(texturePointer + i))[0];
		v.ty = (*(texturePointer + i))[1];
		vert.push_back(v);	
	}
}

void assignPos(vector<Vertex>& vertPos, vector<Vertex>& vert)
{
	for (int i = 0; i < 3; i++)
	{
		vert[i].x = vertPos[i].x;
		vert[i].y = vertPos[i].y;
		vert[i].z = vertPos[i].z;
	}
}

void assignNorm(vector<Vertex>& vertNorm, vector<Vertex>& vert)
{
	for (int i = 0; i < 3; i++)
	{
		vert[i].nx = vertNorm[i].nx;
		vert[i].ny = vertNorm[i].ny;
		vert[i].nz = vertNorm[i].nz;
	}
}

void assignTexture(vector<Vertex>& vertTex, vector<Vertex>& vert)
{
	for (int i = 0; i < 3; i++)
	{
		vert[i].tx = vertTex[i].tx;
		vert[i].ty = vertTex[i].ty;
	}
}

//sort vertex by y value (v0, v1, v2 from top to bottom)
void sortVertsByY(vector<Vertex>& vert)
{
	sort(vert.begin(), vert.end(), Vertex::SortByY);
}

//define the shape of the triangle
void setupTri(GzRender *render, vector<Edge>& edge, vector<Vertex>& vert, int &oriented) // using oriented value to record triangle shape
{
	GzColor color;
	//shading calculation
	if (render->interp_mode == GZ_COLOR)
	{
		//get color for all three vertexes, using normal vector to represent colors
		for (int i = 0; i < 3; i++)
		{
			getColor(render, vert[i], color);
			//record color using nx, ny, and nz
			vert[i].nx = color[0];
			vert[i].ny = color[1];
			vert[i].nz = color[2];
		}
	}
	else if (render->interp_mode == GZ_NORMALS)
	{
		//do nothing, keep nx, ny, nz as the normal
	}
	//texture calculation
	warp2Perspective(vert);
	//shape calculation
	if (vert[0].y == vert[1].y) //inverted triangle, v2 is lower than v0 and v1
	{
		edge[0].Assign(vert[0], vert[1]);
		edge[1].Assign(vert[1], vert[2]);
		edge[2].Assign(vert[0], vert[2]);
		oriented = 0;
	}
	else if (vert[1].y == vert[2].y) //regular triangle, v0 is higher than v1 and v2
	{
		edge[0].Assign(vert[0], vert[1]);
		edge[1].Assign(vert[1], vert[2]);
		edge[2].Assign(vert[0], vert[2]);
		oriented = 1;
	}
	else //L or R triangle
	{
		edge[0].Assign(vert[0], vert[1]);
		edge[1].Assign(vert[1], vert[2]);
		edge[2].Assign(vert[0], vert[2]);
		float slopex0 = edge[0].SlopeX(edge[0].start, edge[0].end);
		float slopex2 = edge[2].SlopeX(edge[2].start, edge[2].end);
		//calculate slope to determine L or R
		if (slopex0 < slopex2) //L triangle
		{
			oriented = 2;
		}
		else if (slopex0 > slopex2) //R triangle
		{
			oriented = 3;
		}
	}
}

Vertex CoordToVertex(const GzCoord& coord)
{
    return Vertex(coord);
}

void scanLineRast(GzRender *render, vector<Edge>& edge, vector<Vertex>& vert, int &oriented)
{
	float deltaY = ceil(vert[0].y) - vert[0].y;
	float deltaX = 0;
	Span spanLine;
	int i = 0;
	int j = 0;
	GzIntensity red;
	GzIntensity green; 
	GzIntensity blue;
	GzIntensity alpha;
	GzDepth fbZ;
	GzColor color;
	GzTextureIndex tex = {0, 0};
	//setup the slope for scan line
	float slopex0 = edge[0].SlopeX(edge[0].start, edge[0].end);
	float slopex1 = edge[1].SlopeX(edge[1].start, edge[1].end);
	float slopex2 = edge[2].SlopeX(edge[2].start, edge[2].end);

	float slopez0 = edge[0].SlopeZ(edge[0].start, edge[0].end);
	float slopez1 = edge[1].SlopeZ(edge[1].start, edge[1].end);
	float slopez2 = edge[2].SlopeZ(edge[2].start, edge[2].end);
	
	float slopez = 0;
	//initialize the edge cursor position on edge0
	edge[0].edgeCursor.x = edge[0].start.x + slopex0 * deltaY;
	edge[0].edgeCursor.y = edge[0].start.y + deltaY;
	edge[0].edgeCursor.z = edge[0].start.z + slopez0 * deltaY;
	//initialize the edge cursor position on edge2
	edge[2].edgeCursor.x = edge[2].start.x + slopex2 * deltaY;
	edge[2].edgeCursor.y = edge[2].start.y + deltaY;
	edge[2].edgeCursor.z = edge[2].start.z + slopez2 * deltaY;
	//color slope
	//setup the slope for color
	float slopecR0 = edge[0].SlopeCR(edge[0].start, edge[0].end); //red color change on edge0
	float slopecR1 = edge[1].SlopeCR(edge[1].start, edge[1].end); //red color change on edge1
	float slopecR2 = edge[2].SlopeCR(edge[2].start, edge[2].end); //red color change on edge2

	float slopecG0 = edge[0].SlopeCG(edge[0].start, edge[0].end); //green color change on edge0
	float slopecG1 = edge[1].SlopeCG(edge[1].start, edge[1].end); //green color change on edge1
	float slopecG2 = edge[2].SlopeCG(edge[2].start, edge[2].end); //green color change on edge2

	float slopecB0 = edge[0].SlopeCB(edge[0].start, edge[0].end); //blue color change on edge0
	float slopecB1 = edge[1].SlopeCB(edge[1].start, edge[1].end); //blue color change on edge1
	float slopecB2 = edge[2].SlopeCB(edge[2].start, edge[2].end); //blue color change on edge2
	
	float slopezR;
	float slopezG;
	float slopezB;
	//initialize the edge cursor color on edge0
	edge[0].edgeCursor.nx = edge[0].start.nx + slopecR0 * deltaY;
	edge[0].edgeCursor.ny = edge[0].start.ny + slopecG0 * deltaY;
	edge[0].edgeCursor.nz = edge[0].start.nz + slopecB0 * deltaY;
	//initialize the edge cursor color on edge2
	edge[2].edgeCursor.nx = edge[2].start.nx + slopecR2 * deltaY;
	edge[2].edgeCursor.ny = edge[2].start.ny + slopecG2 * deltaY;
	edge[2].edgeCursor.nz = edge[2].start.nz + slopecB2 * deltaY;
	//texture slope
	//setup the slope for color
	float slopetx0 = edge[0].SlopeTx(edge[0].start, edge[0].end);
	float slopetx1 = edge[1].SlopeTx(edge[1].start, edge[1].end);
	float slopetx2 = edge[2].SlopeTx(edge[2].start, edge[2].end);

	float slopety0 = edge[0].SlopeTy(edge[0].start, edge[0].end);	
	float slopety1 = edge[1].SlopeTy(edge[1].start, edge[1].end);
	float slopety2 = edge[2].SlopeTy(edge[2].start, edge[2].end);
	
	float slopezTx;
	float slopezTy;
	//initialize the edge cursor texture on edge0
	edge[0].edgeCursor.tx = edge[0].start.tx + slopetx0 * deltaY;
	edge[0].edgeCursor.ty = edge[0].start.ty + slopety0 * deltaY;
	//initialize the edge cursor texture on edge2
	edge[2].edgeCursor.tx = edge[2].start.tx + slopetx2 * deltaY;
	edge[2].edgeCursor.ty = edge[2].start.ty + slopety2 * deltaY;

	//scan the first part of the triangle
	while (edge[0].edgeCursor.y < edge[0].end.y)
	{
		//setup span DDA for various shapes
		if (oriented == 2 || oriented == 1) // L or regular triangle	
		{
			//set span cursor position
			spanLine.Assign(edge[0].edgeCursor, edge[2].edgeCursor);
			deltaX = ceil(edge[0].edgeCursor.x) - edge[0].edgeCursor.x;
			spanLine.spanCursor.x = edge[0].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[0].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[0].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.z = edge[0].edgeCursor.z + deltaX * slopez;
			//set span cursor color
			slopezR = spanLine.SlopeZR(edge[0].edgeCursor, edge[2].edgeCursor);
			slopezG = spanLine.SlopeZG(edge[0].edgeCursor, edge[2].edgeCursor);
			slopezB = spanLine.SlopeZB(edge[0].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.nx = edge[0].edgeCursor.nx + deltaX * slopezR;
			spanLine.spanCursor.ny = edge[0].edgeCursor.ny + deltaX * slopezG;
			spanLine.spanCursor.nz = edge[0].edgeCursor.nz + deltaX * slopezB;
			//set span cursor texture
			slopezTx = spanLine.SlopeZtx(edge[0].edgeCursor, edge[2].edgeCursor);
			slopezTy = spanLine.SlopeZty(edge[0].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.tx = edge[0].edgeCursor.tx + deltaX * slopezTx;
			spanLine.spanCursor.ty = edge[0].edgeCursor.ty + deltaX * slopezTy;
		} 
		else if (oriented == 3) // R triangle
		{
			//set span cursor position
			spanLine.Assign(edge[2].edgeCursor, edge[0].edgeCursor);
			deltaX = ceil(edge[2].edgeCursor.x) - edge[2].edgeCursor.x;
			spanLine.spanCursor.x = edge[2].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[2].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[2].edgeCursor, edge[0].edgeCursor);
			spanLine.spanCursor.z = edge[2].edgeCursor.z + deltaX * slopez;
			//set span cursor color
			slopezR = spanLine.SlopeZR(edge[2].edgeCursor, edge[0].edgeCursor);
			slopezG = spanLine.SlopeZG(edge[2].edgeCursor, edge[0].edgeCursor);
			slopezB = spanLine.SlopeZB(edge[2].edgeCursor, edge[0].edgeCursor);
			spanLine.spanCursor.nx = edge[2].edgeCursor.nx + deltaX * slopezR;
			spanLine.spanCursor.ny = edge[2].edgeCursor.ny + deltaX * slopezG;
			spanLine.spanCursor.nz = edge[2].edgeCursor.nz + deltaX * slopezB;
			//set span cursor texture
			slopezTx = spanLine.SlopeZtx(edge[2].edgeCursor, edge[0].edgeCursor);
			slopezTy = spanLine.SlopeZty(edge[2].edgeCursor, edge[0].edgeCursor);
			spanLine.spanCursor.tx = edge[2].edgeCursor.tx + deltaX * slopezTx;
			spanLine.spanCursor.ty = edge[2].edgeCursor.ty + deltaX * slopezTy;
		}
		//scan one horizontal line
		while (((oriented == 2 || oriented == 1) && spanLine.spanCursor.x < edge[2].edgeCursor.x) || (oriented == 3 && spanLine.spanCursor.x < edge[0].edgeCursor.x))
		{
			i = (int)spanLine.spanCursor.x;
			j = (int)edge[0].edgeCursor.y;
			//check if it is behind the camera
			if (spanLine.spanCursor.z < 0) //discard triangles which located behind the camera
			{
				continue;
			}
			GzGetDisplay(render->display, i, j, &red, &green, &blue, &alpha, &fbZ);
			//test interpolated-Z against FB-Z for each pixel - low Z wins
			if (spanLine.spanCursor.z < fbZ) 
			{
				if (render->interp_mode == GZ_COLOR)
				{
					unwarp(spanLine.spanCursor, tex);
					render->tex_fun(tex[0], tex[1], color);
					color[0] = color[0] * spanLine.spanCursor.nx;
					color[1] = color[1] * spanLine.spanCursor.ny;
					color[2] = color[2] * spanLine.spanCursor.nz;
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)spanLine.spanCursor.z);
				}
				else if (render->interp_mode == GZ_NORMALS)
				{
					unwarp(spanLine.spanCursor, tex);
					render->tex_fun(tex[0], tex[1], color);
					memcpy(&render->Ka, color, sizeof(GzColor));
					memcpy(&render->Kd, color, sizeof(GzColor));
					normalizeNormal(spanLine.spanCursor);
					getColor(render, spanLine.spanCursor, color);
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)spanLine.spanCursor.z);
				}
				else
				{
					GzPutDisplay(render->display, i, j, ctoi(render->flatcolor[0]), ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), 0, (GzDepth)spanLine.spanCursor.z);
				}
			}
			//x++
			spanLine.spanCursor.x++;
			spanLine.spanCursor.z = spanLine.spanCursor.z + slopez;
			//span color++
			spanLine.spanCursor.nx = spanLine.spanCursor.nx + slopezR;
			spanLine.spanCursor.ny = spanLine.spanCursor.ny + slopezG;
			spanLine.spanCursor.nz = spanLine.spanCursor.nz + slopezB;
			//span texture++
			spanLine.spanCursor.tx = spanLine.spanCursor.tx + slopezTx;
			spanLine.spanCursor.ty = spanLine.spanCursor.ty + slopezTy;
		}
		//y++
		edge[0].edgeCursor.x = edge[0].edgeCursor.x + slopex0;
		edge[0].edgeCursor.y++;
		edge[0].edgeCursor.z = edge[0].edgeCursor.z + slopez0;
		edge[2].edgeCursor.x = edge[2].edgeCursor.x + slopex2;
		edge[2].edgeCursor.y++;
		edge[2].edgeCursor.z = edge[2].edgeCursor.z + slopez2;
		//edge color++
		edge[0].edgeCursor.nx = edge[0].edgeCursor.nx + slopecR0;
		edge[0].edgeCursor.ny = edge[0].edgeCursor.ny + slopecG0;
		edge[0].edgeCursor.nz = edge[0].edgeCursor.nz + slopecB0;
		edge[2].edgeCursor.nx = edge[2].edgeCursor.nx + slopecR2;
		edge[2].edgeCursor.ny = edge[2].edgeCursor.ny + slopecG2;
		edge[2].edgeCursor.nz = edge[2].edgeCursor.nz + slopecB2;
		//edge texture++
		edge[0].edgeCursor.tx = edge[0].edgeCursor.tx + slopetx0;
		edge[0].edgeCursor.ty = edge[0].edgeCursor.ty + slopety0;
		edge[2].edgeCursor.tx = edge[2].edgeCursor.tx + slopetx2;
		edge[2].edgeCursor.ty = edge[2].edgeCursor.ty + slopety2;
	}
	
	deltaY = ceil(vert[1].y) - vert[1].y;
	//initialize the edge cursor position on edge1
	edge[1].edgeCursor.x = edge[1].start.x + slopex1 * deltaY;
	edge[1].edgeCursor.y = edge[1].start.y + deltaY;
	edge[1].edgeCursor.z = edge[1].start.z + slopez1 * deltaY;
	//initialize the edge cursor color on edge1
	edge[1].edgeCursor.nx = edge[1].start.nx + slopecR1 * deltaY;
	edge[1].edgeCursor.ny = edge[1].start.ny + slopecG1 * deltaY;
	edge[1].edgeCursor.nz = edge[1].start.nz + slopecB1 * deltaY;
	//initialize the edge cursor texture on edge1
	edge[1].edgeCursor.tx = edge[1].start.tx + slopetx1 * deltaY;
	edge[1].edgeCursor.ty = edge[1].start.ty + slopety1 * deltaY;

	//scan the second part of the triangle
	while (edge[1].edgeCursor.y < edge[1].end.y)
	{
		//setup span DDA for various shapes
		if (oriented == 2) // L triangle	
		{
			//set span cursor pos
			spanLine.Assign(edge[1].edgeCursor, edge[2].edgeCursor);
			deltaX = ceil(edge[1].edgeCursor.x) - edge[1].edgeCursor.x;
			spanLine.spanCursor.x = edge[1].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[1].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[1].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.z = edge[1].edgeCursor.z + deltaX * slopez;
			//set span cursor color
			slopezR = spanLine.SlopeZR(edge[1].edgeCursor, edge[2].edgeCursor);
			slopezG = spanLine.SlopeZG(edge[1].edgeCursor, edge[2].edgeCursor);
			slopezB = spanLine.SlopeZB(edge[1].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.nx = edge[1].edgeCursor.nx + deltaX * slopezR;
			spanLine.spanCursor.ny = edge[1].edgeCursor.ny + deltaX * slopezG;
			spanLine.spanCursor.nz = edge[1].edgeCursor.nz + deltaX * slopezB;
			//set span cursor texture
			slopezTx = spanLine.SlopeZtx(edge[1].edgeCursor, edge[2].edgeCursor);
			slopezTy = spanLine.SlopeZty(edge[1].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.tx = edge[1].edgeCursor.tx + deltaX * slopezTx;
			spanLine.spanCursor.ty = edge[1].edgeCursor.ty + deltaX * slopezTy;
		} 
		else if (oriented == 3 || oriented == 0) // R or inverted triangle
		{
			//set span cursor pos
			spanLine.Assign(edge[2].edgeCursor, edge[1].edgeCursor);
			deltaX = ceil(edge[2].edgeCursor.x) - edge[2].edgeCursor.x;
			spanLine.spanCursor.x = edge[2].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[2].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[2].edgeCursor, edge[1].edgeCursor);
			spanLine.spanCursor.z = edge[2].edgeCursor.z + deltaX * slopez;
			//set span cursor color
			slopezR = spanLine.SlopeZR(edge[2].edgeCursor, edge[1].edgeCursor);
			slopezG = spanLine.SlopeZG(edge[2].edgeCursor, edge[1].edgeCursor);
			slopezB = spanLine.SlopeZB(edge[2].edgeCursor, edge[1].edgeCursor);
			spanLine.spanCursor.nx = edge[2].edgeCursor.nx + deltaX * slopezR;
			spanLine.spanCursor.ny = edge[2].edgeCursor.ny + deltaX * slopezG;
			spanLine.spanCursor.nz = edge[2].edgeCursor.nz + deltaX * slopezB;
			//set span cursor texture
			slopezTx = spanLine.SlopeZtx(edge[2].edgeCursor, edge[1].edgeCursor);
			slopezTy = spanLine.SlopeZty(edge[2].edgeCursor, edge[1].edgeCursor);
			spanLine.spanCursor.tx = edge[2].edgeCursor.tx + deltaX * slopezTx;
			spanLine.spanCursor.ty = edge[2].edgeCursor.ty + deltaX * slopezTy;
		}
		//scan one horizontal line
		while ((oriented == 2 && spanLine.spanCursor.x < edge[2].edgeCursor.x) || ((oriented == 3 || oriented == 0) && spanLine.spanCursor.x < edge[1].edgeCursor.x))
		{
			i = (int)spanLine.spanCursor.x;
			j = (int)edge[1].edgeCursor.y;
			//check if it is behind the camera
			if (spanLine.spanCursor.z < 0) //discard triangles which located behind the camera
			{
				continue;
			}
			
			GzGetDisplay(render->display, i, j, &red, &green, &blue, &alpha, &fbZ);
			//test interpolated-Z against FB-Z for each pixel - low Z wins
			if (spanLine.spanCursor.z < fbZ) 
			{
				if (render->interp_mode == GZ_COLOR)
				{
					unwarp(spanLine.spanCursor, tex);
					render->tex_fun(tex[0], tex[1], color);
					color[0] = color[0] * spanLine.spanCursor.nx;
					color[1] = color[1] * spanLine.spanCursor.ny;
					color[2] = color[2] * spanLine.spanCursor.nz;
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)spanLine.spanCursor.z);
				}
				else if (render->interp_mode == GZ_NORMALS)
				{
					unwarp(spanLine.spanCursor, tex);
					render->tex_fun(tex[0], tex[1], color);
					memcpy(&render->Ka, color, sizeof(GzColor));
					memcpy(&render->Kd, color, sizeof(GzColor));
					normalizeNormal(spanLine.spanCursor);
					getColor(render, spanLine.spanCursor, color);
					normalizeNormal(spanLine.spanCursor);
					getColor(render, spanLine.spanCursor, color);
					GzPutDisplay(render->display, i, j, ctoi(color[0]), ctoi(color[1]), ctoi(color[2]), 0, (GzDepth)spanLine.spanCursor.z);
				}
				else
				{
					GzPutDisplay(render->display, i, j, ctoi(render->flatcolor[0]), ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), 0, (GzDepth)spanLine.spanCursor.z);
				}
			}
			// x++
			spanLine.spanCursor.x++;
			spanLine.spanCursor.z = spanLine.spanCursor.z + slopez;
			//span color++
			spanLine.spanCursor.nx = spanLine.spanCursor.nx + slopezR;
			spanLine.spanCursor.ny = spanLine.spanCursor.ny + slopezG;
			spanLine.spanCursor.nz = spanLine.spanCursor.nz + slopezB;
			//span texture++
			spanLine.spanCursor.tx = spanLine.spanCursor.tx + slopezTx;
			spanLine.spanCursor.ty = spanLine.spanCursor.ty + slopezTy;
		}
		//y++
		edge[1].edgeCursor.x = edge[1].edgeCursor.x + slopex1;
		edge[1].edgeCursor.y++;
		edge[1].edgeCursor.z = edge[1].edgeCursor.z + slopez1;
		edge[2].edgeCursor.x = edge[2].edgeCursor.x + slopex2;
		edge[2].edgeCursor.y++;
		edge[2].edgeCursor.z = edge[2].edgeCursor.z + slopez2;
		//edge color++
		edge[1].edgeCursor.nx = edge[1].edgeCursor.nx + slopecR1;
		edge[1].edgeCursor.ny = edge[1].edgeCursor.ny + slopecG1;
		edge[1].edgeCursor.nz = edge[1].edgeCursor.nz + slopecB1;
		edge[2].edgeCursor.nx = edge[2].edgeCursor.nx + slopecR2;
		edge[2].edgeCursor.ny = edge[2].edgeCursor.ny + slopecG2;
		edge[2].edgeCursor.nz = edge[2].edgeCursor.nz + slopecB2;
		//edge texture++
		edge[1].edgeCursor.tx = edge[1].edgeCursor.tx + slopetx1;
		edge[1].edgeCursor.ty = edge[1].edgeCursor.ty + slopety1;
		edge[2].edgeCursor.tx = edge[2].edgeCursor.tx + slopetx2;
		edge[2].edgeCursor.ty = edge[2].edgeCursor.ty + slopety2;
	}
}

//HW3
//trans degree to radian
float degree2Radian(float degree)
{
	return (float)(degree / 180) * PI;
}

//matrix multiplication calculation
void matrixMul(const GzMatrix matrix_top, const GzMatrix matrix_new, GzMatrix matrix_out)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j =  0; j < 4; j++)
		{
			matrix_out[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j =  0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				matrix_out[i][j] = matrix_out[i][j] + matrix_top[i][k] * matrix_new[k][j];
			}
		}
	}
}

//set Xsp
int setXsp(GzRender *render)
{
//x/2,  0,     0,       x/2
//0,    -y/2,  0,       y/2
//0,    0,     Zmax/d,  0
//0,    0,     0,       1
	float d;
	float radian;
	radian = degree2Radian(render->camera.FOV);
	d = 1 / (tan(radian / 2));
	render->Xsp[0][0] = render->display->xres / 2.0;
	render->Xsp[0][1] = 0;
	render->Xsp[0][2] = 0;
	render->Xsp[0][3] = render->display->xres / 2.0;
	
	render->Xsp[1][0] = 0;
	render->Xsp[1][1] = -render->display->yres / 2.0;
	render->Xsp[1][2] = 0;
	render->Xsp[1][3] = render->display->yres / 2.0;

	render->Xsp[2][0] = 0;
	render->Xsp[2][1] = 0;
	render->Xsp[2][2] = INT_MAX / d;
	render->Xsp[2][3] = 0;

	render->Xsp[3][0] = 0;
	render->Xsp[3][1] = 0;
	render->Xsp[3][2] = 0;
	render->Xsp[3][3] = 1;
	return GZ_SUCCESS;
}

//set Xpi
int setXpi(GzRender *render)
{
//1,  0,   0,    0
//0,  1,   0,    0
//0,  0,   1,    0
//0,  0,   1/d,  1
	float d;
	float radian;
	radian = degree2Radian(render->camera.FOV);
	d = 1 / (tan(radian / 2));
	render->camera.Xpi[0][0] = 1;
	render->camera.Xpi[1][1] = 1;
	render->camera.Xpi[2][2] = 1;
	render->camera.Xpi[3][3] = 1;
	render->camera.Xpi[3][2] = 1 / d;
	return GZ_SUCCESS;
}

Vertex normalize(const Vertex& v)
{
	float sum = v.x * v.x + v.y * v.y + v.z * v.z;
	float absoluteValue = sqrt(sum);
	Vertex vector;
	vector.x = v.x / absoluteValue;
	vector.y = v.y / absoluteValue;
	vector.z = v.z / absoluteValue;
	return vector;
}

void normalizeCoord(GzCoord& v)
{
	float sum = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	float absoluteValue = sqrt(sum);
	v[0] = v[0] / absoluteValue;
	v[1] = v[1] / absoluteValue;
	v[2] = v[2] / absoluteValue;
}

void normalizeNormal(Vertex& v)
{
	float sum = v.nx * v.nx + v.ny * v.ny + v.nz * v.nz;
	float absoluteValue = sqrt(sum);
	v.nx = v.nx / absoluteValue;
	v.ny = v.ny / absoluteValue;
	v.nz = v.nz / absoluteValue;
}

float dotProduct(const Vertex& a, const Vertex& b)
{
	float dp = a.x * b.x + a.y * b.y + a.z * b.z;
	return dp;
}

Vertex crossProduct(const Vertex& a, const Vertex& b)
{
	Vertex c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

//set Xiw
int setXiw(GzRender *render)
{
//Xx,  Xy,   Xz,   -XdotC
//Yx,  Yy, 	 Yz,   -YdotC
//Zx,  Zy,   Zz,   -ZdotC
//0,   0,    0,    1
	Vertex cl;
	Vertex cz;//Z
	float dp;
	Vertex up;
	Vertex cUp;
	Vertex cy;//Y
	Vertex cx;//X
	Vertex camera;//camera
	//camera Z-axis
	cl.x = render->camera.lookat[0] - render->camera.position[0];
	cl.y = render->camera.lookat[1] - render->camera.position[1];
	cl.z = render->camera.lookat[2] - render->camera.position[2];
	cz = normalize(cl);
	//up vector 
	up.x = render->camera.worldup[0];
	up.y = render->camera.worldup[1];
	up.z = render->camera.worldup[2];
	dp = dotProduct(up, cz);
	//camera Y-axis
	cUp.x = up.x - dp * cz.x;
	cUp.y = up.y - dp * cz.y;
	cUp.z = up.z - dp * cz.z;
	cy = normalize(cUp);
	//camera X-axis
	cx = crossProduct(cy, cz);
	//camera location
	camera.x = render->camera.position[0];
	camera.y = render->camera.position[1];
	camera.z = render->camera.position[2];
	
	render->camera.Xiw[0][0] = cx.x;
	render->camera.Xiw[0][1] = cx.y;
	render->camera.Xiw[0][2] = cx.z;
	render->camera.Xiw[0][3] = -dotProduct(cx, camera);

	render->camera.Xiw[1][0] = cy.x;
	render->camera.Xiw[1][1] = cy.y;
	render->camera.Xiw[1][2] = cy.z;
	render->camera.Xiw[1][3] = -dotProduct(cy, camera);

	render->camera.Xiw[2][0] = cz.x;
	render->camera.Xiw[2][1] = cz.y;
	render->camera.Xiw[2][2] = cz.z;
	render->camera.Xiw[2][3] = -dotProduct(cz, camera);

	render->camera.Xiw[3][0] = 0;
	render->camera.Xiw[3][1] = 0;
	render->camera.Xiw[3][2] = 0;
	render->camera.Xiw[3][3] = 1;

	return GZ_SUCCESS;
}

//Xform positions of verts  
void transVert(vector<Vertex>& vertW, GzMatrix matrix, vector<Vertex>& vertS)
{
	float outVect[4][3];
    float transVect[4][3];

	for (int i = 0; i < 3; i++)
	{
		transVect[0][i] = vertW[i].x;
		transVect[1][i] = vertW[i].y;
		transVect[2][i] = vertW[i].z;
		transVect[3][i] = 1;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			outVect[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				outVect[i][j] = outVect[i][j] + matrix[i][k] * transVect[k][j];
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		vertS[i].x = outVect[0][i] / outVect[3][i];
		vertS[i].y = outVect[1][i] / outVect[3][i];
		vertS[i].z = outVect[2][i] / outVect[3][i];
	}
}

//Xform normal of verts  
void transNorm(vector<Vertex>& vertW, GzMatrix matrix, vector<Vertex>& vertS)
{
	float outVect[4][3];
    float transVect[4][3];

	for (int i = 0; i < 3; i++)
	{
		transVect[0][i] = vertW[i].nx;
		transVect[1][i] = vertW[i].ny;
		transVect[2][i] = vertW[i].nz;
		transVect[3][i] = 1;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			outVect[i][j] = 0;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				outVect[i][j] = outVect[i][j] + matrix[i][k] * transVect[k][j];
			}
		}
	}

	for (int i = 0; i < 3; i++)
	{
		vertS[i].nx = outVect[0][i] / outVect[3][i];
		vertS[i].ny = outVect[1][i] / outVect[3][i];
		vertS[i].nz = outVect[2][i] / outVect[3][i];
	}
}

//check the triangle
int checkTri(GzDisplay *display, vector<Vertex>& v)
{
	int d_f = 0;
	int v_n = 0;
	for (int i = 0; i < 3; i++)
	{
		if (v[i].z < 0) //check if this triangle has vertex behind the camera
		{
			d_f = 1;
			break;
		}
		if ((v[i].x < 0 || v[i].x > display->xres) || (v[i].y < 0 || v[i].y > display->yres)) //check if this triangle is out of the screen
		{
			v_n++;
		}
	}
	if (v_n == 3) //all three verts are out of the screen
	{
		d_f = 1;
	}
	return d_f;
}

int getColor(GzRender *render, const Vertex v, GzColor color)
{
	Vertex reflect;
	Vertex eye;
	Vertex normal;
	Vertex lightDir;
	Vertex lightClr;
	Vertex sumS;
	Vertex sumD;
	float NL;
	float NE;
	float RE;

	//preparation
	eye.x = 0;
	eye.y = 0;
	eye.z = -1;

	sumS.x = 0;
	sumS.y = 0;
	sumS.z = 0;
	
	sumD.x = 0;
	sumD.y = 0;
	sumD.z = 0;
	
	normal.x = v.nx;
	normal.y = v.ny;
	normal.z = v.nz;

	for (int i = 0; i < render->numlights; i++)
	{
		lightDir.x = render->lights[i].direction[0];
		lightDir.y = render->lights[i].direction[1];
		lightDir.z = render->lights[i].direction[2];
		lightClr.x = render->lights[i].color[0];
		lightClr.y = render->lights[i].color[1];
		lightClr.z = render->lights[i].color[2];

		NL = dotProduct(lightDir, normal);
		NE = dotProduct(eye, normal);
		//check light and eye direction
		if (NL * NE < 0)
		{
			continue;
		}
		else if (NL * NE > 0 && NL < 0)
		{
			//flip normal
			normal.x = -normal.x;
			normal.y = -normal.y;
			normal.z = -normal.z;
			NL = -NL;
		}
		sumD.x = sumD.x + lightClr.x * NL;
		sumD.y = sumD.y + lightClr.y * NL;
		sumD.z = sumD.z + lightClr.z * NL;
		//R = 2(NL)N - L
		reflect.x = 2 * NL * normal.x - lightDir.x;
		reflect.y = 2 * NL * normal.y - lightDir.y;
		reflect.z = 2 * NL * normal.z - lightDir.z;
		reflect = normalize(reflect);
		RE = dotProduct(reflect, eye);
		//clamped RE to maintain [0, 1] bounded range
		if (RE < 0)
		{
			RE = 0;
		}
		else if (RE > 1)
		{
			RE = 1;
		}
		RE = pow(RE, render->spec);
		sumS.x = sumS.x + lightClr.x * RE;
		sumS.y = sumS.y + lightClr.y * RE;
		sumS.z = sumS.z + lightClr.z * RE;
	}
	//HW4
	//color[0] = render->Ks[0] * sumS.x + render->Kd[0] * sumD.x + render->Ka[0] * render->ambientlight.color[0];
	//color[1] = render->Ks[1] * sumS.y + render->Kd[1] * sumD.y + render->Ka[1] * render->ambientlight.color[1];
	//color[2] = render->Ks[2] * sumS.z + render->Kd[2] * sumD.z + render->Ka[2] * render->ambientlight.color[2];
	//HW5 dealing with texture
	//Gouraud shading
	if (render->interp_mode == GZ_COLOR)
	{
		color[0] = sumS.x + sumD.x + render->ambientlight.color[0];
		color[1] = sumS.y + sumD.y + render->ambientlight.color[1];
		color[2] = sumS.z + sumD.z + render->ambientlight.color[2];
	}
	//Phong shading
	if (render->interp_mode == GZ_NORMALS)
	{
		color[0] = render->Ks[0] * sumS.x + render->Kd[0] * sumD.x + render->Ka[0] * render->ambientlight.color[0];
		color[1] = render->Ks[1] * sumS.y + render->Kd[1] * sumD.y + render->Ka[1] * render->ambientlight.color[1];
		color[2] = render->Ks[2] * sumS.z + render->Kd[2] * sumD.z + render->Ka[2] * render->ambientlight.color[2];
	}
	//check color
	if (color[0] > 1)
	{
		color[0] = 1;
	}
	if (color[1] > 1)
	{
		color[1] = 1;
	}
	if (color[2] > 1)
	{
		color[2] = 1;
	}
	if (color[0] < 0)
	{
		color[0] = 0;
	}
	if (color[1] < 0)
	{
		color[1] = 0;
	}
	if (color[2] < 0)
	{
		color[2] = 0;
	}
	return GZ_SUCCESS;
}

//Transforming (warping) vertex parameter values to perspective space 
void warp2Perspective(vector<Vertex> &v)
{
	float vz;
	for(int i = 0; i < 3; i++)
	{
		vz = v[i].z / (INT_MAX - v[i].z);
		v[i].tx = v[i].tx / (vz + 1);
		v[i].ty = v[i].ty / (vz + 1);
	}
}

//Transforming (unwarping) interpolated pixel values back to affine space 
void unwarp(Vertex &v, GzTextureIndex tex)
{
	float vz;
	vz = v.z / (INT_MAX - v.z);
	tex[0] = v.tx * (vz + 1);
	tex[1] = v.ty * (vz + 1);
}
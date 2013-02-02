/*   CS580 HW   */
// 3D_Graphics_&_Rendering_HW3.cpp:
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
void setupTri(vector<Edge>& edge, vector<Vertex>& vert, int &oriented);
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
			}
		}
	}
	else //there is matrix in the stack
	{
		matrixMul(render->Ximage[render->matlevel-1], matrix, render->Ximage[render->matlevel]);
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
			GzColor* colorPointer = (GzColor*)(valueList[0]);
			render->flatcolor[0] = (*colorPointer)[0];
			render->flatcolor[1] = (*colorPointer)[1];
			render->flatcolor[2] = (*colorPointer)[2];
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
	for (int i = 0; i < numParts; i++)
	{
		//set renderer attribute states
		if (nameList[i] == GZ_POSITION)
		{
			//GzCoord* coordPointer = (GzCoord*)(valueList[0]);
			vector<Vertex> v(3);
			vector<Vertex> vert(3);
			vector<Edge> edge(3);
			int oriented = 0;

			//set Vertex
			setVert(valueList, vert);
			//transform vertex from world space to screen space
			transVert(vert, render->Ximage[render->matlevel-1], v);
			//check the position of the triangles, and discard the useless triangle
			if ((checkTri(render->display, v)) == 1)
			{
				continue;
			}
			//sort Vertex
			sortVertsByY(v);
			setupTri(edge, v, oriented);
			scanLineRast(render, edge, v, oriented);
		}
		else if (nameList[i] == GZ_NULL_TOKEN)
		{
			//do nothing
		}
	}
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

//sort vertex by y value (v0, v1, v2 from top to bottom)
void sortVertsByY(vector<Vertex>& vert)
{
	sort(vert.begin(), vert.end(), Vertex::SortByY);
}

//define the shape of the triangle
void setupTri(vector<Edge>& edge, vector<Vertex>& vert, int &oriented) // using oriented value to record triangle shape
{
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
	//setup the slope for scan line
	float slopex0 = edge[0].SlopeX(edge[0].start, edge[0].end);
	float slopex1 = edge[1].SlopeX(edge[1].start, edge[1].end);
	float slopex2 = edge[2].SlopeX(edge[2].start, edge[2].end);
	float slopez0 = edge[0].SlopeZ(edge[0].start, edge[0].end);
	float slopez1 = edge[1].SlopeZ(edge[1].start, edge[1].end);
	float slopez2 = edge[2].SlopeZ(edge[2].start, edge[2].end);
	float slopez = 0;
	//initialize the cursor on edge0
	edge[0].edgeCursor.x = edge[0].start.x + slopex0 * deltaY;
	edge[0].edgeCursor.y = edge[0].start.y + deltaY;
	edge[0].edgeCursor.z = edge[0].start.z + slopez0 * deltaY;
	//initialize the cursor on edge2
	edge[2].edgeCursor.x = edge[2].start.x + slopex2 * deltaY;
	edge[2].edgeCursor.y = edge[2].start.y + deltaY;
	edge[2].edgeCursor.z = edge[2].start.z + slopez2 * deltaY;

	//scan the first part of the triangle
	while (edge[0].edgeCursor.y < edge[0].end.y)
	{
		//setup span DDA for various shapes
		if (oriented == 2 || oriented == 1) // L or regular triangle	
		{
			spanLine.Assign(edge[0].edgeCursor, edge[2].edgeCursor);
			deltaX = ceil(edge[0].edgeCursor.x) - edge[0].edgeCursor.x;
			spanLine.spanCursor.x = edge[0].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[0].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[0].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.z = edge[0].edgeCursor.z + deltaX * slopez;
		} 
		else if (oriented == 3) // R triangle
		{
			spanLine.Assign(edge[2].edgeCursor, edge[0].edgeCursor);
			deltaX = ceil(edge[2].edgeCursor.x) - edge[2].edgeCursor.x;
			spanLine.spanCursor.x = edge[2].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[2].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[2].edgeCursor, edge[0].edgeCursor);
			spanLine.spanCursor.z = edge[2].edgeCursor.z + deltaX * slopez;
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
				GzPutDisplay(render->display, i, j, ctoi(render->flatcolor[0]), ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), 0, (GzDepth)spanLine.spanCursor.z);
			}
			
			// x++
			spanLine.spanCursor.x++;
			spanLine.spanCursor.z = spanLine.spanCursor.z + slopez;
		}
		//y++
		edge[0].edgeCursor.x = edge[0].edgeCursor.x + slopex0;
		edge[0].edgeCursor.y++;
		edge[0].edgeCursor.z = edge[0].edgeCursor.z + slopez0;
		edge[2].edgeCursor.x = edge[2].edgeCursor.x + slopex2;
		edge[2].edgeCursor.y++;
		edge[2].edgeCursor.z = edge[2].edgeCursor.z + slopez2;
	}
	
	deltaY = ceil(vert[1].y) - vert[1].y;
	//initialize the cursor on edge1
	edge[1].edgeCursor.x = edge[1].start.x + slopex1 * deltaY;
	edge[1].edgeCursor.y = edge[1].start.y + deltaY;
	edge[1].edgeCursor.z = edge[1].start.z + slopez1 * deltaY;

	//scan the second part of the triangle
	while (edge[1].edgeCursor.y < edge[1].end.y)
	{
		//setup span DDA for various shapes
		if (oriented == 2) // L triangle	
		{
			spanLine.Assign(edge[1].edgeCursor, edge[2].edgeCursor);
			deltaX = ceil(edge[1].edgeCursor.x) - edge[1].edgeCursor.x;
			spanLine.spanCursor.x = edge[1].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[1].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[1].edgeCursor, edge[2].edgeCursor);
			spanLine.spanCursor.z = edge[1].edgeCursor.z + deltaX * slopez;
		} 
		else if (oriented == 3 || oriented == 0) // R or inverted triangle
		{
			spanLine.Assign(edge[2].edgeCursor, edge[1].edgeCursor);
			deltaX = ceil(edge[2].edgeCursor.x) - edge[2].edgeCursor.x;
			spanLine.spanCursor.x = edge[2].edgeCursor.x + deltaX;
			spanLine.spanCursor.y = edge[2].edgeCursor.y;
			slopez = spanLine.SlopeZ(edge[2].edgeCursor, edge[1].edgeCursor);
			spanLine.spanCursor.z = edge[2].edgeCursor.z + deltaX * slopez;
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
				GzPutDisplay(render->display, i, j, ctoi(render->flatcolor[0]), ctoi(render->flatcolor[1]), ctoi(render->flatcolor[2]), 0, (GzDepth)spanLine.spanCursor.z);
			}
			
			// x++
			spanLine.spanCursor.x++;
			spanLine.spanCursor.z = spanLine.spanCursor.z + slopez;
		}
		//y++
		edge[1].edgeCursor.x = edge[1].edgeCursor.x + slopex1;
		edge[1].edgeCursor.y++;
		edge[1].edgeCursor.z = edge[1].edgeCursor.z + slopez1;
		edge[2].edgeCursor.x = edge[2].edgeCursor.x + slopex2;
		edge[2].edgeCursor.y++;
		edge[2].edgeCursor.z = edge[2].edgeCursor.z + slopez2;
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
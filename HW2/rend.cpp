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

using namespace std;

//additional function declaration 
void sortVertsByY(GzPointer *valueList, vector<Vertex>& vert);
void setupTri(vector<Edge>& edge, vector<Vertex>& vert, int &oriented);
void scanLineRast(GzRender *render, vector<Edge>& edge, vector<Vertex>& vert, int &oriented);
short	ctoi(float color);


int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display)
{
/* 
- malloc a renderer struct
- keep closed until BeginRender inits are done
- span interpolator needs pointer to display for pixel writes
- check for legal class GZ_Z_BUFFER_RENDER
*/
	*render = new GzRender;
	if (*render == NULL) 
	{
		return GZ_FAILURE;
	}
	(*render)->renderClass = renderClass;
	(*render)->display = display;
	//initialize the render flag as close
	(*render)->open = 0;

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


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/
	render->open = 1;
	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/
	//iterative set various attributes
	for (int i = 0; i < numAttributes; i++)
	{
		//set renderer attribute states
		if(nameList[i] == GZ_RGB_COLOR)
		{
			GzColor* colorPointer = (GzColor*)(valueList[0]);
			render->flatcolor[0] = (*colorPointer)[0];
			render->flatcolor[1] = (*colorPointer)[1];
			render->flatcolor[2] = (*colorPointer)[2];
		}
	}
	return GZ_SUCCESS;
}


int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
- Invoke the scan converter and return an error code
*/
	for (int i = 0; i < numParts; i++)
	{
		//set renderer attribute states
		if (nameList[i] == GZ_POSITION)
		{
			
			GzCoord* coordPointer = (GzCoord*)(valueList[0]);
			//create a vector to store the sort result
			vector<Vertex> vert(3);
			vector<Edge> edge(3);
			sortVertsByY(valueList, vert);
			int oriented = 0;
			setupTri(edge, vert, oriented);
			scanLineRast(render, edge, vert, oriented);
		}
		else if (nameList[i] == GZ_NULL_TOKEN)
		{
			//do nothing
		}
	}
	return GZ_SUCCESS;
}

/* NOT part of API - just for general assistance */
//sort vertex by y value (v0, v1, v2 from top to bottom)
void sortVertsByY(GzPointer *valueList, vector<Vertex>& vert)
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
	//sort vertex by y value (v0, v1, v2 from top to bottom)
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
			if (fbZ == 0 || spanLine.spanCursor.z < fbZ) 
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
			if (fbZ == 0 || spanLine.spanCursor.z < fbZ) 
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

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}


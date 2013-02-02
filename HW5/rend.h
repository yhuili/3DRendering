#include "disp.h" /* include your own disp.h file (e.g. hw1)*/

/* Camera defaults */
#define	DEFAULT_FOV		35.0
#define	DEFAULT_IM_Z	(-10.0)  /* world coords for image plane origin */
#define	DEFAULT_IM_Y	(5.0)    /* default look-at point = 0,0,0 */
#define	DEFAULT_IM_X	(-10.0)

#define	DEFAULT_AMBIENT	{0.1, 0.1, 0.1}
#define	DEFAULT_DIFFUSE	{0.7, 0.6, 0.5}
#define	DEFAULT_SPECULAR	{0.2, 0.3, 0.4}
#define	DEFAULT_SPEC		32

#define	MATLEVELS	100		/* how many matrix pushes allowed */
#define	MAX_LIGHTS	10		/* how many lights allowed */



/* Dummy definition : change it later */
//#ifndef GzLight
//#define GzLight		GzPointer
//#endif

//#ifndef GzTexture
//#define GzTexture	GzPointer
//#endif

#ifndef GZRENDER
#define GZRENDER
typedef struct {			/* define a renderer */
  GzRenderClass	renderClass;
  GzDisplay		*display;
  short		    open;
  GzCamera		camera;
  short		    matlevel;	        /* top of stack - current xform */
  GzMatrix		Ximage[MATLEVELS];	/* stack of xforms (Xsm) */ //used in transform
  GzMatrix		Xnorm[MATLEVELS];	/* xforms for norms (Xim) */ //used in shading
  GzMatrix		Xsp;		        /* NDC to screen (pers-to-screen) */
  GzColor		flatcolor;          /* color state for flat shaded triangles */
  int			interp_mode;
  int			numlights;
  GzLight		lights[MAX_LIGHTS];
  GzLight		ambientlight;
  GzColor		Ka, Kd, Ks;
  float		    spec;		/* specular power */
  GzTexture		tex_fun;    /* tex_fun(float u, float v, GzColor color) */
}  GzRender;
#endif

// Function declaration
// HW2
int GzNewRender(GzRender **render, GzRenderClass renderClass, GzDisplay *display);
int GzFreeRender(GzRender *render);
int GzBeginRender(GzRender	*render);
int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList);
int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList);

// HW3
int GzPutCamera(GzRender *render, GzCamera *camera);
int GzPushMatrix(GzRender *render, GzMatrix	matrix);
int GzPopMatrix(GzRender *render);

// Object Translation
int GzRotXMat(float degree, GzMatrix mat);
int GzRotYMat(float degree, GzMatrix mat);
int GzRotZMat(float degree, GzMatrix mat);
int GzTrxMat(GzCoord translate, GzMatrix mat);
int GzScaleMat(GzCoord scale, GzMatrix mat);


//define a Vertex class that including x, y, and z
#ifndef VERTEX
#define VERTEX
class Vertex
{
public:
	//position vector
	float x; 
	float y; 
	float z;
	//normal/color vector
	float nx;
	float ny;
	float nz;
	//texture vector
	float tx;
	float ty;
	//color vector

	Vertex() {};
    Vertex(const GzCoord& vertex)
    {
        x = vertex[0];
        y = vertex[1];
        z = vertex[2];
    }

	static bool SortByY(const Vertex& v1, const Vertex& v2)
    {
        if (v1.y == v2.y)
		{
            return v1.x < v2.x;
		}
        return v1.y < v2.y;
    }
};
#endif

//define a Edge class that including start vertex and end vertex
#ifndef EDGE
#define EDGE
class Edge
{
public:
	Vertex start;
	Vertex end;
	Vertex edgeCursor;
	
	Edge() {};
	float SlopeX(const Vertex &start, const Vertex &end)
	{
		return (end.x - start.x)/(end.y - start.y);
	}
	float SlopeZ(const Vertex &start, const Vertex &end)
	{
		return (end.z - start.z)/(end.y - start.y);
	}
	float SlopeCR(const Vertex &start, const Vertex &end)
	{
		return (end.nx - start.nx)/(end.y - start.y);
	}
	float SlopeCG(const Vertex &start, const Vertex &end)
	{
		return (end.ny - start.ny)/(end.y - start.y);
	}
	float SlopeCB(const Vertex &start, const Vertex &end)
	{
		return (end.nz - start.nz)/(end.y - start.y);
	}
	float SlopeTx(const Vertex &start, const Vertex &end)
	{
		return (end.tx - start.tx)/(end.y - start.y);
	}
	float SlopeTy(const Vertex &start, const Vertex &end)
	{
		return (end.ty - start.ty)/(end.y - start.y);
	}
	void Assign(const Vertex& s, const Vertex& e) 
	{
		start = s;
		end = e;
	}
	void interpolateEdge(){}
};
#endif

//define a Span class that including start vertex and end vertex
#ifndef SPAN
#define SPAN
class Span
{
public:
	Vertex start;
	Vertex end;
	Vertex spanCursor;
	
	Span() {};
	float SlopeZ(const Vertex &start, const Vertex &end)
	{
		return (end.z - start.z)/(end.x - start.x);
	}
	float SlopeZR(const Vertex &start, const Vertex &end)
	{
		return (end.nx - start.nx)/(end.x - start.x);
	}
	float SlopeZG(const Vertex &start, const Vertex &end)
	{
		return (end.ny - start.ny)/(end.x - start.x);
	}
	float SlopeZB(const Vertex &start, const Vertex &end)
	{
		return (end.nz - start.nz)/(end.x - start.x);
	}
	float SlopeZtx(const Vertex &start, const Vertex &end)
	{
		return (end.tx - start.tx)/(end.x - start.x);
	}
	float SlopeZty(const Vertex &start, const Vertex &end)
	{
		return (end.ty - start.ty)/(end.x - start.x);
	}
	void Assign(const Vertex& s, const Vertex& e) 
	{
		start = s;
		end = e;
	}
	void interpolateSpan(){}
};
#endif
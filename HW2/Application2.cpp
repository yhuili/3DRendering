// Application2.cpp: implementation of the Application2 class.
//
//////////////////////////////////////////////////////////////////////

/* application code for homework assignment #2
 * CSCI580, CS Dept., USC 
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE2  "pot4.screen.asc"
#define OUTFILE2 "output.ppm"

/* 
	NOTE: the application does not include disp.h.
	Keep your defs in disp.h private.
*/

#include "stdio.h"
#include "Gz.h"
#include "disp.h"
#include "rend.h"

void shade2(GzCoord norm, GzCoord	color);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application2::Application2()
{

}

Application2::~Application2()
{

}

/*
	- CS580 HW
	- This is the routine to call your functions
	- Check the code and modify it if needed.
*/


int Application2::Render() 
{
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 		/* vertex attribute pointers */
	GzToken         nameListColor[3];       /* color type names */
	GzPointer       valueListColor[3];      /* color type rgb pointers */
	GzColor		color; 
	GzCoord		vertexList[3];	/* vertex position coordinates */ 
	GzCoord		normalList[3];	/* vertex normals */ 
	GzTextureIndex	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int		i; 
	int		xRes, yRes, dispClass;	/* display parameters */ 
	int		status; 
 
	status = 0; 

	/* 
	 * initialize the display and the renderer 
	 */ 

	m_nWidth = 256;		// frame buffer and display width
	m_nHeight = 256;    // frame buffer and display height

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);
	status |= GzNewDisplay(&m_pDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight); 
 
	status |= GzGetDisplayParams(m_pDisplay, &xRes, &yRes, &dispClass); 
	 
	status |= GzInitDisplay(m_pDisplay); 
 
	status |= GzNewRender(&m_pRender, GZ_Z_BUFFER_RENDER, m_pDisplay); 

	status |= GzBeginRender(m_pRender); 

	if (status) exit(GZ_FAILURE); 

	/* 
	 * Tokens associated with triangle vertex values 
	 */ 
	nameListTriangle[0] = GZ_POSITION; /* define vert coordinates only */

	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE2 , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE2 , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and pass vert info to render/scan convert each triangle 
	*/ 
	i = 0; 

	while( fscanf(infile, "%s", dummy) == 1) { 	/* read in tri word */
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[0][0]), &(vertexList[0][1]),  
		&(vertexList[0][2]), 
		&(normalList[0][0]), &(normalList[0][1]), 	
		&(normalList[0][2]), 
		&(uvList[0][0]), &(uvList[0][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[1][0]), &(vertexList[1][1]), 	
		&(vertexList[1][2]), 
		&(normalList[1][0]), &(normalList[1][1]), 	
		&(normalList[1][2]), 
		&(uvList[1][0]), &(uvList[1][1]) ); 
	    fscanf(infile, "%f %f %f %f %f %f %f %f", 
		&(vertexList[2][0]), &(vertexList[2][1]), 	
		&(vertexList[2][2]), 
		&(normalList[2][0]), &(normalList[2][1]), 	
		&(normalList[2][2]), 
		&(uvList[2][0]), &(uvList[2][1]) ); 
		//the first point x1, y1, z1, the second point x2, y2, z2, the third point x3, y3, z3
	    /* 
	    * Set up shading attributes for each triangle 
	    */ 
	    shade2(normalList[0], color);/* shade based on the norm of vert0 */
	    valueListColor[0] = (GzPointer)color; 
	    nameListColor[0] = GZ_RGB_COLOR; 
	    GzPutAttribute(m_pRender, 1, nameListColor, valueListColor); 
 
	    /* 
	     * Set the value pointers to the first vertex of the 	
	     * triangle, then feed it to the renderer 
	     */ 
	     valueListTriangle[0] = (GzPointer)vertexList; 


	     GzPutTriangle(m_pRender, 1, nameListTriangle, valueListTriangle);
	} 

	GzFlushDisplay2File(outfile, m_pDisplay); 	/* write out or update display to file*/
	GzFlushDisplay2FrameBuffer(m_pFrameBuffer, m_pDisplay);	// write out or update display to frame buffer
 
	/* 
	 * Clean up and exit 
	 */ 

	if( fclose( infile ) )
      AfxMessageBox( "The input file was not closed\n" );

	if( fclose( outfile ) )
      AfxMessageBox( "The output file was not closed\n" );

	status |= GzFreeRender(m_pRender); 
	status |= GzFreeDisplay(m_pDisplay); 
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
} 


/* 
This doesn't really belong in the application program, but for this 
simplified case of a renderer that doesn't do any shading itself, this 
is the easiest place to put it.
*/

void shade2(GzCoord norm, GzCoord	color)
{
  GzCoord	light;
  float		coef;

  light[0] = 0.707f;
  light[1] = 0.5f;
  light[2] = 0.5f;

  coef = light[0]*norm[0] + light[1]*norm[1] + light[2]*norm[2];
  if (coef < 0) 	coef *= -1;

  if (coef > 1.0)	coef = 1.0;
  color[0] = coef*0.95f;
  color[1] = coef*0.65f;
  color[2] = coef*0.88f;
}




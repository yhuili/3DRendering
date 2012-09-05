/**
 * Test application class member functions for homework assignment #1
 * USC csci 580 *
*/

// Application1.cpp: implementation of the Application1 class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CS580HW.h"
#include "Application1.h"

#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* CS580 HW : I/O file definition */
#define INFILE1  "rects"
#define OUTFILE1 "output.ppm"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application1::Application1()
{
 
}

Application1::~Application1()
{

}

/*
	- CS580 HW
	- This is the routine to call your functions
	- Check the code and modify it if needed.
*/

int Application1::Render() 
{
	int		i, j; 
	int		xRes, yRes, dispClass;	/* display parameters */ 
	int		status; 
 	 
	status = 0; 
 
	/* 
	 * initialize the display and the renderer 
	 */ 

	m_nWidth = 512;		// frame buffer and display width
	m_nHeight = 512;    // frame buffer and display height

	status |= GzNewFrameBuffer(&m_pFrameBuffer, m_nWidth, m_nHeight);

	status |= GzNewDisplay(&m_pDisplay, GZ_RGBAZ_DISPLAY, m_nWidth, m_nHeight); 
 
	status |= GzGetDisplayParams(m_pDisplay, &xRes, &yRes, &dispClass); 

	status |= GzInitDisplay(m_pDisplay);  /* init for new frame */
 
	if (status) exit(GZ_FAILURE); 
 
	// I/O File open

	FILE *infile;
	if( (infile = fopen( INFILE1 , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile = fopen( OUTFILE1 , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}
	
	int	ulx, uly, lrx, lry, r, g, b;
	while( fscanf(infile, "%d %d %d %d %d %d %d", 
			&ulx, &uly, &lrx, &lry, &r, &g, &b) == 7) { 
		for (j = uly; j <= lry; j++) {
		  for (i = ulx; i <= lrx; i++) {
		    GzPutDisplay(m_pDisplay, i, j, r, g, b, 1, 0);
		  }
		}
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

	status |= GzFreeDisplay(m_pDisplay); 
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 

}

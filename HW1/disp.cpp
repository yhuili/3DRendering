/*   CS580 HW   */
// 3D_Graphics_&_Rendering_HW1.cpp:
// Name: Hui Li
#include    "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"

using namespace std;

int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* create a framebuffer:
 -- allocate memory for framebuffer : (sizeof)GzPixel x width x height
 -- pass back pointer 
*/
	//3 bytes for R,G,B
	//allocate memory and call constructor
	*framebuffer = new char [sizeof(char) * 3 * width * height];
	if (*framebuffer == NULL) 
	{
		return GZ_FAILURE;
	}
	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay** display, GzDisplayClass dispClass, int xRes, int yRes)
{

/* create a display:
  -- allocate memory for indicated class and resolution
  -- pass back pointer to GzDisplay object in display
*/
	//allocate memory and call constructor
	*display = new GzDisplay;
	//check boundaries, then initialize width and height resolution for display
	if (xRes > MAXXRES) 
	{ 
		(*display)->xres = MAXXRES;
	}
	else if (xRes < 0)
	{
		(*display)->xres = 0;
	}
	else 
	{
		(*display)->xres = xRes;
	}

	if (yRes > MAXYRES)
	{
		(*display)->yres = MAXYRES;
	}
	else if (yRes < 0)
	{
		(*display)->yres = 0;
	}
	else
	{
		(*display)->yres = yRes;
	}

	//initialize RGBAZ for display
	(*display)->dispClass = dispClass;

	//initialize the display flag as close
	(*display)->open = 0;
	
	//allocate memory to the display framebuffer
	(*display)->fbuf = new GzPixel [((*display)->xres) * ((*display)->yres)];
	if (((*display)->fbuf) == NULL) 
	{	
		return GZ_FAILURE;
	}

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* clean up, free memory */
	//free memory
	delete[] display->fbuf;
	delete display;
	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes, GzDisplayClass	*dispClass)
{
/* pass back values for an open display */
	if (display == NULL)
	{
		return GZ_FAILURE;
	}
	*xRes = display->xres;
	*yRes = display->yres;
	*dispClass = display->dispClass;
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* set everything to some default values - start a new frame */
	if (display == NULL)
	{
		return GZ_FAILURE;
	}
	//raster scan the canvas, set r, g, b to 3935(light grey) as the background, and set aplha to 1, z to 0
	for (int i = 0; i < display->yres; i++)
	{
		for (int j = 0; j < display->xres; j++)
		{
            display->fbuf[ARRAY(j,i)].red = 3935;
            display->fbuf[ARRAY(j,i)].green = 3935;
            display->fbuf[ARRAY(j,i)].blue = 3935;
            display->fbuf[ARRAY(j,i)].alpha = 1;
			display->fbuf[ARRAY(j,i)].z = 0;
        }
    }

	//set the display flag as open
	display->open = 1;
	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* write pixel values into the display */
	if (display == NULL)
	{
		return GZ_FAILURE;
	}
	if (display->xres >= i && display->yres >= j && i >= 0 && j >= 0)
	{
		//check intensity of color red
		if (r > MAXINTENSITY)
		{
			display->fbuf[ARRAY(i,j)].red = MAXINTENSITY;
		}
		else if (r < 0)
		{
			display->fbuf[ARRAY(i,j)].red = 0;
		}
		else
		{
			display->fbuf[ARRAY(i,j)].red = r;
		}

		//check intensity of color green
		if (g > MAXINTENSITY)
		{
			display->fbuf[ARRAY(i,j)].green = MAXINTENSITY;
		}
		else if (g < 0)
		{
			display->fbuf[ARRAY(i,j)].green = 0;
		}
		else
		{
			display->fbuf[ARRAY(i,j)].green = g;
		}

		//check intensity of color blue
		if (b > MAXINTENSITY)
		{
			display->fbuf[ARRAY(i,j)].blue = MAXINTENSITY;
		}
		else if (b < 0)
		{
			display->fbuf[ARRAY(i,j)].blue = 0;
		}
		else
		{
			display->fbuf[ARRAY(i,j)].blue = b;
		}

		display->fbuf[ARRAY(i,j)].alpha = a;
		display->fbuf[ARRAY(i,j)].z = z;
		return GZ_SUCCESS;
	}
	else 
		return GZ_FAILURE;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	/* pass back pixel value in the display */
	/* check display class to see what vars are valid */
	if (display->xres >= i && display->yres >= j && i >= 0 && j >= 0)
	{
		*r = display->fbuf[ARRAY(i,j)].red;
		*g = display->fbuf[ARRAY(i,j)].green;
		*b = display->fbuf[ARRAY(i,j)].blue;
		*a = display->fbuf[ARRAY(i,j)].alpha;
		*z = display->fbuf[ARRAY(i,j)].z;
		return GZ_SUCCESS;
	}
	else
		return GZ_FAILURE;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{

	/* write pixels to ppm file based on display class -- "P6 %d %d 255\r" */
	//write to ascii header as the format of P6 %d %d 255\r
	fprintf(outfile, "P6 %d %d 255\r", display->xres, display->yres);
	//write rgb to each pixel as the format of rgbrgbrgbrgb...
	for (int i = 0; i < display->yres; i++)
	{
		for (int j = 0; j < display->xres; j++)
		{
			//store the result of right shift 4 as char
            char ppmRed = display->fbuf[ARRAY(j,i)].red >> 4;
            char ppmGreen = display->fbuf[ARRAY(j,i)].green >> 4;
            char ppmBlue = display->fbuf[ARRAY(j,i)].blue >> 4;
            fprintf(outfile, "%c%c%c", ppmRed, ppmGreen, ppmBlue);
        }
    }
	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{

	/* write pixels to framebuffer: 
		- Put the pixels into the frame buffer
		- Caution: store the pixel to the frame buffer as the order of blue, green, and red 
		- Not red, green, and blue !!!
	*/
	for (int i = 0; i < display->yres; i++)
	{
		for (int j = 0; j < display->xres; j++)
		{
			char fRed = display->fbuf[ARRAY(j,i)].red >> 4;
            char fGreen = display->fbuf[ARRAY(j,i)].green >> 4;
            char fBlue = display->fbuf[ARRAY(j,i)].blue >> 4;
			framebuffer[ARRAY(j,i) * 3] = fBlue;   
            framebuffer[ARRAY(j,i) * 3 + 1] = fGreen;   
            framebuffer[ARRAY(j,i) * 3 + 2] = fRed;
        }
    }
	return GZ_SUCCESS;
}
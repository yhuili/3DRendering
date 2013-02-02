/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"
#include "math.h"

GzColor	*image;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
      }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
  if (u > 1)
  {
	  u = 1;
  }
  if (u < 0)
  {
	  u = 0;
  }
  if (v > 1)
  {
	  v = 1;
  }
  if (v < 0)
  {
	  v = 0;
  }
/* determine texture cell corner values and perform bilinear interpolation */
  float px, py;
  float s, t;
  px = u * (xs - 1);
  py = v * (ys - 1);
  s = px - floor(px);
  t = py - floor(py);
/* set color to interpolated GzColor value and return */
  GzColor colorA;
  GzColor colorB;
  GzColor colorC;
  GzColor colorD;
  //color at A
  for (i = 0; i < 3; i++)
  {
	  colorA[i] = image[xs * (int)floor(py) + (int)floor(px)][i];
  }
  //color at B
  for (i = 0; i < 3; i++)
  {
	  colorB[i] = image[xs * (int)floor(py) + (int)ceil(px)][i];
  }
  //color at C
  for (i = 0; i < 3; i++)
  {
	  colorC[i] = image[xs * (int)ceil(py) + (int)ceil(px)][i];
  }
  //color at D
  for (i = 0; i < 3; i++)
  {
	  colorD[i] = image[xs * (int)ceil(py) + (int)floor(px)][i];
  }
  //color at P
  for (i = 0; i < 3; i++)
  {
	  color[i] = s * t * colorC[i] + (1 - s) * t * colorD[i] + s * (1 - t) * colorB[i] + (1 - s) * (1 - t) * colorA[i]; 
  }
  return GZ_SUCCESS;
}

class complexNumber
{
public:
	float r; 
	float i;
	complexNumber() {};
	~complexNumber() {};
};

/* Procedural texture function */
//Fractal texture: Julia Set
int ptex_fun(float u, float v, GzColor color)
{
	int N = 150;
	complexNumber x;
	complexNumber c;
	//C
	c.r = -0.624;
	c.i = 0.435;
	//X = u + vi
	x.r = 2*u-0.7;
    x.i = v-0.9;
	int i;
	float clr;
    for(i = 0; i < N; i++) 
	{
		//X ^ 2 + C
		//(XrYr - XiYi)r + Cr
        float xr = x.r * x.r - x.i * x.i + c.r;
		//(XrYi + XiYr)i + Ci
        float xi = x.r * x.i + x.i * x.r + c.i;
		if ((x.r * x.r + x.i * x.i) > 4.0)
		{
			break;
		}
        x.r = xr;
        x.i = xi;
    }
	float lengthX = sqrt(x.r * x.r + x.i * x.i);
	if (i == N)
	{
		color[0] = lengthX / 2;
		color[1] = lengthX / 2;
		color[2] = lengthX / 2;
	}
	else
	{
		clr = ((float)i) / N;
		color[0] = clr * 10;
		color[1] = clr * 8;
		color[2] = clr * 10;
	}
    return GZ_SUCCESS;
}




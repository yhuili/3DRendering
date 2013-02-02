Name: Hui Li
ID: 3194819503
Email Address: hli997@usc.edu
VS Version: Microsoft Visual Studio 2010 Professional Version 10.0.40219.1 SP1Rel

Some Instructions:
1. Please add disp.h, disp.cpp, rend.h, and rend.cpp to corresponding files.
2. There are four additional functions to help finish the process of shading, they are:

void setTexture(GzPointer *valueList, vector<Vertex>& vert); //set the texture, and store in the vert vector
void assignTexture(vector<Vertex>& vertTex, vector<Vertex>& vert); //assign u,v texture to vert
void warp2Perspective(vector<Vertex> &v); //Transforming (warping) vertex parameter values to perspective space 
void unwarp(Vertex &v, GzTextureIndex tex); //Transforming (unwarping) interpolated pixel values back to affine space 

3. In tex_fun.cpp, I define my customized ptex_fun() using Julia Set, my C = -0.624 + 0.435i, and X = 2 * u - 0.7 + (v - 0.9)i.
4. I have prepared the line valueListShader[5] = (GzPointer)(ptex_fun);	/* use ptex_fun */ with comment, you can just use it.
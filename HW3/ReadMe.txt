Name: Hui Li
Email Address: hli997@usc.edu
VS Version: Microsoft Visual Studio 2010 Professional Version 10.0.40219.1 SP1Rel

Some Instructions:
1. Please add both disp.h and disp.cpp to corresponding files.
2. There are ten additional function to help finish the process of transform, they are:

float degree2Radian(float degree);                                                         //transform degree to radian
void matrixMul(const GzMatrix matrix_top, const GzMatrix matrix_new, GzMatrix matrix_out); //matrix multiplication calculation
int setXsp(GzRender *render);                                                              //set Xsp matrix
int setXpi(GzRender *render);                                                              //set Xpi matrix
int setXiw(GzRender *render);                                                              //set Xiw matrix
Vertex normalize(const Vertex& v);                                                         //normalize the vector
float dotProduct(const Vertex& a, const Vertex& b);                                        //dot prodcut calculation, the result is the degree
Vertex crossProduct(const Vertex& a, const Vertex& b);                                     //cross prodcut calculation, the result is the normal

void transVert(vector<Vertex>& vertW, GzMatrix matrix, vector<Vertex>& vertS);             //transform the vertex from world space to screen space

int checkTri(GzDisplay *display, vector<Vertex>& v);                                       //check the triangle



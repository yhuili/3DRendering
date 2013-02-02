Name: Hui Li
Email Address: hli997@usc.edu
VS Version: Microsoft Visual Studio 2010 Professional Version 10.0.40219.1 SP1Rel

Some Instructions:
1. Please add disp.h, disp.cpp, rend.h, and rend.cpp to corresponding files.
2. There are seven additional functions to help finish the process of shading, they are:

void normalizeCoord(GzCoord& v); //normalize the Coord
void normalizeNormal(Vertex& v); //normalize the Normal
void setNorm(GzPointer *valueList, vector<Vertex>& vert); //set the normal, and store in the vert vector
void transNorm(vector<Vertex>& vertW, GzMatrix matrix, vector<Vertex>& vertS); //transform the vertex from world space to screen space
void assignPos(vector<Vertex>& vertPos, vector<Vertex>& vert); //assign position to vert
void assignNorm(vector<Vertex>& vertNorm, vector<Vertex>& vert); //assign normal to vert
int getColor(GzRender *render, const Vertex v, GzColor color); //calculate the color based on the light

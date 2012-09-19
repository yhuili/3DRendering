Name: Hui Li
ID: 3194819503
Email Address: hli997@usc.edu
VS Version: Microsoft Visual Studio 2010 Professional Version 10.0.40219.1 SP1Rel

Some Instructions:
1. Please add both disp.h and disp.cpp to corresponding files.
2. There are three additional function to help finish this scan line algorithm, they are:

void sortVertsByY(GzPointer *valueList, vector<Vertex>& vert); //sort vertex by y value (v0, v1, v2 from top to bottom)

void setupTri(vector<Edge>& edge, vector<Vertex>& vert, int &oriented); //define the shape of the triangle

void scanLineRast(GzRender *render, vector<Edge>& edge, vector<Vertex>& vert, int &oriented); //implement scan line algorithm

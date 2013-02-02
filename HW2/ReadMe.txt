Some Instructions:
1. Please add both disp.h and disp.cpp to corresponding files.
2. There are three additional functions to help finish this scan line algorithm, they are:

void sortVertsByY(GzPointer *valueList, vector<Vertex>& vert); //sort vertex by y value (v0, v1, v2 from top to bottom)

void setupTri(vector<Edge>& edge, vector<Vertex>& vert, int &oriented); //define the shape of the triangle

void scanLineRast(GzRender *render, vector<Edge>& edge, vector<Vertex>& vert, int &oriented); //implement scan line algorithm

/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI (float) 3.14159265358979323846

#include <iterator>
#include <algorithm>
#include <vector>
using namespace std;

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
/* HW 3.1
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
*/
	float rad_degree = degree * (PI / 180);
	GzMatrix rotXtemp = {
		1.0,	0.0,	0.0,	0.0,
		0.0,	cos(rad_degree),	-1*sin(rad_degree),	0.0,
		0.0,	sin(rad_degree),	cos(rad_degree),	0.0,
		0.0,	0.0,	0.0,	1.0
	};
	//memcpy doesnt work in this case for some reason, manually copy everything
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = rotXtemp[i][j];
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
/* HW 3.2
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
*/
	float rad_degree = degree * (PI / 180);
	GzMatrix rotYtemp = {
		cos(rad_degree),	0.0,	sin(rad_degree),	0.0,
		0.0,	1.0,	0.0,	0.0,
		-1 * sin(rad_degree),	0.0,	cos(rad_degree),	0.0,
		0.0,	0.0,	0.0,	1.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = rotYtemp[i][j];
		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
/* HW 3.3
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
*/
	float rad_degree = degree * (PI / 180);
	GzMatrix rotZtemp = {
		cos(rad_degree),	-1 * sin(rad_degree),	0.0,	0.0,
		sin(rad_degree),	cos(rad_degree),	0.0,	0.0,
		0.0,	0.0,	1.0,	0.0,
		0.0,	0.0,	0.0,	1.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = rotZtemp[i][j];
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
/* HW 3.4
// Create translation matrix
// Pass back the matrix using mat value
*/
	GzMatrix transtemp = {
		1.0,	0.0,	0.0,	translate[0],
		0.0,	1.0,	0.0,	translate[1],
		0.0,	0.0,	1.0,	translate[2],
		0.0,	0.0,	0.0,	1.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = transtemp[i][j];
		}
	}

	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
/* HW 3.5
// Create scaling matrix
// Pass back the matrix using mat value
*/
	GzMatrix scaletemp = {
		scale[0],	0.0,	0.0,	0.0,
		0.0,	scale[1],	0.0,	0.0,
		0.0,	0.0,	scale[2],	0.0,
		0.0,	0.0,	0.0,	1.0
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = scaletemp[i][j];
		}
	}

	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = (unsigned short)max(0, min(xRes, MAXXRES));
	yres = (unsigned short)max(0, min(yRes, MAXYRES));

	framebuffer = new char[xres * yres * 3]; //3 bytes x width x height
	pixelbuffer = new GzPixel[xres * yres];

/* HW 3.6
- setup Xsp and anything only done once 
- init default camera 
*/ 
	matlevel = -1; // -1 for empty stack
	// Local temporary matrix that will be looped through for assignment
	GzMatrix	Xsp_temp =
	{
		xres/2,	0.0,	0.0,	xres/2,
		0.0,	-(yres/2),	0.0,	yres/2,
		0.0,	0.0,	MAXINT,	0.0,
		0.0,	0.0,	0.0,	1.0
	};
	// Actually assign values, C++ will not simple let you assign entire GzMatrix
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Xsp[i][j] = Xsp_temp[i][j];
		}
	}
	//Camera default FOV
	m_camera.FOV = DEFAULT_FOV;
	//Array assignment very painful in C++
	//https://stackoverflow.com/questions/5732798/c-array-assignment-of-multiple-values
	//Camera default position
	m_camera.position[0] = DEFAULT_IM_X;
	m_camera.position[1] = DEFAULT_IM_Y;
	m_camera.position[2] = DEFAULT_IM_Z;
	//Camera default look-at point = 0,0,0
	m_camera.lookat[0] = 0;
	m_camera.lookat[1] = 0;
	m_camera.lookat[2] = 0;
	//Camera default world-up = 0,1,0
	m_camera.worldup[0] = 0;
	m_camera.worldup[1] = 1;
	m_camera.worldup[2] = 0;


	/*
	GzMatrix	Xsp =
	{
		1.0,	0.0,	0.0,	0.0,
		0.0,	1.0,	0.0,	0.0,
		0.0,	0.0,	1.0,	0.0,
		0.0,	0.0,	0.0,	1.0
	};
	*/
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete[] framebuffer;
	delete[] pixelbuffer;
}

// Clamps a GzIntensity 0-4095
GzIntensity ClampColor(GzIntensity pixelvalue) {
	return max(0, min(pixelvalue, 4095));
}

// Rightshift 4 with fill zero and cast to char mentioned in lecture
char IntensitytoRGB(GzIntensity pixelvalue) {
	return (char)(pixelvalue >> 4);
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	GzPixel defaultpixel; //defaultpixel all max intensity for simplicity
	defaultpixel.red = 4095;
	defaultpixel.blue = 4095;
	defaultpixel.green = 4095;
	defaultpixel.z = MAXINT; //set default z-buffer to max int
	defaultpixel.alpha = 0;

	for (int i = 0; i < xres * yres; i++) {
		pixelbuffer[i] = defaultpixel;
	}

	for (int i = 0; i < xres * yres * 3; i++) {
		framebuffer[i] = IntensitytoRGB(4095); //blue
		i += 1;
		framebuffer[i] = IntensitytoRGB(4095); //green
		i += 1;
		framebuffer[i] = IntensitytoRGB(4095); //red
	}
	return GZ_SUCCESS;
}

// Returns magnitude of vector for normalization
double getMagnitude(GzCoord vec) {
	double sum = (double)(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	double mag = sqrt(sum);
	return mag;
}

// Returns dot product of Gzcoord x and y
double getDot(GzCoord x, GzCoord y) {
	double dot = (double)(x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
	return dot;
}

int GzRender::GzBeginRender()
{
/* HW 3.7 
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 


	//Create and set Xpi matrix (easier matrix first)
	//Convert FOV to radians
	double rad_FOV = m_camera.FOV * (double)(PI / 180);
	// Calculate 1/d = tan(FOV/2)
	double d_inv = tan(rad_FOV / 2);
	GzMatrix	Xpi =
	{
		1.0,	0.0,	0.0,	0.0,
		0.0,	1.0,	0.0,	0.0,
		0.0,	0.0,	d_inv,	0.0,
		0.0,	0.0,	d_inv,	1.0
	};
	//Manually set entries of camera Xpi since C++ is bad at it
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m_camera.Xpi[i][j] = Xpi[i][j];
		}
	}

	// Calculate lookat - position vector for Z calculation
	GzCoord cl = {m_camera.lookat[0]-m_camera.position[0]
		,m_camera.lookat[1] - m_camera.position[1]
		,m_camera.lookat[2] - m_camera.position[2]};
	double cl_mag = getMagnitude(cl);
	// Calculate camera Z axis (normalized position -> lookat vector)
	GzCoord cam_Z_axis = {cl[0]/cl_mag,cl[1]/cl_mag,cl[2]/cl_mag};
	
	// Calculate camera Y axis
	// Construct up' = up - (up dot Z) Z
	double up_dot_Z = getDot(m_camera.worldup, cam_Z_axis);
	// Seperated into x y and z components
	GzCoord up_p = { m_camera.worldup[0] - (up_dot_Z * cam_Z_axis[0])
		,m_camera.worldup[1] - (up_dot_Z * cam_Z_axis[1])
		,m_camera.worldup[2] - (up_dot_Z * cam_Z_axis[2])};
	// Normalize up prime for cam_Y_axis
	double up_p_mag = getMagnitude(up_p);
	GzCoord cam_Y_axis = { up_p[0] / up_p_mag, up_p[1] / up_p_mag, up_p[2] / up_p_mag };
	
	// Given camera Z and Y axis X = (Y x Z)
	// Calculate Cross Product
	GzCoord cam_X_axis = {
		(cam_Y_axis[1] * cam_Z_axis[2]) - (cam_Y_axis[2]*cam_Z_axis[1]), // YyZz - YzZy
		(cam_Y_axis[2] * cam_Z_axis[0]) - (cam_Y_axis[0]*cam_Z_axis[2]), // YzZx - YxZz
		(cam_Y_axis[0] * cam_Z_axis[1]) - (cam_Y_axis[1]*cam_Z_axis[0]) // YxZy - YyZx
	};

	// Finally build the Xiw matrix
	GzMatrix	Xiw =
	{
		cam_X_axis[0],	cam_X_axis[1],	cam_X_axis[2],	-1*getDot(cam_X_axis,m_camera.position),
		cam_Y_axis[0],	cam_Y_axis[1],	cam_Y_axis[2],	-1*getDot(cam_Y_axis,m_camera.position),
		cam_Z_axis[0],	cam_Z_axis[1],	cam_Z_axis[2],	-1*getDot(cam_Z_axis,m_camera.position),
		0.0,	0.0,	0.0,	1.0
	};
	/*Xiw = 
	{
		Xx, Xy, Xz, -X*C,
		Yx, Yy, Yz, -Y*C,
		Zx, Zy, Zx, -Z*C,
		0,	0,	0,	1,
	}
	*/
	//Manually set entries of camera Xiw since C++ is bad at it
	for (int i = 0; i < 4 ; i++) {
		for (int j = 0; j < 4; j++) {
			m_camera.Xiw[i][j] = Xiw[i][j];
		}
	}

	//Push 3 matrices to stack
	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw);
	
	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	m_camera.FOV = camera.FOV;

	//https://stackoverflow.com/questions/13054243/set-one-array-equal-to-another-without-a-loop
	memcpy(m_camera.lookat, camera.lookat, sizeof(m_camera.lookat));
	memcpy(m_camera.position, camera.position, sizeof(m_camera.position));
	memcpy(m_camera.worldup, camera.worldup, sizeof(m_camera.worldup));
	memcpy(m_camera.Xpi, camera.Xpi, sizeof(m_camera.Xpi));
	memcpy(m_camera.Xiw, camera.Xiw, sizeof(m_camera.Xiw));

	return GZ_SUCCESS;	
}


// Multiplies x and y matrices then stores result in result matrix
void matmul4_4(GzMatrix x, GzMatrix y, GzMatrix result) {
	for (int i = 0; i < 4; i++){
		//Loop through rows of x
		for (int j = 0; j < 4; j++) {
			//Loop through cols of y
			for (int k = 0; k < 4; k++) {
				// Loop through and add x[row][k] * y[k][col]
				result[i][j] += x[i][k] * y[k][j];
			}
		}
	}
}


int GzRender::GzPushMatrix(GzMatrix	matrix)
{
	/* HW 3.9
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/
	//If matrix is empty, add matrix
	//Otherwise multiply matrix to top of stack and add it to the stack
	//Increment matlevel if no stack overflow
	if (matlevel < MATLEVELS) {
		//check for stack size < 100
		if (matlevel == -1) {
			// If matrix is empty add matrix to level 0
			memcpy(Ximage[0], matrix, sizeof(Ximage[0]));
		}
		else {
			// Otherwise multiply matrix to top of stack (Ximage[matlevel])
			// Temporary result matrix to store result of multiplication
			GzMatrix result = {
				0.0,	0.0,	0.0,	0.0,
				0.0,	0.0,	0.0,	0.0,
				0.0,	0.0,	0.0,	0.0,
				0.0,	0.0,	0.0,	0.0
			};
			matmul4_4(Ximage[matlevel],matrix, result);
			//Example: Xsp is in top of the stack
			//Multiply new matrix (Xpi) to get Xsi = Xsp * Xpi
			//Xsi goes to top of stack
			memcpy(Ximage[matlevel + 1], result, sizeof(Ximage[matlevel + 1]));
		}
		matlevel += 1;
	}
	//Note that even translation comes in as 4x4 matrix
	
	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	//Decrement matlevel (if not empty)
	if (matlevel == -1) {
		matlevel -= 1;
	}
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	/* HW2 Modified this fuction to check if z < existing_pixel.z and that z >= 0 before putting*/
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		GzPixel existing_pixel = pixelbuffer[ARRAY(i, j)];
		GzPixel putpixel;
		putpixel.red = r;
		putpixel.green = g;
		putpixel.blue = b;
		putpixel.alpha = a;
		putpixel.z = z;
		if (putpixel.z < existing_pixel.z && putpixel.z >= 0) {
			pixelbuffer[ARRAY(i, j)] = putpixel;
		}
	}
	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		GzPixel getpixel = pixelbuffer[ARRAY(i, j)];
		*r = getpixel.red;
		*g = getpixel.green;
		*b = getpixel.blue;
		*a = getpixel.alpha;
		*z = getpixel.z;
	}
	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\r", xres, yres);
	int j = 0;
	for (int i = 0; i < xres * yres; i++) {
		// i loops through pixelbuffer
		// j loops through framebuffer
		GzPixel getpixel = pixelbuffer[i];
		GzIntensity clampblue = ClampColor(getpixel.blue);
		char blue = IntensitytoRGB(clampblue);
		j += 1;
		GzIntensity clampgreen = ClampColor(getpixel.green);
		char green = IntensitytoRGB(clampgreen);
		j += 1;
		GzIntensity clampred = ClampColor(getpixel.red);
		char red = IntensitytoRGB(clampred);
		j += 1;
		fprintf(outfile, "%c%c%c", red, green, blue);
	}
	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	int j = 0;
	for (int i = 0; i < xres * yres; i++) {
		// i loops through pixelbuffer
		// j loops through framebuffer
		GzPixel getpixel = pixelbuffer[i];
		GzIntensity clampblue = ClampColor(getpixel.blue);
		framebuffer[j] = IntensitytoRGB(clampblue);
		j += 1;
		GzIntensity clampgreen = ClampColor(getpixel.green);
		framebuffer[j] = IntensitytoRGB(clampgreen);
		j += 1;
		GzIntensity clampred = ClampColor(getpixel.red);
		framebuffer[j] = IntensitytoRGB(clampred);
		j += 1;

	}
	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/
	for (int i = 0; i < numAttributes; i++) {
		int token = nameList[i];
		switch (token) {
			//Note token == 99 to change color
		case GZ_RGB_COLOR:
			float* value = (float*)valueList[i];
			flatcolor[0] = value[0];
			flatcolor[1] = value[1];
			flatcolor[2] = value[2];

		}
	}
	return GZ_SUCCESS;
}

//Custom comparison function for sorting vectors referencing:
//https://www.geeksforgeeks.org/sorting-2d-vector-in-c-set-2-in-descending-order-by-row-and-column/
bool sorty(const vector<float>& v1,
	const vector<float>& v2) {
	return v1[1] < v2[1]; //Sort by smallest first
}

bool sortx(const vector<float>& v1,
	const vector<float>& v2) {
	return v1[0] < v2[0]; //Smallest x goes first
}

// Basic DDA class container that returns x and z values for a y scan line that moves down
// Given row y, return interpolated x and z based off of slope
class DDA {
public:
	vector<float> start;
	vector<float> end;
	float slopex; //dy/x
	float slopez; //dy/dz

	DDA(vector<float> in_start, vector<float> in_end) {
		start = in_start;
		end = in_end;
		slopex = (end[0] - start[0]) / (end[1] - start[1]); //(x2-x1)/(y2-y1)
		slopez = (end[2] - start[2]) / (end[1] - start[1]); //(z2-z1)/(y2-y1)
	}

	//Helper functions to get current 
	float getCurrentX_Y(float delta_y) {
		return start[0] + (slopex * delta_y);
	}

	float getCurrentZ_Y(float delta_y) {
		return start[2] + (slopez * delta_y);
	}

};

// Span DDA class that goes row by row and returns z values while looping through x values in a row
// Unwrap from vectors for simplicity (this is basicaly solely for calculating zbuffer)
class SpanDDA {
public:
	float start_x;
	float start_z;//NOTE: these z values should come from the top-down DDA getCurrentZ_Y
	float end_x;
	float end_z; //NOTE: these z values should come from the top-down DDA getCurrentZ_Y
	float slopez; //dz/dx

	SpanDDA(float s_x, float s_z, float e_x, float e_z) {
		start_x = s_x;
		start_z = s_z;
		end_x = e_x;
		end_z = e_z;
		slopez = (end_z - start_z) / (end_x - start_x); //(z2-z1)/(x2-x1)
	}

	float getCurrentZ_X(float delta_x) {
		return start_z + (slopez * delta_x);
	}
};

// 1. Converts vertex to 4D homogenous vector
// 2. Performs vector-matrix multiplication
// 3. Converts 4D output back to 3D for HW2 rendering
vector<float> vertexTransform(vector<float> vertex, GzMatrix transform_mat) {
	// Convert to 4D
	vector<float> hom_vert = vertex;
	hom_vert.insert(hom_vert.end(), 1.0);

	// Perform multiplication (result stored in transform_vert)
	vector<float> transform_vert = { 0,0,0,0 };
	for (int i = 0; i < 4; i++) {
		//Loop through rows of matrix
		for (int j = 0; j < 4; j++) {
			// Loop through cols of vertex
			transform_vert[i] += transform_mat[i][j] * hom_vert[j];
		}
	}

	// Convert back to 3D (divide x,y,z by w)
	vector<float> return_vec = {transform_vert[0]/transform_vert[3],transform_vert[1] / transform_vert[3],transform_vert[2] / transform_vert[3] };
	return return_vec;
}


int GzRender::GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	for (int i = 0; i < numParts; i++) {
		int token = nameList[i];
		switch (token) {
		case GZ_NULL_TOKEN:
			continue;
		case GZ_POSITION:
			GzCoord* vertexList = (GzCoord*)valueList[i]; //Pointer to 3 GzCoords


			//Convert to std c++ vectors for easy built in sorting
			vector<float> vertex1(begin(vertexList[0]), end(vertexList[0]));
			vector<float> vertex2(begin(vertexList[1]), end(vertexList[1]));
			vector<float> vertex3(begin(vertexList[2]), end(vertexList[2]));


			//Transform all 3 verticies using whatever matrix is at top of stack
			vector<float> transform1= vertexTransform(vertex1, Ximage[matlevel]);
			vector<float> transform2 = vertexTransform(vertex2, Ximage[matlevel]);
			vector<float> transform3 = vertexTransform(vertex3, Ximage[matlevel]);

			//Check if any z is less than zero and skip the whole triangle if true
			if (transform1[2] < 0 || transform2[2] < 0 || transform3[2] < 0) {
				continue;
			}

			//Vector of vectors (2D vector)
			vector<vector<float>> allvertex;
			allvertex.push_back(transform1);
			allvertex.push_back(transform2);
			allvertex.push_back(transform3);


			//1. Sort vertices by Y coordinate for DDA evaluation
			sort(allvertex.begin(), allvertex.end(), sorty);

			int start_row = ceil(allvertex[0][1]);
			int end_row = ceil(allvertex[2][1]);
			//Start and end rows for rasterization are the smallest and largest y vertex values (assuming this is sorted by y values)

			//2. Set up edge DDA objects 
			//1,2,3 correspond to lowest to highest y values (order top to bottom in image)
			DDA edge_1_2 = DDA(allvertex[0], allvertex[1]);
			DDA edge_2_3 = DDA(allvertex[1], allvertex[2]);
			DDA edge_1_3 = DDA(allvertex[0], allvertex[2]);


			//3. Determine which edges are left or right
			// Many edge cases for when vertices have equal values

			//Get slopes for top-bottom and top-middle edges
			float long_slope = edge_1_3.slopex;
			float short_slope = edge_1_2.slopex;

			// Either left is 1-2-3 or 1-3
			if (short_slope > long_slope) {
				//When short_slope > long_slope
				//the configuration is in L(1-3) R(1-2-3) (single triangle example)
				for (int j = start_row; j < end_row; j++) {
					float delta_y_1 = j - allvertex[0][1]; //startpoint for vertex 1 interpolation
					float delta_y_2 = j - allvertex[1][1]; //startpoint for vertex 2 interpolation

					float left_x = edge_1_3.getCurrentX_Y(delta_y_1);
					float left_z = edge_1_3.getCurrentZ_Y(delta_y_1);

					float right_x_1_2 = edge_1_2.getCurrentX_Y(delta_y_1);
					float right_x_2_3 = edge_2_3.getCurrentX_Y(delta_y_2);

					float right_x;
					float right_z;

					// Choose leftmost of the two right side edges
					if (right_x_1_2 < right_x_2_3) {
						right_x = right_x_1_2;
						right_z = edge_1_2.getCurrentZ_Y(delta_y_1);
					}
					else {
						right_x = right_x_2_3;
						right_z = edge_2_3.getCurrentZ_Y(delta_y_2);
					}

					GzIntensity red = ctoi(flatcolor[0]);
					GzIntensity green = ctoi(flatcolor[1]);
					GzIntensity blue = ctoi(flatcolor[2]);


					//Put in values for triangle edges
					//GzPut(ceil(left_x), j, red, green, blue, 0, left_z);
					//GzPut(ceil(right_x), j, red, green, blue, 0, right_z);

					//Now use SpanDDA to fill in individual rows
					SpanDDA rowspan = SpanDDA(left_x, left_z, right_x, right_z);
					for (int k = ceil(left_x); k < ceil(right_x); k++) {
						float delta_x = k - left_x;
						//k is every integer x value from left to right
						float current_z = rowspan.getCurrentZ_X(delta_x);
						GzPut(k, j, red, green, blue, 0, current_z);
					}

					continue;
				}
			}
			else {
				// Otherwise the configuration is L(1-2-3) R(1-3)
				// Very similar to above just changing some labels and < to >
				for (int j = start_row; j < end_row; j++) {

					float delta_y_1 = j - allvertex[0][1]; //startpoint for vertex 1 interpolation
					float delta_y_2 = j - allvertex[1][1]; //startpoint for vertex 2 interpolation

					float right_x = edge_1_3.getCurrentX_Y(delta_y_1);
					float right_z = edge_1_3.getCurrentZ_Y(delta_y_1);

					float left_x_1_2 = edge_1_2.getCurrentX_Y(delta_y_1);
					float left_x_2_3 = edge_2_3.getCurrentX_Y(delta_y_2);

					float left_x;
					float left_z;

					// Choose rightmost of the two left side edges
					if (left_x_1_2 > left_x_2_3) {
						left_x = left_x_1_2;
						left_z = edge_1_2.getCurrentZ_Y(delta_y_1);
					}
					else {
						left_x = left_x_2_3;
						left_z = edge_2_3.getCurrentZ_Y(delta_y_2);
					}

					GzIntensity red = ctoi(flatcolor[0]);
					GzIntensity green = ctoi(flatcolor[1]);
					GzIntensity blue = ctoi(flatcolor[2]);


					//GzPut(ceil(left_x), j, red, green, blue, 0, left_z);
					//GzPut(ceil(right_x), j, red, green, blue, 0, right_z);

					//Now use SpanDDA to fill in individual rows
					SpanDDA rowspan = SpanDDA(left_x, left_z, right_x, right_z);
					for (int k = ceil(left_x); k < ceil(right_x); k++) {
						float delta_x = k - left_x;
						//k is every integer x value from left to right
						float current_z = rowspan.getCurrentZ_X(delta_x);
						GzPut(k, j, red, green, blue, 0, current_z);
					}

				}
				continue;
			}

		}
	}
	return GZ_SUCCESS;
}


// Application5.cpp: implementation of the Application5 class.
//
//////////////////////////////////////////////////////////////////////

/*
 * application test code for homework assignment #5
*/

#include "stdafx.h"
#include "CS580HW.h"
#include "Application5.h"
#include "Gz.h"
#include "rend.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define INFILE  "ppot.asc"
#define OUTFILE "output.ppm"


float AAFilter[AAKERNEL_SIZE][3] 		/* X-shift, Y-shift, weight */
{
-0.52, 0.38, 0.128, 		0.41, 0.56, 0.119,		0.27, 0.08, 0.294,
-0.17, -0.29, 0.249,		0.58, -0.55, 0.104,		-0.31, -0.71, 0.106
};

//Pointer because we will create new GzRender similar to m_pRender


extern int tex_fun(float u, float v, GzColor color); /* image texture function */
extern int ptex_fun(float u, float v, GzColor color); /* procedural texture function */
extern int GzFreeTexture();

void shade(GzCoord norm, GzCoord color);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Application5::Application5()
{

}

Application5::~Application5()
{
	Clean();
}

int Application5::Initialize()
{
	GzCamera	camera;  
	int		    xRes, yRes;	/* display parameters */ 

	GzToken		nameListShader[9]; 	    /* shader attribute names */
	GzPointer   valueListShader[9];		/* shader attribute pointers */
	GzToken     nameListLights[10];		/* light info */
	GzPointer   valueListLights[10];
	int			shaderType, interpStyle;
	float		specpower;
	int		status; 
 

	status = 0; 

	/* 
	 * Allocate memory for user input
	 */
	m_pUserInput = new GzInput;

	/* 
	 * initialize the display and the renderer 
	 */ 
 	m_nWidth = 256;		// frame buffer and display width
	m_nHeight = 256;    // frame buffer and display height

	m_pRender = new GzRender(m_nWidth, m_nHeight);
	m_pRender->GzDefault();

	//HW6 create array of 6 renderers in m_pAARenderArray
	//Then compute anti-aliasing samples and store them all in the original m_pRender
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		m_pAARenderArray[i] = new GzRender(m_nWidth, m_nHeight);
		m_pAARenderArray[i]->GzDefault();
	}
	//We will use the original m_pRender's framebuffer to store all anti-aliased values
	m_pFrameBuffer = m_pRender->framebuffer; 

/* Translation matrix */
GzMatrix	scale = 
{ 
	3.25,	0.0,	0.0,	0.0, 
	0.0,	3.25,	0.0,	-3.25, 
	0.0,	0.0,	3.25,	3.5, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateX = 
{ 
	1.0,	0.0,	0.0,	0.0, 
	0.0,	.7071,	.7071,	0.0, 
	0.0,	-.7071,	.7071,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 
 
GzMatrix	rotateY = 
{ 
	.866,	0.0,	-0.5,	0.0, 
	0.0,	1.0,	0.0,	0.0, 
	0.5,	0.0,	.866,	0.0, 
	0.0,	0.0,	0.0,	1.0 
}; 

#if 1 	/* set up app-defined camera if desired, else use camera defaults */
    camera.position[X] = -3;
    camera.position[Y] = -25;
    camera.position[Z] = -4;

    camera.lookat[X] = 7.8;
    camera.lookat[Y] = 0.7;
    camera.lookat[Z] = 6.5;

    camera.worldup[X] = -0.2;
    camera.worldup[Y] = 1.0;
    camera.worldup[Z] = 0.0;

    camera.FOV = 63.7;              /* degrees *              /* degrees */

	status |= m_pRender->GzPutCamera(camera); 
	//HW6 put same camera in all 6 renderers
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		status |=  m_pAARenderArray[i]->GzPutCamera(camera);
	}
#endif 

	/* Start Renderer */
	status |= m_pRender->GzBeginRender();
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		status |= m_pAARenderArray[i]->GzBeginRender();
	}
	/* Light */
	GzLight	light1 = { {-0.7071, 0.7071, 0}, {0.5, 0.5, 0.9} };
	GzLight	light2 = { {0, -0.7071, -0.7071}, {0.9, 0.2, 0.3} };
	GzLight	light3 = { {0.7071, 0.0, -0.7071}, {0.2, 0.7, 0.3} };
	GzLight	ambientlight = { {0, 0, 0}, {0.3, 0.3, 0.3} };

	/* Material property */
	GzColor specularCoefficient = { 0.3, 0.3, 0.3 };
	GzColor ambientCoefficient = { 0.1, 0.1, 0.1 };
	GzColor diffuseCoefficient = {0.7, 0.7, 0.7};

/* 
  renderer is ready for frame --- define lights and shader at start of frame 
*/

        /*
         * Tokens associated with light parameters
         */
        nameListLights[0] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[0] = (GzPointer)&light1;
        nameListLights[1] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[1] = (GzPointer)&light2;
        nameListLights[2] = GZ_DIRECTIONAL_LIGHT;
        valueListLights[2] = (GzPointer)&light3;
        status |= m_pRender->GzPutAttribute(3, nameListLights, valueListLights);
		//HW6 Assign lights
		for (int i = 0; i < AAKERNEL_SIZE; i++) {
			status |= m_pAARenderArray[i]->GzPutAttribute(3, nameListLights, valueListLights);
		}

        nameListLights[0] = GZ_AMBIENT_LIGHT;
        valueListLights[0] = (GzPointer)&ambientlight;
        status |= m_pRender->GzPutAttribute(1, nameListLights, valueListLights);

		//Why doesnt starter code just use a single nameListLights
		for (int i = 0; i < AAKERNEL_SIZE; i++) {
			status |= m_pAARenderArray[i]->GzPutAttribute(1, nameListLights, valueListLights);
		}
        /*
         * Tokens associated with shading 
         */
        nameListShader[0]  = GZ_DIFFUSE_COEFFICIENT;
        valueListShader[0] = (GzPointer)diffuseCoefficient;

	/* 
	* Select either GZ_COLOR or GZ_NORMALS as interpolation mode  
	*/
        nameListShader[1]  = GZ_INTERPOLATE;
        //interpStyle = GZ_COLOR;         /* Gouraud shading */
        interpStyle = GZ_NORMALS;         /* Phong shading */
        valueListShader[1] = (GzPointer)&interpStyle;

        nameListShader[2]  = GZ_AMBIENT_COEFFICIENT;
        valueListShader[2] = (GzPointer)ambientCoefficient;
        nameListShader[3]  = GZ_SPECULAR_COEFFICIENT;
        valueListShader[3] = (GzPointer)specularCoefficient;
        nameListShader[4]  = GZ_DISTRIBUTION_COEFFICIENT;
        specpower = 32;
        valueListShader[4] = (GzPointer)&specpower;

        nameListShader[5]  = GZ_TEXTURE_MAP;
#if 0   /* set up null texture function or valid pointer */
        valueListShader[5] = (GzPointer)0;
#else
        valueListShader[5] = (GzPointer)(tex_fun);	/* or use ptex_fun */
		//valueListShader[5] = (GzPointer)(ptex_fun);	/* or use ptex_fun */

#endif
        status |= m_pRender->GzPutAttribute(6, nameListShader, valueListShader);

		//HW6 put shader attributes in all 6 renderers
		//Create new aa_shiftx and aa_shifty attributes to put in renderers
		for (int i = 0; i < AAKERNEL_SIZE; i++) {
			nameListShader[6] = GZ_AASHIFTX;
			valueListShader[6] = (GzPointer)&AAFilter[i][0];

			nameListShader[7] = GZ_AASHIFTY;
			valueListShader[7] = (GzPointer)&AAFilter[i][1];

			nameListShader[8] = GZ_AAWEIGHT;
			valueListShader[8] = (GzPointer)&AAFilter[i][2];

			status |= m_pAARenderArray[i]->GzPutAttribute(9, nameListShader, valueListShader);
		}

	status |= m_pRender->GzPushMatrix(scale);  
	status |= m_pRender->GzPushMatrix(rotateY); 
	status |= m_pRender->GzPushMatrix(rotateX); 
	//HW6 set transformations in aa_renderarray
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		status |= m_pAARenderArray[i]->GzPushMatrix(scale);
		status |= m_pAARenderArray[i]->GzPushMatrix(rotateY);
		status |= m_pAARenderArray[i]->GzPushMatrix(rotateX);
	}

	if (status) exit(GZ_FAILURE); 

	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Render() 
{
	GzToken		nameListTriangle[3]; 	/* vertex attribute names */
	GzPointer	valueListTriangle[3]; 	/* vertex attribute pointers */
	GzCoord		vertexList[3];	/* vertex position coordinates */ 
	GzCoord		normalList[3];	/* vertex normals */ 
	GzTextureIndex  	uvList[3];		/* vertex texture map indices */ 
	char		dummy[256]; 
	int			status; 


	/* Initialize Display */
	status |= m_pRender->GzDefault();  /* init for new frame */
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		m_pAARenderArray[i]->GzDefault();
	}
	/* 
	* Tokens associated with triangle vertex values 
	*/ 
	nameListTriangle[0] = GZ_POSITION; 
	nameListTriangle[1] = GZ_NORMAL; 
	nameListTriangle[2] = GZ_TEXTURE_INDEX;  

	// I/O File open
	FILE *infile;
	if( (infile  = fopen( INFILE , "r" )) == NULL )
	{
         AfxMessageBox( "The input file was not opened\n" );
		 return GZ_FAILURE;
	}

	FILE *outfile;
	if( (outfile  = fopen( OUTFILE , "wb" )) == NULL )
	{
         AfxMessageBox( "The output file was not opened\n" );
		 return GZ_FAILURE;
	}

	/* 
	* Walk through the list of triangles, set color 
	* and render each triangle 
	*/ 
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

	    /* 
	     * Set the value pointers to the first vertex of the 	
	     * triangle, then feed it to the renderer 
	     * NOTE: this sequence matches the nameList token sequence
	     */ 
	     valueListTriangle[0] = (GzPointer)vertexList; 
		 valueListTriangle[1] = (GzPointer)normalList; 
		 valueListTriangle[2] = (GzPointer)uvList; 
		 // dont even put the triangles into main renderer to ensure that we are using weigted sums
		 //m_pRender->GzPutTriangle(3, nameListTriangle, valueListTriangle); 
		 //HW6 Also put these triangles into render array
		 for (int i = 0; i < AAKERNEL_SIZE; i++) {
			 m_pAARenderArray[i]->GzPutTriangle(3, nameListTriangle, valueListTriangle);
		 }
	} 

	//HW6 Anti-Aliasing 
	int total_pixels = (m_pRender->xres) * (m_pRender->yres);
	// For each pixel, do sum of that pixel the 6 renderer arrays (weights already handled while colors are still floats)
	// NOTE: weight is multiplied during PutTriangle to adjust color before converting to intensities
	// Then set the m_pRender pixel values to the weighted sum
	for (int j = 0; j<total_pixels; j++) {
		GzIntensity sum_red = 0;
		GzIntensity sum_blue = 0;
		GzIntensity sum_green = 0;
		for (int i = 0; i < AAKERNEL_SIZE; i++) {
			sum_red += m_pAARenderArray[i]->pixelbuffer[j].red;
			sum_blue += m_pAARenderArray[i]->pixelbuffer[j].blue;
			sum_green += m_pAARenderArray[i]->pixelbuffer[j].green;
		}
		m_pRender->pixelbuffer[j].red = sum_red;
		m_pRender->pixelbuffer[j].blue = sum_blue;
		m_pRender->pixelbuffer[j].green = sum_green;
	}


	m_pRender->GzFlushDisplay2File(outfile); 	/* write out or update display to file*/
	m_pRender->GzFlushDisplay2FrameBuffer();	// write out or update display to frame buffer

	/* 
	 * Close file
	 */ 

	if( fclose( infile ) )
      AfxMessageBox(_T( "The input file was not closed\n" ));

	if( fclose( outfile ) )
      AfxMessageBox(_T( "The output file was not closed\n" ));
 
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS); 
}

int Application5::Clean()
{
	/* 
	 * Clean up and exit 
	 */ 
	int	status = 0; 

	delete(m_pRender);
	for (int i = 0; i < AAKERNEL_SIZE; i++) {
		delete(m_pAARenderArray[i]);
	}
	status |= GzFreeTexture();
	
	if (status) 
		return(GZ_FAILURE); 
	else 
		return(GZ_SUCCESS);
}




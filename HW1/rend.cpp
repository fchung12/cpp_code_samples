#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"


GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	//Constructor for x and y resolution
	//wrap min and max to clamp resolution values
	xres = (unsigned short) max(0,min(xRes,MAXXRES));
	yres = (unsigned short) max(0,min(yRes,MAXYRES));

	framebuffer = new char[xres*yres*3]; //3 bytes x width x height
	pixelbuffer = new GzPixel[xres * yres];

}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete[] framebuffer;
	delete[] pixelbuffer;
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	GzPixel defaultpixel; //defaultpixel all 0 for simplicity
	defaultpixel.red = 0;
	defaultpixel.blue = 0;
	defaultpixel.green = 0;
	defaultpixel.z = 0;
	defaultpixel.alpha = 0;

	for (int i = 0; i < xres * yres; i++) {
		pixelbuffer[i] = defaultpixel;
	}

	for (int i = 0; i < xres * yres * 3; i++) {
		framebuffer[i] = 0; //blue
		i += 1;
		framebuffer[i] = 0; //green
		i += 1;
		framebuffer[i] = 0; //red
	}

	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		GzPixel putpixel;
		putpixel.red = r;
		putpixel.green = g;
		putpixel.blue = b;
		putpixel.alpha = a;
		putpixel.z = z;
		pixelbuffer[ARRAY(i, j)] = putpixel;
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

// Clamps a GzIntensity 0-4095
GzIntensity ClampColor(GzIntensity pixelvalue) {
	return max(0, min(pixelvalue, 4095));
}

// Rightshift 4 with fill zero and cast to char mentioned in lecture
char IntensitytoRGB(GzIntensity pixelvalue) {
	return (char)(pixelvalue >> 4);
}

int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile,"P6 %d %d 255\r", xres, yres);
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


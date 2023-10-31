/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
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
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */
    
  // Clamp u,v to defined domain [0,1]
  u = max(0, min(1, u));
  v = max(0, min(1, v));

  // Scale uv to get sample point xy
  float x = u * (xs - 1);
  float y = v * (ys - 1);

  // Set corner points for bilinear interpolation (also convert to int)
  int min_x = (int)floor(x);
  int min_y = (int)floor(y);
  int max_x = (int)ceil(x);
  int max_y = (int)ceil(y);

  // Get s and t based on corner points
  float s = x - floor(x);
  float t = y - floor(y);

  // Copy and pasted gz.h ARRAY function using c++ lambda to turn it into function
  // https://en.cppreference.com/w/cpp/language/lambda
  auto ARRAY = [](int x, int y) { return (x + y * xs); };	/* simplify fbuf indexing */
  // May as well use lambda to create consistent Color function
  auto Color = [](float s, float t, float A, float B, float C, float D) {
      return ((s * t * C) + ((1 - s) * t * D) + (s * (1 - t) * B) + ((1 - s) * (1 - t) * A));
  };

  float A_red = image[ARRAY(min_x,min_y)][RED];
  float B_red = image[ARRAY(max_x, min_y)][RED];
  float C_red = image[ARRAY(max_x, max_y)][RED];
  float D_red = image[ARRAY(min_x, max_y)][RED];
  float interp_red = Color(s, t, A_red, B_red, C_red, D_red);

  float A_green = image[ARRAY(min_x, min_y)][GREEN];
  float B_green = image[ARRAY(max_x, min_y)][GREEN];
  float C_green = image[ARRAY(max_x, max_y)][GREEN];
  float D_green = image[ARRAY(min_x, max_y)][GREEN];
  float interp_green = Color(s, t, A_green, B_green, C_green, D_green);

  float A_blue = image[ARRAY(min_x, min_y)][BLUE];
  float B_blue = image[ARRAY(max_x, min_y)][BLUE];
  float C_blue = image[ARRAY(max_x, max_y)][BLUE];
  float D_blue = image[ARRAY(min_x, max_y)][BLUE];
  float interp_blue = Color(s, t, A_blue, B_blue, C_blue, D_blue);

  color[0] = interp_red;
  color[1] = interp_green;
  color[2] = interp_blue;


	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{

    // Simple Checkerboard
    // Clamp u,v to defined domain [0,1]
    u = max(0, min(1, u));
    v = max(0, min(1, v));

    int num_intervals = 6;
   
    // Scale uv up to interval values (if u is 4.4/6 it scales to 4.4) 
    float scale_u = u * num_intervals;
    float scale_v = v * num_intervals;
    // Then use floor and ceiling to determine odd or even interval (4 < 4.4 < 5) means odd interval
    
    // returns 1 if x is in an odd interval using mod 2, 0 otherwise
    auto getOddInterval = [](float x) {
        int odd_min = ((int)floor(x) % 2);
        int odd_max = ((int)ceil(x) % 2);
        if (odd_min == 0 && odd_max == 1) {
            return 1;
        }
        else {
            return 0;
        }
    };

    // Determine whether scaled uv is odd or even
    int odd_u = getOddInterval(scale_u);
    int odd_v = getOddInterval(scale_v);

    // odd/odd or even/even -> Black
    // odd/even or even/odd -> White
    if (odd_u == odd_v) {
        GzColor color_odd = { 0,0,0 };
        color[0] = color_odd[0];
        color[1] = color_odd[1];
        color[2] = color_odd[2];
    }
    else {
        GzColor color_even = { 1,1,1 };
        color[0] = color_even[0];
        color[1] = color_even[1];
        color[2] = color_even[2];
    }


	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}


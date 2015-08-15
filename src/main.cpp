//*****************************************************************************
//File Name: main.cpp
//
//Description: This program is a Ray Tracer which produces an image and saves it
//             with the project. The file is called RayTrace.bmp. For more
//             information on Ray Tracing see:
//             http://en.wikipedia.org/wiki/Ray_tracing_(graphics)
//
//Author: Joe Crouch
//*****************************************************************************


# include <iostream>
# include <fstream>
# include <cmath>
# include <sstream>
# include <string>
# include <vector>
# include <limits>

# include <stdlib.h>
# include <stdio.h>
# include <time.h>

// Classes to be Included
#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Colour.h"
#include "Source.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"


using namespace std;

// Create a structure to hold red green blue values
struct RGBType {
	double r;
	double g;
	double b;
};

// Function for saving a bmp file
void savebmp(const char *filename, int w, int h, int dpi, RGBType *data){
	FILE *f;
	int k = w*h;
	int s = 4*k;
	int filesize =54+s;

	double factor = 39.375;
	int m = static_cast<int>(factor);

	int ppm = dpi*m;

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

	/*bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);*/

	bmpinfoheader[ 4] = (unsigned char)(w);
	bmpinfoheader[ 5] = (unsigned char)(w>>8);
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);

	bmpinfoheader[ 8] = (unsigned char)(h);
	bmpinfoheader[ 9] = (unsigned char)(h>>8);
	bmpinfoheader[10] = (unsigned char)(h>>16);
	bmpinfoheader[11] = (unsigned char)(h>>24);

	/*bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s>>8);
	bmpinfoheader[23] = (unsigned char)(s>>16);
	bmpinfoheader[24] = (unsigned char)(s>>24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm>>8);
	bmpinfoheader[27] = (unsigned char)(ppm>>16);
	bmpinfoheader[28] = (unsigned char)(ppm>>24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm>>8);
	bmpinfoheader[31] = (unsigned char)(ppm>>16);
	bmpinfoheader[32] = (unsigned char)(ppm>>24);*/

	f = fopen(filename,"wb");

	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);

	for(int i = 0; i<k; i++){

		double red = (data[i].r)*255;
		double green = (data[i].g)*255;
		double blue = (data[i].b)*255;

		unsigned char colour[3] = {(int)floor(blue),(int)floor(green),(int)floor(red)};

		fwrite(colour,1,3,f);
	}

	fclose(f);
}

//Function for calculating and returning the index of the first object a ray intersects with
int closestObjectIndex(vector<double> object_intersections) {

	int index_of_minimum_value;

	//if there are no intersections between the ray and objects return -1
	if (object_intersections.size() == 0) {

		return -1;
	}
	//If there is one intersection between the ray and the objects
	else if (object_intersections.size() == 1){
		if(object_intersections.at(0) > 0){
			// if that intersection is greater than zero then its our index of minimum value
			return 0;
		}
		else{
			//otherwise the only intersection is negative
			return -1;
		}
	}
	//more than one intersection
	else {
		//first find maximum value

		double max = 0;
		for ( int i = 0; i < object_intersections.size(); i++) {
			if (max < object_intersections.at(i)) {
				max = object_intersections.at(i);
			}
		}

		//then starting from the maximum value find the minimum positive value
		if (max > 0) {
			// only want positive intersections
			for (int index = 0; index < object_intersections.size(); index++){
				if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max){
					max = object_intersections.at(index);
					index_of_minimum_value = index;
				}
			}
			return index_of_minimum_value;
		}
		else{
			//all the intersetions were negative
			return -1;
		}
	}
}

//Function that calculates and returns the colour at each pixel that a ray intersects with
Colour getColourAt(Vect intersection_position,Vect intersection_ray_direction, vector<Object*> scene_objects, int index_of_closest_object, vector<Source*> light_sources, double accuracy, double ambientlight){
	//get colour of closest object
	Colour closest_object_colour = scene_objects.at(index_of_closest_object) -> getColour();
	//collect the normal to the closest object
	Vect closest_object_normal = scene_objects.at(index_of_closest_object) -> getNormalAt(intersection_position);

	//set the output as a colour and give a base value as if in shadow
	Colour final_colour = closest_object_colour.colourScalar(ambientlight);

	//loop over each light in the scene
	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		//calculate direction of light to intersection point
		Vect light_direction = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();

		//calculate angle between the normal of the object and the light direction using the dotproduct
		float cosine_angle = closest_object_normal.dotProduct(light_direction);

		// if the angle is not parallel
		if(cosine_angle > 0) {
			//test for shadows
			//set shadowed as false to start
			bool shadowed = false;

			//Calculate distance vector between the light and the intersection point
			Vect distance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
			//Use the distance vector to calculate the distance magnitude
			float distance_to_light_magnitude = distance_to_light.magnitude();

			//Create ray between intersection point and the light source in the scene
			Ray shadow_ray (intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());

			//Create array for intersections between shadow ray and objects before the light source
			vector<double> secondary_intersections;

			//Loop over objects and see if they intersect with shadow ray
			for ( int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}

			//Loop over intersections and check to see if are in shadow
			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					/*if (secondary_intersections.at(c) <= distance_to_light_magnitude)*/ {
						shadowed = true;
					}
				}
				if(shadowed==true){break;}
			}

			// if not in shadow
			if (shadowed == false) {
				//Set final colour to be combination of the object colour and light colour depending on the angle between the point of intersection and the light
				final_colour = final_colour.colourAdd(closest_object_colour.colourMult(light_sources.at(light_index)->getLightColour()).colourScalar(cosine_angle));

				//If the object has a special colour add shine to the final colour
				if (closest_object_colour.getColourSpecial() > 0 && closest_object_colour.getColourSpecial() <=1) {
					//special ranges 0 to 1
					double dot1 = closest_object_normal.dotProduct(intersection_ray_direction.negative());
					Vect scalar1 = closest_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersection_ray_direction);
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersection_ray_direction.negative().vectAdd(scalar2);
					Vect reflection_direction = add2.normalize();

					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0){
						specular = pow(specular, 10);
						final_colour = final_colour.colourAdd(light_sources.at(light_index)->getLightColour().colourScalar(specular*closest_object_colour.getColourSpecial()));
					}
				}

			}

		}
	}
	// clip final colour so value is between 0 and 1 as otherwise shine comes out as wrong colour
	return final_colour.clip();
}

//Create integer value for a pixel
int thispixel;

int main (int argc, char *argv[]){
	cout << "rendering..." << endl;

	//create integer values for the image size
	int dpi = 72;
	int width = 1920;
	int height = 1080;
	int area = width*height;

	// set the ratio between height and width
	double aspectratio = (double)width/(double)height;
	//Set a value for the light to create different colours
	double ambientlight = 0.2;
	// set an accuracy value
	double accuracy = 0.000001;

	// create array to hold colour for each pixel
	RGBType *pixels = new RGBType[area];

	//set the origin
	Vect Origin (0,0,0);

	//set vectors for position
	Vect X (1,0,0);
	Vect Y (0,1,0);
	Vect Z (0,0,1);

	//place camera at coordinate
	Vect campos (1,-0.5,0);

	//set a point that the user would like the camera pointed
	Vect look_at (0,-0.9,0);
	//Calculate difference between camera position and the point the camera is "looking" at
	Vect diff_btw (campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(), campos.getVectZ() - look_at.getVectZ());

	//set the camera direction towards the look_at point
	Vect camdir = diff_btw.negative().normalize();
	//give the camera a direction for "right"
	Vect camright = Y.crossProduct(camdir).normalize();
	//give the camera a direction for "down"
	Vect camdown = camright.crossProduct(camdir);
	//Create a camera with the properties set above
	Camera scene_cam (campos, camdir, camright, camdown);

	//Set a range of colours
	Colour white_light (1.0,1.0,1.0,0);
	Colour nice_red (1,0.2,0.2,0.3);
	Colour blue (0,0,1,0.4);
	Colour grey (0.5,0.5,0.5,0);
	Colour random(0.2,0.6,0.8,0);
	Colour black (0,0,0,0);

	//set and create lights with position and colour
	Vect light_pos (-7,10,0);
	Vect light_pos2(7,5,7);
	Light scene_light (light_pos,white_light);
	Light scene_light2 (light_pos2,white_light);

	//Create a vector of "Sources" containing the different sources of light
	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&scene_light2));

	// create scene objects, in this cas a sphere at orgin and a plane normal to the y axis
	Vect sphere1_pos (0,-0.9,0);
	Sphere scene_sphere1 (sphere1_pos,0.1,nice_red);
	Vect sphere2_pos (0,-0.7,0);
	Sphere scene_sphere2 (sphere2_pos,0.1,nice_red);
	Vect sphere3_pos (0,-0.5,0);
	Sphere scene_sphere3 (sphere3_pos,0.1,nice_red);
	Vect sphere4_pos (0,-0.5,0.2);
	Sphere scene_sphere4 (sphere4_pos,0.1,nice_red);
	Vect sphere5_pos (0,-0.5,-0.2);
	Sphere scene_sphere5 (sphere5_pos,0.1,nice_red);

	Vect sphere6_pos (0,-0.9,-0.2);
	Sphere scene_sphere6 (sphere6_pos,0.1,nice_red);
	Vect sphere7_pos (0,-0.9,-0.4);
	Sphere scene_sphere7 (sphere7_pos,0.1,nice_red);
	Vect sphere8_pos (0,-0.759,-0.541);
	Sphere scene_sphere8 (sphere8_pos,0.1,nice_red);
	Vect sphere9_pos (0,-0.559,-0.541);
	Sphere scene_sphere9 (sphere9_pos,0.1,nice_red);
	Vect sphere10_pos (0,-0.359,-0.541);
	Sphere scene_sphere10 (sphere10_pos,0.1,nice_red);

    Vect sphere11_pos (0,-0.159,-0.541);
	Sphere scene_sphere11 (sphere11_pos,0.1,nice_red);
    Vect sphere12_pos (0,-0.018,-0.4);
	Sphere scene_sphere12 (sphere12_pos,0.1,nice_red);
	Vect sphere13_pos (0,-0.018,-0.2);
	Sphere scene_sphere13 (sphere13_pos,0.1,nice_red);
	Vect sphere14_pos (0,-0.018,0);
	Sphere scene_sphere14 (sphere14_pos,0.1,nice_red);
	Vect sphere15_pos (0,-0.9,0.5);
	Sphere scene_sphere15 (sphere15_pos,0.1,nice_red);

	Vect sphere16_pos (0,-0.7,0.5);
	Sphere scene_sphere16 (sphere16_pos,0.1,nice_red);
	Vect sphere17_pos (0,-0.5,0.5);
	Sphere scene_sphere17 (sphere17_pos,0.1,nice_red);
	Vect sphere18_pos (0,-0.3,0.5);
	Sphere scene_sphere18 (sphere18_pos,0.1,nice_red);
	Vect sphere19_pos (0,-0.1,0.5);
	Sphere scene_sphere19 (sphere19_pos,0.1,nice_red);
	Vect sphere20_pos (0,-0.1,0.7);
	Sphere scene_sphere20 (sphere20_pos,0.1,nice_red);

	Vect sphere21_pos (0,-0.1,0.9);
	Sphere scene_sphere21 (sphere21_pos,0.1,nice_red);
	Vect sphere22_pos (0,-0.1,1.1);
	Sphere scene_sphere22 (sphere22_pos,0.1,nice_red);
	Vect sphere23_pos (0,-0.3,1.1);
	Sphere scene_sphere23 (sphere23_pos,0.1,nice_red);
	Vect sphere24_pos (0,-0.5,1.1);
	Sphere scene_sphere24 (sphere24_pos,0.1,nice_red);
	Vect sphere25_pos (0,-0.7,1.1);
	Sphere scene_sphere25 (sphere25_pos,0.1,nice_red);

	Vect sphere26_pos (0,-0.9,1.1);
	Sphere scene_sphere26 (sphere26_pos,0.1,nice_red);
	Vect sphere27_pos (0,-0.5,0.9);
	Sphere scene_sphere27 (sphere27_pos,0.1,nice_red);
	Vect sphere28_pos (0,-0.5,0.7);
	Sphere scene_sphere28 (sphere28_pos,0.1,nice_red);
	Vect sphere29_pos (0,-0.9,1.4);
	Sphere scene_sphere29 (sphere29_pos,0.1,nice_red);
	Vect sphere30_pos (0,-0.7,1.4);
	Sphere scene_sphere30 (sphere30_pos,0.1,nice_red);

	Vect sphere31_pos (0,-0.5,1.4);
	Sphere scene_sphere31 (sphere31_pos,0.1,nice_red);
	Vect sphere32_pos (0,-0.3,1.4);
	Sphere scene_sphere32 (sphere32_pos,0.1,nice_red);
	Vect sphere33_pos (0,-0.1,1.4);
	Sphere scene_sphere33 (sphere33_pos,0.1,nice_red);
	Vect sphere34_pos (0,-0.1,1.6);
	Sphere scene_sphere34 (sphere34_pos,0.1,nice_red);
	Vect sphere35_pos (0,-0.1,1.8);
	Sphere scene_sphere35 (sphere35_pos,0.1,nice_red);

	Vect sphere36_pos (0,-0.3,1.8);
	Sphere scene_sphere36 (sphere36_pos,0.1,nice_red);
	Vect sphere37_pos (0,-0.5,1.8);
	Sphere scene_sphere37 (sphere37_pos,0.1,nice_red);
	Vect sphere38_pos (0,-0.1,2);
	Sphere scene_sphere38 (sphere38_pos,0.1,nice_red);
	Vect sphere39_pos (0,-0.1,2.2);
	Sphere scene_sphere39 (sphere39_pos,0.1,nice_red);
	Vect sphere40_pos (0,-0.3,2.2);
	Sphere scene_sphere40 (sphere40_pos,0.1,nice_red);

	Vect sphere41_pos (0,-0.5,2.2);
	Sphere scene_sphere41 (sphere41_pos,0.1,nice_red);
	Vect sphere42_pos (0,-0.7,2.2);
	Sphere scene_sphere42 (sphere42_pos,0.1,nice_red);
	Vect sphere43_pos (0,-0.9,2.2);
	Sphere scene_sphere43 (sphere43_pos,0.1,nice_red);
	Vect sphere44_pos (0,-0.9,2.5);
	Sphere scene_sphere44 (sphere44_pos,0.1,nice_red);
	Vect sphere45_pos (0,-0.9,2.7);
	Sphere scene_sphere45 (sphere45_pos,0.1,nice_red);

	Vect sphere46_pos (0,-0.9,2.9);
	Sphere scene_sphere46 (sphere46_pos,0.1,nice_red);
	Vect sphere47_pos (0,-0.9,3.2);
	Sphere scene_sphere47 (sphere47_pos,0.1,nice_red);
	Vect sphere48_pos (0,-0.7,2.5);
	Sphere scene_sphere48 (sphere48_pos,0.1,nice_red);
	Vect sphere49_pos (0,-0.5,2.5);
	Sphere scene_sphere49 (sphere49_pos,0.1,nice_red);
	Vect sphere50_pos (0,-0.3,2.5);
	Sphere scene_sphere50 (sphere50_pos,0.1,nice_red);

	Vect sphere51_pos (0,-0.1,2.5);
	Sphere scene_sphere51 (sphere51_pos,0.1,nice_red);
	Vect sphere52_pos (0,-0.1,2.7);
	Sphere scene_sphere52 (sphere52_pos,0.1,nice_red);
	Vect sphere53_pos (0,-0.1,2.9);
	Sphere scene_sphere53 (sphere53_pos,0.1,nice_red);
	Vect sphere54_pos (0,-0.9,3.4);
	Sphere scene_sphere54 (sphere54_pos,0.1,nice_red);
	Vect sphere55_pos (0,-0.5,2.7);
	Sphere scene_sphere55 (sphere55_pos,0.1,nice_red);

	Vect sphere56_pos (0,-0.9,3.6);
	Sphere scene_sphere56 (sphere56_pos,0.1,nice_red);
	Vect sphere57_pos (0,-0.7,3.6);
	Sphere scene_sphere57 (sphere57_pos,0.1,nice_red);
	Vect sphere58_pos (0,-0.5,3.6);
	Sphere scene_sphere58 (sphere58_pos,0.1,nice_red);
	Vect sphere59_pos (0,-0.5,3.4);
	Sphere scene_sphere59 (sphere59_pos,0.1,nice_red);
	Vect sphere60_pos (0,-0.5,3.2);
	Sphere scene_sphere60 (sphere60_pos,0.1,nice_red);

	Vect sphere61_pos (0,-0.3,3.2);
	Sphere scene_sphere61 (sphere61_pos,0.1,nice_red);
	Vect sphere62_pos (0,-0.1,3.2);
	Sphere scene_sphere62 (sphere62_pos,0.1,nice_red);
	Vect sphere63_pos (0,-0.1,3.4);
	Sphere scene_sphere63 (sphere63_pos,0.1,nice_red);
	Vect sphere64_pos (0,-0.1,3.6);
	Sphere scene_sphere64 (sphere64_pos,0.1,nice_red);
	Vect sphere65_pos (0,-0.1,3.9);
	Sphere scene_sphere65 (sphere65_pos,0.1,nice_red);

	Vect sphere66_pos (0,-0.9,5);
	Sphere scene_sphere66 (sphere66_pos,0.1,nice_red);
	Vect sphere67_pos (0,-0.7,5);
	Sphere scene_sphere67 (sphere67_pos,0.1,nice_red);
	Vect sphere68_pos (0,-0.5,5);
	Sphere scene_sphere68 (sphere68_pos,0.1,nice_red);
	Vect sphere69_pos (0,-0.5,4.8);
	Sphere scene_sphere69 (sphere69_pos,0.1,nice_red);
	Vect sphere70_pos (0,-0.5,4.6);
	Sphere scene_sphere70 (sphere70_pos,0.1,nice_red);

	Vect sphere71_pos (0,-0.3,4.6);
	Sphere scene_sphere71 (sphere71_pos,0.1,nice_red);
	Vect sphere72_pos (0,-0.1,4.6);
	Sphere scene_sphere72 (sphere72_pos,0.1,nice_red);
	Vect sphere73_pos (0,-0.1,4.8);
	Sphere scene_sphere73 (sphere73_pos,0.1,nice_red);
	Vect sphere74_pos (0,-0.1,5);
	Sphere scene_sphere74 (sphere74_pos,0.1,nice_red);
	Vect sphere75_pos (0,-0.9,4.6);
	Sphere scene_sphere75 (sphere75_pos,0.1,nice_red);

	Vect sphere76_pos (0,-0.9,4.8);
	Sphere scene_sphere76 (sphere76_pos,0.1,nice_red);
	Vect sphere77_pos (0,-0.3,3.9);
	Sphere scene_sphere77 (sphere77_pos,0.1,nice_red);
	Vect sphere78_pos (0,-0.5,3.9);
	Sphere scene_sphere78 (sphere78_pos,0.1,nice_red);
	Vect sphere79_pos (0,-0.5,4.1);
	Sphere scene_sphere79 (sphere79_pos,0.1,nice_red);
	Vect sphere80_pos (0,-0.5,4.3);
	Sphere scene_sphere80 (sphere80_pos,0.1,nice_red);

	Vect sphere81_pos (0,-0.3,4.3);
	Sphere scene_sphere81 (sphere81_pos,0.1,nice_red);
	Vect sphere82_pos (0,-0.7,4.1);
	Sphere scene_sphere82 (sphere82_pos,0.1,nice_red);
	Vect sphere83_pos (0,-0.9,4.1);
	Sphere scene_sphere83 (sphere83_pos,0.1,nice_red);
	Vect sphere84_pos (0,-0.1,4.3);
	Sphere scene_sphere84 (sphere84_pos,0.1,nice_red);

	Plane scene_plane (Y,-1, blue);

	//create a vector of "Objects"
	vector<Object*> scene_objects;
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere1));
	/*scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere3));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere4));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere5));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere6));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere7));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere8));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere9));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere10));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere11));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere12));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere13));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere14));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere15));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere16));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere17));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere18));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere19));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere20));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere21));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere22));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere23));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere24));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere25));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere26));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere27));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere28));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere29));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere30));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere31));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere32));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere33));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere34));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere35));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere36));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere37));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere38));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere39));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere40));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere41));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere42));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere43));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere44));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere45));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere46));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere47));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere48));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere49));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere50));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere51));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere52));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere53));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere54));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere55));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere56));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere57));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere58));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere59));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere60));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere61));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere62));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere63));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere64));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere65));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere66));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere67));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere68));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere69));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere70));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere71));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere72));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere73));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere74));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere75));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere76));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere77));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere78));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere79));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere80));

	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere81));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere82));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere83));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere84));*/

	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));

	//two variables for calculating the ray direction from the camera for each pixel
	double xamount, yamount;

	//for each pixel in the image
	for (int x = 0; x<width; x++){
		for(int y = 0; y<height; y++){
			//set the pixel we are calculating the colour for
			thispixel = y*width +x;

			//set the xamount and yamount values for the ray depending on the size of the image to be produced
			if (width > height) {
				xamount = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height)/2);
				yamount = ((height - y) + 0.5)/height;
			}
			else if (height > width){
				xamount = (x + 0.5)/width;
				yamount = (((height - y)+0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
			}
			else {
				xamount = (x+0.5)/width;
				yamount = ((height - y) + 0.5)/height;
			}

			//set the ray to come from the camera position
			Vect cam_ray_origin = scene_cam.getCameraPosition();
			// set the direction of the ray at the particular pixel we are interested in during this loop
			Vect cam_ray_direction = camdir.vectAdd(camright.vectMult(xamount - 0.5).vectAdd(camdown.vectMult(yamount - 0.5))).normalize();

			//create a ray with the properties set above
			Ray cam_ray (cam_ray_origin, cam_ray_direction);

			//Create a vector of intersections
			vector<double> intersections;

			//loop over the objects in the scene and see if they intersect with the camera ray
			for (int index = 0; index < scene_objects.size(); index++){
				intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
			}

			//calculate the closest object the ray intersects with using the function defined above
			int index_of_closest_object = closestObjectIndex(intersections);

			//if the ray does not intersect with any objects set the colour to black
			if(index_of_closest_object == -1){
				pixels[thispixel].r = 0;
				pixels[thispixel].g = 0;
				pixels[thispixel].b = 0;
			 }
			 //if the ray does intersect with an object
			else{
				//accuracy used to make sure are looking at point on sphere not inside the sphere
				if (intersections.at(index_of_closest_object) > accuracy ){
					//determine the position and direction vectors at the point of intersection
					Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_closest_object)));
					Vect intersection_ray_direction = cam_ray_direction;

					// Use the function defined above to calculate the colour for the pixel at the point of intersection
					Colour intersection_colour = getColourAt(intersection_position, intersection_ray_direction, scene_objects, index_of_closest_object, light_sources, accuracy, ambientlight);

					//set the pixel to this colour
					pixels[thispixel].r = intersection_colour.getColourRed();
					pixels[thispixel].g = intersection_colour.getColourGreen();
					pixels[thispixel].b = intersection_colour.getColourBlue();
				}
			}
		}
	}

  	// create and save the image using the function defined above
	savebmp("RayTrace2.bmp",width,height,dpi,pixels);
	system("pause");
	return 0;
}

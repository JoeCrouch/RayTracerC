//Create a Sphere class
#ifndef _SPHERE_H
#define _SPHERE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Colour.h"

//set Sphere class as a subclass of the Object class
//set Sphere to take a center, a radius and a colour
class Sphere : public Object {
	Vect center;
	double radius;
	Colour colour;

	public:

	Sphere ();

	Sphere(Vect, double, Colour);

	// Functions for returning values of a sphere
	Vect getSphereCenter () { return center; }
	double getSphereRadius () { return radius; }
	virtual Colour getColour () { return colour; }

	//function for returning the normal to a sphere
	virtual Vect getNormalAt(Vect point){
		//normal always points away from the center of a sphere
		Vect normal_Vect = point.vectAdd(center.negative()).normalize();
		return normal_Vect;
	}
	//function for finding if a ray intersects with a Sphere
	virtual double findIntersection(Ray ray) {
		//Collect the origin of the ray
		Vect ray_origin = ray.getRayOrigin();
		double ray_origin_x = ray_origin.getVectX();
		double ray_origin_y = ray_origin.getVectY();
		double ray_origin_z = ray_origin.getVectZ();

		//Collect the direction of the ray
		Vect ray_direction = ray.getRayDirection();
		double ray_direction_x = ray_direction.getVectX();
		double ray_direction_y = ray_direction.getVectY();
		double ray_direction_z = ray_direction.getVectZ();

		//Collect the center of the sphere
		Vect sphere_center = center;
		double sphere_center_x = sphere_center.getVectX();
		double sphere_center_y = sphere_center.getVectY();
		double sphere_center_z = sphere_center.getVectZ();

		//Using the formaula for a quadratic equation after equating the equation for a sphere with the equation for a ray set "a","b" and "c"
		//double a = 1; //normalized
		double b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_y - sphere_center_y)*ray_direction_y) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
		double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);

		//calculate the discriminant within the quadratic formula
		double discriminant = b*b - 4*c;

		// if the discriminant is greater than 0 then the quadratic equation has two real solutions and the ray intersects with the Sphere twice
		if (discriminant > 0) {

			//consider the first root, using -0.0000001 to ensure the point is on the outside of sphere not on the inside
			double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.0000001;

			//if the first root is greater than 0
			if (root_1 > 0){
				//first root is the smallest positive root
				return root_1;
			}
			else{
				//the second root is smallest positive root
				double root_2 = ((-1*b + sqrt(discriminant))/2) - 0.0000001;
				return root_2;
			}
		}
		// if the determinant is 0 then the ray just intersects the Sphere in one location but this is not useful as these points will be found by rays that intersect the sphere twice.
		// if the determinant is less than 0 then the quadratic equation has no real roots.
		else {
			//ray missed the sphere
			return -1;
		}
	}


};

//set default values for an unspecified Sphere
Sphere::Sphere() {
	center = Vect(0,0,0);
	radius = 1.0;
	colour = Colour(0.5,0.5,0.5, 0);
}
//Allow specified values for a Sphere to be set
Sphere::Sphere(Vect centerValue, double radiusValue, Colour colourValue) {
	center = centerValue;
	radius = radiusValue;
	colour = colourValue;
}

#endif

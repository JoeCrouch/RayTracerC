//Create a Plane class
#ifndef _PLANE_H
#define _PLANE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Colour.h"

//Set Plane as a Subclass of the Object class
//Set Plane to take a normal a distance and a colour
class Plane : public Object {
	Vect normal;
	double distance;
	Colour colour;

	public:

	Plane ();

	Plane(Vect, double, Colour);

	//functions for returning information about a Plane
	Vect getPlaneNormal () { return normal; }
	double getPlaneDistance () { return distance; }
	virtual Colour getColour () { return colour; }

	//function for returning the normal to a Plane object
	virtual Vect getNormalAt (Vect point) {
		return normal;
	}

	//function for finding intersection between a plane and a ray
	virtual double findIntersection(Ray ray) {
		//Collect the direction of the ray
		Vect ray_direction = ray.getRayDirection();

		//collect angle between the ray and the normal of the plane
		double a = ray_direction.dotProduct(normal);

		//if the ray is parallel to the plane return -1
		if (a == 0) {
			return -1;
		}
		//if the ray intersects the plane
		else {
			double b = normal.dotProduct(ray.getRayOrigin().vectAdd(normal.vectMult(distance).negative()));
			return -1*b/a;
		}
	}


};

//set default values for an unspecified plane
Plane::Plane() {
	normal = Vect(1,0,0);
	distance = 0;
	colour = Colour(0,0,1, 0);
}

//allow specified values for a plane class
Plane::Plane(Vect normalValue, double distanceValue, Colour colourValue) {
	normal = normalValue;
	distance = distanceValue;
	colour = colourValue;
}

#endif

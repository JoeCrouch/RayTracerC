// Create a Vector class
#ifndef _VECT_H
#define _VECT_H

#include "math.h"

// set vector to take 3 double values, i.e a 3 dimensional coordinate
class Vect {
	double x, y, z;

	public:

	Vect ();

	Vect(double, double, double);

	// functions for returning specified values of a vector
	double getVectX() { return x; }
	double getVectY() { return y; }
	double getVectZ() { return z; }

	//function for calculating the magnitude of a vector
	double magnitude () {
		return sqrt((x*x)+(y*y)+(z*z));
	}

	//function for normalizing a vector
	Vect normalize () {
		double magnitude = sqrt((x*x)+(y*y)+(z*z));
		return Vect (x/magnitude, y/magnitude, z/magnitude);
	}

	//function for calculating -1 * a vector
	Vect negative (){
	return Vect (-x, -y, -z);
	}

	//function for calculating the dot product of two vectors
	double dotProduct(Vect v){
		return x*v.getVectX() + y*v.getVectY() + z*v.getVectZ();
	}

	//function for calculating the cross product of two vectors
	Vect crossProduct(Vect v){
		return Vect(y*v.getVectZ()-z*v.getVectY(),z*v.getVectX()-x*v.getVectZ(),x*v.getVectY()-y*v.getVectX());
	}

	//function for adding two vectors together
	Vect vectAdd (Vect v){
		return Vect (x + v.getVectX(), y + v.getVectY(), z + v.getVectZ());
	}

	//function for multiplying a vector by a scalar
	Vect vectMult (double scalar){
		return Vect (x*scalar, y*scalar, z*scalar);
	}
};

// set default value for an unspecified vector
Vect::Vect() {
	x = 0;
	y = 0;
	z = 0;
}

//allow specified values to be set for a vector
Vect::Vect(double i, double j, double k ) {
	x = i;
	y = j;
	z = k;
}

#endif

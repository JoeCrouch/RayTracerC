//Creat a Ray class
#ifndef _RAY_H
#define _RAY_H

#include "Vect.h"

//Set ray to take an origin and a direction
class Ray {
	Vect origin, direction;

	public:

	Ray ();

	Ray(Vect, Vect);

	//functions for returning values for a ray
	Vect getRayOrigin () { return origin; }
	Vect getRayDirection () { return direction; }


};
//set default values for an unspecified ray
Ray::Ray() {
	origin = Vect(0,0,0);
	direction = Vect(1,0,0);
}
//allow specified values for a ray
Ray::Ray(Vect o, Vect d) {
	origin = o;
	direction = d;
}

#endif

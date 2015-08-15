// Create an Object class
#ifndef _OBJECT_H
#define _OBJECT_H

#include "Ray.h"
#include "Vect.h"
#include "Colour.h"

class Object {
	public:

	Object ();

	// function for returning the colour of an object
	virtual Colour getColour () { return Colour (0.0,0.0,0.0,0.0); }

	//function for returning the normal at a point on an object
	virtual Vect getNormalAt(Vect intersection_position){
		return Vect (0,0,0);
	}

	//function for finding the intersection of an object with a ray
	virtual double findIntersection(Ray ray) {
		return 0;
	}

};

Object::Object() {}

#endif

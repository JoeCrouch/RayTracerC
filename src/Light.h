//Create a Light class
#ifndef _LIGHT_H
#define _LIGHT_H

//include headers for vector functions colours and soource
#include "Source.h"
#include "Vect.h"
#include "Colour.h"

//Set light class as a subclass of the source class
class Light : public Source {
	//set class to have a postion vector and a colour
	Vect position;
	Colour colour;	
	
	public:
	
	Light ();
	
	Light(Vect, Colour);
	
	// functions for light class
	//collect position of light
	virtual Vect getLightPosition () { return position; }
	//collect colour of light
	virtual Colour getLightColour () { return colour; }
	
	
};

//set standard positon vector and colour for unspecified light
Light::Light() {
	position = Vect(0,0,0);
	colour = Colour(1,1,1, 0);
}

//Allow light to have specified vector position and colour
Light::Light(Vect p, Colour c) {
	position = p;
	colour = c;
}

#endif

//Create a Camera class
#ifndef _Camera_H
#define _Camera_H

// Include vector header
#include "Vect.h"

//Set Camera to take a position a direction and a right and down direction
class Camera {
	Vect campos, camdir, camright, camdown;
	
	public:
	
	Camera ();
	
	Camera(Vect, Vect, Vect, Vect);
	
	// fuctions for returning information about a Camera class
	Vect getCameraPosition () { return campos; }
	Vect getCameraDirection () { return camdir; }
	Vect getCameraRight() { return camright; }
	Vect getCameraDown () { return camdown; }
	
	
};

//set default values for an unspecified camera class
Camera::Camera() {
	campos = Vect(0,0,0);
	camdir = Vect(0,0,1);
	camright = Vect(0,0,0);
	camdown = Vect(0,0,0);
}

//Allow specific values for a camera class
Camera::Camera(Vect pos, Vect dir, Vect right, Vect down) {
	campos = pos;
	camdir = dir;
	camright = right;
	camdown = down;
}

#endif

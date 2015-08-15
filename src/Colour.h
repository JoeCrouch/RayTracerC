//Create a colour class
#ifndef _COLOUR_H
#define _COLOUR_H

//set colour class to contain a value for red, green, blue and special. Special used for shinyness etc
class Colour {
	double red, green, blue, special;

	public:

	Colour ();
	Colour(double, double, double, double);

	//functions for colour class
	//functions for returning values given a colour
	double getColourRed() { return red; }
	double getColourGreen() { return green; }
	double getColourBlue() { return blue; }
	double getColourSpecial() { return special; }

	//functions for changing specific values for a colour
	void setColourRed(double redValue) { red = redValue; }
	void setColourGreen(double greenValue) { green = greenValue; }
	void setColourBlue(double blueValue) { blue = blueValue; }
	void setColourSpecial(double specialValue) { special = specialValue; };

	//create a fuction for brightness equal to the mean of the colours
	double brightness() {
		return(red + green + blue)/3;
	}
	//funtcion for multiplying a colour values by a scalar
	Colour colourScalar(double scalar){
		return Colour (red*scalar, green*scalar, blue*scalar, special);
	}

	//function for adding two colours together
	Colour colourAdd(Colour colour) {
		return Colour (red + colour.getColourRed(), green + colour.getColourGreen(), blue + colour.getColourBlue(), special);
	}

	//function for calculating "multiplication" of two colours
	Colour colourMult(Colour colour){
		return Colour (red*colour.getColourRed(), green*colour.getColourGreen(), blue*colour.getColourBlue(), special);
	}
	//function for calculating "average" of two colours
	Colour colourAverage(Colour colour){
		return Colour ((red + colour.getColourRed())/2, (green + colour.getColourGreen())/2, (blue + colour.getColourBlue())/2, special);
	}
	// function for making sure values in colour are between 0 and 1
	Colour clip() {
		//calculate sum of the colour values in colour
		double all_light = red + green + blue;
		//calculate how much this value is over 3
		double excesslight = all_light-3;
		//if there is excess colour
		if(excesslight > 0){
			//set colour values in colour to be the colour plus a fraction of the excess colour
			red = red + excesslight*(red/all_light);
			green = green + excesslight*(green/all_light);
			blue = blue + excesslight*(blue/all_light);
		}
		//force red, green and blue to be between 0 and 1
		if (red > 1) {red = 1;}
		if (green > 1) {green = 1;}
		if (blue > 1) {blue = 1;}
		if (red < 0) {red = 0;}
		if (green < 0) {green = 0;}
		if (blue < 0) {blue = 0;}

		//return colour
		return Colour (red, green, blue, special);
	}
};

//set standard value for colour as grey if unspecified
Colour::Colour() {
	red = 0.5;
	green = 0.5;
	blue = 0.5;
}

// allow to specify values for colour
Colour::Colour(double r, double g, double b, double s ) {
	red = r;
	green = g;
	blue = b;
	special = s;
}

#endif

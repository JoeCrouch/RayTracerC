//Create a source class
#ifndef _SOURCE_H
#define _SOURCE_H

class Source{
	public:
	
	Source();
	
	//functions for returning values of a source class
	virtual Vect getLightPosition() {return Vect(0,0,0);}
	virtual Colour getLightColour() {return Colour(1,1,1,0);}
	
};

Source::Source() {}

#endif

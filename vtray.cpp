// implement vtray main here
#include <iostream>

#include "json_parser.hpp"
#include "render_image.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	//JSONParser a;
	//a.readFile("scene1.json");
	//a.pullAllInfo();
	//a.printAllInfo();
	Render b("scene1.json", true);
	b.findAllIntersect();
	b.createImage("output.png");
	//JSONParser c;
	//string hello = c.readEntireJsonIntoString("scene4.json");

	return 0;
}
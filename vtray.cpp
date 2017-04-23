// implement vtray main here
#include <iostream>

#include "json_parser.hpp"
#include "render_image.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	//JSONParser a;
	Render b("scene0.json");
	//a.readFile("scene1.json");
	//a.pullAllInfo();
	//a.printAllInfo();
	b.createImage("output.png");
	
	return 0;
}
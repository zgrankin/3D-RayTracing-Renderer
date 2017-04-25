// implement vtray main here
#include <iostream>
#include "json_parser.hpp"
#include "render_image.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	Render b("scene2.json", true);
	b.findAllIntersect();
	b.createImage("output.png");
	return 0;
}
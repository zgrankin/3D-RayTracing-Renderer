// implement vtray main here
#include <iostream>

#include "json_parser.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	JSONParser a;
	a.readAll("scene0.json");
	a.pullLightInfo();

	cout << a.lightsVect[0].intensity << endl;
	cout << a.lightsVect[0].theLocation.x << endl;
	cout << a.lightsVect[0].theLocation.y << endl;
	cout << a.lightsVect[0].theLocation.z << endl;
	return 0;
}
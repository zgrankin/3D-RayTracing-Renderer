#ifndef _RENDER_
#define _RENDER_

#include <string>
#include <qimage.h>
#include "json_parser.hpp"

using namespace std;

struct PointNColor {
	Location point;
	Color color;
};


class Render
{
public:
	Render(string filename);

	~Render();

	void createImage(string filename);

	Location findFocalPoint(); // find the coordinate values of the focus point
	Location findL(Location centerObject, Location focalPoint); // distance from the focus to the center of the object
	Location findDuv(Location point, Location focalPoint);
	double magnitude(Location theVector);
	double findTca(Location L, Location Duv);
	Location findW(Location focalPoint, Location Duv, double tca);
	double findD(Location w, Location centerObject);
	double findThc(double radius, double d);
	Location findClosestIntersect(Location focalPoint, Location Duv, double tca, double thc);
	bool calculateIntersect(Location point);
	void findAllIntersect();



	void autoexposure();

private:
	JSONParser json;
	QImage image;

	Camera camera;
	vector<Lights> lightsVect;
	vector<Objects> objectsVect;

	vector<PointNColor> pixels;
	Color black;
	PointNColor intersectionP;
};

#endif
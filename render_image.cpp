#include "render_image.hpp"

#include <qimage.h>
#include <qstring.h>
#include <qfile.h>
#include <cmath>

Render::Render(string filename)
{
	json.readFile(QString::fromStdString(filename));
	json.pullAllInfo();
	camera = json.returnCamera();
	lightsVect = json.returnLightsVect();
	objectsVect = json.returnObjectsVect();
}

Render::~Render()
{

}

void Render::createImage(string filename)
{
	image = QImage(camera.size.first, camera.size.second, QImage::Format_ARGB32);
	/*for (unsigned int i = 0; i < image.width() * image.height(); i++)
	{
		image.setPixelColor(i % image.width(), i / image.width(), QColor(255, 0, 0, 255));
	}*/

	for (unsigned int i = 0; i < image.width(); i++)
		for (unsigned int j = 0; j < image.height(); j++)
		{
			image.setPixelColor(i, j, QColor(255, 0, 0, 255));
		}

	QFile outputFile(QString::fromStdString(filename));
	outputFile.open(QIODevice::WriteOnly);
	image.save(&outputFile, "PNG");
}

Location Render::findFocalPoint()
{
	Location focalPoint;
	// focal point is private variable being modified
	if (camera.normal.x != 0) {
		focalPoint = camera.center;
		focalPoint.x = camera.center.x - camera.focus;
	}
	else if (camera.normal.y != 0) {
		focalPoint = camera.center;
		focalPoint.y = camera.center.y - camera.focus;
	}
	else if (camera.normal.z != 0) {
		focalPoint = camera.center;
		focalPoint.z = camera.center.z - camera.focus;
	}

	return focalPoint;
}

Location Render::findL(Location centerObject, Location focalPoint)
{
	Location Lvect;

	Lvect.x = centerObject.x - focalPoint.x;
	Lvect.y = centerObject.y - focalPoint.y;
	Lvect.z = centerObject.z - focalPoint.z;

	return Lvect;
}

Location Render::findDuv(Location point, Location focalPoint)
{
	Location Duv;

	Duv.x = point.x - focalPoint.x;
	Duv.y = point.y - focalPoint.y;
	Duv.z = point.z - focalPoint.z;

	double mag = magnitudeD(Duv);

	Duv.x = Duv.x / mag;
	Duv.y = Duv.y / mag;
	Duv.z = Duv.z / mag;

	return Duv;
}

double Render::magnitudeD(Location Duv)
{
	return sqrt(pow(Duv.x, 2) + pow(Duv.y, 2) + pow(Duv.z, 2));
}

double Render::findTca(Location L, Location Duv)
{
	return L.x * Duv.x + L.y * Duv.y + L.z * Duv.z;
}

Location Render::findW(Location focalPoint, Location Duv, double tca)
{
	Location w;

	w.x = focalPoint.x + (Duv.x * tca);
	w.y = focalPoint.y + (Duv.y * tca);
	w.z = focalPoint.z + (Duv.z * tca);

	return w;
}

double Render::findD(Location w, Location centerObject)
{
	return sqrt(pow((w.x - centerObject.x), 2) + pow((w.y - centerObject.y), 2) + pow((w.z - centerObject.z), 2));
}

double Render::findThc(double radius, double d)
{
	double thc;
	// if d > radius there will be no intersection therefore no future calculations need be made for the object
	if (d > radius) {
		thc = -1;
	}
	else {
		thc = sqrt(pow(radius, 2) - pow(d, 2));
	}

	return thc;
}

Location Render::findClosestIntersect(Location focalPoint, Location Duv, double tca, double thc)
{
	Location intersect1, intersect2, closestIntersect;
	double magnitude1, magnitude2;
	intersect1.x = focalPoint.x + (Duv.x * (tca - thc));
	intersect1.y = focalPoint.y + (Duv.y * (tca - thc));
	intersect1.z = focalPoint.z + (Duv.z * (tca - thc));

	intersect2.x = focalPoint.x + (Duv.x * (tca + thc));
	intersect2.y = focalPoint.y + (Duv.y * (tca + thc));
	intersect2.z = focalPoint.z + (Duv.z * (tca + thc));

	magnitude1 = sqrt(pow((intersect1.x - focalPoint.x), 2) + pow((intersect1.y - focalPoint.y), 2) + pow((intersect1.z - focalPoint.z), 2));
	magnitude2 = sqrt(pow((intersect2.x - focalPoint.x), 2) + pow((intersect2.y - focalPoint.y), 2) + pow((intersect2.z - focalPoint.z), 2));

	if (magnitude1 < magnitude2)
		closestIntersect = intersect1;
	else if (magnitude2 < magnitude1)
		closestIntersect = intersect2;

	return closestIntersect;
}

bool Render::calculateIntersect()
{
	// point I want to send the direction ray through on the camera
	Location point; point.x = 0; point.y = 0; point.z = 0;

	Location F = findFocalPoint();
	Location L = findL(objectsVect[0].center, F);
	Location duv = findDuv(point, F);
	double tca = findTca(L, duv);
	Location w = findW(F, duv, tca);
	double d = findD(w, objectsVect[0].center);
	double thc = findThc(objectsVect[0].radius, d);
	if (thc == -1)
		return false;
	else {
		intersectPoints.push_back( findClosestIntersect(F, duv, tca, thc) );
		return true;
	}
}
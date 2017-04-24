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

	black.r = 0;
	black.g = 0;
	black.b = 0;
}

Render::~Render()
{

}

void Render::createImage(string filename)
{
	image = QImage(camera.size.first, camera.size.second, QImage::Format_ARGB32);

	/*for (unsigned int j = 0; j < image.height(); j++)
		for (unsigned int i = 0; i < image.width(); i++)
		{
			image.setPixelColor(i, j, QColor(255, 0, 0, 255));
		}*/
	int pos = 0;
	for (unsigned int j = 0; j < image.height(); j++)
		for (unsigned int i = 0; i < image.width(); i++)
		{
			image.setPixelColor(i, j, QColor(pixels[pos].color.r, pixels[pos].color.g, pixels[pos].color.b, 255));
			pos++;
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

	double mag = magnitude(Duv);

	Duv.x = Duv.x / mag;
	Duv.y = Duv.y / mag;
	Duv.z = Duv.z / mag;

	return Duv;
}

double Render::magnitude(Location theVector)
{
	return sqrt(pow(theVector.x, 2) + pow(theVector.y, 2) + pow(theVector.z, 2));
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

	magnitude1 = magnitude(intersect1);
	magnitude2 = magnitude(intersect2);

	if (magnitude1 < magnitude2)
		closestIntersect = intersect1;
	else if (magnitude2 < magnitude1)
		closestIntersect = intersect2;
	else
		closestIntersect = intersect1;

	return closestIntersect;
}

bool Render::calculateIntersect(Location point)
{
	vector<PointNColor> ip;
	for (unsigned int objNumber = 0; objNumber < objectsVect.size(); objNumber++) 
	{
		Location F = findFocalPoint();
		Location L = findL(objectsVect[objNumber].center, F);
		Location duv = findDuv(point, F);
		double tca = findTca(L, duv);
		Location w = findW(F, duv, tca);
		double d = findD(w, objectsVect[objNumber].center);
		double thc = findThc(objectsVect[objNumber].radius, d);

		if (thc != -1)
		{
			PointNColor intersect;
			intersect.point = findClosestIntersect(F, duv, tca, thc);
			intersect.color = objectsVect[objNumber].color;
			ip.push_back(intersect);
		}
	}

	if (ip.empty()) {
		return false;
	}
	else
	{
		int minMagIndex;
		if (ip.size() > 1)
		{
			int a;
			a = 1;
		}

		double maggy;
		maggy = magnitude(findL(ip[0].point, findFocalPoint()));
		minMagIndex = 0;
		for (unsigned int i = 1; i < ip.size(); i++)
		{
			
			if (magnitude(findL(ip[i].point, findFocalPoint())) <= maggy)
			{
				minMagIndex = i;
			}
		}
		intersectionP = ip[minMagIndex];
		return true;
	}
}

void Render::findAllIntersect()
{
	Location point;
	PointNColor pixel;
	double startx, endx;
	double starty, endy;
	startx = -1 * (camera.size.first) / 2;
	endx = (camera.size.first) / 2;
	starty = -1 * (camera.size.second) / 2;
	endy = (camera.size.second) / 2;

	for (double j = starty; j < endy; j++)
		for (double i = startx; i < endx; i++)
	{
		point.x = i * camera.resolution.first;
		point.y = j * camera.resolution.second;
		point.z = camera.center.z; // need to fix to handle facing different directions
		if (calculateIntersect(point))
		{
			pixel.point.x = i;
			pixel.point.y = j;
			pixel.color = intersectionP.color;
			pixels.push_back(pixel);
		}
		else
		{
			pixel.point.x = i;
			pixel.point.y = j;
			pixel.color = black;
			pixels.push_back(pixel);
		}
	}
}


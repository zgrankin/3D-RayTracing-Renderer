#include "render_image.hpp"

#include <qimage.h>
#include <qstring.h>
#include <qfile.h>
#include <cmath>

Render::Render()
{
	black.r = 0;
	black.g = 0;
	black.b = 0;
	white.r = 255;
	white.g = 255;
	white.b = 255;
}

Render::Render(string filename, bool isFile)
{
	string text = filename;
	if (isFile) {
		json.readFile(QString::fromStdString(filename)); json.pullAllInfo();
		camera = json.returnCamera(); lightsVect = json.returnLightsVect(); objectsVect = json.returnObjectsVect();
	}
	else if (!isFile) {
		json.JSONDocFromString(text); json.pullAllInfo();
		camera = json.returnCamera(); lightsVect = json.returnLightsVect(); objectsVect = json.returnObjectsVect();
	}
	black.r = 0;  black.g = 0; black.b = 0; white.r = 255; white.g = 255; white.b = 255;
}

Render::~Render()
{

}

void Render::createImage(string filename)
{
	image = QImage(camera.size.first, camera.size.second, QImage::Format_ARGB32);
	autoexposure(); // execute this to scale down the colors before creating image
	int pos = 0;
	for (unsigned int j = 0; j < image.height(); j++)
		for (unsigned int i = 0; i < image.width(); i++) {
			QColor color(pixels[pos].color.r, pixels[pos].color.g, pixels[pos].color.b);
			image.setPixel(i, j, color.rgb());
			pos++;
		}
	QFile outputFile(QString::fromStdString(filename));
	outputFile.open(QIODevice::WriteOnly);
	if (!filename.empty()) {
		image.save(&outputFile, "PNG");
	}
}

void Render::multiplyColorByScale(Color &theColor, double scale)
{
	Location vect;
	theColor.r *= scale;
	theColor.g *= scale;
	theColor.b *= scale;
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
	Duv = findL(point, focalPoint);
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

Location Render::findW(Location focalPoint, Location Duv, double tca)
{
	Location w;

	w.x = focalPoint.x + (Duv.x * tca);
	w.y = focalPoint.y + (Duv.y * tca);
	w.z = focalPoint.z + (Duv.z * tca);

	return w;
}

double Render::findMagnitudeFirstMinusSecond(Location w, Location centerObject)
{
	return sqrt(pow((w.x - centerObject.x), 2) + pow((w.y - centerObject.y), 2) + pow((w.z - centerObject.z), 2));
}

double Render::findDotProduct(Location first, Location second)
{
	return first.x * second.x + first.y * second.y + first.z * second.z;
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
	magnitude1 = magnitude(intersect1);

	intersect2.x = focalPoint.x + (Duv.x * (tca + thc));
	intersect2.y = focalPoint.y + (Duv.y * (tca + thc));
	intersect2.z = focalPoint.z + (Duv.z * (tca + thc));
	magnitude2 = magnitude(intersect2);

	if ((tca - thc) > 0 && (tca + thc) < 0)
		closestIntersect = intersect1;
	else if ((tca - thc) < 0 && (tca + thc) > 0)
		closestIntersect = intersect2;
	else if ((tca - thc) < 0 && (tca + thc) < 0) {
		closestIntersect = intersect1;
		closestIntersect.bad = true;
	}
	else if (magnitude1 < magnitude2)
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
	PointNColor intersect;
	vector<double> objLambertVect, objNumbVect;
	for (unsigned int objNumber = 0; objNumber < objectsVect.size(); objNumber++) {
		if (objectsVect[objNumber].type == "sphere") {
			Location F = findFocalPoint();
			Location L = findL(objectsVect[objNumber].center, F);
			Location duv = findDuv(point, F);
			double tca = findDotProduct(L, duv);
			Location w = findW(F, duv, tca);
			double d = findMagnitudeFirstMinusSecond(w, objectsVect[objNumber].center);
			double thc = findThc(objectsVect[objNumber].radius, d);
			if (thc != -1) {
				intersect.point = findClosestIntersect(F, duv, tca, thc);
				if (intersect.point.bad == false) {
					intersect.color = objectsVect[objNumber].color;
					ip.push_back(intersect);
					objLambertVect.push_back(objectsVect[objNumber].lambert);
					objNumbVect.push_back(objNumber);
				}
			}
		}
		else if (objectsVect[objNumber].type == "plane") {
			intersect.point = findIntersectPlane(objectsVect[objNumber].center, objNumber, point, false);
			if (intersect.point.bad == false) {
				intersect.color = objectsVect[objNumber].color;
				ip.push_back(intersect);
				objLambertVect.push_back(objectsVect[objNumber].lambert);
				objNumbVect.push_back(objNumber);
			}
		}
	}
	if (determineClosestObject(ip, objLambertVect, objNumbVect)) return true; // will modify private variable value to set current intersectionP we will be looking at
	else return false;
}

bool Render::determineClosestObject(vector<PointNColor> ip, vector<double> objLambertVect, vector<double> objNumbVect)
{
	if (ip.empty()) {
		return false;
	}
	else {
		int minMagIndex;
		if (ip.size() > 1) {
			int a;
			a = 1;
		}
		double maggy;
		maggy = magnitude(findL(ip[0].point, findFocalPoint()));
		minMagIndex = 0;
		double newMag;
		for (unsigned int i = 1; i < ip.size(); i++)
		{
			newMag = magnitude(findL(ip[i].point, findFocalPoint()));
			if (newMag <= maggy) {
				maggy = newMag;
				minMagIndex = i;
			}
		}
		intersectionP = ip[minMagIndex];
		currentObjectLambert = objLambertVect[minMagIndex];
		currentObjectNumber = objNumbVect[minMagIndex];
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
			if (i == 0 && j == 0)
			{
				int a;
				a = 1;
			}
			pixel.point.x = i;
			pixel.point.y = j;
			
			if (objectsVect[currentObjectNumber].type == "sphere")
				pixel.color = findLightContributionSphere(intersectionP.point, objectsVect[currentObjectNumber].center, lightsVect);
			else if (objectsVect[currentObjectNumber].type == "plane")
				pixel.color = findLightContributionPlane(intersectionP.point, objectsVect[currentObjectNumber].center, lightsVect, objectsVect[currentObjectNumber].normal);
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

Color Render::findLightContributionSphere(Location crossPoint, Location centerSphere, vector<Lights> lightSources)
{
	Location nu, lu;
	double lightScaleFactor;
	Color color;
	double lambert = currentObjectLambert;
	vector<Color> colorSummation;
	nu = findDuv(crossPoint, centerSphere); // normal to sphere unit vector
	double lightMag;
	for (int i = 0; i < lightSources.size(); i++)
	{
		lightMag = magnitude(findL(lightSources[i].theLocation, crossPoint));
		color = intersectionP.color;
		lu = findDuv(lightSources[i].theLocation, crossPoint); // normal to light unit vector
		lightScaleFactor = findDotProduct(nu, lu) * lightSources[i].intensity * lambert;
		if (lightScaleFactor < 0 || shadowIntersect(crossPoint, lu, lightMag,currentObjectNumber))  {
			lightScaleFactor = 0;
		}
		multiplyColorByScale(color, lightScaleFactor); // modify the color by reference
		colorSummation.push_back(color); // push scaled color into vector to be used later for summation
	}

	color = black;
	for (int i = 0; i < colorSummation.size(); i++)
	{
		color.r += colorSummation[i].r;
		color.g += colorSummation[i].g;
		color.b += colorSummation[i].b;
	}

	return color;
}

Color Render::findLightContributionPlane(Location crossPoint, Location centerPlane, vector<Lights> lightSources, Location nu)
{
	Location lu;
	double lightScaleFactor;
	Color color;
	double lambert = currentObjectLambert;
	vector<Color> colorSummation;
	double lightMag;
	for (int i = 0; i < lightSources.size(); i++)
	{
		lightMag = magnitude(findL(lightSources[i].theLocation, crossPoint));
		color = intersectionP.color;
		lu = findDuv(lightSources[i].theLocation, crossPoint); // normal to light unit vector
		lightScaleFactor = findDotProduct(nu, lu) * lightSources[i].intensity * lambert;
		if (lightScaleFactor < 0 || shadowIntersect(crossPoint, lu, lightMag, currentObjectNumber)) {
			lightScaleFactor = 0;
		}
		multiplyColorByScale(color, lightScaleFactor); // modify the color by reference
		colorSummation.push_back(color); // push scaled color into vector to be used later for summation
	}

	color = black;
	for (int i = 0; i < colorSummation.size(); i++)
	{
		color.r += colorSummation[i].r;
		color.g += colorSummation[i].g;
		color.b += colorSummation[i].b;
	}

	return color;
}

Location Render::findIntersectPlane(Location centerPlane, double objNumber, Location point, bool searchShadow) // if search shadow is true then the point passed in is actually light unit vector
{
	Location focalPoint;
	Location rayDu, nu;
	if (!searchShadow) {
		rayDu = findDuv(point, findFocalPoint()); // equivalent to Ru in diagram
		focalPoint = findFocalPoint();
	}
	else if (searchShadow) {
		rayDu = point;
		focalPoint = objectFocalForShadow;
	}
	nu; // normal unit vector
	nu.x = objectsVect[objNumber].normal.x / magnitude(objectsVect[objNumber].normal);
	nu.y = objectsVect[objNumber].normal.y / magnitude(objectsVect[objNumber].normal);
	nu.z = objectsVect[objNumber].normal.z / magnitude(objectsVect[objNumber].normal);
	double D = findDotProduct(rayDu, nu);
	Location planeIntersect;
	if (D != 0)
	{
		Location focalToCenter = findL(objectsVect[objNumber].center, focalPoint);
		double distFocalToPlaneNormal = findDotProduct(focalToCenter, nu);
		double iterationsToPlane = distFocalToPlaneNormal / D; // t in diagram
		planeIntersect.x = focalPoint.x + iterationsToPlane * rayDu.x;
		planeIntersect.y = focalPoint.y + iterationsToPlane * rayDu.y;
		planeIntersect.z = focalPoint.z + iterationsToPlane * rayDu.z;

		if (iterationsToPlane < 0) {
			planeIntersect.bad = true;
		}
		else if (iterationsToPlane > 0) {
			planeIntersect.bad = false;
		}
	}
	else {
		planeIntersect.bad = true;
	}

	return planeIntersect;
}

bool Render::shadowIntersect(Location F, Location lu, double lightMag, unsigned int objectIndex) // where F is intersection point on object
{
	vector<PointNColor> ip;
	PointNColor intersect;
	objectFocalForShadow = F;
	vector<double> objLambertVect, objNumbVect;
	for (unsigned int objNumber = 0; objNumber < objectsVect.size(); objNumber++) {
		if (objectIndex != objNumber) {
			if (objectsVect[objNumber].type == "sphere") {
				Location L = findL(objectsVect[objNumber].center, F);
				double tca = findDotProduct(L, lu);
				Location w = findW(F, lu, tca);
				double d = findMagnitudeFirstMinusSecond(w, objectsVect[objNumber].center);
				double thc = findThc(objectsVect[objNumber].radius, d);
				if (thc != -1) {
					intersect.point = findClosestIntersect(F, lu, tca, thc);
					if (intersect.point.bad == false && (magnitude(findL(intersect.point, F)) < lightMag) && 
						findDotProduct(findL(intersect.point, camera.center), camera.normal) > 0) {
						return true;
					}
				}
			}
			else if (objectsVect[objNumber].type == "plane") {
				intersect.point = findIntersectPlane(objectsVect[objNumber].center, objNumber, lu, true);
				if (intersect.point.bad == false && (magnitude(findL(intersect.point, F)) < lightMag) &&
					findDotProduct(findL(intersect.point, camera.center), camera.normal) > 0) {
					return true;
				}
			}
		}
	}
	return false;
}

void Render::autoexposure()
{
	const double maxColor = 255;
	double highestColor = 255;
	for (unsigned int i = 0; i < image.width() * image.height(); i++)
	{
		highestColor = (pixels[i].color.r > highestColor) ? pixels[i].color.r :
					   (pixels[i].color.g > highestColor) ? pixels[i].color.g :
					   (pixels[i].color.b > highestColor) ? pixels[i].color.b :
						highestColor;
	}

	double scale = maxColor / highestColor;
	for (unsigned int i = 0; i < image.width() * image.height(); i++)
	{
		pixels[i].color.r *= scale;
		pixels[i].color.g *= scale;
		pixels[i].color.b *= scale;
	}
}

void Render::setCameraValues(Camera cam)
{
	camera = cam;
}

void Render::setObjectsValues(Objects obj)
{
	objectsVect.push_back(obj);
}

void Render::setLightsValues(Lights lit)
{
	lightsVect.push_back(lit);
}

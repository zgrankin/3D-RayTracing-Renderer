#ifndef _RENDER_
#define _RENDER_

#include <string>
#include <qimage.h>
#include <qcolor.h>
#include <thread>
#include <mutex>
#include "json_parser.hpp"

using namespace std;

struct PointNColor {
	Location point;
	Color color;
};


class Render
{
public:
	Render();
	Render(string filename, bool isFile);

	~Render();

	void createImage(string filename);

	Location findFocalPoint(); // find the coordinate values of the focus point
	Location findL(Location centerObject, Location focalPoint); // distance from the focus to the center of the object
	Location findDuv(Location point, Location focalPoint);
	double magnitude(Location theVector);
	Location findW(Location focalPoint, Location Duv, double tca);
	double findMagnitudeFirstMinusSecond(Location w, Location centerObject);
	double findDotProduct(Location first, Location second);
	double findThc(double radius, double d);
	Location findClosestIntersect(Location focalPoint, Location Duv, double tca, double thc);
	bool calculateIntersect(Location point, PointNColor &intersectionP, double &currentObjectLambert, double &currentObjectNumber);
	bool determineClosestObject(vector<PointNColor> ip, vector<double> objLambertVect, vector<double> objNumbVect, PointNColor &intersectionP, double &currentObjectLambert, double &currentObjectNumber);
	void findAllIntersect(int numThreads, int curThread);
	Color findLightContributionSphere(Location crossPoint, Location centerSphere, vector<Lights> lightSources, PointNColor intersectionP, double currentObjectLambert, double currentObjectNumber);
	Color findLightContributionPlane(Location crossPoint, Location centerPlane, vector<Lights> lightSources, Location nu, PointNColor intersectionP, double currentObjectLambert, double currentObjectNumber);

	Location findIntersectPlane(Location centerPlane, double objNumber, Location point, bool searchShadow, Location objectFocalForShadow);

	bool shadowIntersect(Location F, Location lu, double lightMag, unsigned int objectIndex);

	void multiplyColorByScale(Color &theColor, double scale);
	Location multiplyVectorByScale(Location loc, double scale);
	void autoexposure();

	void createThreads(unsigned int numThread);

	// Test functions
	void setCameraValues(Camera cam);
	//void setObjectsValues(Objects obj);
	//void setLightsValues(Lights lit);

private:
	Location computeCrossProduct(Location a, Location b);
	Location findpanRuv(Location cnuv, Location upuv);
	Location findpanUuv(Location panRuv, Location cnuv);

	JSONParser json;
	QImage image;

	Camera camera;
	vector<Lights> lightsVect;
	vector<Objects> objectsVect;
	vector<PointNColor> pixels;
	Color black;
	Color white;

	Location emptyLoc;

	Location upuv;
	Location panRuv;
	Location panUuv;
};

#endif
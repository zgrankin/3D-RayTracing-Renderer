#ifndef _JSON_PARSER_
#define _JSON_PARSER_

#include <QString>
#include <vector>

#include <qjsonobject.h>
#include <qjsondocument.h>

using namespace std;

struct Location {
	double x;
	double y;
	double z;
	bool bad = false;
};

struct Color {
	double r;
	double g;
	double b;
};


struct Camera {
	Location center;
	Location normal;
	double focus;
	pair<double, double> resolution;	
	pair<double, double> size;
};

struct Lights {
	double intensity;
	Location theLocation;
};

struct Objects {
	string type;
	Location center;
	Color color;
	double lambert;

	double radius;
	Location normal;
};



class JSONParser {
public:
	JSONParser();
	~JSONParser();

	void readFile(QString filename);

	void JSONDocFromString(string jsonText);

	void pullCameraInfo();

	void pullLightInfo();

	void pullObjectInfo();

	void pullAllInfo();

	void printLightsInfo();

	void printCameraInfo();

	void printObjectsInfo();

	void printAllInfo();

	Camera returnCamera();

	vector<Lights> returnLightsVect();

	vector<Objects> returnObjectsVect();

	double magnitude(Location theVector);

	//string readEntireJsonIntoString(string filename);

private:
	QJsonDocument document;
	QJsonObject renderInfo;
	Camera camera;
	vector<Lights> lightsVect;
	vector<Objects> objectsVect;
};

#endif
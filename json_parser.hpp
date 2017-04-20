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
};

struct Camera {
	
};

struct Lights {
	double intensity;
	Location theLocation;
};

struct Objects {

};



class JSONParser {
public:
	JSONParser();
	~JSONParser();

	void readAll(QString filename);

	void pullCameraInfo();

	void pullLightInfo();

	void pullObjectInfo();

	vector<Lights> lightsVect;

private:
	QJsonDocument document;
	QJsonObject renderInfo;
};

#endif
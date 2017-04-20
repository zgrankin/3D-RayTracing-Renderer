#include "json_parser.hpp"

#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qstring.h>
#include <qfile.h>

JSONParser::JSONParser()
{

}

JSONParser::~JSONParser()
{

}

void JSONParser::readAll(QString filename)
{
	QString temp;
	QFile file;
	file.setFileName(filename);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	temp = file.readAll();
	file.close();
	document = QJsonDocument::fromJson(temp.toUtf8());
	renderInfo = document.object();
}

void JSONParser::pullCameraInfo()
{
	QJsonObject cameraObject = renderInfo["camera"].toObject();
	// store center
	cameraObject["center"].toObject()["x"].toDouble();
	cameraObject["center"].toObject()["y"].toDouble();
	cameraObject["center"].toObject()["z"].toDouble();
	
	// store focus
	cameraObject["focus"].toDouble();

	// store normal
	cameraObject["normal"].toObject()["x"].toDouble();
	cameraObject["normal"].toObject()["y"].toDouble();
	cameraObject["normal"].toObject()["z"].toDouble();

	// store resolution
	cameraObject["resolution"].toArray()[0].toDouble();
	cameraObject["resolution"].toArray()[1].toDouble();

	// store size
	cameraObject["size"].toArray()[1].toDouble();
	cameraObject["size"].toArray()[1].toDouble();
}

void JSONParser::pullLightInfo()
{
	QJsonArray lightsJSONArray = renderInfo["lights"].toArray();
	Lights temp;
	size_t SIZE = lightsJSONArray.size();
	// store center
	for (unsigned int i = 0; i < SIZE; i++)
	{
		temp.intensity = lightsJSONArray[i].toObject()["intensity"].toDouble();
		temp.theLocation.x = lightsJSONArray[i].toObject()["location"].toObject()["x"].toDouble();
		temp.theLocation.y = lightsJSONArray[i].toObject()["location"].toObject()["y"].toDouble();
		temp.theLocation.z = lightsJSONArray[i].toObject()["location"].toObject()["z"].toDouble();
		lightsVect.push_back(temp);
	}

}

#include "json_parser.hpp"

#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qstring.h>
#include <qfile.h>
#include <fstream>
#include <iostream>

JSONParser::JSONParser()
{
	
}

JSONParser::~JSONParser()
{

}

void JSONParser::readFile(QString filename)
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

void JSONParser::JSONDocFromString(string jsonText)
{
	document = QJsonDocument::fromJson(QString::fromStdString(jsonText).toUtf8());
	renderInfo = document.object();
}

void JSONParser::pullCameraInfo()
{
	QJsonObject cameraObject = renderInfo["camera"].toObject();

	try {
		// store center
		camera.center.x = cameraObject["center"].toObject()["x"].toDouble();
		camera.center.y = cameraObject["center"].toObject()["y"].toDouble();
		camera.center.z = cameraObject["center"].toObject()["z"].toDouble();

		// store focus
		camera.focus = cameraObject["focus"].toDouble();

		// store normal
		camera.normal.x = cameraObject["normal"].toObject()["x"].toDouble();
		camera.normal.y = cameraObject["normal"].toObject()["y"].toDouble();
		camera.normal.z = cameraObject["normal"].toObject()["z"].toDouble();

		// store resolution
		camera.resolution.first  = cameraObject["resolution"].toArray()[0].toDouble();
		camera.resolution.second = cameraObject["resolution"].toArray()[1].toDouble();

		// store size
		camera.size.first  = cameraObject["size"].toArray()[1].toDouble();
		camera.size.second = cameraObject["size"].toArray()[1].toDouble();
	}
	catch (exception& e) {
		throw runtime_error("Error: Camera is missing critical information");
	}
}

void JSONParser::pullLightInfo()
{
	QJsonArray lightsJSONArray = renderInfo["lights"].toArray();
	Lights temp;
	size_t SIZE = lightsJSONArray.size();

	for (unsigned int i = 0; i < SIZE; i++)
	{
		try {
			temp.intensity = lightsJSONArray[i].toObject()["intensity"].toDouble();
			temp.theLocation.x = lightsJSONArray[i].toObject()["location"].toObject()["x"].toDouble();
			temp.theLocation.y = lightsJSONArray[i].toObject()["location"].toObject()["y"].toDouble();
			temp.theLocation.z = lightsJSONArray[i].toObject()["location"].toObject()["z"].toDouble();
			lightsVect.push_back(temp);
		}
		catch (exception& e) {
			throw runtime_error("Error: Lights are missing critical information");
		}
	}
}

void JSONParser::pullObjectInfo() 
{
	QJsonArray objectsJSONArray = renderInfo["objects"].toArray();

	Objects temp;
	size_t SIZE = objectsJSONArray.size();

	for (unsigned int i = 0; i < SIZE; i++)
	{
		try {
			temp.center.x = objectsJSONArray[i].toObject()["center"].toObject()["x"].toDouble();
			temp.center.y = objectsJSONArray[i].toObject()["center"].toObject()["y"].toDouble();
			temp.center.z = objectsJSONArray[i].toObject()["center"].toObject()["z"].toDouble();
			temp.color.r = objectsJSONArray[i].toObject()["color"].toObject()["r"].toDouble();
			temp.color.g = objectsJSONArray[i].toObject()["color"].toObject()["g"].toDouble();
			temp.color.b = objectsJSONArray[i].toObject()["color"].toObject()["b"].toDouble();
			temp.lambert = objectsJSONArray[i].toObject()["lambert"].toDouble();
			temp.type = objectsJSONArray[i].toObject()["type"].toString().toStdString();

			if (temp.type == "sphere") {
				temp.radius = objectsJSONArray[i].toObject()["radius"].toDouble();
			}
			else if (temp.type == "plane") {
				temp.normal.x = objectsJSONArray[i].toObject()["normal"].toObject()["x"].toDouble();
				temp.normal.y = objectsJSONArray[i].toObject()["normal"].toObject()["y"].toDouble();
				temp.normal.z = objectsJSONArray[i].toObject()["normal"].toObject()["z"].toDouble();
			}
			else {
				throw runtime_error("Error: Object is not of defined type.");
			}

			objectsVect.push_back(temp);
		}
		catch (exception& e) {
			throw runtime_error("Error: Objects are missing critical information");
		}
	}
}

void JSONParser::pullAllInfo()
{
	pullCameraInfo();
	pullLightInfo();
	pullObjectInfo();
}

void JSONParser::printLightsInfo()
{
	for (unsigned int i = 0; i < lightsVect.size(); i++) {
		cout << "Light " << i + 1 << endl;
		cout << "Intensity: " << lightsVect[i].intensity << endl;
		cout << "Location: " << "{" << lightsVect[i].theLocation.x << ", ";
		cout << lightsVect[i].theLocation.y << ", ";
		cout << lightsVect[i].theLocation.z << "}\n" << endl;
	}
}

void JSONParser::printCameraInfo()
{
	cout << "Camera" << endl;
	cout << "Center: {" << camera.center.x << ", " << camera.center.y << ", "
		<< camera.center.z << "}" << endl;
	cout << "Focus: " << camera.focus << endl;
	cout << "Normal: {" << camera.normal.x << ", " << camera.normal.y << ", "
		<< camera.normal.z << "}" << endl;
	cout << "Resolution: {" << camera.resolution.first << ", "
		<< camera.resolution.second << "}" << endl;
	cout << "Size: {" << camera.size.first << ", " << camera.size.second << "}\n" << endl;
}

void JSONParser::printObjectsInfo()
{
	for (unsigned int i = 0; i < objectsVect.size(); i++) {
		cout << "Object " << i + 1 << endl;
		cout << objectsVect[i].type << endl;
		cout << "Center: " << "{" << objectsVect[i].center.x << ", ";
		cout << objectsVect[i].center.y << ", ";
		cout << objectsVect[i].center.z << "}" << endl;

		cout << "Color: " << "{" << objectsVect[i].color.r << ", ";
		cout << objectsVect[i].color.g << ", ";
		cout << objectsVect[i].color.b << "}" << endl;

		cout << "Lambert: " << objectsVect[i].lambert << endl;

		if (objectsVect[i].type == "sphere") {
			cout << "Radius: " << objectsVect[i].radius << endl << endl;
		}

		else if (objectsVect[i].type == "plane") {
			cout << "Normal: " << "{" << objectsVect[i].normal.x << ", ";
			cout << objectsVect[i].normal.y << ", ";
			cout << objectsVect[i].normal.z << "}\n" << endl;
		}

		else {
			throw runtime_error("Error: Object is not of defined type.");
		}
	}
}

void JSONParser::printAllInfo()
{
	printCameraInfo();
	printLightsInfo();
	printObjectsInfo();
}

Camera JSONParser::returnCamera()
{
	return camera;
}

vector<Lights> JSONParser::returnLightsVect()
{
	return lightsVect;
}

vector<Objects> JSONParser::returnObjectsVect()
{
	return objectsVect;
}

//string JSONParser::readEntireJsonIntoString(string filename)
//{
//	string temp = "";
//	ifstream in(filename);
//	int count = 0;
//	while (!in.fail())
//	{
//		string holder;
//		getline(in, holder, '\n');
//		temp += holder;
//		count++;
//	}
//	return temp;
//}

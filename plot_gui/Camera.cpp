/**************************Camera.cpp****************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* Camera.cpp: Zooming In/Out
* Date: 2/4/21
*******************************************************************************/
#include "Camera.h"
#include <iostream>
#include <fstream>

double Camera::xMin;
double Camera::xMax;
double Camera::yMin;
double Camera::yMax;
double Camera::pXMin; 
double Camera::pXMax;
double Camera::scaleFactor = 1;
double Camera::pScalefactor;
Graphics *Camera::graphics;

void Camera::Init(double xMin, double xMax, double yMin, double yMax, Graphics &graphics)
{
	Camera::xMin = xMin, Camera::xMax = xMax, Camera::yMin = yMin, Camera::yMax = yMax;
	Camera::graphics = &graphics;
}


void Camera::translateX(double dx)
{
	dx = dx/graphics->getScreenWidth()*(xMax-xMin);
	Camera::xMin += dx;
	Camera::xMax += dx;
}

void Camera::translateY(double dy)
{
	dy = dy/graphics->getScreenHeight()*(yMax-yMin);
	Camera::yMin += dy;
	Camera::yMax += dy;
}

double Camera::suggestedDx()
{
	return 5;
}

double Camera::suggestedDy()
{
	return 5;
}

std::pair<double, double> Camera::transformCartesianToScreen(std::pair<double, double> p)
{
	//*For x coordinate : (x - xMin) / (xMax - xMin) * screen width
	//* For y coordinate : screen height - (y - yMin) / (yMax - yMin) * screen height
	std::pair<double, double> outPut;
	p.first *= scaleFactor;
	p.second *= scaleFactor;
	outPut.first = (p.first - xMin) / (xMax - xMin) * Camera::graphics->getScreenWidth();
	outPut.second = Camera::graphics->getScreenHeight() - (p.second - yMin) / (yMax - yMin) * Camera::graphics->getScreenHeight();
	return outPut;
}
double Camera::transformScreenToCartesian(double x)
{
	double sf = 1/scaleFactor;
	double xMinS = xMin*sf, xMaxS = xMax*sf;
	return (x) / (Camera::graphics->getScreenWidth())*(xMaxS - xMinS)+xMinS;
}
double Camera::domainRange()
{
	return (Camera::xMax - Camera::xMin) * .5 / Camera::scaleFactor;
}

bool Camera::shouldRecalc()
{
	return true;
	/*static bool scaled = scaleFactor;
	if(scaled == scaleFactor)
		return true || (std::abs(Camera::pXMin/scaleFactor - Camera::xMin/scaleFactor) > Camera::domainRange());
	else
	{
		scaled = scaleFactor;
		return true;
	}*/
	
}

void Camera::draw(double &xMin, double &xMax)
{
	Camera::pXMin = xMin;
	Camera::pXMax = xMax;
	xMin = Camera::xMin;
	xMax = Camera::xMax;

}

void Camera::drawScale(double& scale)
{
	Camera::pScalefactor = scale;
	scale = Camera::scaleFactor;
}

void Camera::scale(double delta)
{
	if (Camera::scaleFactor + delta > 0.01)
	Camera::scaleFactor += delta;
}


/**************************Camera.h****************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* Camera.h: Zooming In/Out
* Date: 2/4/21
*******************************************************************************/
#pragma once
#include <SDL2/SDL.h>
#include "Graphics.h"

class Graphics;

class Camera {
public:
	static void Init(double xMin, double xMax, double yMin, double yMax, Graphics &graphics); 
	static void translateX(double dx);
	static void translateY(double dy);
	static double suggestedDx();
	static double suggestedDy();
	static double xMin, xMax, yMin, yMax;
	static std::pair<double, double> transformCartesianToScreen(std::pair<double, double> p);
	static double transformScreenToCartesian(double x);
	static double domainRange();
	static bool shouldRecalc();
	static void draw(double &xMin, double &xMax);
	static void drawScale(double& scale);
	static void scale(double delta);
	static double scaleFactor;
	static Graphics *graphics;
	static void update();
private:
	static double pXMin, pXMax;
	static double pScalefactor;
	
};
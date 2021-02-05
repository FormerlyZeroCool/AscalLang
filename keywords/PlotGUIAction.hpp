/*
 * PlotGUIAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PLOTGUIACTION_HPP_
#define KEYWORDS_PLOTGUIACTION_HPP_

#include "../Keyword.hpp"
#include "../Vect2D.hpp"
#include "../CrossPlatform.hpp"
#include <unistd.h>
#include <dlfcn.h>

#include "../plot_gui/Graphics.h"
#include "../plot_gui/Input.h"
#include "../plot_gui/Camera.h"
//#include "..\plot_gui\Graphics.h"
//
class PlotGUIAction: public Keyword {
public:
	PlotGUIAction(AscalExecutor* runtime, std::unordered_map<std::string, Object>* memory, std::map<std::string, setting<bool> >* boolsettings);
	//~PlotGUIAction();

	std::string action(AscalFrame<double>* frame) override;


	void update(float elapsedTime);
	void draw(Graphics& graphics, Vect2D<std::pair<double, double>> &points, SDL_Texture* texture, SDL_Rect destRect);

	//Graphics _graphics; //store graphics object
	SDL_Window* bird;
	SDL_Renderer* word;
	
	//For plotGUI to use the y index of the Vect2D corresponds to the index of the function name in the functions vector
	//the x index in cartesian space is x*dx+xMin
	Vect2D<std::pair<double, double>> calcTable(const std::vector<std::string>& functions, double xMin, double xMax, double xStepSize, double yStepSize);

	double xMin, xMax, yMin, yMax, dx, dy;

	struct Point {
		double x, y;
	};

	Point cameraPosition = { 0,0 };
private:
	//Graphics _graphics; //store graphics object
	SDL_Point* coordinates = {};
};

#endif /* KEYWORDS_PLOTGUIACTION_HPP_ */

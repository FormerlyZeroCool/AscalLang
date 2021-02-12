/**************************PlotGUIAction.cpp************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* PlotGUIAction.cpp: Creates forked instances of Plotting GUI's and Renders
* Date: 2/1/21
*******************************************************************************/
#include "PlotGUIAction.hpp"

namespace {
	const int FPS = 50;
	const int MAX_FRAME_TIME = 5 * 1000 / FPS; //Max amount of time a frame is allowed to last
}

PlotGUIAction::PlotGUIAction(AscalExecutor* runtime, std::unordered_map<std::string, Object>* memory, std::map<std::string, setting<bool> >* boolsettings) :
	Keyword(runtime, memory, boolsettings)
{
	this->keyWord = "plotGUI";
}

std::string PlotGUIAction::action(AscalFrame<double>* frame)
{
	SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	std::vector<SubStr> params = Object("", "", exp.data).params;
	std::vector<std::string> functions = ParsingUtil::split(params[0].data, std::string("|"));

	//Here to aid potential future development
	double xMin = runtime->callOnFrame(frame, params[1].data);
	double xMax = runtime->callOnFrame(frame, params[2].data);
	double yMin = runtime->callOnFrame(frame, params[3].data);
	double yMax = runtime->callOnFrame(frame, params[4].data);
	double dx = runtime->callOnFrame(frame, params[5].data);
	double dy = runtime->callOnFrame(frame, params[6].data);
	double scale = 1;

	int pid = fork();
	if (!pid)
	{
		Graphics graphics;
		SDL_Event event;
		Input input;
		Point cameraPosition = { 0,0 };
		bool drawLine = true;
		struct Point {
			double x, y;
		};
		Camera::Init(xMin, xMax, yMin, yMax, graphics);


		//Essentially a 1d vector but you can address it with x, and y coordinates, y addresses different functions
        //x addresses the x point xMin+dx*x, using xMin, and dx defined above
		Vect2D<std::pair<double, double> > points = calcTable(functions, Camera::xMin - Camera::domainRange(), Camera::xMax + Camera::domainRange(), dx, dy);
		Camera::draw(xMin, xMax);
		//Get the amount of miliseconds since SDL-Lib was intialized
		//This will help us know when to update screen
		int delta_time = SDL_GetTicks();
		//DEBUG
		std::cout << "\nNumber of Points being plotted: " << points.size() << std::endl;

		SDL_Texture* texture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
		SDL_Rect destRect = {0,0,graphics.getScreenWidth(), graphics.getScreenHeight() };
		SDL_Texture* plotTexture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
		reDraw(points, graphics, drawLine, plotTexture, destRect);
		while (true) {
			input.beginNewFrame(); //reset released key/press key
			const int CURRENT_TIME_MS = SDL_GetTicks();
			int ELAPSED_TIME_MS = CURRENT_TIME_MS - delta_time;

			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_KEYDOWN) {
					if (event.key.repeat == 0) {
						input.keyDownEvent(event); //if we are holding key : start keydown event
					}
				}
				else if (event.type == SDL_KEYUP) { // if key was released
					input.keyUpEvent(event);
				}
				else if (event.type == SDL_QUIT) {
					throw 0; //User exits window (Force)
				}
			}

			if (input.wasKeyPressed(SDL_SCANCODE_ESCAPE) == true) {
				throw 0; //User exits normally with ESC key
			}

			else if (input.isKeyHeld(SDL_SCANCODE_Q) == true) {
				Camera::scale(0.007);
				if (Camera::shouldRecalc()) {
					Camera::drawScale(scale);
					points = calcTable(functions, Camera::xMin - Camera::domainRange(), Camera::xMax + Camera::domainRange(), dx, dy);
				}
				SDL_DestroyTexture(plotTexture);
				plotTexture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
				reDraw(points, graphics, drawLine, plotTexture, destRect);
			}

			else if (input.isKeyHeld(SDL_SCANCODE_E) == true) {
				Camera::scale(-0.007);
				if (Camera::shouldRecalc()) {
					Camera::drawScale(scale);
					points = calcTable(functions, Camera::xMin - Camera::domainRange(), Camera::xMax + Camera::domainRange(), dx, dy);
				}
				SDL_DestroyTexture(plotTexture);
				plotTexture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
				reDraw(points, graphics, drawLine, plotTexture, destRect);
			}

			else if (input.isKeyHeld(SDL_SCANCODE_A) == true) {
				Camera::translateX(Camera::suggestedDx() * -1);
				if (Camera::shouldRecalc()) {
					Camera::draw(xMin, xMax);
					points = calcTable(functions, Camera::xMin - Camera::domainRange(), Camera::xMax + Camera::domainRange(), dx, dy);
				}
				SDL_DestroyTexture(plotTexture);
				plotTexture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
				reDraw(points, graphics, drawLine, plotTexture, destRect);
			}
			else if (input.isKeyHeld(SDL_SCANCODE_D) == true) {
				Camera::translateX(Camera::suggestedDx());
				if (Camera::shouldRecalc()) {
					Camera::draw(xMin, xMax);
					points = calcTable(functions, Camera::xMin - Camera::domainRange(), Camera::xMax + Camera::domainRange(), dx, dy);
				}
				SDL_DestroyTexture(plotTexture);
				plotTexture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
				reDraw(points, graphics, drawLine, plotTexture, destRect);
			}

			else if (input.isKeyHeld(SDL_SCANCODE_W) == true) {
				Camera::translateY(Camera::suggestedDy() * -1);
				if (Camera::shouldRecalc()) {
					Camera::draw(xMin, xMax);
					points = calcTable(functions, Camera::xMin - Camera::domainRange(), Camera::xMax + Camera::domainRange(), dx, dy);
				}
				SDL_DestroyTexture(plotTexture);
				plotTexture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
				reDraw(points, graphics, drawLine, plotTexture, destRect);
			}

			else if (input.isKeyHeld(SDL_SCANCODE_S) == true) {
				Camera::translateY(Camera::suggestedDy());
				if (Camera::shouldRecalc()) {
					Camera::draw(xMin, xMax);
					points = calcTable(functions, Camera::xMin - Camera::domainRange(), Camera::xMax + Camera::domainRange(), dx, dy);
				}
				SDL_DestroyTexture(plotTexture);
				plotTexture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
				reDraw(points, graphics, drawLine, plotTexture, destRect);
			}

			else if (input.wasKeyPressed(SDL_SCANCODE_L) == true) {
				if (drawLine == false) {
					drawLine = true;
				}
				else if (drawLine == true) {
					drawLine = false;
				}
			}

			else if (input.wasKeyPressed(SDL_SCANCODE_P) == true) {
				cameraPosition.x = 0; 
				cameraPosition.y = 0;
				graphics.d_rect.h = 0;
				graphics.d_rect.w = 0;
			}
			

			delta_time = CURRENT_TIME_MS;
			SDL_SetRenderTarget(graphics.getRenderer(), NULL);

			//SDL_SetRenderTarget(graphics.getRenderer(), NULL);
			//graphics.blitSurface(texture, NULL, NULL);
			//SDL_Rect source = destRect;
			graphics.blitSurface(plotTexture, NULL, NULL);
			//SDL_SetRenderTarget(graphics.getRenderer(), NULL);
			//SDL_RenderPresent(graphics.getRenderer());
			//graphics.clear();
		//	graphics.blitSurface(texy, NULL, NULL);
			SDL_RenderPresent(graphics.getRenderer());
			//SDL_DestroyTexture(texy);
			
			//SDL_Delay(50);
			CrossPlatform::usleep(20000);
			
		}

		throw 0;
	}
	if (*(*boolsettings)["o"])
	{
		std::cout << "plotGUI" + exp.data.substr(0, exp.data.size() - 2) + "\n";
	}
	//Automatically moves to next expression after keyword execution because we return MAX
	return MAX;
}


void PlotGUIAction::draw(Graphics& graphics, Vect2D<std::pair<double, double>>& points, SDL_Texture* texture, SDL_Rect destRect) {
}

void PlotGUIAction::update(float elapsedTime) {
}

Vect2D<std::pair<double, double>> PlotGUIAction::calcTable(const std::vector<std::string>& functions, double xMin, double xMax, double xStepSize, double yStepSize)
{
	xMin /= Camera::scaleFactor;
	xMax /= Camera::scaleFactor;
	xStepSize /= Camera::scaleFactor;
	//std::cout << "Scale factor: " << Camera::scaleFactor << "\n";
	//std::cout << "Step Size: " << xStepSize << "\n";
	int tableWidth = (xMax - xMin) / (xStepSize > 0 ? xStepSize : 1);
	double dx = (xMax - xMin) / tableWidth;
	double dy = yStepSize > 0 ? yStepSize : 1;
	double xi;
	Vect2D<std::pair<double, double> > outPuts(tableWidth, functions.size() - 1);
	std::stringstream exp;
	for (int j = 0; j < functions.size(); j++)
	{
		const std::string& function = functions[j];
		for (int i = 0; i < tableWidth; i++)
		{
			xi = xMin + dx * (i);
			FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr, nullptr, nullptr);
			exp << function << '(' << ParsingUtil::to_string(xi) << ')';
			calledFunction->exp = exp.str();
			exp.str(std::string());
			outPuts.push_back(
				std::pair<double, double>(xi, runtime->calculateExpression(calledFunction))
			);
		}
	}
	return outPuts;
}

void PlotGUIAction::reDraw(std::vector<std::pair<double, double> >& points, Graphics& graphics, bool drawLine, SDL_Texture* texture, SDL_Rect destRect)
{
	bool isFirst = 1;
	std::pair<double, double> previousPoint;
	SDL_SetRenderTarget(graphics.getRenderer(), texture);
	//set background color and clear() to fill with the specified color
	SDL_SetRenderDrawColor(graphics.getRenderer(), 0x00, 0x00, 0x00, 0x00);
	graphics.clear();
	//set x/y axis color and draw
	SDL_SetRenderDrawColor(graphics.getRenderer(), 0xFF, 0xFF, 0xFF, 0x00);

	//Draw x/y axis
	std::pair<double, double> startX = Camera::transformCartesianToScreen(std::pair<double, double>(Camera::xMin / Camera::scaleFactor, 0));
	std::pair<double, double> startY = Camera::transformCartesianToScreen(std::pair<double, double>(0, Camera::yMin / Camera::scaleFactor));
	std::pair<double, double> endX = Camera::transformCartesianToScreen(std::pair<double, double>(Camera::xMax / Camera::scaleFactor, 0));
	std::pair<double, double> endY = Camera::transformCartesianToScreen(std::pair<double, double>(0, Camera::yMax / Camera::scaleFactor));

	SDL_RenderDrawLine(graphics.getRenderer(), startX.first, startX.second, endX.first, endX.second);
	SDL_RenderDrawLine(graphics.getRenderer(), startY.first, startY.second, endY.first, endY.second);
	for (int i = 0; i < points.size(); i++) {
		/* In order to translate/scale the function correctly we use the following functions:
		* For x coordinate: (x - xMin) / (xMax - xMin) * screen width
		* For y coordinate: screen height - (y - yMin) / (yMax - yMin) * screen height
		* And to scroll horizontally we keep a scroll offset and add that to all the points
		* when drawing them. The offset is the opposite of what direction the user is scrolling
		*/
		SDL_SetRenderDrawColor(graphics.getRenderer(), 255, 0, 0, 0);
		std::pair<double, double> p = Camera::transformCartesianToScreen(points[i]);
		SDL_RenderDrawPoint(graphics.getRenderer(), p.first, p.second);
		//std::cout << realX << "," << realY << std::endl;
		if (drawLine == true) {
			if (!isFirst)
			{
				SDL_RenderDrawLine(graphics.getRenderer(), previousPoint.first, previousPoint.second, p.first, p.second);
			}
			previousPoint = p;


			if (isFirst)
			{
				isFirst = 0;
			}
		}
	}

}


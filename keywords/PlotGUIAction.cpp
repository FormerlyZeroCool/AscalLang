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

	int pid = fork();
	if (!pid)
	{
		Graphics graphics;
		Camera camera;
		SDL_Event event;
		Input input;
		Point cameraPosition = { 0,0 };
		bool drawLine = true;
		struct Point {
			double x, y;
		};

		//Essentially a 1d vector but you can address it with x, and y coordinates, y addresses different functions
        //x addresses the x point xMin+dx*x, using xMin, and dx defined above
		Vect2D<std::pair<double, double> > points = calcTable(functions, xMin, xMax, dx, dy);

		//Get the amount of miliseconds since SDL-Lib was intialized
		//This will help us know when to update screen
		int delta_time = SDL_GetTicks();
		//DEBUG
		std::cout << "\nNumber of Points being plotted: " << points.size() << std::endl;

		SDL_Texture* texture = SDL_CreateTexture(graphics.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, graphics.getScreenWidth(), graphics.getScreenHeight());
		SDL_Rect destRect = {0,0,graphics.getScreenWidth(), graphics.getScreenHeight() };

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
				graphics.d_rect.h -= 10;
				graphics.d_rect.w -= 10;
			}

			else if (input.isKeyHeld(SDL_SCANCODE_E) == true) {
				if (graphics.d_rect.h <= -5) {
					graphics.d_rect.h += 10;
					graphics.d_rect.w += 10;
				}
			}

			else if (input.isKeyHeld(SDL_SCANCODE_A) == true) {
				cameraPosition.x -= 10;
			}
			else if (input.isKeyHeld(SDL_SCANCODE_D) == true) {
				cameraPosition.x += 10;
			}

			else if (input.isKeyHeld(SDL_SCANCODE_W) == true) {
				cameraPosition.y -= 10;
			}

			else if (input.isKeyHeld(SDL_SCANCODE_S) == true) {
				cameraPosition.y += 10;
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
	
			SDL_SetRenderTarget(graphics.getRenderer(), texture);
			//set background color and clear() to fill with the specified color
			SDL_SetRenderDrawColor(graphics.getRenderer(), 0x00, 0x00, 0x00, 0x00);
			graphics.clear();
			//set x/y axis color and draw
			SDL_SetRenderDrawColor(graphics.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderDrawLine(graphics.getRenderer(), -800 + (-cameraPosition.x), graphics.getScreenHeight() / 2 + (-cameraPosition.y), 1200 + (-cameraPosition.x), graphics.getScreenHeight() / 2 + (-cameraPosition.y));
			SDL_RenderDrawLine(graphics.getRenderer(), graphics.getScreenWidth() / 2 + (-cameraPosition.x), -800 + (-cameraPosition.y), graphics.getScreenWidth() / 2 + (-cameraPosition.x), 1400 + (-cameraPosition.y));
			//set points color and iterate through each point
			SDL_SetRenderDrawColor(graphics.getRenderer(), 255, 0, 0, 0);
			unsigned char isFirst = 1;
			double previous_x = 0;
			double previous_y = 0;
			for (int i = 0; i < points.size(); i++) {
				/* In order to translate/scale the function correctly we use the following functions:
				* For x coordinate: (x - xMin) / (xMax - xMin) * screen width
				* For y coordinate: screen height - (y - yMin) / (yMax - yMin) * screen height
				* And to scroll horizontally we keep a scroll offset and add that to all the points
				* when drawing them. The offset is the opposite of what direction the user is scrolling
				*/

				double realX = ((points[i].first - xMin) / (xMax - xMin)) * 742;
				double realY = 584 - ((points[i].second - yMin) / (yMax - yMin) * 584);
				SDL_RenderDrawPoint(graphics.getRenderer(), realX + (-cameraPosition.x), realY + (-cameraPosition.y));

			if (drawLine == true){
					if (!isFirst)
					{
						SDL_RenderDrawLine(graphics.getRenderer(), previous_x + (-cameraPosition.x), previous_y + (-cameraPosition.y), realX + (-cameraPosition.x), realY + (-cameraPosition.y));
					}

				previous_x = realX;
				previous_y = realY;

				if (isFirst)
				{
					isFirst = 0;
				}
			  }
			}
			SDL_SetRenderTarget(graphics.getRenderer(), NULL);
			graphics.blitSurface(texture, NULL, &destRect);
			SDL_RenderPresent(graphics.getRenderer());
			camera.Update(std::min(ELAPSED_TIME_MS, MAX_FRAME_TIME), graphics);

			CrossPlatform::usleep(10000);
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


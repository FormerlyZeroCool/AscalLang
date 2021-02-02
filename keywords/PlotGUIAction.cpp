
#include "PlotGUIAction.hpp"

PlotGUIAction::PlotGUIAction(AscalExecutor* runtime, std::unordered_map<std::string, Object>* memory, std::map<std::string, setting<bool> >* boolsettings) :
	Keyword(runtime, memory, boolsettings)
{
	this->keyWord = "plotGUI";
}

std::string PlotGUIAction::action(AscalFrame<double>* frame)
{
	//Graphics graphics;

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

	std::cout << "DATA: " << xMin << " , " << xMax << " , " << yMin << " , " << yMax << " , " << dx << " , " << dy << std::endl;

	int pid = fork();
	std::cout << "FORK PID IS: " << pid << std::endl;
	if (!pid)
	{
		std::cout << "just so we know you know?" << std::endl;
		//Essentially a 1d vector but you can address it with x, and y coordinates, y addresses different functions
		//x addresses the x point xMin+dx*x, using xMin, and dx defined above
		Vect2D<double> points = calcTable(functions, xMin, xMax, dx, dy);

		/*
		void* handle = dlopen("SDL2.dll", RTLD_LAZY);
		void* var = bird;
		biggie obj;
		obj = (biggie)dlsym(handle, "SDL_CreateWindowAndRenderer");
		obj(640, 480, 0, &this->bird, &this->word);
		while (true) {
			std::cin.get();
		}*/

		Graphics graphics;
		SDL_Event event;
		Input input;

		//Get the amount of miliseconds since SDL-Lib was intialized
		//This will help us know when to update screen
		int delta_time = SDL_GetTicks();


		std::cout << "This is after fork()!" << std::endl;
		while (true) {
			input.beginNewFrame(); //reset released key/press key

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

			const int CURRENT_TIME_MS = SDL_GetTicks();
			int ELAPSED_TIME_MS = CURRENT_TIME_MS - delta_time;

			//this->_graphics = graphics; //updated graphics


			graphics.drawRect2();
			graphics.drawRect();

			CrossPlatform::usleep(30000);

			//graphics.clear();
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

Vect2D<double> PlotGUIAction::calcTable(const std::vector<std::string>& functions, double xMin, double xMax, double xStepSize, double yStepSize)
{
	{
		int tableWidth = (xMax - xMin) / (xStepSize > 0 ? xStepSize : 1);
		double dx = (xMax - xMin) / tableWidth;
		double dy = yStepSize > 0 ? yStepSize : 1;
		double xi;
		Vect2D<double> outPuts(tableWidth, functions.size() - 1);
		std::stringstream exp;
		for (int j = 0; j < functions.size(); j++)
		{
			std::string function = functions[j];
			for (int i = 0; i < tableWidth; i++)
			{
				xi = xMin + dx * (i);
				FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr, nullptr, nullptr);
				exp << function << '(' << ParsingUtil::to_string(xi) << ')';
				calledFunction->exp = exp.str();
				exp.str(std::string());
				outPuts.push_back(
					runtime->calculateExpression(calledFunction));
			}
		}
		return outPuts;
	}
}


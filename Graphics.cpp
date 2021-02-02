/**************************Graphics.cpp****************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* Graphics.cpp: Holds all the information that deals with graphics & rendering
* Date: 1/28/21
*******************************************************************************/
//#include "plot_gui\Graphics.h"
#include "plot_gui/Graphics.h"
//#include <SDL2\SDL.h>
//extern "C"

#include <SDL2/SDL.h>

//#include <SDL.h>
#include <string>
#include <iostream>

Graphics::Graphics() {
	SDL_CreateWindowAndRenderer(640, 480, 0, &this->_window, &this->_renderer);
	SDL_SetWindowTitle(this->_window, "PLOT_GUI");
}

Graphics::~Graphics() {
	std::cout << "\nGraphics.cpp deconstructor invoked" << std::endl;
	SDL_DestroyWindow(this->_window);
	//avoid memory leaks from having a pointer to an SDL rendering memory dont let it hang, destroy when done.
	SDL_DestroyRenderer(this->_renderer); 
}


void Graphics::blitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle) {
	SDL_Rect tmp = { destinationRectangle->x, destinationRectangle->y, destinationRectangle->w, destinationRectangle->h };
	SDL_RenderCopy(this->_renderer, texture, sourceRectangle, &tmp);
}

void Graphics::windowBlitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle) {
	SDL_Rect tmp = { destinationRectangle->x, destinationRectangle->y, destinationRectangle->w, destinationRectangle->h };
	SDL_RenderCopy(this->_renderer, texture, sourceRectangle, &tmp);
}

void Graphics::flip() {
	SDL_RenderPresent(this->_renderer);
}

void Graphics::clear() {
	SDL_RenderClear(this->_renderer);
}

SDL_Renderer* Graphics::getRenderer() const {
	return this->_renderer;
}

void Graphics::drawRect() {
	SDL_Rect rect;
	rect.x = 250;
	rect.y = 150;
	rect.h = 200;
	rect.w = 200;

	SDL_SetRenderDrawColor(this->_renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(this->_renderer, &rect);

	SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);

	flip();
}

void Graphics::drawRect2() {
	static int x = 20;
	x += 2;

	SDL_Rect rect;
	rect.x = x;
	rect.y = 50;
	rect.h = 120;
	rect.w = 120;

	SDL_SetRenderDrawColor(this->_renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(this->_renderer, &rect);

	SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);

	flip();
}

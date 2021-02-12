/**************************Graphics.cpp****************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* Graphics.cpp: Holds all the information that deals with graphics & rendering
* Date: 1/28/21
*******************************************************************************/
#include "plot_gui/Graphics.h"
#include "plot_gui/Camera.h"
//extern "C"

#include <SDL2/SDL.h>
#include <string>
#include <iostream>

Graphics::Graphics() {
	_window = SDL_CreateWindow("GUI", 50, 50, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_TARGETTEXTURE);
	//SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &this->_window, &this->_renderer);
	//SDL_SetWindowTitle(this->_window, "PLOT_GUI");
	d_rect = {0,0,0,0};
}

Graphics::~Graphics() {
	std::cout << "\nGraphics.cpp deconstructor invoked" << std::endl;
	SDL_DestroyWindow(this->_window);
	//avoid memory leaks from having a pointer to an SDL rendering memory dont let it hang, destroy when done.
	SDL_DestroyRenderer(this->_renderer); 
}


void Graphics::blitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle) {
	//SDL_Rect tmp = { destinationRectangle->x, destinationRectangle->y, destinationRectangle->w, destinationRectangle->h };
	//SDL_Rect srcTmp = { sourceRectangle->x - Camera::GetRect().x, sourceRectangle->y - Camera::GetRect().y, sourceRectangle->w - Camera::GetRect().w, sourceRectangle->h - Camera::GetRect().h };
	SDL_RenderCopy(this->_renderer, texture, sourceRectangle, destinationRectangle);
	//SDL_RenderCopy(this->_renderer, texture, &srcTmp, destinationRectangle);
}

void Graphics::windowBlitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle) {
	SDL_Rect tmp = { destinationRectangle->x, destinationRectangle->y, destinationRectangle->w, destinationRectangle->h };
	SDL_RenderCopy(this->_renderer, texture, sourceRectangle, &tmp);
}

int Graphics::getScreenHeight() {
	return SCREEN_HEIGHT;
}

int Graphics::getScreenWidth() {
	return SCREEN_WIDTH;
}

void draw() {
	SDL_Rect destRect = { 0,0,640,480 };
	SDL_Rect sourceRect = { 0,0,640,480 };
	
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

SDL_Window* Graphics::getWindow() const {
	return this->_window;
}

void Graphics::drawRect() {
	SDL_Rect rect;
	rect.x = 110;
	rect.y = 20;
	rect.h = 400;
	rect.w = 400;

	SDL_SetRenderDrawColor(this->_renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(this->_renderer, &rect);

	//SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);

	flip();
}

void Graphics::drawRect2() {
	SDL_SetRenderDrawColor(this->_renderer, 0, 0, 255, 255); // Draw in solid blue

	SDL_RenderDrawLine(this->_renderer, 10, 10, 50, 25); // Draw a line

	SDL_Rect r;
	r.x = 150;
	r.y = 25;
	r.h = 75;
	r.w = 120;

	SDL_RenderFillRect(this->_renderer, &r); // Draw a filled rectangle

	/*
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

	flip(); */
}

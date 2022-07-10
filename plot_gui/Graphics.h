/**************************Graphics.h****************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* Graphics.cpp: Holds all the information that deals with graphics & rendering
* Date: 1/28/21
*******************************************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <map>
//extern "C"
#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>

struct SDL_Window;
struct SDL_Renderer;

typedef void (*biggie)(int, int, Uint32, SDL_Window**, SDL_Renderer**);

class Graphics {
public:
	Graphics();
	~Graphics();

	//void blitzSurface will draw a texture to a certain part of the screen
	//sourceRectangle is what we are drawing to the screen (A part of the texture or NULL for entire texture)
	//destinationRectangle is where on the screen we will draw it
	void blitSurface(SDL_Texture* source, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle);

	void windowBlitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle);

	void flip();

	void clear();

	void draw();

	void drawRect();

	void drawRect2();

	SDL_Renderer* getRenderer() const;

	SDL_Window* getWindow() const;

	SDL_Rect d_rect;

	int getScreenWidth();
	int getScreenHeight();

	

private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	int SCREEN_WIDTH = 1024, SCREEN_HEIGHT = 768;
};
#endif
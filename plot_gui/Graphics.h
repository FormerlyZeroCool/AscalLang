/**************************Graphics.h****************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* Graphics.cpp: Holds all the information that deals with graphics & rendering
* Date: 1/28/21
*******************************************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H
//#include <SDL_main.h>
//#include <SDL2\SDL.h>
//#include <SDL2\SDL_stdinc.h>
//#include <SDL2\SDL_main.h>
//#include <SDL2\SDL.h>
//#include <SDL.h>
#include <map>
//#include <SDL2\SDL.h>
//#include <SDL2/SDL.h>
//#include <SDL.h>
//extern "C"

#include <SDL2/SDL.h>


//#include <SDL2/SDL.h>


struct SDL_Window;
struct SDL_Renderer;

typedef void (*biggie)(int, int, Uint32, SDL_Window**, SDL_Renderer**);

class Graphics {
public:
	Graphics();
	~Graphics();

	//void blitzSurface will draw a texture to a certain part of the screen
	//source is what we are drawing to the screen, sourceRectangle is what part of the spritesheet we will draw
	//destinationRectangle is where on the screen we will draw it
	void blitSurface(SDL_Texture* source, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle);

	void windowBlitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle);

	void flip();

	void clear();

	void drawRect();

	void drawRect2();

	SDL_Renderer* getRenderer() const;

private:
	SDL_Window* _window;
	SDL_Renderer* _renderer;

};
#endif
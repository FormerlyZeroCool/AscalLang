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
	static void Init(); 
	static SDL_Rect GetRect(); 
	static void Update(float elapsedTime, Graphics &graphics);
};
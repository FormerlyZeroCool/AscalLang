/**************************Camera.cpp****************************************
* Branch: GUI_Plot
* Developed By: Arhum Z. Nayyar (Rummy)
* Camera.cpp: Zooming In/Out
* Date: 2/4/21
*******************************************************************************/
#include "Camera.h"
#include <iostream>
#include <fstream>

static SDL_Rect m_rect;

void Camera::Init()
{
	m_rect = { 0, 0, 640, 480 };
}


SDL_Rect Camera::GetRect()
{
	SDL_Rect GetRect = { m_rect.x,m_rect.y,m_rect.w,m_rect.h };
	return GetRect;
}

void Camera::Update(float elapsedTime, Graphics& graphics)
{
	m_rect.x = graphics.d_rect.x;
	m_rect.y = graphics.d_rect.y;
	m_rect.w = graphics.d_rect.w;
	m_rect.h = graphics.d_rect.h;
	GetRect();
}


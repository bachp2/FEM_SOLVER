#pragma once
#include <SDL.h>
struct mRect {
	SDL_Rect rect;
	SDL_Color color;
	bool isInRect(int px, int py)
	{
		auto inXRange = px >= rect.x && px <= rect.x + rect.w;
		auto inYrange = py >= rect.y && py <= rect.y + rect.h;
		return inXRange && inYrange;
	}
};

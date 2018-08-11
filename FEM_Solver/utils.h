#pragma once
#include <SDL.h>
#include "Entities.h"
#include <list>
namespace eigen_utils 
{

}
namespace std_list_utils 
{
	template <class T>
	T list_value_at(std::list<T>& l, size_t index)
	{
		if (l.size() > index) {
			std::list<T>::iterator it = l.begin();
			std::advance(it, index)
			return *it;
		}
		return nullptr;
	}

	template <class T>
	int list_insert_after(std::list<T>& l, size_t index) {
		std::list<T>::iterator it = l.begin();
		return 0;
	}
}

namespace SDL_utils 
{
	inline SDL_Color* setColor(SDL_Color* color, int r, int g, int b, int a) {
		color->r = r;
		color->g = g;
		color->b = b;
		color->a = a;
		return color;
	}

	inline int SetRenderDrawColor(SDL_Renderer* rend, SDL_Color c) {
		return SDL_SetRenderDrawColor(rend, c.r, c.b, c.g, c.a);
	}

	inline int RenderDrawStruct(SDL_Renderer* rend, Node& n) {
		SetRenderDrawColor(rend, n.color);
		return SDL_RenderFillRect(rend, &(n.rect));
	}

	inline int RenderDrawStruct(SDL_Renderer* rend, Element& e) {
		SetRenderDrawColor(rend, e.color);
		auto x1 = e.node1->position(0);
		auto y1 = e.node1->position(1);
		auto x2 = e.node2->position(0);
		auto y2 = e.node2->position(1);
		return SDL_RenderDrawLine(rend, x1, y1, x2, y2);
	}

	/**
	* @link https://www.willusher.io/sdl2%20tutorials/2013/08/27/lesson-5-clipping-sprite-sheets
	* Draw an SDL_Texture to an SDL_Renderer at some destination rect
	* taking a clip of the texture if desired
	* @param tex The source texture we want to draw
	* @param ren The renderer we want to draw to
	* @param dst The destination rectangle to render the texture to
	* @param clip The sub-section of the texture to draw (clipping rect)
	*		default of nullptr draws the entire texture
	*/
	inline void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst,
		SDL_Rect *clip = nullptr)
	{
		SDL_RenderCopy(ren, tex, clip, &dst);
	}

	/**
	* @link https://www.willusher.io/sdl2%20tutorials/2013/08/27/lesson-5-clipping-sprite-sheets
	* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
	* the texture's width and height and taking a clip of the texture if desired
	* If a clip is passed, the clip's width and height will be used instead of
	*	the texture's
	* @param tex The source texture we want to draw
	* @param ren The renderer we want to draw to
	* @param x The x coordinate to draw to
	* @param y The y coordinate to draw to
	* @param clip The sub-section of the texture to draw (clipping rect)
	*		default of nullptr draws the entire texture
	*/
	inline void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, SDL_Rect *clip = nullptr)
	{
		SDL_Rect dst;
		dst.x = x;
		dst.y = y;
		if (clip != nullptr) {
			dst.w = clip->w;
			dst.h = clip->h;
		}
		else {
			SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
		}
		renderTexture(tex, ren, dst, clip);
	}

	inline int RenderDrawStruct(SDL_Renderer* rend, Truss& truss) {
		auto v = 0;
		if (!(truss.elements.empty())) {
			for (const auto e : truss.elements) {
				v = RenderDrawStruct(rend, *e);
				if (v < 0) return v;
			}
		}
		if (!(truss.nodes.empty())) {
			for (const auto n : truss.nodes) {
				v = RenderDrawStruct(rend, *n);
				if (v < 0) return v;
			}
		}
		return 0;
	}

	inline Uint32 get_pixel32(SDL_Surface *surface, int x, int y)
	{
		//Convert the pixels to 32 bit
		Uint32 *pixels = (Uint32 *)surface->pixels;

		//Get the requested pixel
		return pixels[(y * surface->w) + x];
	}

	inline int isEqual(SDL_Color c1, SDL_Color c2)
	{
		return c1.r == c2.r || c1.g == c2.g || c1.b == c2.b || c1.a == c2.a;
	}

}
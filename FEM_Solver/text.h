#pragma once
#include <SDL_ttf.h>
#include "TextBuffer.h"
#include "caret.h"
#include "utils.h"

class Text {
public:
	void free_texture(SDL_Texture *t)
	{
		if (t != nullptr) {
			SDL_DestroyTexture(t);
		}
	}

	void free_texture()
	{
		if (texture != nullptr) {
			SDL_DestroyTexture(texture);
		}
	}

	int load_text_from_string(SDL_Renderer *rend, std::string input, SDL_Color colour, int linelevel)
	{
		//Get rid of preexisting texture
		free_texture();
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, input.c_str(), colour);
		if (textSurface != NULL)
		{
			//Create texture from surface pixels
			texture = SDL_CreateTextureFromSurface(rend, textSurface);
			if (texture == NULL)
			{
				printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Get image dimensions
				rect.x = leftMargin;
				rect.y = topMargin + linelevel*TTF_FontLineSkip(font);// TODO: create constriant for the scripting pane
				rect.w = textSurface->w;
				rect.h = textSurface->h;
			}

			//Get rid of old surface
			SDL_FreeSurface(textSurface);
		}
		else if (!input.empty())
		{
			printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		}

		return texture != nullptr;
	}
	/*
	TODO: have only a content text buffer in the text struct .Right now I'm using vector of string to store content and a currline string to display lione the cursor's currently on.
	*/
	bool append_to_content(char in)
	{
		/*int ln, col;
		content.get_ln_col_at_point(ln, col);*/
		content.buff_insert(in);
		return true;
	}

	void scroll_down(mRect &scriptingPane, const int amount = 15) {
		window.y += amount;
		if (window.h + window.y > scriptingPane.rect.h) {
			window.y = scriptingPane.rect.h - window.h;
		}
	}

	void scroll_up(mRect &scriptingPane, const int amount = 15) {
		window.y -= amount;
		if (window.y < scriptingPane.rect.y) {
			window.y = 0;
		}
	}

	void draw_to_render_texture(SDL_Renderer *rend, mRect &scriptingPane)
	{
		free_texture(render_texture);
		render_texture = SDL_CreateTexture(
			rend, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,
			scriptingPane.rect.w, scriptingPane.rect.h
			);
		SDL_SetRenderTarget(rend, render_texture);
		//SDL_RenderClear(gRenderer);
		SDL_utils::SetRenderDrawColor(rend, scriptingPane.color);
		SDL_RenderFillRect(rend, &scriptingPane.rect);

		std::stringstream ss(content.str());
		std::string line;
		int i = 0;
		while (std::getline(ss, line, '\n')) {
			load_text_from_string(rend, line, default_color, i);
			SDL_RenderCopy(rend, texture, NULL, &(rect));
			i++;
		}
		auto entire_script_height = (i+2)*TTF_FontLineSkip(font);
		if (entire_script_height > scriptingPane.rect.h) scriptingPane.rect.h = entire_script_height;
		//update cursor
		//int iline, icol;
		//content.get_ln_col_at_point(iline, icol);
		//TTF_SizeText(font, content.get_current_line().c_str(), &cursor.rect.x, NULL);
		int advance;
		TTF_GlyphMetrics(font, 'd', NULL, NULL, NULL, NULL, &advance);
		caret.rect.x = leftMargin + advance*(caret.current_column - 1);
		caret.rect.y = topMargin + TTF_FontAscent(font) - caret.rect.h;
		caret.rect.y += (caret.current_line - 1)*TTF_FontLineSkip(font);
		if (caret.rect.y + caret.rect.h >= window.y + window.h) {
			scroll_down(scriptingPane, caret.rect.y + caret.rect.h - window.y + window.h);
		}
		if (caret.rect.y < window.y) {
			scroll_up(scriptingPane, window.y - caret.rect.y);
		}
		//draw cursor
		caret.update(rend);
		SDL_SetRenderTarget(rend, NULL);
	}
	TextBuffer content;
	TTF_Font *font;
	SDL_Color default_color = { 0, 0, 0, 255 };
	SDL_Texture *render_texture;
	SDL_Rect rect;
	SDL_Rect window;
	unsigned int size;
	int leftMargin;
	int topMargin;
	Caret caret;
	SDL_Texture *texture;
};

#pragma once
#include "mrect.h"
#include "text.h"
#include "base.h"
struct Caret : mRect 
{
public:
	static const int DEFAULT_BLINKS_PER_SECOND = 1;
	size_t total_lines, current_line, current_column;
	bool mvisibility;
	int mblink_time;//in ms
	Caret() {
		baseticks = _getTicks();
		total_lines = 1;
		current_line = current_column = 1;
		mvisibility = true;
		mblink_time = (int) 500.f / DEFAULT_BLINKS_PER_SECOND;
	}

	Caret(TextBuffer *tb, bool visibility = true, int blinks_per_second = 2)
	{
		baseticks = _getTicks();
		total_lines = 1;
		current_line = current_column = 1;
		mvisibility = visibility;
		mblink_time = (int) 500.f / blinks_per_second;
		textBuffer = tb;
	}

	Caret(TextBuffer *tb, int blink_time, bool visibility = true)
	{
		baseticks = _getTicks();
		total_lines = 1;
		current_line = current_column = 1;
		mvisibility = visibility;
		mblink_time = blink_time;
		textBuffer = tb;
	}

	~Caret() { textBuffer = nullptr; }

	void move() {
		textBuffer->move_caret_to_point(current_line, current_column);
		baseticks = currticks;
		mvisibility = true;
		color.a = 100;
	}
	
	void blink() {
		currticks = _getTicks();
		auto diff = currticks - baseticks;
		if (diff >= mblink_time)
		{
			baseticks = currticks;
			_switch(mvisibility);
			color.a = mvisibility ? 100 : 0;
		}
	}

	void update(SDL_Renderer *rend)
	{
		blink();
		SDL_utils::SetRenderDrawColor(rend, color);
		SDL_RenderFillRect(rend, &(rect));
	}

	void setTextBuffer(TextBuffer* tb) { textBuffer = tb; }
private:
	Uint32 baseticks = 0;
	Uint32 currticks = 0;
	TextBuffer *textBuffer;

	/*!
	\brief Internal wrapper to SDL_GetTicks that ensures a non-zero return value.

	\return The tick count.
	*/
	Uint32 _getTicks()
	{
		Uint32 ticks = SDL_GetTicks();

		/*
		* Since baseticks!=0 is used to track initialization
		* we need to ensure that the tick count is always >0
		* since SDL_GetTicks may not have incremented yet and
		* return 0 depending on the timing of the calls.
		*/
		if (ticks == 0) {
			return 1;
		}
		else {
			return ticks;
		}
	}

	void _switch(bool& in) { in = !in; }
	void _clamp(size_t& in, size_t min, size_t max) {
		assert(max >= min);
		if (in < min) in = 1;
		else if (in > max) in = max;
	}
};
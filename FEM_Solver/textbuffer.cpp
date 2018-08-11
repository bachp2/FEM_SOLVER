#include "TextBuffer.h"
#include <assert.h>

TextBuffer::TextBuffer()
{
	size = 0;
	capacity = DEFAULT_BUFFER_SIZE;
	buffer = (char*) malloc((capacity+1)*sizeof(char)); //put aside a last slot for EOF
	point = &buffer[0];
	gapstrt = &buffer[0];
	gapend = &buffer[capacity-1];
	buffer[capacity] = '\0';
}


TextBuffer::~TextBuffer()
{
	free(buffer);
	buffer = NULL;
	point = NULL;
	gapstrt = NULL;
	gapend = NULL;
	size = 0;
	capacity = 0;
}


/*
Since this routine is used in conjunction with move point. 
I'll make sure that the conditions are valid before using this method
TODO: implement positioning of gap at point when the point is moved by user input -> Testing phase
*/
void TextBuffer::focus_gap_on_point()
{
	if (point == gapstrt) return;
	if (size == capacity)
	{
		gapstrt = point;
		gapend = gapstrt - 1;
		return;
	}
	auto slot_moves = point - gapstrt;
	if (slot_moves > 0) {
		slot_moves -= get_gap_size();
		for (size_t i = 0; i < slot_moves; ++i)
		{
			*gapstrt = *(gapend + 1);
			gapstrt++;
			gapend++;
			clamp(gapstrt);
			clamp(gapend);
		}

		if (point = gapend+1) {
			point = gapstrt;
		}
	}
	else
	{
		slot_moves *= -1;
		for (size_t i = 0; i < slot_moves; ++i)
		{
			*gapend = *(gapstrt-1);
			gapstrt--;
			gapend--;
			clamp(gapstrt);
			clamp(gapend);
		}
	}
	assert(point == gapstrt);
	//assert(gapend == gapstrt + get_gap_size());
}

//NOTE: gap start will overstep gap end when the capacity is full
//I just don't feel the need to enforce gapstart <= gapend but it is a thing to keep in mind!
//On second thougth this issue need to be resolved since alot of the buffer logics depend on this invariant
//   s.Bach Phan 10:05PM 16-7-2018
int TextBuffer::buff_insert(char ch)
{
	if (point != gapstrt) focus_gap_on_point();
	if (size == capacity) {
		buff_expand();
	}
	//insert char at point ptr
	*point = ch;
	//increase point position
	point++;
	//move gap start index
	gapstrt++;
	//may not be a potential solution!!!
	//if( gapstrt!=gapend ) gapstrt++;
	
	//increse size count
	size++;
	return 1;
}

int TextBuffer::buff_insert(std::string str)
{
	auto str_size = str.length();
	for (size_t i = 0; i < str_size; ++i) {
		buff_insert(str[i]);
	}
	return 1;
}

int TextBuffer::buff_throw()
{
	if (point != gapstrt) focus_gap_on_point();
	if (point == &buffer[0]) {
		return 0;
	}
	point--;
	gapstrt--;
	size--;
	return 1;
}

/*
TODO: this method currently won't work with seeking from a generic pointer position since it is not aware of the gap buffer in current implementation -> TESTING PHASE
*/
int TextBuffer::seekc_backward(char ch, char*& cptr)
{
	auto old = cptr;
	while (cptr != &buffer[0]) {
		cptr--;
		if (cptr >= gapstrt && cptr <= gapend && size != capacity) continue;
		if (*cptr == ch) {
			return 1;
		}
	}
	cptr = old;
	return 0;
}

/*
TODO: same issue, see seekc_backward TODO -> TESTING PHASE -> WORKING
*/
int TextBuffer::seekc_forward(char ch, char*& cptr)
{
	auto old = cptr;
	while (cptr != &buffer[capacity]) {
		cptr++;
		if (cptr >= gapstrt && cptr <= gapend && size != capacity) continue;
		if (*cptr == ch) {
			return 1;
		}
	}
	cptr = old;
	return 0;
}

void TextBuffer::buff_expand()
{
	auto old_buff = buffer;
	auto newslots = sizeof(int);
	auto newcap = capacity + newslots;
	buffer = (char*)malloc((newcap + 1)*sizeof(char));
	size_t c = 0;
	while (&old_buff[c] != gapstrt) {
		buffer[c] = old_buff[c];
		c++;
	}
	gapstrt = &buffer[c];
	point = gapstrt;

	c = 0;
	while (&old_buff[capacity - c] != gapend) {
		buffer[newcap - c] = old_buff[capacity - c];
		c++;
	}
	gapend = &buffer[newcap - c];
	auto gap_end_index = gapend - &buffer[0];
	capacity = newcap;
	free(old_buff);
}


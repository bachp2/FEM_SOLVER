#pragma once
#include <cstdlib>
#include <string>
#include <assert.h>
#define DEFAULT_BUFFER_SIZE 16
#define TESTING_MODE

class TextBuffer
{
	char* buffer;
	char* point;
	char* gapstrt;
	char* gapend;
	size_t size;
	size_t capacity;
public:
	TextBuffer();
	~TextBuffer();
	
	void move_point_left() {
		if (point - 1 == gapend && size != capacity) point -= get_gap_size()+1;
		else if(point != begin()) point--;
		//if (*point == '\n') point--;
		//printf("gap start index %d, gap end %d, point %d\n", get_gap_start_index(), get_gap_end_index(), get_point_index());
		//printf("end: %d\n\n", end()-begin());
	}

	void move_point_right() {
		auto tmp = point;
		if (point == gapstrt && size != capacity) tmp += get_gap_size() + 1;
		else if (point == gapstrt - 1 && size != capacity) tmp += get_gap_size() + 1;
		else if (point != end()) tmp++;
		if (tmp <= end())
		{
			point = tmp;
		}
		//printf("gap start index %d, gap end %d, point %d\n", get_gap_start_index(), get_gap_end_index(), get_point_index());
		//printf("end: %d\n\n", end() - begin());
	}

	//NOTE: There is a bug affecting the movement to the fist line if the cursor's at the end of second line->FIXED
	void move_point_up() {
		auto tmp1 = point;
		if (seekc_backward('\n', tmp1))
		{
			auto col = point - tmp1 - 1;
			//account for gap
			if (point > gapend && tmp1 <= gapstrt && size != capacity) {
				col -= get_gap_size();
			}
			point = seekc_backward('\n', tmp1) ? tmp1+1 : begin();
			for (int i = 0; i < col; ++i) {
				if (*point == '\n') break;
				move_point_right();
			}
		}
	}

	void move_point_down() {
		auto p = point;
		auto tmp1 = point;
		int col = 0;
		tmp1 = !seekc_backward('\n', p) ? begin() : p+1;

		col = point - tmp1 + 1;
		//account for gap
		if (point > gapend && tmp1 <= gapstrt && size != capacity) {
			col -= get_gap_size();
		}
		if (*point != '\n') seekc_forward('\n', point);
		
		if (*point == '\n')
		{
			for (int i = 0; i < col; ++i) {
				move_point_right();
				if (*point == '\n') break;
			}
		}
		
	}

	void focus_gap_on_point();
	int buff_insert(char ch);
	int buff_insert(std::string str);
	int buff_throw();
	bool isFull() { return size == capacity; }
	bool isNotFull() { return size != capacity; }
	size_t get_capacity() { return capacity; }
	size_t get_size() { return size; }
	bool empty() { return size == 0; }

	std::string str() 
	{ 
		std::string str;
		auto iter = &buffer[0];
		while (iter != gapstrt) {
			str.push_back(*iter);
			iter++;
		}
		iter += get_gap_size();
		while (iter != &buffer[capacity]) {
			str.push_back(*iter);
			iter++;
		}
		//std string is not null terminated
		//str.push_back(*iter);
		return str;
	};

#ifdef TESTING_MODE
	size_t get_gap_size() { return gapend - gapstrt + 1; }
	size_t get_point_index() { return point - &buffer[0]; }
	size_t get_gap_start_index() { return gapstrt - &buffer[0]; }
	size_t get_gap_end_index() { return gapend - &buffer[0]; }
	std::string str_with_gap()
	{
		std::string str;
		auto iter = &buffer[0];
		while (iter != gapstrt) {
			str.push_back(*iter);
			iter++;
		}
		auto gap_size = get_gap_size();
		for (int i = 0; i < gap_size; ++i) {
			str.push_back('_');
		}
		iter += get_gap_size();
		while (iter != &buffer[capacity]) {
			str.push_back(*iter);
			iter++;
		}
		//std string is not null terminated
		//str.push_back(*iter);
		return str;
	};
#endif // TESTING_MODE

void move_caret_to_point(size_t& cln, size_t& ccol) {
		auto ln = 1;
		auto p = point;
		while (seekc_backward('\n', p)) {
			ln++;
		}
		p = point;
		auto ref_pointer = seekc_backward('\n', p) ? p+1 : &buffer[0];
		//count backward until meet the ref pointer while skipping buffer gap
		int col = point - ref_pointer + 1;
		if (point > gapend && ref_pointer <= gapstrt && size != capacity) {
			col -= get_gap_size();
		}
		cln = ln;
		ccol = col;
	}
private:
#ifndef TESTING_MODE
	size_t get_gap_size() { return gapend - gapstrt + 1; }
	size_t get_point_index() { return point - &buffer[0]; }
	size_t get_gap_start_index() { return gapstrt - &buffer[0]; }
	size_t get_gap_end_index() { return gapend - &buffer[0]; }
#endif // !TESTING_MODE
	void buff_expand();
	void clamp(char *& cptr) {
		if (cptr < begin()) cptr = begin();
		else if (cptr >= end()) cptr = end() - 1;
	}
	int seekc_backward(char ch, char*& cptr);
	int seekc_forward(char ch, char*& cptr);
	char *begin() { return &buffer[0]; }
	char *end() { return &buffer[capacity]; }
};


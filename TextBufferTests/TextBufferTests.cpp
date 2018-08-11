#include "stdafx.h"
#include <stdio.h>
#include "CppUnitTest.h"
#include "../FEM_Solver/TextBuffer.h"
#define CAST2INT(a) static_cast<int>(a)  
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
char logger_buff[100];
/*
I'm basically done with the testing except testing for edge cases. Should look into testing the buffer in its full capacity!
	s.Bach Phan (19 July 2018 10:09PM)
*/
namespace TextBufferTests
{		
	TEST_CLASS(BufferTests)
	{
	public:
		TEST_METHOD(insert_test)
		{
			TextBuffer tbuff;
			char ch = 'a';
			Assert::AreEqual(tbuff.buff_insert(ch), 1);
		}
		TEST_METHOD(insert_expanse_cap)
		{
			TextBuffer tbuff;
			Assert::AreEqual(CAST2INT(tbuff.get_capacity()), DEFAULT_BUFFER_SIZE);
			for (size_t i = 0; i <= DEFAULT_BUFFER_SIZE; ++i) {
				char ch = 'a';
				tbuff.buff_insert(ch);
			}

			Assert::AreEqual(CAST2INT(tbuff.get_capacity()), DEFAULT_BUFFER_SIZE + CAST2INT(sizeof(int)));
		}
		TEST_METHOD(pointers_position)
		{
			TextBuffer tbuff;
			char cstr[] = "Into my heart an air that kill";
			auto str_size = sizeof(cstr);
			//Assert::AreEqual(CAST2INT(str_size), 13);
			for (size_t i = 0; i < str_size; ++i) {
				tbuff.buff_insert(cstr[i]);
			}
			Assert::AreEqual(CAST2INT(tbuff.get_capacity()), 32);
			Logger::WriteMessage("capacity test: PASSED!");
			Assert::AreEqual(CAST2INT(tbuff.get_point_index()), CAST2INT(str_size));
			Logger::WriteMessage("point index test: PASSED!");
			Assert::AreEqual(CAST2INT(tbuff.get_gap_start_index()), CAST2INT(str_size));
			Logger::WriteMessage("gap start test: PASSED!");
			Assert::AreEqual(CAST2INT(tbuff.get_gap_end_index()), CAST2INT(31));
			Logger::WriteMessage("gap end test: PASSED!");
		}
		TEST_METHOD(buffer_string)
		{
			TextBuffer tbuff;
			char cstr[] = "Into my heart an air that kills";
			auto str_size = sizeof(cstr);
			//Assert::AreEqual(CAST2INT(str_size), 13);
			for (size_t i = 0; i < str_size; ++i) {
				tbuff.buff_insert(cstr[i]);
			}
			auto buffer = tbuff.get_text_buffer();
			Logger::WriteMessage(buffer);
			Assert::AreEqual(tbuff.str().c_str(), cstr);
		}

		TEST_METHOD(move_point) {
			int curr_ln, curr_col;
			curr_col = curr_ln = 0;
			TextBuffer tbuff;
			std::string poem;
			poem.append("Into my heart an air that kills\n");
			poem.append("From yon far country blows:\n");
			poem.append("What are those blue remembered hills,\n");
			poem.append("What spires, what farms are those?\n");
			poem.append("That is the land of lost content,\n");
			poem.append("I see it shining plain,\n");
			poem.append("The happy highways where I went\n");
			poem.append("And cannot come again.\n");
			tbuff.buff_insert(poem);
			Assert::AreNotEqual(tbuff.get_size(), tbuff.get_capacity());
			Assert::AreEqual(tbuff.str(), poem);
			tbuff.get_ln_col_at_point(curr_ln, curr_col);
			Assert::AreEqual(curr_ln, 9);
			Assert::AreEqual(curr_col, 1);
			tbuff.move_point_to(5,5);
			tbuff.get_ln_col_at_point(curr_ln, curr_col);
			Assert::AreEqual(curr_ln, 5);
			Logger::WriteMessage("check current line after moving: PASSED!");
			Assert::AreEqual(curr_col, 5);
			Logger::WriteMessage("check current column after moving: PASSED!");
		}

		TEST_METHOD(get_current_ln_col)
		{
			int curr_ln, curr_col;
			curr_col = curr_ln = 0;
			TextBuffer tbuff;
			std::string poem;
			poem.append("Into my heart an air that kills\n");
			poem.append("From yon far country blows:\n");
			poem.append("What are those blue remembered hills,\n");
			poem.append("What spires, what farms are those?\n");
			poem.append("That is the land of lost content,\n");
			poem.append("I see it shining plain,\n");
			poem.append("The happy highways where I went\n");
			poem.append("And cannot come again.\n");
			tbuff.buff_insert(poem);
			Assert::AreNotEqual(tbuff.get_size(), tbuff.get_capacity());
			Assert::AreEqual(tbuff.str(), poem);
			tbuff.get_ln_col_at_point(curr_ln, curr_col);
			Assert::AreEqual(curr_ln, 9);
			Assert::AreEqual(curr_col, 1);
		}
		TEST_METHOD(focus_gap_on_point)
		{
			int curr_ln, curr_col;
			curr_col = curr_ln = 0;
			TextBuffer tbuff;
			std::string poem;
			poem.append("Into my heart an air that kills\n");
			poem.append("From yon far country blows:\n");
			poem.append("What are those blue remembered hills,\n");
			poem.append("What spires, what farms are those?\n");
			poem.append("That is the land of lost content,\n");
			poem.append("I see it shining plain,\n");
			poem.append("The happy highways where I went\n");
			poem.append("And cannot come again.\n");
			tbuff.buff_insert(poem);
			Assert::AreNotEqual(tbuff.get_size(), tbuff.get_capacity());
			Assert::AreEqual(tbuff.str(), poem);
			tbuff.get_ln_col_at_point(curr_ln, curr_col);
			Assert::AreEqual(curr_ln, 9);
			Assert::AreEqual(curr_col, 1);
			Assert::AreEqual(CAST2INT(tbuff.get_gap_size()), 2);
			//tbuff.move_point_to(8, 16);
			tbuff.move_point_to(5, 5); 
			tbuff.focus_gap_on_point();
			Logger::WriteMessage(tbuff.str_with_gap().c_str());
			sprintf_s(logger_buff, "gap start index: %d\n", tbuff.get_gap_start_index());
			Logger::WriteMessage(logger_buff);
			sprintf_s(logger_buff, "gap end index: %d\n", tbuff.get_gap_end_index());
			Logger::WriteMessage(logger_buff);
			sprintf_s(logger_buff, "point index: %d\n", tbuff.get_point_index());
			Logger::WriteMessage(logger_buff);
		}
		TEST_METHOD(full_capacity) {
			int curr_ln, curr_col;
			curr_col = curr_ln = 0;
			TextBuffer tbuff;
			tbuff.buff_insert("Into my heart an ai\n");
			tbuff.buff_insert("Into my heart an ai\n");
			tbuff.buff_insert("Into my heart an ai\n");
			Assert::AreEqual(tbuff.get_size(), tbuff.get_capacity());
			tbuff.get_ln_col_at_point(curr_ln, curr_col);
			Assert::AreEqual(curr_ln, 4);
			Assert::AreEqual(curr_col, 1);
			tbuff.move_point_to(1, 2);
			tbuff.focus_gap_on_point();
			Logger::WriteMessage(tbuff.str_with_gap().c_str());
			sprintf_s(logger_buff, "gap start index: %d\n", tbuff.get_gap_start_index());
			Logger::WriteMessage(logger_buff);
			sprintf_s(logger_buff, "gap end index: %d\n", tbuff.get_gap_end_index());
			Logger::WriteMessage(logger_buff);
			sprintf_s(logger_buff, "point index: %d\n", tbuff.get_point_index());
			Logger::WriteMessage(logger_buff);
			tbuff.buff_insert('a');
			Logger::WriteMessage(tbuff.str_with_gap().c_str());
		}
	};
}
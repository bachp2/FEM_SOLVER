#pragma once
#include "base.h"
#include "mrect.h"
#include "text.h"
#include "caret.h"
//#define LOGGING

enum MouseBehavior { MOUSEFREE, ON_CLICKED, ON_DRAGED, SCRNIN, SCRNOUT };
typedef std::shared_ptr<Node> nPtr;
typedef std::shared_ptr<Element> ePtr;
namespace GUI_Flag {
	enum State {
		SCALABLE    = 1 << 0,
		SCRIPTABLE  = 1 << 1,
		DRAWABLE    = 1 << 2,
		SCROLLABLE  = 1 << 3,
		ZOOMABLE    = 1 << 4,
		SELECTABLE  = 1 << 5,

	};
}
struct Pane : mRect {


};

struct Divider : mRect {


};

struct Mouse {
	int posX, posY;
	SDL_SystemCursor mid;
	SDL_Cursor* cursor;
	MouseBehavior state = MOUSEFREE;
	bool willOverlapped = false;
	bool isOnNode = false;
	bool isOnElement = false;
	void free_cursor() {
		if (cursor != NULL) SDL_FreeCursor(cursor);
	}

	void setSystemCursor(SDL_SystemCursor id) 
	{
		cursor = SDL_CreateSystemCursor(id);
		SDL_SetCursor(cursor);
	}
};

extern Truss truss;
struct Logger {
	std::ofstream outputFile;
	std::string fname;
	bool flag;//zero if empty
	Logger(std::string name) : fname(name) {}
};


namespace IO {
	namespace aux {
		inline void logging_position(std::ofstream& o, int x, int y) {
			o << "<" << x << "," << y << ">";
		}
	}

	inline void logging(Logger &logger, std::string des, int x, int y) {
		std::ofstream outputFile;
		outputFile.open(logger.fname, std::ios::app);
		outputFile << des << ": <" << x << ", " << y << ">\n";
		outputFile.close();
	}

	inline void logging(Logger &logger, Truss &truss) {
		std::ofstream outputFile;
		outputFile.open(logger.fname, std::ios::app);
		for (auto &n : truss.nodes) {
			outputFile << "node" << n->id << "\t";
			aux::logging_position(outputFile, n->position(0), n->position(1));
			outputFile << "\n";
		}
		outputFile.close();
	}
}

class Window_Manager
{
public:
	int wInit();
	void closeRoutine();
	void handleEvents();
	MouseBehavior handleMouseStates(Uint32 mouseEvent, Uint8 btn_type);
	void drawAndUpdate();
	void doLoop();
	void doHandle();
	int onEntity();
	Window_Manager() {};
	~Window_Manager() {};

	SDL_Window *gWindow = nullptr;
	SDL_Renderer *gRenderer = nullptr;

	//SDL_Surface *wSurface = nullptr;
	SDL_Color backGroundColor = { 255, 255, 255, 255 };
	int wWidth, wHeight;
	Mouse mouse;
	Vector2i mouse_pos_on_dragged;
	Text text;
private:
	bool isRunning = true;
	FPSmanager fpsManager;
	mRect scriptingPane;
	mRect drawingPane;
	mRect paneDivider;
};


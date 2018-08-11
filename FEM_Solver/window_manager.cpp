#include "Window_Manager.h"
#ifdef LOGGING 
Logger logger("truss_structure.log");
#endif 
Truss truss(0, 0);
int Window_Manager::wInit()
{
	// Allocate console for program output
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// Init video
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Video Initialization Error: %s", SDL_GetError());
		return 1;
	}
	// Create first window
	wWidth = 1024;
	wHeight = 620;
	gWindow = SDL_CreateWindow(
		"FEM SOLVER",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		wWidth, wHeight, SDL_WINDOW_SHOWN
		);
	if (gWindow == NULL) {
		printf("Window Creation Error: %s", SDL_GetError());
		return 1;
	}
	SDL_SetWindowResizable(gWindow, SDL_TRUE);
	//Create Renderer for main window
	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	//SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	//mouse.setSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	SDL_initFramerate(&fpsManager);
	SDL_setFramerate(&fpsManager, 60);

	// Init Text struct
	if (TTF_Init() < 0) {
		printf("Error: %s\n", TTF_GetError());
	}
	text.size = 13;
	text.default_color = { 131,148,150,255 };
	//text.default_color = { 0,0,0,255 };
	text.font = TTF_OpenFont("VeraMono.ttf",text.size);
	text.leftMargin = 3;
	text.topMargin = 3;
	assert(text.font != nullptr);

	//set cursor color
	text.caret.color = { 153, 153, 102, 100 };
	TTF_GlyphMetrics(text.font, 'd', NULL, NULL, NULL, &text.caret.rect.h, &text.caret.rect.w);
	text.caret.setTextBuffer(&text.content);

	//start text input
	SDL_StartTextInput();

	// Create Panes for scripting and drawing
	paneDivider.color = { 18,215,110,255 };
	paneDivider.rect.h = wHeight;
	paneDivider.rect.w = 4;
	paneDivider.rect.x = wWidth / 2 - paneDivider.rect.w / 2;
	paneDivider.rect.y = 0;

	scriptingPane.color = { 0,43,54,255 };
	scriptingPane.rect.h = wHeight;
	scriptingPane.rect.w = wWidth / 2 - paneDivider.rect.w / 2;
	scriptingPane.rect.x = 0;
	scriptingPane.rect.y = 0;
	text.window = scriptingPane.rect;

	drawingPane.color = { 118,118,19,255 };
	drawingPane.rect.h = wHeight;
	drawingPane.rect.w = wWidth / 2 - paneDivider.rect.w / 2;
	drawingPane.rect.x = wWidth / 2 + paneDivider.rect.w / 2;
	drawingPane.rect.y = 0;
	return 0;
}

void Window_Manager::closeRoutine()
{
	// Stop text input
	SDL_StopTextInput();

	// Free global font
	TTF_CloseFont(text.font);
	text.font = nullptr;

	// Close and destroy the window
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	
	// Destroy textures;
	SDL_DestroyTexture(text.texture);
	text.texture = nullptr;
	SDL_DestroyTexture(text.render_texture);
	text.render_texture = nullptr;

	// Free mouse cursor
	mouse.free_cursor();
	mouse.cursor = nullptr;
	// Free window surface
	//SDL_FreeSurface(wSurface);
	//wSurface = nullptr;

	// Destroy window renderer
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	
	// Quit
	TTF_Quit();
	SDL_Quit();
}
void Window_Manager::drawAndUpdate()
{
	assert(gRenderer != nullptr);
	//internal call to draw background
	SDL_utils::SetRenderDrawColor(gRenderer, backGroundColor);
	SDL_RenderClear(gRenderer);

	SDL_utils::SetRenderDrawColor(gRenderer, scriptingPane.color);
	SDL_RenderFillRect(gRenderer, &scriptingPane.rect);

	SDL_utils::SetRenderDrawColor(gRenderer, drawingPane.color);
	SDL_RenderFillRect(gRenderer, &drawingPane.rect);
	
	SDL_utils::SetRenderDrawColor(gRenderer, paneDivider.color);
	SDL_RenderFillRect(gRenderer, &paneDivider.rect);

	//Draw Text to Scripting Pane
	SDL_utils::renderTexture(text.render_texture, gRenderer, 0, 0, &text.window);

	//draw entities
	SDL_utils::RenderDrawStruct(gRenderer, truss);
	
	//push to front
	SDL_RenderPresent(gRenderer);
	SDL_UpdateWindowSurface(gWindow);
}

void Window_Manager::handleEvents()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev) != 0) {
		switch (ev.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (ev.key.keysym.sym == SDLK_BACKSPACE && !text.content.empty()) 
			{
				//lop off character
				text.content.buff_throw();
				text.caret.move();
			}
			else if (ev.key.keysym.sym == SDLK_RETURN)
			{
				text.append_to_content('\n');
				text.caret.move();
				//printf("\n");
			}
			else if (ev.key.keysym.sym == SDLK_LEFT)
			{
				text.content.move_point_left();
				text.caret.move();
			}
			else if (ev.key.keysym.sym == SDLK_RIGHT) 
			{
				text.content.move_point_right();
				text.caret.move();
				//printf("ln:%d, col:%d", text_caret.current_line, text_caret.current_column);
			} 
			else if (ev.key.keysym.sym == SDLK_UP)
			{
				text.content.move_point_up();
				text.caret.move();
			}
			else if (ev.key.keysym.sym == SDLK_DOWN)
			{
				text.content.move_point_down();
				text.caret.move();
			}
#ifdef LOGGING  
			if (ev.key.keysym.sym == SDLK_a) {
				printf("Opened Logger...");
				logger.outputFile.open(logger.fname);
				logger.outputFile << "mouse state " << mouse.state << "\n";
				logger.outputFile.close();
				IO::logging(logger, "mouse postition", mouse.posX, mouse.posY);
				logger.flag = 1;
				IO::logging(logger, truss);
				printf("Recorded to %s", logger.fname.c_str());
			}
#endif
			break;
		case SDL_TEXTINPUT:
			//printf(ev.text.text);
			text.append_to_content(ev.text.text[0]);
			text.caret.move();
			//assert(text_caret.cols_of_lines[text_caret.current_line - 1] != 0)
			break;
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			mouse.state = handleMouseStates(ev.type, ev.button.button);
			break;
		case SDL_MOUSEWHEEL:
			if (ev.wheel.y > 0) // scroll up
			{
				text.scroll_up(scriptingPane);
			}
			else if (ev.wheel.y < 0) // scroll down
			{
				text.scroll_down(scriptingPane);
			}
			break;
		}
	}
}
MouseBehavior Window_Manager::handleMouseStates(Uint32 mouseEvent, Uint8 btn_type)
{
	switch (mouse.state) {
	case MOUSEFREE:
		if (btn_type == SDL_BUTTON_LEFT) {
			if (mouseEvent == SDL_MOUSEBUTTONDOWN)
				mouse.state = ON_CLICKED;
		}
		else {
			if (mouseEvent == SDL_MOUSEBUTTONDOWN) {
				mouse_pos_on_dragged(0) = mouse.posX;
				mouse_pos_on_dragged(1) = mouse.posY;
				mouse.state = ON_DRAGED;
			}
		}
		break;
	case ON_CLICKED:
		if (mouseEvent == SDL_MOUSEBUTTONUP)
			mouse.state = MOUSEFREE;
		break;
	case ON_DRAGED:
		if (mouseEvent == SDL_MOUSEBUTTONUP)
			mouse.state = MOUSEFREE;
		break;
	}
	return mouse.state;
}

int Window_Manager::onEntity()
{
	auto wSurface = SDL_GetWindowSurface(gWindow);
	auto p32 = SDL_utils::get_pixel32(wSurface, mouse.posX, mouse.posY);
	SDL_Color c;
	SDL_GetRGBA(p32, wSurface->format, &(c.r), &(c.g), &(c.b), &(c.a));
	wSurface = nullptr;
	auto flag = SDL_utils::isEqual(c, backGroundColor);
	return !flag;
}

void Window_Manager::doLoop()
{
	while (isRunning)
	{
		SDL_GetMouseState(&mouse.posX, &mouse.posY);
		//text_caret.blink();
		handleEvents();
		doHandle();
		drawAndUpdate();
		//delay excecution and calculate running time FPS
		/*printf("FPS:%d\n", SDL_calculateRunningTimeFPS
			(
				SDL_framerateDelay(&fpsManager)
			));*/
	}
}

void Window_Manager::doHandle()
{
	text.draw_to_render_texture(gRenderer, scriptingPane);
	
	if (drawingPane.isInRect(mouse.posX, mouse.posY))
		mouse.setSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	else if (paneDivider.isInRect(mouse.posX, mouse.posY))
		mouse.setSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	else
		mouse.setSystemCursor(SDL_SYSTEM_CURSOR_IBEAM); 

	if (mouse.state == ON_CLICKED) {
		assert(&truss != nullptr);
		if (!(mouse.isOnNode)) {
			for (auto it = truss.nodes.begin(); it != truss.nodes.end(); ++it) {
				if (truss.isMouseOnNode(*it, mouse.posX, mouse.posY)) {
					mouse.isOnNode = true;
					(*it)->isSelected = true;
					break;
				}
				else if (truss.checkNodeOverlapped(*it, mouse.posX, mouse.posY)) {
					mouse.willOverlapped = true;
					break;
				}
			}
		}

		if (!(mouse.willOverlapped) && !(mouse.isOnNode)) {
			nPtr nptr(new Node(Vector2f(mouse.posX, mouse.posY), { 250,128,114,255 }, 5));
			truss.connectNodes(nptr);
			truss.appendNode(nptr);
		}
		
		if (mouse.isOnNode) {
			for (auto it = truss.nodes.begin(); it != truss.nodes.end(); ++it) {
				if ((*it)->isSelected) (*it)->updatePosition(mouse.posX, mouse.posY);
			}
		}

	}
	else if (mouse.state == ON_DRAGED) {
		SDL_Rect dragRect;
		dragRect.x = mouse_pos_on_dragged(0);
		dragRect.y = mouse_pos_on_dragged(1);
		dragRect.w = mouse.posX - mouse_pos_on_dragged(0);
		dragRect.h = mouse.posY - mouse_pos_on_dragged(1);
		SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
		SDL_RenderDrawRect(gRenderer, &dragRect);
	}
	else if (mouse.state == MOUSEFREE) {
		mouse.willOverlapped = false;
		mouse.isOnNode = false;
		for (auto it = truss.nodes.begin(); it != truss.nodes.end(); ++it) {
			(*it)->isSelected = false;
		}
	}
}
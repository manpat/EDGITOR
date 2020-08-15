#include "VARIABLES.h"
#include "COLOR.h"
#include <filesystem>

// manpat: really not a fan of this - this should be cmakes job :(
#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>
#else
#include <SDL.h>
#include <SDL_image.h>
#endif


  //
 //   VARIABLES   ///////////////////////////////////////////////// ///////  //////   /////    ///     //      /
//

SDL_Renderer* RENDERER;
SDL_Texture* FONTMAP;

TTF_Font* FONT;
uint16_t FONT_CHRW;
uint16_t FONT_CHRH;

bool QUIT = false;
float FPS = 0;

std::string CURRENT_PATH = (std::filesystem::current_path().string());
std::string CURRENT_FILE = "";

char KEY_TEXT[256];
int* KEY_TEXT_HOLD = nullptr;
int KEY_TEXT_INT;

int16_t MOUSE_X;
int16_t MOUSE_Y;
int16_t MOUSE_PREVX;
int16_t MOUSE_PREVY;

int WINDOW_X;
int WINDOW_Y;
int WINDOW_W;
int WINDOW_H;

int DISPLAY_W;
int DISPLAY_H;

bool MOUSEBUTTON_LEFT = false;
bool MOUSEBUTTON_PRESSED_LEFT = false;
bool MOUSEBUTTON_MIDDLE = false;
bool MOUSEBUTTON_PRESSED_MIDDLE = false;
bool MOUSEBUTTON_RIGHT = false;
bool MOUSEBUTTON_PRESSED_RIGHT = false;
int16_t MOUSEWHEEL_X = 0;
int16_t MOUSEWHEEL_Y = 0;

bool KEYBOARD_CTRL = false;
bool KEYBOARD_PRESSED_CTRL = false;
bool KEYBOARD_SHIFT = false;
bool KEYBOARD_PRESSED_SHIFT = false;
bool KEYBOARD_ALT = false;
bool KEYBOARD_PRESSED_ALT = false;

bool KEYBOARD_SPACE = false;
bool KEYBOARD_PRESSED_SPACE = false;
bool KEYBOARD_ESC = false;
bool KEYBOARD_PRESSED_ESC = false;

// TOOL
uint16_t CURRENT_TOOL = TOOL::BRUSH;
uint16_t TEMP_CURRENT_TOOL = CURRENT_TOOL;



SDL_Cursor* create_system_cursor()
{
	static const bool arrow[] = {
		1,0,0,0,0,0,1,0,
		0,1,0,0,0,1,0,0,
		0,0,1,0,1,0,0,0,
		0,0,0,1,0,0,0,0,
		0,0,1,0,1,0,0,0,
		0,1,0,0,0,1,0,0,
		1,0,0,0,0,0,1,0,
		0,0,0,0,0,0,0,0
	};

	Uint8 data[8*8];
	Uint8 mask[8*8];
	int n = -1;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (j % 8) {
				data[n] <<= 1;
				mask[n] <<= 1;
			}
			else {
				++n;
				data[n] = mask[i] = 0;
			}
			if (arrow[i * 8 + j])
			{
				data[n] |= 0x00;
				mask[n] |= 0x01;
			}
		}
	}

	return SDL_CreateCursor(data, mask, 8, 8, 3, 3);
}

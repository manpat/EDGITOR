#pragma once

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

void INIT_SDL();
SDL_Window* INIT_WINDOW();
SDL_Renderer* INIT_RENDERER(SDL_Window* WINDOW);
SDL_Texture* INIT_FONT(SDL_Renderer* renderer);
void UPDATE_INPUT();

void SYSTEM_BRUSH_UPDATE();
void SYSTEM_LAYER_UPDATE();
void SYSTEM_CANVAS_UPDATE();

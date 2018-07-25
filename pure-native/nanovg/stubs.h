#include <stdio.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <caml/mlvalues.h>
#include <caml/callback.h>
#include <caml/memory.h>
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

int sdl_init( SDL_Window **w, int width, int height, char* name);
void sdl_cleanup();
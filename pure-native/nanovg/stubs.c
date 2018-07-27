#include "stubs.h"

static struct NVGcontext *vg = NULL;
static SDL_Window *window = NULL;
static int quit = 0;
static int width;
static int height;

CAMLprim value ovg_measureText(value text) {
  CAMLparam1(text);
  CAMLlocal1(tuple);

  static float bounds[4] = { 0, 0, 0, 0 };
  static char* s;
  static int w;

  s = String_val(text);

  nvgFontSize(vg, 24.0f);
  nvgFontFace(vg, "sans");
  nvgTextBounds(vg, 0, 0, s, NULL, &bounds);

  w = bounds[2] - bounds[0];

  CAMLreturn(Val_int(w));
};

CAMLprim value ovg_draw(value draw) {
  CAMLparam1(draw);
  CAMLlocal3(next, cmd, c);
  
  // Empty list
  if (!Is_block(draw)) {
    CAMLreturn(Val_unit);
  }

  cmd = Field(draw, 0);
  next = Field(draw, 1);
  static double x, y, w, h;
  static char *s;
  static int r, g, b, a;

  while (1) {
    /*
    type drawCmd =
      | Rect(float, float, float, float)
      | FillColor(Color.color)
      | FontSize(float)
      | FontFace(string)
      | TextAlign(text_align)
      | Text(float, float, string)
      | StrokeColor(Color.color)
      | StrokeWidth(float);
    */
    if (Is_block(cmd)) {
      switch (Tag_val(cmd)) {
        case 0:
          // Rect(float, float, float, float)
          x = Double_val(Field(cmd, 0));
          y = Double_val(Field(cmd, 1));
          w = Double_val(Field(cmd, 2));
          h = Double_val(Field(cmd, 3));
          nvgRect(vg, x, y, w, h);
          break;
        case 1:
          // FillColor(int, int, int, int)
          r = Field(cmd, 0);
          g = Field(cmd, 1);
          b = Field(cmd, 2);
          a = Field(cmd, 3);
          nvgFillColor(vg, nvgRGBA(r, g, b, a));
          break;
        /*
        case 2:
          // FontSize(float)
          w = Double_val(Field(cmd, 0));
          nvgFontSize(vg, w);
          break;
        case 3:
          // FontFace(string)
          s = String_val(Field(cmd, 0));
          nvgFontFace(vg, s);
          break;
        case 4:
          // TextAlign
          break;
        */
        case 5:
          // Text(float, float, string)
          x = Double_val(Field(cmd, 0));
          y = Double_val(Field(cmd, 1));
          s = String_val(Field(cmd, 2));
          nvgBeginPath(vg);
          nvgTextAlign(vg,NVG_ALIGN_LEFT|NVG_ALIGN_TOP);
          nvgFontSize(vg, 24.0f);
          nvgFontFace(vg, "sans");
          nvgFillColor(vg, nvgRGBA(0, 0, 0, 255));
          nvgText(vg, x, y, s, NULL);
          nvgFill(vg);
          break;
        /*
        case 6:
          // StrokeColor
          break;
        case 7:
          // StrokeWidth(float)
          w = Double_val(Field(cmd, 0));
          nvgStrokeWidth(vg, w);
          break;
        */
      }
    } else {
      /*
      type drawCmd =
        | Stroke
        | BeginPath
        | Fill
        | End;
      */
      switch (Int_val(cmd)) {
        // Stroke
        case 0:
          nvgStroke(vg);
          break;
        // BeginPath
        case 1:
          nvgBeginPath(vg);
          break;
        // Fill
        case 2:
          nvgFill(vg);
          break;
      }
    }
    if (!Is_block(next)) {
      CAMLreturn(Val_unit);
    }
    cmd = Field(next, 0);
    next = Field(next, 1);
  }

  CAMLreturn(Val_unit);
};

CAMLprim value ovg_run_event_loop() {
  CAMLlocal1(event);
  static SDL_Event e;
  static SDL_MouseButtonEvent *mouse_event = &e;

  SDL_WaitEvent(&e);
  switch( e.type )
  {
  case SDL_KEYDOWN:
    return Val_int(1);
    break;
  case SDL_KEYUP:
    return Val_int(0);
    break;
  case SDL_MOUSEBUTTONDOWN:
    event = caml_alloc_shr(2, 0);
    Store_field(event, 0, Val_int(mouse_event->x));
    Store_field(event, 1, Val_int(mouse_event->y));
    break;
  case SDL_MOUSEBUTTONUP:
    event = caml_alloc_shr(2, 1);
    Store_field(event, 0, Val_int(mouse_event->x));
    Store_field(event, 1, Val_int(mouse_event->y));
    break;
  case SDL_MOUSEMOTION:
    break;
  case SDL_MOUSEWHEEL:
    break;
  case SDL_QUIT:
    quit = 1;
    break;
  };

  return event;
};

CAMLprim value ovg_render(value draw) {
  CAMLparam1(draw);

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
  nvgBeginFrame( vg, width, height, 1);
  nvgResetTransform( vg );

  ovg_draw(draw);

  nvgEndFrame( vg );
  nvgluBindFramebuffer( NULL );
  SDL_GL_SwapWindow( window );

  CAMLreturn(Val_unit);
}

CAMLprim value ovg_create_window(value x, value y, value n) {
  width = Int_val(x);
  height = Int_val(y);
  char *name;
  name = String_val(n);

  if ( sdl_init( &window, width, height, name ) != 0 )
  {
    printf( "Couldn't init SDL: %s\n", SDL_GetError() );
    return 1;
  }

  // struct NVGcontext *vg = NULL;
  struct NVGLUframebuffer *fb = NULL;

  vg = nvgCreateGL3( NVG_ANTIALIAS|NVG_STENCIL_STROKES );
  if ( !vg )
  {
    printf( "Couldn't create nanovg gl3 context\n" );
    SDL_Quit();
    return 2;
  }

  int font = nvgCreateFont(vg, "sans", "Roboto-Regular.ttf");

  return Val_unit;
}

int sdl_init( SDL_Window **w, int width, int height, char* name)
{
  if ( SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS ) == 0 )
  {
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    *w = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL );
    if ( *w )
    {
      SDL_GL_CreateContext( *w );

      glewExperimental = GL_TRUE;
      GLenum err = glewInit();
      if ( err == GLEW_OK )
      {
        glEnable(GL_STENCIL_TEST);

        return 0;
      }
      else
      {
        SDL_SetError( "GLEW Reported an Error: %s", glewGetErrorString(err) );
      }
    }
  }
  if ( *w ) SDL_DestroyWindow( *w );
  *w = NULL;
  return 1;
}

CAMLprim value ovg_cleanup() {
  nvgDeleteGL3( vg );
  SDL_Quit();
};
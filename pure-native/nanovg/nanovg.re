type event =
  | KeyUp
  | KeyDown
  | MouseButtonDown(int, int)
  | MouseButtonUp(int, int)
  | MouseMotion(int, int)
  | MouseWheel
  | Quit
  | UnhandledEvent;

type text_align =
  | Left
  | Center
  | Right
  | Top
  | Middle
  | Bottom
  | Baseline;

type drawCmd =
  | Stroke
  | BeginPath
  | Fill
  | Rect(float, float, float, float)
  | FillColor(int, int, int, int)
  | FontSize(float)
  | FontFace(string)
  | TextAlign(text_align)
  | Text(float, float, string)
  | StrokeColor(int, int, int, int)
  | StrokeWidth(float);

type draw = array(drawCmd);

external createWindow : (int, int, string) => unit = "ovg_create_window";

external draw : draw => unit = "ovg_draw";

external runEventLoop : unit => event = "ovg_run_event_loop";

external render : (draw, int) => event = "ovg_render";

external cleanup : unit => unit = "ovg_cleanup";

external measureText : string => int = "ovg_measureText";
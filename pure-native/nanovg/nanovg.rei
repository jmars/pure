type event =
  | KeyUp
  | KeyDown
  | MouseButtonDown(int, int)
  | MouseButtonUp(int, int)
  | MouseMotion(int, int)
  | Resized(int, int)
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

let createWindow: (int, int, string) => unit;

let draw: draw => unit;

let runEventLoop: unit => event;

let render: (draw, int) => event;

let cleanup: unit => unit;

let measureText: string => int;
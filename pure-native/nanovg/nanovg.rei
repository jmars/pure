type event =
  | KeyUp
  | KeyDown
  | MouseButtonDown(int, int)
  | MouseButtonUp(int, int)
  | MouseMotion(int, int)
  | MouseWheel;

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

type draw = list(drawCmd);

let createWindow: (int, int, string) => unit;

let draw: draw => unit;

let runEventLoop: unit => event;

let render: draw => unit;

let cleanup: unit => unit;
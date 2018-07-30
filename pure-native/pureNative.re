open Pure;

type anyNode = [ | `Window | `View | `Text];

type text = {
  mutable layout: ReconcilerSpec.cssLayout,
  mutable draw: Nanovg.draw,
  mutable props,
};

type view = {
  mutable layout: ReconcilerSpec.cssLayout,
  mutable draw: Nanovg.draw,
  mutable props,
  mutable children: list(node([ | `View | `Text])),
}
and node(_) =
  | Window(ref(view)): node([< anyNode > `Window])
  | View(view): node([< anyNode > `View])
  | Text(text): node([< anyNode > `Text]);

let defaultText = () => {
  layout: {
    left: 0.,
    top: 0.,
    right: 0.,
    bottom: 0.,
    width: 0.,
    height: 0.,
  },
  draw: [||],
  props: PurePrimitives.defaultProps,
};

let defaultView = () => {
  layout: {
    left: 0.,
    top: 0.,
    right: 0.,
    bottom: 0.,
    width: 0.,
    height: 0.,
  },
  draw: [||],
  props: PurePrimitives.defaultProps,
  children: [],
};

let some_default = (d, s) =>
  switch (s) {
  | Some(s) => s
  | None => d
  };

let rec draw_of_childNode = (x, y, n: node([ | `Text | `View])) =>
  switch (n) {
  | View(v) => draw_of_view(x, y, v)
  | Text(s) => draw_of_text(x, y, s)
  }
and draw_of_view = (x, y, v) => {
  open Nanovg;
  let bg =
    some_default(
      Color.rgba(255, 255, 255, 1.0),
      v.props.style.backgroundColor,
    );
  let color = Color.toRgb(bg);
  let self = [|
    BeginPath,
    Rect(
      x +. v.layout.left,
      y +. v.layout.top,
      v.layout.width,
      v.layout.height,
    ),
    FillColor(
      color.red,
      color.green,
      color.blue,
      255.0 *. color.alpha |> int_of_float,
    ),
    Fill,
  |];
  Array.append(
    self,
    Array.concat(
      List.map(
        draw_of_childNode(x +. v.layout.left, y +. v.layout.top),
        v.children,
      ),
    ),
  );
}
and draw_of_text = (x, y, s) => {
  let value = some_default("", s.props.value);
  [|Text(x +. s.layout.left, y +. s.layout.top, value)|];
};

let window = Window(ref(defaultView()));

let rec triggerEvent = (e: Nanovg.event, xv: int, yv: int, v: view) => {
  let left = (v.layout.left |> int_of_float) + xv;
  let top = (v.layout.top |> int_of_float) + yv;
  let right = (v.layout.left +. v.layout.width |> int_of_float) + xv;
  let bottom = (v.layout.top +. v.layout.height |> int_of_float) + yv;
  switch (e) {
  | KeyUp => ()
  | KeyDown => ()
  | MouseButtonUp(_x, _y) => ()
  | MouseButtonDown(x, y) =>
    switch (v.props.onClick) {
    | None => ()
    | Some(f) =>
      if (x > left && x < right) {
        if (y > top && y < bottom) {
          f();
        };
      }
    }
  | MouseMotion(_x, _y) => ()
  | MouseWheel => ()
  | Quit => ()
  | UnhandledEvent => ()
  };
  List.iter(
    c =>
      switch (c) {
      | View(cv) => triggerEvent(e, left, top, cv)
      | _ => ()
      },
    v.children,
  );
};

module Host: ReconcilerSpec.HostConfig = {
  type hostNode =
    | Node(node(anyNode)): hostNode;
  let createInstance: Pure.pureElement => hostNode =
    el =>
      switch (el) {
      | Nested(primitive, props, _) =>
        switch (primitive) {
        | View => Node(View({...defaultView(), props}))
        | Text => Node(Text({...defaultText(), props}))
        }
      | Flat(_) => Node(window)
      };
  let createTextInstance = value =>
    Node(
      Text({
        ...defaultText(),
        props: {
          ...PurePrimitives.defaultProps,
          value: Some(value),
        },
      }),
    );
  let commitUpdate = (Node(node), _oldProps, props) =>
    switch (node) {
    | View(v) => v.props = props
    | Text(t) => t.props = props
    | Window(_) => ()
    };
  let appendChild = (Node(parent), Node(node)) =>
    switch (parent, node) {
    | (Window(w), View(c)) => w := c
    | (View(p), View(_) as n) => p.children = p.children @ [n]
    | (View(p), Text(_) as t) => p.children = p.children @ [t]
    | (Window(_), _) => ()
    | (_, Window(_)) => ()
    | (Text(_), _) => ()
    };
  let removeChild = (Node(parent), Node(node)) =>
    switch (parent, node) {
    | (View(p), View(_) as n) =>
      p.children = List.filter(c => n == c, p.children)
    | (View(p), Text(_) as t) =>
      p.children = List.filter(c => t == c, p.children)
    | (Text(_), _) => ()
    | (Window(_), _) => ()
    | (_, Window(_)) => ()
    };
  let applyLayout = (Node(node), layout) => {
    open ReconcilerSpec;
    switch (node) {
    | View(v) => v.layout = layout
    | Text(t) => t.layout = layout
    | Window(_) => ()
    };
    ();
  };
};

module NanoVGReconciler = Reconciler.Make(Host);

let render = (pureElement: Pure.pureElement, windowName) => {
  let w =
    switch (window) {
    | Window(v) => v
    | _ => assert(false)
    };
  Nanovg.createWindow(400, 400, windowName);
  NanoVGReconciler.updateQueue :=
    NanoVGReconciler.updateQueue^
    @ [
      HostRoot({
        node: Host.createInstance(pureElement),
        children: pureElement,
      }),
    ];
  let break = ref(false);
  while (! break^) {
    NanoVGReconciler.perfomWork();
    let draw = draw_of_view(0., 0., w^);
    let event = Nanovg.render(draw, Array.length(draw));
    switch (event) {
    | MouseMotion(_, _) => ()
    | MouseButtonDown(x, y) => triggerEvent(event, 0, 0, w^)
    | MouseButtonUp(_, _) => ()
    | KeyUp => ()
    | KeyDown => ()
    | MouseWheel => ()
    | Quit => break := true
    | UnhandledEvent => ()
    };
    ();
  };
  Nanovg.cleanup();
};
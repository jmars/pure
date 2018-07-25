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
  draw: [],
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
  draw: [],
  props: PurePrimitives.defaultProps,
  children: [],
};

let some_default = (d, s) =>
  switch (s) {
  | Some(s) => s
  | None => d
  };

let rec draw_of_childNode = (n: node([ | `Text | `View])) =>
  switch (n) {
  | View(v) => draw_of_view(v)
  | Text(s) => draw_of_text(s)
  }
and draw_of_view = v => {
  open Nanovg;
  let bg =
    some_default(
      Color.rgba(255, 255, 255, 1.0),
      v.props.style.backgroundColor,
    );
  let color = Color.toRgb(bg);
  let self = [
    BeginPath,
    Rect(v.layout.left, v.layout.top, v.layout.width, v.layout.height),
    FillColor(
      color.red,
      color.green,
      color.blue,
      255.0 *. color.alpha |> int_of_float,
    ),
    Fill,
  ];
  self @ List.concat(List.map(draw_of_childNode, v.children));
}
and draw_of_text = s => {
  let value = some_default("", s.props.value);
  [Text(s.layout.left, s.layout.top, value)];
};

let window = Window(ref(defaultView()));

let rec triggerEvent = (e: Nanovg.event, v: view) =>
  switch (e) {
  | KeyUp => ()
  | KeyDown => ()
  | MouseButtonDown(_x, _y) => ()
  | MouseButtonUp(x, y) =>
    switch (v.props.onClick) {
    | None => ()
    | Some(f) =>
      let left = v.layout.left |> int_of_float;
      let top = v.layout.top |> int_of_float;
      let right = v.layout.right |> int_of_float;
      let bottom = v.layout.bottom |> int_of_float;
      if (x > left && x < right) {
        if (y > top && y < bottom) {
          f();
        };
      };
      List.iter(
        c =>
          switch (c) {
          | View(v) => triggerEvent(e, v)
          | _ => ()
          },
        v.children,
      );
    }
  | MouseMotion(_x, _y) => ()
  | MouseWheel => ()
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
      | _ => Node(window)
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
    | _ => ()
    };
  let appendChild = (Node(parent), Node(node)) =>
    switch (parent, node) {
    | (Window(w), View(c)) => w := c
    | (View(p), View(_) as n) => p.children = p.children @ [n]
    | (View(p), Text(_) as t) => p.children = p.children @ [t]
    | _ => ()
    };
  let removeChild = (Node(parent), Node(node)) =>
    switch (parent, node) {
    | (View(p), View(_) as n) =>
      p.children = List.filter(c => n == c, p.children)
    | (View(p), Text(_) as t) =>
      p.children = List.filter(c => t == c, p.children)
    | _ => ()
    };
  let applyLayout = (Node(node), layout) => {
    switch (node) {
    | View(v) => v.layout = layout
    | _ => ()
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
  Nanovg.createWindow(800, 600, windowName);
  NanoVGReconciler.updateQueue :=
    NanoVGReconciler.updateQueue^
    @ [
      HostRoot({
        node: Host.createInstance(pureElement),
        children: pureElement,
      }),
    ];
  let i = ref(0);
  while (i^ < 10) {
    NanoVGReconciler.perfomWork();
    let draw = draw_of_view(w^);
    Nanovg.render(draw);
    let event = Nanovg.runEventLoop();
    triggerEvent(event, w^);
    i := i^ + 1;
    ();
  };
  Nanovg.cleanup();
};
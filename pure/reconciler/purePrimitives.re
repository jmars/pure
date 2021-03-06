open Pure;

let defaultLayout = LayoutSupport.defaultStyle;

let defaultStyle: style = {backgroundColor: None};

let defaultProps: Pure.props = {
  id: None,
  value: None,
  title: None,
  disabled: None,
  placeholder: None,
  src: None,
  onClick: None,
  onChangeText: None,
  layout: defaultLayout,
  style: defaultStyle,
};

let createNativeElement =
    (
      nativeElement: primitive,
      ~id: option(string)=?,
      ~value: option(string)=?,
      ~title: option(string)=?,
      ~onClick: option(unit => unit)=?,
      ~layout=defaultLayout,
      ~style=defaultStyle,
      ~children: list(pureElement),
      _: unit,
    ) =>
  switch (nativeElement) {
  | View =>
    Nested(
      nativeElement,
      {...defaultProps, id, value, onClick, layout, style, title},
      children,
    )
  | Text =>
    Nested(
      nativeElement,
      {
        ...defaultProps,
        id,
        value,
        onClick,
        layout: {
          ...layout,
          width:
            if (layout.width < 0) {
              Nanovg.measureText(
                switch (value) {
                | Some(v) => v
                | None => ""
                },
              );
            } else {
              layout.width;
            },
        },
        style,
        title,
      },
      children,
    )
  };

let view = createNativeElement(View);

let text = createNativeElement(Text);
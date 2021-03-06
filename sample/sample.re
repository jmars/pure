open PurePrimitives;

module Test = {
  let createElement = (~children as _, _) =>
    Pure.element({
      ...Pure.statelessComponent("Test"),
      render: _self => <view> <text> (Pure.string("Hello")) </text> </view>,
    });
};

module ReducerComponent = {
  type state = int;
  type action =
    | Add(int)
    | Reset
    | Min;
  let createElement = (~children as _, _) =>
    Pure.element({
      ...Pure.reducerComponent("Reducer Component"),
      initialState: () => 0,
      reducer: (action: action, state: state) =>
        switch (action) {
        | Add(value) => Pure.Update(value + state)
        | Reset => Pure.Update(0)
        | Min => Pure.Update(state - 1)
        },
      render: self =>
        <view
          layout={
            ...defaultLayout,
            flex: 1,
            justifyContent: JustifySpaceAround,
          }>
          <view
            layout={
              ...defaultLayout,
              height: 200,
              alignItems: AlignCenter,
              justifyContent: JustifyCenter,
            }>
            <text
              layout={...defaultLayout, flex: 1, marginTop: 100}
              value=(string_of_int(self.state))
            />
          </view>
          <view
            layout={
              ...defaultLayout,
              flex: 1,
              margin: 16,
              justifyContent: JustifyCenter,
              alignItems: AlignCenter,
            }
            onClick=(() => self.send(Add(1)))
            style={backgroundColor: Some(Color.rgba(150, 0, 0, 0.2))}>
            <text layout={...defaultLayout, flex: 1} value="add" />
          </view>
          <view
            layout={
              ...defaultLayout,
              flex: 1,
              margin: 16,
              justifyContent: JustifyCenter,
              alignItems: AlignCenter,
            }
            onClick=(() => self.send(Reset))
            style={backgroundColor: Some(Color.rgba(0, 0, 0, 0.2))}>
            <text layout={...defaultLayout, flex: 1} value="reset" />
          </view>
          <view
            layout={
              ...defaultLayout,
              flex: 1,
              margin: 16,
              justifyContent: JustifyCenter,
              alignItems: AlignCenter,
            }
            onClick=(() => self.send(Min))
            style={backgroundColor: Some(Color.rgba(0, 0, 0, 0.2))}>
            <text layout={...defaultLayout, flex: 1} value="minus" />
          </view>
        </view>,
    });
};
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
    | Add(int);
  let createElement = (~children as _, _) =>
    Pure.element({
      ...Pure.reducerComponent("Reducer Component"),
      initialState: () => 2,
      reducer: (action: action, state: state) =>
        switch (action) {
        | Add(value) => Pure.Update(value + state)
        },
      render: _self =>
        <view
          layout={
            ...defaultLayout,
            flex: 1,
            justifyContent: JustifySpaceAround,
          }>
          <view layout={...defaultLayout, height: 200, flexDirection: Row} />
          <view
            layout={...defaultLayout, flex: 1, margin: 16}
            style={backgroundColor: Some(Color.rgba(150, 0, 0, 0.2))}
          />
          <view
            layout={...defaultLayout, flex: 1, margin: 16}
            style={backgroundColor: Some(Color.rgba(0, 0, 0, 0.2))}
          />
          <view
            layout={...defaultLayout, flex: 1, margin: 16}
            style={backgroundColor: Some(Color.rgba(0, 0, 0, 0.2))}
          />
        </view>,
    });
};
type style = {backgroundColor: option(Color.color)};

type props = {
  id: option(string),
  value: option(string),
  title: option(string),
  placeholder: option(string),
  src: option(string),
  disabled: option(bool),
  onClick: option(unit => unit),
  onChangeText: option(string => unit),
  layout: LayoutTypes.cssStyle,
  style,
};

type primitive =
  | View
  | Text;

type sideEffects = unit => unit;

type stateless = unit;

type actionless = unit;

module PureCallback = {
  type t('payload) = 'payload => unit;
  let default = _event => ();
  let chain = (handlerOne, handlerTwo, payload) => {
    handlerOne(payload);
    handlerTwo(payload);
  };
};

type reduce('payload, 'action) =
  ('payload => 'action) => PureCallback.t('payload);

type update('state, 'action) =
  | NoUpdate
  | Update('state)
and self('state, 'action) = {
  state: 'state,
  send: 'action => unit,
};

type element =
  | String(string)
  | Component(component('state, 'action)): element
  | Nil
and pureElement =
  | Flat(element)
  | Nested(primitive, props, list(pureElement))
and componentSpec('state, 'initialState, 'action) = {
  debugName: string,
  render: self('state, 'action) => pureElement,
  initialState: unit => 'initialState,
  didMount: self('state, 'action) => unit,
  reducer: ('action, 'state) => update('state, 'action),
}
and component('state, 'action) = componentSpec('state, 'state, 'action);

let basicComponent = debugName : componentSpec(_, _, _) => {
  debugName,
  render: _self => assert(false),
  initialState: () => (),
  didMount: _self => (),
  reducer: (_action, _state) => NoUpdate,
};

let statelessComponent = debugName => {
  ...basicComponent(debugName),
  initialState: () => (),
};

let statefulComponent = debugName => basicComponent(debugName);

let reducerComponent = debugName => basicComponent(debugName);

let string = value => Flat(String(value));

let null = Flat(Nil);

let nil = Flat(Nil);

let element = component => Flat(Component(component));
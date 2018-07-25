let degrees = d => d *. (4.0 *. atan(1.0)) /. 180.;

type color =
  | RGBA(int, int, int, float)
  | HSLA(float, float, float, float);

type rgba = {
  red: int,
  green: int,
  blue: int,
  alpha: float,
};

type hsl = {
  hue: float,
  saturation: float,
  lightness: float,
  alpha: float,
};

let rgba = (r, g, b, a) => RGBA(r, g, b, a);

let rgb = (r, g, b) => RGBA(r, g, b, 1.0);

let hsla = (h, s, l, a) => HSLA(h, s, l, a);

let hsl = (h, s, l) => HSLA(h, s, l, 1.0);

let greyscale = p => HSLA(0.0, 0.0, 1.0 -. p, 1.0);

let fmod = (f, n) => {
  let integer = floor(f) |> int_of_float;
  float_of_int(integer mod n) +. f -. float_of_int(integer);
};

let rgbToHsl = (red, green, blue) => {
  let r = red / 255 |> float_of_int;
  let g = green / 255 |> float_of_int;
  let b = blue / 255 |> float_of_int;
  let cMax = max(max(r, g), b);
  let cMin = min(min(r, g), b);
  let c = cMax -. cMin;
  let hue =
    degrees(60.)
    *. (
      if (cMax === r) {
        fmod((g -. b) /. c, 6);
      } else if (cMax == g) {
        (b -. r) /. c +. 2.;
      } else {
        (r -. g) /. c +. 4.;
      }
    );
  let lightness = (cMax +. cMin) /. 2.;
  let saturation =
    if (lightness == 0.) {
      0.;
    } else {
      c /. (1. -. float_of_int(abs(int_of_float(2. *. lightness -. 1.))));
    };
  (hue, saturation, lightness);
};

let toHsl = color =>
  switch (color) {
  | HSLA(h, s, l, a) => {hue: h, saturation: s, lightness: l, alpha: a}
  | RGBA(r, g, b, a) =>
    let (h, s, l) = rgbToHsl(r, g, b);
    {hue: h, saturation: s, lightness: l, alpha: l};
  };

let complement = color =>
  switch (color) {
  | HSLA(h, s, l, a) => hsla(h +. degrees(180.), s, l, a)
  | RGBA(r, g, b, a) =>
    let (h, s, l) = rgbToHsl(r, g, b);
    hsla(h +. degrees(180.), s, l, a);
  };

let hslToRgb = (hue, saturation, lightness) => {
  let chroma =
    (1. -. float_of_int(abs(int_of_float(2. *. lightness -. 1.))))
    *. saturation;
  let normHue = hue /. degrees(60.);
  let x =
    chroma
    *. (1. -. (fmod(normHue, 2) -. 1. |> int_of_float |> abs |> float_of_int));
  let (r, g, b) =
    if (normHue < 0.) {
      (0., 0., 0.);
    } else if (normHue < 1.) {
      (chroma, x, 0.);
    } else if (normHue < 2.) {
      (x, chroma, 0.);
    } else if (normHue < 3.) {
      (0., chroma, x);
    } else if (normHue < 4.) {
      (0., x, chroma);
    } else if (normHue < 5.) {
      (x, 0., chroma);
    } else if (normHue < 6.) {
      (chroma, 0., x);
    } else {
      (0., 0., 0.);
    };
  let m = lightness -. chroma /. 2.;
  (r +. m, g +. m, b +. m);
};

let frnd = f => floor(f +. 0.5) |> int_of_float;

let toRgb = color =>
  switch (color) {
  | RGBA(r, g, b, a) => {red: r, green: g, blue: b, alpha: a}
  | HSLA(h, s, l, a) =>
    let (r, g, b) = hslToRgb(h, s, l);
    {
      red: frnd(255. *. r),
      green: frnd(255. *. g),
      blue: frnd(255. *. b),
      alpha: a,
    };
  };
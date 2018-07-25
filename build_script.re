open Bsb_internals;

let ( +/ ) = Filename.concat;

gcc("lib" +/ "nanovg.o", ["pure-native" +/ "nanovg" +/ "nanovg.c"]);
gcc("lib" +/ "nanovg_stubs.o", ["pure-native" +/ "nanovg" +/ "stubs.c"]);
open Bsb_internals;

let (+/) = Filename.concat;

gcc("lib" +/ "nanovg.o", ["pure-native" +/ "nanovg" +/ "src" +/ "nanovg.c"]);
gcc("lib" +/ "nanovg_stubs.o", ["pure-native" +/ "nanovg" +/ "stubs.c"]);
gcc("lib" +/ "glew.o", ["pure-native" +/ "glew" +/ "src" +/ "glew.c"]);
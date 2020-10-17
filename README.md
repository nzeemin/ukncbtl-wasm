# ukncbtl-wasm
UKNCBTL — UKNC Back to Life! emulator, WASM version.

*UKNCBTL* is cross-platform UKNC emulator for Windows/Linux/Mac OS X.
*UKNC* (*УКНЦ*, Электроника МС-0511) is soviet school computer based on two PDP-11 compatible processors KM1801VM2.

This is UKNCBTL emulator, version for WebAssembly (WASM) to compile with Emscripten and to run in any modern browser.

Take a look at the emulator here:
https://nzeemin.github.io/ukncbtl-wasm/index.html

### Emulator files
The following files are the result of the compilation:
* `emul.js`
* `emul.wasm`
* `emul.html`
* `index.html`

To make it work you have to put the files on web server; WebAssembly will not work just from a file opened in a browser.

### Emulator URL parameters
The emulator recognizes and uses the following (optional) URL parameters:
* `state=URL` — load saved emulator state from the URL and apply it
* `diskN=URL` — load disk image from the URL and attach it; `N`=0..3
* `run=1` — run the emulator

Note that the URLs are to download files from the Web by JavaScript code, so that's under restriction of Cross-Origin Resource Sharing (CORS) policy defined on your server.

-----
The UKNCBTL project consists of:
* [ukncbtl](https://github.com/nzeemin/ukncbtl/) written for Win32 and works under Windows 2000/2003/2008/XP/Vista/7/8/10.
* [ukncbtl-renders](https://github.com/nzeemin/ukncbtl-renders/) — renderers for UKNCBTL Win32.
* [ukncbtl-qt](https://github.com/nzeemin/ukncbtl-qt/) is based on Qt framework and works under Windows, Linux and Mac OS X.
* [ukncbtl-testbench](https://github.com/nzeemin/ukncbtl-testbench/) — test bench for regression and performance testing.
* [ukncbtl-utils](https://github.com/nzeemin/ukncbtl-utils/) — various utilities: rt11dsk, sav2wav, ESCParser etc.
* [ukncbtl-doc](https://github.com/nzeemin/ukncbtl-doc/) — documentation and screenshots.
* [ukncbtl-wasm](https://github.com/nzeemin/ukncbtl-wasm/) — web version of the emulator.
* Project wiki: https://github.com/nzeemin/ukncbtl-doc/wiki
  * Screenshots: https://github.com/nzeemin/ukncbtl-doc/wiki/Screenshots-ru
  * User's Manual (in Russian): https://github.com/nzeemin/ukncbtl-doc/wiki/Users-Manual-ru

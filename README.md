# Browser-USBtan

Browser-USBtan aims to provide a browser-extension together with a matching native-messaging-host application that allows you to use a USB-reader to generate TAN numbers from flickercodes.

## Project status
**This is currently a proof-of-concept**. The browser-extension works, scrapes the flickercode and talks to the native-messaging-host application. However, that application cannot generate TAN numbers, but only responds with the reversed flickercode string.

The extension is only tested with Firefox. It may work with Chrome, too.

## Installation

To install the extension, you'll have to load it into your browser. In Firefox you can go to [the Debugging Page](about:debugging#/runtime/this-firefox) and load a temporary plugin. Simply navigate to your checkout and select `browser-extension/manifest.json` to load the Plugin.

To install the native-host application, you'll need a C++17 compiler and a recent version of CMake. 
```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```
This will build the application and install a descriptor in `$HOME/.mozilla/native-messaging-hosts/`. The application itself will currently reside in your temporary directory.

## Usage
The extension automatically looks at all URL starting with `file:///`. You can open the javascript-console to see what is going on.
If you want to use it for different URLs, you need to change `browser-extension/manifest.json` accordingly.

For testing purposes, it is a good idea to go to your bank's website, go to a page that will produce a flickercode, save that locally and add a `<base href="...">` to it, so you can load it locally.

## How it works
The extension hooks into the drawing of the HTML canvas that displays the flicker-code. The flickering is screen-scraped, converted to a string and passed via the native-messaging-host that is then supposed to return a TAN.
That TAN will then be written to the form-field. In the future, when TANs are actually working, the form will also be automatically submitted.

## Contributing
Pull requests are welcome.

## License
[GNU AGPLv3](https://choosealicense.com/licenses/agpl-3.0/)

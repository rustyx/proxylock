# proxylock

Proxylock is a Windows application that monitors Internet Proxy Settings and enforces that *proxy auto-configuration* and *configuration via a script* are disabled.

There are (corporate) applications that like to redirect all Web traffic for filtering purposes.

Proxylock will detect such redirections and restore the proxy settings to the previous state.

## How to use

Download the latest proxylock.exe from the [releases](../../releases) tab or compile one from source.

Put it into your Startup folder and run it once.

There is no GUI; if all is well, you should not be able to enable proxy auto-configuration.

To stop - open Task Manager Details tab and kill proxylock.exe.

## Development environment

As a development environment, Eclipse CDT or your favorite text editor can be used. Mingw32 or Visual Studio is needed to compile the code.


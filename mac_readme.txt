https://gist.github.com/v3n/27e810ac744b076ceeb7

open terminal
-------------
right click draw_robots folder and click New Terminal at Folder

install development tools
-------------------------
> cc
note: if you get some message from clang, you already have them!

install brew
------------
> /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

install glfw
------------
> brew install glfw

build and run
-------------
> sh mac_build.sh && ./a.out

================================================================
if you get error: ld: symbol(s) not found for architecture arm64
================================================================

remove glfw and build/install from source
-----------------------------------------
> brew uninstall glfw
> git clone https://github.com/glfw/glfw.git
> cd glfw
> cmake -DCMAKE_OSX_ARCHITECTURES=arm64 .
> make
> sudo make install

build and run
-------------
> sh mac_build_special.sh && ./a.out


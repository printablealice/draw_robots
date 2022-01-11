# draw\_robots


## step 1: get code

*goal: have a folder named draw_robots (with all the files in it) sitting on your Desktop*

- option a (recommended): clone to desktop
    > git clone https://github.com/printablealice/draw_robots.git

- option b: Code > Download ZIP

    __Windows Only: before extracting right click .zip file > Properties > Unblock > OK__


## step 2 (Windows): build and run

- install the c++ build tools (https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) __!! make sure to check the box for Desktop Development with C++__
- open x64 Native Tools Command Prompt for VS 2022 (click the windows icon; type x64; press Enter key)
- navigate to draw\_robots folder
    > cd "C:\Users\YOUR_USER_NAME_HERE\Desktop\draw_robots"
- build and run
    > windows_build.bat && build\main.exe

__! if step 2 fails please post in Issues__


## step 2 (Mac): build and run

- open terminal (right click draw\_robots folder and click New Terminal at Folder)
- install dev tools (if you get some message from clang you already have them)
    > cc
- install brew
    > /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
- install glfw
    > brew install glfw
- build and run
    > sh mac_build.sh && ./main.app

__! if step 2 fails with error like "ld: symbol(s) not found for architecture arm64" please see below; for other errors please post in Issues__


## step 2s (Mac; step 2 failed with the arm64 error): alternate build and run

- remove glfw; build glfw from source; install glfw
    > brew uninstall glfw
    > 
    > git clone https://github.com/glfw/glfw.git
    > 
    > cd glfw
    > 
    > cmake -DCMAKE_OSX_ARCHITECTURES=arm64 .
    > 
    > make
    > 
    > sudo make install
- build and run
    > sh mac_build_special.sh && ./main.app
- note: you may first need to update brew and upgrade packages to get step 2s to work
    > brew update
    > 
    > brew upgrade

__! if step 2s fails please post in Issues__

## step 3: debugging

### step 3a: compile in debug mode
- Windows: you need the flag -Od in windows\_build.sh
- Mac: you need the flag -g in mac\_build.sh

### step 3b: debug the executable
- Windows with Remedy or Visual Studio: https://yakvi.github.io/handmade-hero-notes/html/day1.html#debugginginremedybgorvisualstudio
- Mac with Xcode (a.out may not be recognized, but main.app produced by updated mac\_build.sh should be): https://handmade.network/forums/t/2826-mac_osx_debugging_with_xcode_with_a.out_file#13906

<img src="https://github.com/ENDESGA/EDGITOR/blob/master/git/title.png" />
[pron. EDGE-itor]

- - - - - - -

<img alt="Discord" src="https://img.shields.io/discord/732380484956586035?color=FF0040&label=chat&logo=discord&logoColor=FFFFFF">
<img alt="GitHub All Releases" src="https://img.shields.io/github/downloads/ENDESGA/EDGITOR/total?color=FF0040">
<img alt="GitHub" src="https://img.shields.io/github/license/ENDESGA/EDGITOR?color=FF0040">

## minimal C++ pixelart editor
EDGITOR was designed to be extremely concise and clean from the ground up. It uses SDL2 as the core of its rendering system, and is designed to be as efficient as possible. EDGITOR can theoretically support colossal canvases (stress tests got close to 20Kx20K), but that isn't a focused feature since this is a pixelart editor. That being said, this could branch off to support AA and Photoshop-like features to bridge a gap between traditional painting and pixelart.

## FEATURE GOALS:
- Extremely fast and efficient workflow.
- A big focus on palette use and creation.
- Custom-everything, from brushes to effects.
- Tile system to allow easy tile creation.
- Simple per-frame animation system.
- Limitless layers, undos, canvas size.
- Clean and minimal UI.

### COMMUNITY DRIVEN
EDGITOR is now open source, and I'd love for this to grow via the community! Feel free to send pull requests, and add features to this so it can grow faster like never before!

Now you can use Cmake!

## BUILD INSTRUCTIONS
### Windows
- Install Visual Studio 2019
- [Download Nuget.exe Here](https://dist.nuget.org/win-x86-commandline/latest/nuget.exe), and place it in `C:\Program Files\CMake\bin`
- Open Command Prompt, and go to EDGITOR's folder: `cd *EDGITOR_FOLDER_LOCATION*`
- Then execute this command `cmake -B build -G "Visual Studio 16 2019"`
- Open VS19 and set EDGITOR as StartUp Project

### macOS
- Install Xcode
- Install SDL and SDL_ttf frameworks manually
- Run following command in Terminal `cmake -B build -G Xcode`

- - - - - - -

## CREDITS
[SDL_FontCache](https://github.com/grimfang4/SDL_FontCache) (with some fixes) by grimfang4 

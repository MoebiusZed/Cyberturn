# Introduction

This is the ***ROOT*** folder for the unreal game project.

There is already a preconfigured empty UE5 project in here with the unreal mannequin. C++ is enable by deafult too.

_NOTE - Unreal has blur enabled by default, disable this in project settings if you don't want blur._

Thankyou for checking this readme. If you're wondering how to remove the message saying to read the readme, this is how you fix it:
> Delete everything in the `Blueprints` folder.



# Opening Project

If you're a programmer and plan on doing work in C++ you should right click the `Game.uproject` file and select the `Generate Visual Studio project files` option. When this is complete double click the `Game.sln` file and run the game from Visual Studio by clicking `Local Windows Debugger`. Whenever you code in C++ you should close UE5 and recompile it from VS.

If you are not doing anything in C++, then you can open the project straight from double clicking `Game.uproject`. Make sure you click yes on the prompt asking to compile. UE5 will take awhile to open because it's compiling C++ binaries in the background.

If UE5 is bing weird delete all the cache files/folders & generated files/folders, then lauch it from Visual Studio so it's forced to compile everything correctly and you will catch any compiler bugs if anything is wrong.



# Requirements

1. Unreal Engine 5.1
2. Visual Studio 2022

You're installation of UE5 and VS2022 may not work if you're missing certain SDKs and development tools. See **[Fixing UE5 Setup](#fixing-ue5-setup)**



# Changing Template

If you want to use a different unreal template as a starting point for your project then:
1. Delete `Game.uproject`, `Content`. `Config`, `Source`, and other files or folders you don't want in this directory. Do not delete `.gitignore` and `.gitattributes`, they're very important for git to work correctly.
2. Open unreal, select a template, and create the project template anywhere.
3. Open the ***ROOT*** folder of the UE5 project you just created and copy `Game.uproject`, `Content`. `Config`, and `Source` into this folder.

Once you've completed the above steps you're ready to open the project and start working.



# Fixing UE5 Setup

### dotnet is not recognized as an internal or external command

If you get an error similar to this one when generating visual studio project files or opening the project for the first time, it's because `.NET 6` is not installed or its environment path is not set.

To fix this locate `C:\program files\dotnet`, and delete the `dotnet` folder if it exists. If it doesn't exist, then don't do anything.

Next download the .NET 6 SDK ([.NET 6 download here](https://dotnet.microsoft.com/en-us/download/dotnet/6.0)) and install it. _(do not download `ASP.NET Core Runtime`, `.NET Desktop Runtime`, `.NET Runtime`, or any other SDK - just the plain `.NET 6` SDK that has everything included)_

This will fix the dotnet error.

### I don't know how to setup VS2022 for Unreal Engine 5

Follow this guide: [Setting Up Visual Studio Environment for C++](https://docs.unrealengine.com/5.1/en-US/setting-up-visual-studio-development-environment-for-cplusplus-projects-in-unreal-engine/)
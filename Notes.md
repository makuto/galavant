# Notes

Miscellaneous notes regarding working on Galavant, the code, and other random shit.

## Working With Unreal

- When making changes to Galavant's libraries, simply touch a GalavantUnreal source file to ensure 
you get the latest Galavant changes when using Unreal's hot reloading
- After rebuilding Unreal, you may need to delete GalavantUnreal/Binaries/* if you get 'Game Module Could Not Be Loaded' error on startup
- Even if you `make` the project, you'll still need to hit the Compile button in the editor to get Unreal to compile and hotreload your code
- I added a hack which makes the engine auto scale to my desired DPI settings. You'll probably want to change these to fit your preferences. Change the value of `FSlateApplication::Get().SetApplicationScale(1.53f)` in `AGalavantUnrealMain::AGalavantUnrealMain()` to your desired DPI (1.f is the engine default).

## Comment Tags

- TODO: Something needs to get done. I use TodoReview for Sublime to find all of these easily. 
- @Performance: Not justifiable as a TODO, but could be looked at when thinking about performance
- @Purity: Look into changing the code for code purity/cleanliness' sake
- @Callback: The marked function is used as a callback. Preferably @Callback [Callback type name]
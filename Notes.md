# Notes

Miscellaneous notes regarding working on Galavant, the code, and other random shit.

## Working With Unreal

- When making changes to Galavant's libraries, simply touch a GalavantUnreal source file to ensure 
you get the latest Galavant changes when using Unreal's hot reloading
- After rebuilding Unreal, you may need to delete GalavantUnreal/Binaries/* if you get 'Game Module Could Not Be Loaded' error on startup
- Even if you `make` the project, you'll still need to hit the Compile button in the editor to get Unreal to compile and hotreload your code
- I added a hack which makes the editor auto scale to my desired DPI settings. I run the command `HighDPI` in game and it'll make the editor scale for the rest of the runtime
- When exporting .FBX Skeletal meshes from Blender, ensure `Forward` is `Y Forward`, `Up` is `Z Up`, and in `Armature` settings, uncheck `Add Leaf Bones`.

### Useful Unreal Commands
- I set my standalone builds to run with these options: `-windowed -resx=1920 -resy=1080`. I have a 4K monitor which is too performance intensive in game at native, and prefer windowed while working
- `r.setRes 1920x1080f`: Set screen resolution (w = windowed, f = fullscreen, wf = windowed fullscreen)
- `'`: (Hit apostrophe in gameplay): Debug gameplay interface. Use NumPad for various views
- `stat fps`: Show FPS in corner
- `t.MaxFps`: Set max FPS (default seems to be 60)
- `stat StartFile`, `stat StopFile`: Record profiling data. Goes to the nearest `Saved` folder to the executable. Open Window > Developer Tools > Session Frontend and go to the Profiler tab to do anything useful with this data

## Comment Tags

Future tasks:
- TODO: Something needs to get done. I use TodoReview for Sublime to find all of these easily. 
- @Performance: Not justifiable as a TODO, but could be looked at when thinking about performance
- @Purity: Look into changing the code for code purity/cleanliness' sake
- @Stability: Should be changed in order to make the code more stable (segfault protection etc.)

Structure labels:
- @Callback: The marked function is used as a callback. Preferably @Callback [Callback type name]
- @Latelink: The marked thing is latelinked. Frontends must define them before linking can complete
- @LatelinkDef: The marked thing is the frontend definition of a latelinked thing
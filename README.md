# SimpleRenderer
A simple 3D Render engine built from scratch using nothing but C++ with SDL2 and Maths.

## The program reads an object file in obj format, and an optional material file in mtl format and renders the scence with added orbit controls and lighting/shadow options.
![AOIlighting](https://firebasestorage.googleapis.com/v0/b/simplerenderer.appspot.com/o/AOIighting.png?alt=media&token=19fc1f8f-5529-4964-b551-0f676f756839)
![rotated](https://firebasestorage.googleapis.com/v0/b/simplerenderer.appspot.com/o/lightingrotated.png?alt=media&token=e45f882e-c917-42e1-9f2e-1b6f488f299b)
![ambient](https://firebasestorage.googleapis.com/v0/b/simplerenderer.appspot.com/o/ambient.png?alt=media&token=348e23e0-813c-4fd6-a4fb-f57acc7576b2)


## Run
Make sure you have SDL2 installed
clone the repo. Run the following commands
   cd SimpleRenderer
   Put [yourobject].obj and [yourobject].mtl files in this folder
   Rename [yourobject] -> cornell-box
   make

## commands:
  - use arrow keys to move the camera around
  - use x,y to rotate the camera in the x,y axis respectively 
  - lighting options only available in ray tracing mode(comment out drawRayTracing() on line 722)

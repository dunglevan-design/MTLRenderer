# SimpleRenderer
A simple 3D Render engine built from scratch using nothing but C++ with SDL2 and Maths.

## Interpolation 1D and 2D.
"Filling in the gap between 2 points". 
First we decide the colour of the 4 corners. 
glm::vec3 topLeft(255, 0, 0);        // red 
glm::vec3 topRight(0, 0, 255);       // blue 
glm::vec3 bottomRight(0, 255, 0);    // green 
glm::vec3 bottomLeft(255, 255, 0);   // yellow
Then interpolate from topLeft -> topRight so that the points between them has colour going from red to blue
Then interpolate from bottomLeft -> bottomRight so that the points between them has colour going from yellow to green
Now, foreach columns, we interpolate between the top most value and the bottom value
![2 dimensional interpolation](https://firebasestorage.googleapis.com/v0/b/simplerenderer.appspot.com/o/2Dinterpolation.png?alt=media&token=027e3103-1d65-41a5-9965-513da8f0ddac)
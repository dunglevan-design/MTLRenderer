#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <CanvasPoint.h>
#include <Colour.h>
#include <cmath>
#include <CanvasTriangle.h>
#include <glm/glm.hpp>
#include <TextureMap.h>
#include <TexturePoint.h>
#include <ModelTriangle.h>
#include <map>
#include <algorithm>
#include <RayTriangleIntersection.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
#define WIDTH 320
#define HEIGHT 240


map<string, Colour> LoadObjMaterial() {
	map<string, Colour> pallete;
	pallete["Red"] = Colour("Test", 255, 0, 0);
	ifstream inputStream("cornell-box.mtl", ifstream::binary);
	string nextline;
	string colourname;
	for (size_t i = 0; i < 23; i++)
	{
		getline(inputStream, nextline);
		if (nextline.empty()) {
			continue;
		}
		vector<string> arr = split(nextline, ' ');
		if (arr[0] == "newmtl") {
			colourname = arr[1];
		}
		if (arr[0] == "Kd") {
			int red = (round)(stof(arr[1]) * 255);
			int green = (round)(stof(arr[2]) * 255);
			int blue = (round)(stof(arr[3]) * 255);
			Colour colour = Colour(colourname, red, green, blue);
			pallete[colourname] = colour;
		}
	}

	return pallete;
}


vector<ModelTriangle> LoadObjtriangles() {
	vector<ModelTriangle> triangles;
	map<string, Colour>pallete = LoadObjMaterial();

	vector<glm::vec3> vs;
	float scaling = 0.17;
	ifstream inputStream("cornell-box.obj", std::ifstream::binary);
	string nextline;
	getline(inputStream, nextline);
	getline(inputStream, nextline);
	//skip comment line
	//119 = number of lines - first 2 comment lines.
	Colour c = Colour();
	for (size_t i = 0; i < 119; i++)
	{

		// get first object
		getline(inputStream, nextline);
		if (nextline.empty()) {
			continue;
		}
		// get first object

		else if (nextline.at(0) == 'o') {
			//object
			continue;
		}
		else if (nextline.at(0) == 'u') {
			//color
			vector<string> colorlinevector = split(nextline, ' ');
			c = pallete.find(colorlinevector[1])->second;

		}
		else if (nextline.at(0) == 'v') {
			//vectors or facets.
			vector<string> linevector = split(nextline, ' ');
			vs.push_back(glm::vec3(stof(linevector[1]) * scaling, stof(linevector[2]) * scaling, stof(linevector[3]) * scaling));
		}

		else if (nextline.at(0) == 'f') {
			vector<string> facetvalues = split(nextline, ' ');
			facetvalues[1].pop_back();
			facetvalues[2].pop_back();
			facetvalues[3].pop_back();
			int firstvalue = stoi(facetvalues[1]) - 1;
			int secondvalue = stoi(facetvalues[2]) - 1;
			int thirdvalue = stoi(facetvalues[3]) - 1;
			//Colour color = Colour("red", 255, 1, 1);
			ModelTriangle triangle = ModelTriangle(vs[firstvalue], vs[secondvalue], vs[thirdvalue], c);

			triangles.push_back(triangle);

		}
	}
	return triangles;
}

void drawPixel(int x, int y, Colour c, DrawingWindow& window) {
	uint32_t colour = (255 << 24) + ((int)(c.red) << 16) + ((int)(c.green) << 8) + ((int)(c.blue));
	window.setPixelColour(x, y, colour);
}

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> returnVector{ from };
	if (numberOfValues == 1) {
		return returnVector;
	}
	for (int i = 2; i <= numberOfValues; i++)
	{
		returnVector.push_back(from + ((to - from) / (numberOfValues - 1)) * (i - 1));
	}
	return returnVector;
}

std::vector<CanvasPoint> interpolateSinglePoints(CanvasPoint from, CanvasPoint to) {
	vector<CanvasPoint> returnvector{ from };

	float xDiff = from.x - to.x;
	float yDiff = from.y - to.y;
	float zDiff = from.depth - to.depth;
	int numberofsteps = round(max(max(abs(xDiff), abs(yDiff)), abs(zDiff))) + 1;


	vector<float> tests = interpolateSingleFloats(2.2, 8.5, 2);
	vector<float> xs = interpolateSingleFloats(from.x, to.x, numberofsteps);
	vector<float> ys = interpolateSingleFloats(from.y, to.y, numberofsteps);
	vector<float> zs = interpolateSingleFloats(from.depth, to.depth, numberofsteps);

	for (int i = 0; i < numberofsteps; i++)
	{
		CanvasPoint point = CanvasPoint(xs[i], ys[i], zs[i]);
		returnvector.push_back(point);
	}
	return returnvector;
}
std::vector<CanvasPoint> interpolateSinglePoints(CanvasPoint from, CanvasPoint to, int numberofsteps) {
	vector<CanvasPoint> returnvector{ from };

	//int numberOfSteps = std::max(abs(xDiff), abs(yDiff));


	vector<float> tests = interpolateSingleFloats(2.2, 8.5, 2);
	vector<float> xs = interpolateSingleFloats(from.x, to.x, numberofsteps);
	vector<float> ys = interpolateSingleFloats(from.y, to.y, numberofsteps);
	vector<float> zs = interpolateSingleFloats(from.depth, to.depth, numberofsteps);

	for (int i = 0; i < numberofsteps; i++)
	{
		CanvasPoint point = CanvasPoint(xs[i], ys[i], zs[i]);
		returnvector.push_back(point);
	}
	return returnvector;
}


void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	vector<ModelTriangle> triangles = LoadObjtriangles();
	map<string, Colour>pallete = LoadObjMaterial();
	
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	interpolateSinglePoints()
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}

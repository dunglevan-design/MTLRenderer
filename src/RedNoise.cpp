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


void drawline(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow& window)
{

	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;

	for (size_t i = 0; i < numberOfSteps; i++)
	{
		float x = from.x + xStepSize * i;
		float y = from.y + yStepSize * i;

		uint32_t colour = (255 << 24) + ((int)(c.red) << 16) + ((int)(c.green) << 8) + ((int)(c.blue));
		window.setPixelColour(round(x), round(y), colour);
	}
}


void drawTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow& window)
{
	std::cout << triangle << std::endl;
	drawline(triangle.v0(), triangle.v1(), colour, window);
	drawline(triangle.v1(), triangle.v2(), colour, window);
	drawline(triangle.v2(), triangle.v0(), colour, window);
}

void drawRandomStrokeTriangle(DrawingWindow& window)
{
	CanvasPoint v0 = CanvasPoint(rand() % 320, rand() % 240);
	CanvasPoint v1 = CanvasPoint(rand() % 320, rand() % 240);
	CanvasPoint v2 = CanvasPoint(rand() % 320, rand() % 240);
	Colour colour = Colour(rand() % 255, rand() % 255, rand() % 255);
	CanvasTriangle triangle = CanvasTriangle(v0, v1, v2);

	drawTriangle(triangle, colour, window);
}


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


vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues)
{
	vector<glm::vec3> result;

	vector<float> firstColumn = interpolateSingleFloats(from[0], to[0], numberOfValues);
	vector<float> secondColumn = interpolateSingleFloats(from[1], to[1], numberOfValues);
	vector<float> thirdColumn = interpolateSingleFloats(from[2], to[2], numberOfValues);

	for (size_t i = 0; i < numberOfValues; i++)
	{
		glm::vec3 resultvector = glm::vec3(firstColumn[i], secondColumn[i], thirdColumn[i]);
		result.push_back(resultvector);
	}
	return result;
}

void interpolate2D(DrawingWindow& window) {
	glm::vec3 topLeft(255, 0, 0);	   // red
	glm::vec3 topRight(0, 0, 255);	   // blue
	glm::vec3 bottomRight(0, 255, 0);  // green
	glm::vec3 bottomLeft(255, 255, 0); // yellow

	//interpolate first and last column
	vector<glm::vec3> firstcolumn = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
	vector<glm::vec3> lastcolumn = interpolateThreeElementValues(topRight, bottomRight, window.height);

	for (size_t i = 0; i < window.height; i++)
	{
		uint32_t colour = (255 << 24) + ((int)(firstcolumn.at(i).x) << 16) + ((int)(firstcolumn.at(i).y) << 8) + ((int)(firstcolumn.at(i).z));
		window.setPixelColour(0, i, colour);
	}

	for (size_t i = 0; i < window.height; i++)
	{
		uint32_t colour = (255 << 24) + ((int)(lastcolumn.at(i).x) << 16) + ((int)(lastcolumn.at(i).y) << 8) + ((int)(lastcolumn.at(i).z));
		window.setPixelColour(319, i, colour);
	}

	//interpolate rows

	for (size_t y = 0; y < window.height; y++)
	{
		glm::vec3 from = firstcolumn.at(y);
		glm::vec3 to = lastcolumn.at(y);
		vector<glm::vec3> row = interpolateThreeElementValues(from, to, window.width);

		for (size_t x = 0; x < window.width; x++)
		{
			uint32_t colour = (255 << 24) + ((int)(row.at(x).x) << 16) + ((int)(row.at(x).y) << 8) + ((int)(row.at(x).z));
			window.setPixelColour(x, y, colour);
		}
	}
}


void draw(DrawingWindow &window) {
	//window.clearPixels();
	
}

std::vector<CanvasPoint> SortByYcoordinate(CanvasTriangle triangle)
{
	std::vector<CanvasPoint> v;
	if (triangle.v0().y >= triangle.v1().y && triangle.v1().y >= triangle.v2().y)
	{
		v.push_back(triangle.v0());
		v.push_back(triangle.v1());
		v.push_back(triangle.v2());
	}

	else if (triangle.v0().y >= triangle.v2().y && triangle.v2().y >= triangle.v1().y)
	{
		v.push_back(triangle.v0());
		v.push_back(triangle.v2());
		v.push_back(triangle.v1());
	}

	else if (triangle.v1().y >= triangle.v0().y && triangle.v0().y >= triangle.v2().y)
	{
		v.push_back(triangle.v1());
		v.push_back(triangle.v0());
		v.push_back(triangle.v2());
	}
	else if (triangle.v1().y >= triangle.v2().y && triangle.v2().y >= triangle.v0().y)
	{
		v.push_back(triangle.v1());
		v.push_back(triangle.v2());
		v.push_back(triangle.v0());
	}

	else if (triangle.v2().y >= triangle.v0().y && triangle.v0().y >= triangle.v1().y)
	{
		v.push_back(triangle.v2());
		v.push_back(triangle.v0());
		v.push_back(triangle.v1());
	}

	else if (triangle.v2().y >= triangle.v1().y && triangle.v1().y >= triangle.v0().y)
	{
		v.push_back(triangle.v2());
		v.push_back(triangle.v1());
		v.push_back(triangle.v0());
	}
	return v;
}

CanvasPoint find(CanvasPoint top, CanvasPoint mid, CanvasPoint bottom)
{
	int extray = mid.y;
	int extrax = top.x + (mid.y - top.y) / (bottom.y - top.y) * (bottom.x - top.x);
	return CanvasPoint(extrax, extray);
}


void drawFilledTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow& window)
{

	std::vector<CanvasPoint> v = SortByYcoordinate(triangle);
	cout << "3 vertices:" << endl;
	std::cout << v[0] << std::endl
		<< v[1] << std::endl
		<< v[2] << std::endl;
	CanvasPoint top = v[2];
	CanvasPoint mid = v[1];
	CanvasPoint bottom = v[0];
	CanvasPoint extra = find(top, mid, bottom);
	std::cout << "extra" << extra << std::endl;
	// drawline(top, mid, colour,window);
	// drawline(mid, bottom, colour,window);
	// drawline(mid, extra, colour,window);


	//top half of triangle
	float topmidDiffx = top.x - mid.x;
	float topextraDiffx = top.x - extra.x;
	float yDiff = extra.y - top.y;
	float numberOfSteps = max(std::max(abs(topmidDiffx), abs(topextraDiffx)), abs(yDiff));

	float topmidStepSize = topmidDiffx / numberOfSteps;
	float topextraStepSize = topextraDiffx / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;

	for (size_t i = 0; i < numberOfSteps; i++)
	{
		CanvasPoint from = CanvasPoint(top.x - topmidStepSize * i, top.y + yStepSize * i);
		CanvasPoint to = CanvasPoint(top.x - topextraStepSize * i, top.y + yStepSize * i);
		drawline(from, to, colour, window);
	}

	//bottomhalf
	float extrabottomDiffx = extra.x - bottom.x;
	float midbottomDiffx = mid.x - bottom.x;
	float ydiff = bottom.y - extra.y;
	float numberOfsteps = max(std::max(abs(extrabottomDiffx), abs(midbottomDiffx)), abs(ydiff));

	float extrabottomStepSize = extrabottomDiffx / numberOfsteps;
	float midbottomStepSize = midbottomDiffx / numberOfsteps;
	float ystepSize = ydiff / numberOfsteps;

	for (size_t i = 0; i < numberOfsteps; i++)
	{
		CanvasPoint from = CanvasPoint(extra.x - extrabottomStepSize * i, extra.y + ystepSize * i);
		CanvasPoint to = CanvasPoint(mid.x - midbottomStepSize * i, mid.y + ystepSize * i);
		drawline(from, to, colour, window);
	}
}


void drawRandomFilledTriangle(DrawingWindow& window)
{
	CanvasPoint v0 = CanvasPoint(rand() % 320, rand() % 240);
	CanvasPoint v1 = CanvasPoint(rand() % 320, rand() % 240);
	CanvasPoint v2 = CanvasPoint(rand() % 320, rand() % 240);
	Colour colour = Colour(rand() % 255, rand() % 255, rand() % 255);
	CanvasTriangle triangle = CanvasTriangle(v0, v1, v2);
	drawFilledTriangle(triangle, colour, window);
}



void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) {	
			std::cout << "LEFT" << std::endl;
		} 
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_t) {
			std::cout << "you selected draw triangle" << std::endl;
			drawRandomFilledTriangle(window);
		}

		else if (event.key.keysym.sym == SDLK_i) {
			std::cout << "you selected interpolate 2D" << std::endl;
			interpolate2D(window);
		}
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

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}

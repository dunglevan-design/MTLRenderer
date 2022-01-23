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

void drawline(CanvasPoint from, CanvasPoint to, Colour c, DrawingWindow& window, std::vector<std::vector<float>>& z)
{
	std::vector<CanvasPoint> line = interpolateSinglePoints(from, to);
	for (int i = 0; i < line.size(); i++)
	{
		float depth = line[i].depth;
		int x = round(line[i].x);
		int y = round(line[i].y);
		if (0 > x || x >= WIDTH || 0 > y || y >= HEIGHT) continue;
		//plus 1 to shift all negatives to positive
		if (z[x][y] == 0 || 1 / depth > z[x][y]) { // -100
			z[x][y] = 1 / depth;
			uint32_t colour = (255 << 24) + ((int)(c.red) << 16) + ((int)(c.green) << 8) + ((int)(c.blue));
			window.setPixelColour(x, y, colour);
		}

		//uint32_t colour = (255 << 24) + ((int)(c.red) << 16) + ((int)(c.green) << 8) + ((int)(c.blue));
		//window.setPixelColour(round(line[i].x), round(line[i].y), colour);

	}
}

void drawTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow& window, std::vector<std::vector<float>>& z)
{
	drawline(triangle.v0(), triangle.v1(), colour, window, z);
	drawline(triangle.v1(), triangle.v2(), colour, window, z);
	drawline(triangle.v2(), triangle.v0(), colour, window, z);
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
	float extray = mid.y;
	float extrax = top.x + (mid.y - top.y) / (bottom.y - top.y) * (bottom.x - top.x);
	float extraz = top.depth + (mid.y - top.y) / (bottom.y - top.y) * (bottom.depth - top.depth);
	return CanvasPoint(extrax, extray, extraz);
}

void drawFilledTriangle(CanvasTriangle triangle, Colour colour, DrawingWindow& window, std::vector<std::vector<float>>& z)
{
	drawTriangle(triangle, colour, window, z);
	std::vector<CanvasPoint> v = SortByYcoordinate(triangle);
	CanvasPoint top = v[2];
	CanvasPoint mid = v[1];
	CanvasPoint bottom = v[0];
	CanvasPoint extra = find(top, mid, bottom);

	float xDiffa = abs(top.x - mid.x);
	float yDiffa = abs(top.y - mid.y);
	float zDiffa = abs(top.depth - mid.depth);

	float xDiffb = abs(top.x - extra.x);
	float yDiffb = abs(top.y - extra.y);
	float zDiffb = abs(top.depth - extra.depth);

	int numberofPoints = round(std::max(((((xDiffa, yDiffa), zDiffa), xDiffb), yDiffb), zDiffb)) + 2;


	std::vector<CanvasPoint> froms = interpolateSinglePoints(top, mid, numberofPoints);
	std::vector<CanvasPoint> tos = interpolateSinglePoints(top, extra, numberofPoints);

	for (int i = 0; i < numberofPoints; i++)
	{
		drawline(froms[i], tos[i], colour, window, z);

	}

	float xdiffa = abs(bottom.x - extra.x);
	float ydiffa = abs(bottom.y - extra.y);
	float zdiffa = abs(bottom.depth - extra.depth);

	float xdiffb = abs(bottom.x - mid.x);
	float ydiffb = abs(bottom.y - mid.y);
	float zdiffb = abs(bottom.depth - mid.depth);

	int numberofpoints = round(max(((((xdiffa, ydiffa), zdiffa), xdiffb), ydiffb), zdiffb)) + 2;
	std::vector<CanvasPoint> Bottomfroms = interpolateSinglePoints(mid, bottom, numberofpoints);
	std::vector<CanvasPoint> Bottomtos = interpolateSinglePoints(extra, bottom, numberofpoints);

	for (size_t i = 0; i < numberofpoints; i++)
	{
		drawline(Bottomfroms[i], Bottomtos[i], colour, window, z);
	}

}

//uint32_t findTexture(int x, int y, TextureMap img) {
//	return img.pixels[y * 480 + x];
//}
//void drawTextureLine(CanvasPoint from, CanvasPoint to, DrawingWindow& window, TextureMap img, float(&z)[320][240]) {
//	float xDiff = to.x - from.x;
//	float yDiff = to.y - from.y;
//
//	if (xDiff == 0 && yDiff == 0) {
//		return;
//	}
//	float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
//	float xStepSize = xDiff / numberOfSteps;
//	float xTextureStepSize = (to.texturePoint.x - from.texturePoint.x) / numberOfSteps;
//	float yStepSize = yDiff / numberOfSteps;
//	float yTextureStepSize = (to.texturePoint.y - from.texturePoint.y) / numberOfSteps;
//
//	for (size_t i = 0; i <= numberOfSteps; i++)
//	{
//		if (i == numberOfSteps) {
//			cout << "line 198";
//		}
//		float x = from.x + xStepSize * i;
//		float xtexture = from.texturePoint.x + xTextureStepSize * i;
//
//
//		float y = from.y + yStepSize * i;
//		float ytexture = from.texturePoint.y + yTextureStepSize * i;
//
//		uint32_t colour = findTexture(round(xtexture), round(ytexture), img);
//
//		window.setPixelColour(round(x), round(y), colour);
//	}
//}

//CanvasPoint findvk(CanvasPoint v0, CanvasPoint v1, CanvasPoint v2) {
//	float ratio = (v1.y - v0.y) / (v2.y - v0.y);
//	int vky = v1.y;
//	int vkx = v0.x + ratio * (v2.x - v0.x);
//
//	int Texturevkx = v0.texturePoint.x + ratio * (v2.texturePoint.x - v0.texturePoint.x);
//	int Texturevky = v0.texturePoint.y + ratio * (v2.texturePoint.y - v0.texturePoint.y);
//	return CanvasPoint(vkx, vky, Texturevkx, Texturevky);
//}
//
//void TextureMappingTriangle(DrawingWindow& window, float (&z)[320][240]) {
//	TextureMap img = TextureMap("texture.ppm");
//
//	CanvasPoint v0 = CanvasPoint(160, 10, 195, 5);
//	CanvasPoint v1 = CanvasPoint(10, 150, 65, 330);
//	CanvasPoint v2 = CanvasPoint(300, 230, 395, 380);
//
//	drawTextureLine(v0, v1, window, img, z);
//	CanvasPoint vk = findvk(v0, v1, v2);
//	//fill top triangle  (v0 -> v1/vk)
//	float topmidDiffx = v0.x - v1.x;
//	float topextraDiffx = v0.x - vk.x;
//	float yDiff = v1.y - v0.y;
//	int numberOfSteps = max(std::max(abs(topmidDiffx), abs(topextraDiffx)), abs(yDiff));
//
//	float topmidStepSize = topmidDiffx / numberOfSteps;
//	float topextraStepSize = topextraDiffx / numberOfSteps;
//	float yStepSize = yDiff / numberOfSteps;
//
//
//	float texturev0_vk_x_stepsize = (v0.texturePoint.x - vk.texturePoint.x) / numberOfSteps;
//	float texturev0_vk_y_stepsize = (v0.texturePoint.y - vk.texturePoint.y) / numberOfSteps;
//	float texturev0_v1_x_stepsize = (v0.texturePoint.x - v1.texturePoint.x) / numberOfSteps;
//	float texturev0_v1_y_stepsize = (v0.texturePoint.y - v1.texturePoint.y) / numberOfSteps;
//
//
//	for (size_t i = 0; i <= numberOfSteps; i++)
//	{
//		int TexturePointFromX = round(v0.texturePoint.x - texturev0_v1_x_stepsize * i);
//		int TexturePointFromY = round(v0.texturePoint.y - texturev0_v1_y_stepsize * i);
//		CanvasPoint from = CanvasPoint(v0.x - topmidStepSize * i, v0.y + yStepSize * i, TexturePointFromX, TexturePointFromY);
//		CanvasPoint to = CanvasPoint(v0.x - topextraStepSize * i, v0.y + yStepSize * i, (int)round(v0.texturePoint.x - texturev0_vk_x_stepsize * i), (int)round(v0.texturePoint.y - texturev0_vk_y_stepsize * i));
//		if (i == numberOfSteps - 1) {
//			cout << "line 253";
//		}
//		drawTextureLine(from, to, window, img, z);
//	}
//
//	//fill bottom triangle (v1, vk -> v2)
//	float extrabottomDiffx = vk.x - v2.x;
//	float midbottomDiffx = v1.x - v2.x;
//	float ydiff = v2.y - vk.y;
//
//	float ydifftexturepoint = std::max(abs(v2.texturePoint.y - vk.texturePoint.y), abs(v2.texturePoint.y - v1.texturePoint.y));
//	float xdifftexturepoint = std::max(abs(v2.texturePoint.x - vk.texturePoint.x), abs(v2.texturePoint.x - v1.texturePoint.x));
//	float numberOfsteps = max(max(max(std::max(abs(extrabottomDiffx), abs(midbottomDiffx)), abs(ydiff)), ydifftexturepoint), xdifftexturepoint);
//
//	float extrabottomStepSize = extrabottomDiffx / numberOfsteps;
//	float midbottomStepSize = midbottomDiffx / numberOfsteps;
//	float ystepSize = ydiff / numberOfsteps;
//
//	float texturev1_v2_x_stepsize = (v1.texturePoint.x - v2.texturePoint.x) / numberOfsteps;
//	float texturev1_v2_y_stepsize = (v1.texturePoint.y - v2.texturePoint.y) / numberOfsteps;
//	float texturevk_v2_x_stepsize = (vk.texturePoint.x - v2.texturePoint.x) / numberOfsteps;
//	float texturevk_v2_y_stepsize = (vk.texturePoint.y - v2.texturePoint.y) / numberOfsteps;
//
//
//	for (size_t i = 0; i <= numberOfsteps; i++)
//	{
//		CanvasPoint From = CanvasPoint(v1.x - midbottomStepSize * i, v1.y + ystepSize * i, (int)round(v1.texturePoint.x - texturev1_v2_x_stepsize * i), (int)round(v1.texturePoint.y - texturev1_v2_y_stepsize * i));
//		CanvasPoint To = CanvasPoint(vk.x - extrabottomStepSize * i, vk.y + ystepSize * i, (int)round(vk.texturePoint.x - texturevk_v2_x_stepsize * i), (int)round(vk.texturePoint.y - texturevk_v2_y_stepsize * i));
//		drawTextureLine(From, To, window, img, z);
//	}
//	//
//
//
//
//}


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
			triangle.normal = glm::cross(vs[firstvalue], vs[secondvalue]);
			triangles.push_back(triangle);

		}
	}
	return triangles;

}


glm::vec3 getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition, float focalLength, glm::mat3 cameraOrientation) {
	float scalex = 300;
	float scaley = 300;
	float scalez = 300;
	glm::vec3 scaledvertexPosition = glm::vec3(vertexPosition.x * scalex, vertexPosition.y * scaley, vertexPosition.z);

	//glm::vec3 v = scaledvertexPosition - cameraPosition;
	glm::vec3 v = (vertexPosition - cameraPosition) * cameraOrientation;

	float x = focalLength * scalex * v.x / v.z + WIDTH / 2;
	float y = focalLength * scaley * v.y / v.z + HEIGHT / 2;
	float z = -scalez * v.z;
	return glm::vec3(WIDTH - x, y, z);
}



void drawWireframes(DrawingWindow& window, std::vector<std::vector<float>>& z, glm::vec3& cameraPosition, float& focalLength, vector<ModelTriangle> triangles, map<string, Colour>pallete, glm::mat3 cameraOrientation) {

	for (size_t i = 0; i < triangles.size(); i++)
	{
		//cout << triangles[i].vertices[0].x << endl;
		glm::vec3 v0 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[0], focalLength, cameraOrientation);
		glm::vec3 v1 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[1], focalLength, cameraOrientation);
		glm::vec3 v2 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[2], focalLength, cameraOrientation);
		CanvasTriangle canvastriangle = CanvasTriangle(CanvasPoint(v0.x, v0.y, v0.z), CanvasPoint(v1.x, v1.y, v1.z), CanvasPoint(v2.x, v2.y, v2.z));
		drawTriangle(canvastriangle, Colour(255, 255, 255), window, z);
	}

}

void draw3D(DrawingWindow& window, std::vector<std::vector<float>>& z, glm::vec3& cameraPosition, float& focalLength, vector<ModelTriangle> triangles, map<string, Colour>pallete, glm::mat3 cameraOrientation) {
	//vector<ModelTriangle> triangles = LoadObjtriangles(window);
	//map<string, Colour>pallete = LoadObjMaterial(window);


	for (size_t i = 0; i < triangles.size(); i++)
	{
		//cout << triangles[i].vertices[0].x << endl;
		glm::vec3 v0 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[0], focalLength, cameraOrientation);
		glm::vec3 v1 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[1], focalLength, cameraOrientation);
		glm::vec3 v2 = getCanvasIntersectionPoint(cameraPosition, triangles[i].vertices[2], focalLength, cameraOrientation);
		CanvasTriangle canvastriangle = CanvasTriangle(CanvasPoint(v0.x, v0.y, v0.z), CanvasPoint(v1.x, v1.y, v1.z), CanvasPoint(v2.x, v2.y, v2.z));
		drawFilledTriangle(canvastriangle, triangles[i].colour, window, z);
	}
}

void rotateY(glm::vec3& cameraPosition) {
	float theta = 5;
	glm::mat3 rotationMatrix = glm::mat3(1, 0, 0, 0, glm::cos(theta), glm::sin(theta), 0, -glm::sin(theta), -glm::cos(theta));
	cameraPosition = rotationMatrix * cameraPosition;
}


void moveCameraLeft(DrawingWindow& window, glm::vec3& cameraPosition) {

	glm::vec3 newcameraPosition = cameraPosition + glm::vec3(-0.05, 0, 0);
	cameraPosition = newcameraPosition;
}


void RotateXOrientation(glm::mat3& cameraOrientation, glm::vec3& cameraPosition) {
	float angle = 0.1;
	glm::mat3 rotationMatrix(glm::vec3(1, 0, 0), glm::vec3(0, glm::cos(angle), glm::sin(angle)), glm::vec3(0, -glm::sin(angle), glm::cos(angle)));
	cameraOrientation = rotationMatrix * cameraOrientation;
	cameraPosition = rotationMatrix * cameraPosition;
}


void RotateYOrientation(glm::mat3& cameraOrientation, glm::vec3& cameraPosition) {
	float angle = 0.1;
	glm::mat3 rotationMatrix(glm::vec3(glm::cos(angle), 0, -glm::sin(angle)), glm::vec3(0, 1, 0), glm::vec3(glm::sin(angle), 0, glm::cos(angle)));
	cameraOrientation = rotationMatrix * cameraOrientation;
	cameraPosition = rotationMatrix * cameraPosition;
}

glm::vec3 solve(glm::vec3 cameraPosition, glm::vec3 d, ModelTriangle triangle) {
	glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
	glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
	glm::vec3 SPVector = cameraPosition - triangle.vertices[0];
	glm::mat3 DEMatrix(-d, e0, e1);
	glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;

	return possibleSolution;
}

RayTriangleIntersection getClosestIntersection(glm::vec3 cameraPosition, glm::vec3 d, DrawingWindow& window, vector<ModelTriangle> triangles, map<string, Colour>pallete) {

	vector<float> solutionfort;
	vector<glm::vec3> solutions;
	map<float, RayTriangleIntersection> intersectionsmap;
	for (int i = 0; i < triangles.size(); i++)
	{
		glm::vec3 solution = solve(cameraPosition, d, triangles[i]);
		float u = solution.y;
		float v = solution.z;
		float t = solution.x;
		if (t > 0 && u >= 0.0 && v <= 1.0 && v >= 0.0 && v <= 1 && (u + v) <= 1.0) {
			//find the intersection coordinates. 
			// need: index, triangle , coordinate, t.
			glm::vec3 intersectionpoint = triangles[i].vertices[0] + u * triangles[i].vertices[1] + v * triangles[i].vertices[2];
			RayTriangleIntersection intersection = RayTriangleIntersection(intersectionpoint, t, triangles[i], i);
			intersectionsmap[t] = intersection;
			solutionfort.push_back(solution.x);
		}
	}
	// doesnt cut any triangle
	if (solutionfort.empty()) {
		//std::cout << "empty";
		return RayTriangleIntersection();
	}
	// return intersection with minimum t.
	float mint = *min_element(solutionfort.begin(), solutionfort.end());
	//std::cout << "intersection is <<><>"  << intersectionsmap[mint] << std::endl;
	return intersectionsmap[mint];
}

void draw(DrawingWindow& window, glm::vec3& cameraPosition, float& focalLength, vector<ModelTriangle> triangles, map<string, Colour>pallete, glm::mat3& cameraOrientation) {
	window.clearPixels();
	std::vector<std::vector<float>> z(WIDTH, std::vector<float>(HEIGHT, 0));
	draw3D(window, z, cameraPosition, focalLength, triangles, pallete, cameraOrientation);

}


Colour proximityLighting(glm::vec3 distanceFromLight, Colour c) {
	float scale = 6;
	float len = glm::length(distanceFromLight);
	float intensity = 1 / (4 * M_PI * (len * len));
	if (intensity > 1) intensity = 1;

	c.red *= intensity * scale;
	c.blue *= intensity * scale;
	c.green *= intensity * scale;

	return c;
}

Colour AngleOfIncidentLighting(glm::vec3 light_pos, glm::vec3 intersection_point, Colour c, ModelTriangle t) {
	Colour c_new = proximityLighting(light_pos - intersection_point, c);

	glm::vec3 normal = glm::normalize(t.normal);

	glm::vec3 light_direction = glm::normalize(light_pos - intersection_point);

	float intensity = glm::dot(normal, light_direction);
	intensity = min(intensity, 1.0f);
	intensity = max(intensity, 0.5f);

	c_new.red *= intensity;
	c_new.red = min(c_new.red, 255);
	c_new.blue *= intensity;
	c_new.blue = min(c_new.blue, 255);
	c_new.green *= intensity;
	c_new.green = min(c_new.green, 255);

	return c_new;
}

void drawRaytracing(DrawingWindow& window, glm::vec3& cameraPosition, float& focalLength, vector<ModelTriangle> triangles, map<string, Colour>pallete, glm::mat3& cameraOrientation) {
	window.clearPixels();
	for (size_t i = 0; i < triangles.size(); i++)
	{
		std::cout << triangles[i];
	}

	bool shadows = true;

	//glm::vec3 lightSource = glm::vec3(-0.3, -0.3, -2);
	glm::vec3 lightSource = glm::vec3(0.0, 0.75, 0.0);

	glm::vec3 cameraRight = cameraOrientation[0];
	glm::vec3 cameraUp = cameraOrientation[1];
	glm::vec3 cameraForwards = cameraOrientation[2];
	cameraForwards = cameraForwards / glm::length(cameraForwards); // normalise forwards

	float pixelSize = 0.003;
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			//cout << "(" << i << j << ")" << endl;
			shadows = false;
			glm::vec3 pixelWorldSpace = cameraPosition; // create particle at camera position
			pixelWorldSpace += -cameraForwards * focalLength; // translate to middle of image plane
			pixelWorldSpace += cameraRight * (i - (float)WIDTH / 2) * pixelSize; //translate up and down on image place
			pixelWorldSpace += -cameraUp * (j - (float)HEIGHT / 2) * pixelSize; //translate left and right on image plane

			glm::vec3 d = pixelWorldSpace - cameraPosition;
			RayTriangleIntersection intersection = getClosestIntersection(cameraPosition, d, window, triangles, pallete);

			// if this intersection can see the light
			glm::vec3 shadowRay = lightSource - intersection.intersectionPoint;
			float distanceTolight = shadowRay.length();

			RayTriangleIntersection shadowRayintersection = getClosestIntersection(intersection.intersectionPoint, shadowRay, window, triangles, pallete); // see if shadowRayintersects an object

			bool notitself = intersection.triangleIndex != shadowRayintersection.triangleIndex;
			bool modelHit = intersection.intersectionPoint.x != 0 && intersection.intersectionPoint.y != 0 && intersection.intersectionPoint.z != 0;
			bool shadowModelHit = shadowRayintersection.intersectionPoint.x != 0 && shadowRayintersection.intersectionPoint.y != 0 && shadowRayintersection.intersectionPoint.z != 0;
			//float distancetoObject = (shadowRayintersection.intersectionPoint - intersection.intersectionPoint).length();
			if (modelHit && shadowModelHit && shadowRayintersection.distanceFromCamera < distanceTolight) {
				//shadows = true;
			}
			Colour c = intersection.intersectedTriangle.colour;

			//PROXIMITY LIGHTING
			Colour c_new = proximityLighting(shadowRay, c);
			//ANGLE OF INCIDENT LIGHTING
			Colour c_new_angleofincident = AngleOfIncidentLighting(lightSource, intersection.intersectionPoint, c, intersection.intersectedTriangle);
			//Specular lighting

			//cout << "light strength: " << proximityLightStrength << std::endl;
			drawPixel(i, j, c_new_angleofincident, window);


		}

	}

	//draw lightsource. 
	//glm::vec3 lightsourceCanvas = getCanvasIntersectionPoint(cameraPosition, lightSource, focalLength, cameraOrientation);
	//drawPixel(lightsourceCanvas.x, lightsourceCanvas.y, Colour(255, 255, 255), window);
	//drawPixel(lightsourceCanvas.x+1, lightsourceCanvas.y, Colour(255, 255, 255), window);
	//drawPixel(lightsourceCanvas.x-1, lightsourceCanvas.y, Colour(255, 255, 255), window);

	//drawPixel(lightsourceCanvas.x, lightsourceCanvas.y - 1, Colour(255, 255, 255), window);
	//drawPixel(lightsourceCanvas.x+1, lightsourceCanvas.y - 1, Colour(255, 255, 255), window);
	//drawPixel(lightsourceCanvas.x - 1, lightsourceCanvas.y - 1, Colour(255, 255, 255), window);

	//drawPixel(lightsourceCanvas.x, lightsourceCanvas.y + 1, Colour(255, 255, 255), window);
	//drawPixel(lightsourceCanvas.x + 1, lightsourceCanvas.y + 1, Colour(255, 255, 255), window);
	//drawPixel(lightsourceCanvas.x - 1, lightsourceCanvas.y + 1, Colour(255, 255, 255), window);
	cout << "done" << endl;
}




void handleEvent(SDL_Event event, DrawingWindow& window, glm::vec3& cameraPosition, float& focalLength, glm::mat3& cameraOrientation, vector<ModelTriangle> triangles, map<string, Colour>pallete)
{

	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_LEFT) {
			moveCameraLeft(window, cameraPosition);
		}
		else if (event.key.keysym.sym == SDLK_RIGHT) {
			cameraPosition = cameraPosition + glm::vec3(0.05, 0, 0);
		}
		else if (event.key.keysym.sym == SDLK_UP) {
			cameraPosition = cameraPosition + glm::vec3(0, -0.05, 0);

		}
		else if (event.key.keysym.sym == SDLK_DOWN) {
			cameraPosition = cameraPosition + glm::vec3(0, 0.05, 0);
		}
		else if (event.key.keysym.sym == SDLK_b) {
			cameraPosition = cameraPosition + glm::vec3(0, 0.0, 0.05);
		}
		else if (event.key.keysym.sym == SDLK_f) {
			cameraPosition = cameraPosition + glm::vec3(0, 0.0, -0.05);
		}
		else if (event.key.keysym.sym == SDLK_r) {
			rotateY(cameraPosition);
		}
		else if (event.key.keysym.sym == SDLK_x) {
			cout << "pressedx";
			RotateXOrientation(cameraOrientation, cameraPosition);
		}
		else if (event.key.keysym.sym == SDLK_y) {
			cout << "pressedy";
			RotateYOrientation(cameraOrientation, cameraPosition);
		}
		else if (event.key.keysym.sym == SDLK_p) {
			cout << "pressedp";
			cout << getClosestIntersection(cameraPosition, glm::vec3(-0.1, -0.1, -2.0), window, triangles, pallete).distanceFromCamera;
		}
		else if (event.key.keysym.sym == SDLK_0) {
			cout << "pressed0";
			drawRaytracing(window, cameraPosition, focalLength, triangles, pallete, cameraOrientation);
		}

	}
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char* argv[])
{
	vector<ModelTriangle> triangles = LoadObjtriangles();
	map<string, Colour>pallete = LoadObjMaterial();
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	glm::vec3 cameraPosition = glm::vec3(0, 0, 4.0);
	glm::mat3 cameraOrientation(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
	float focalLength = 2.0;
	SDL_Event event;
	while (true)
	{
		// We MUST poll for events - otherwise the window will freeze !
		draw(window, cameraPosition, focalLength, triangles, pallete, cameraOrientation);
		if (window.pollForInputEvents(event))
			//drawRaytracing(window, cameraPosition, focalLength, triangles, pallete, cameraOrientation);

			handleEvent(event, window, cameraPosition, focalLength, cameraOrientation, triangles, pallete);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}


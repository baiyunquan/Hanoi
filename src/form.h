#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "text_renderer.h"
#include "sprite_renderer.h"
#include "object_2D.h"

class Header {
public:
	Header(std::string keyName, std::vector<std::string>& valueName) : key(keyName) , value(valueName){}

	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos);

	std::string key; 
	std::vector<std::string> value;
};

class Line {
public:
	std::vector<Object2D> values;
	std::string key;

	Line(std::string key , std::vector<Object2D>& values) : key(key) , values(values) {}

	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos);
};

class Form {
public:
	Form(std::string keyName, std::vector<std::string> valueName);

	void updateData(const std::vector<std::string>& data);
	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float screenWidth, float screenHeight);

	Header header;
	std::vector<Line> lines;
};
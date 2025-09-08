#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "text_renderer.h"
#include "sprite_renderer.h"
#include "game_object.h"

class Header {
public:
	Header(std::string keyName, std::vector<std::string>& valueName) : key(keyName) , value(valueName){}

	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos);

	std::string key; 
	std::vector<std::string> value;
};

class Line {
public:
	std::vector<GameObject> values;
	std::string key;

	Line(std::string key , std::vector<GameObject>& values) : key(key) , values(values) {}

	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos);
};

class Form {
public:
	Form(std::string keyName, std::vector<std::string> valueName);

	void updateData(std::vector<std::string> data);
	void regCallBack(std::function<void(std::string&, std::string&)>& callback);
	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float screenWidth, float screenHeight);

private:
	Header header;
	std::vector<Line> lines;

	// void callback( keyStr , valueStr)
	std::function<void(std::string&, std::string&)> callBack;
};
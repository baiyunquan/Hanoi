#pragma once

#include <string>
#include <vector>
#include <map>

#include "text_renderer.h"
#include "sprite_renderer.h"
#include "game_object.h"

class Header {
public:
	Header(std::string keyName, std::vector<std::string> valueName) : key(keyName) , value(valueName){}

	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos);
private :
	std::string key; 
	std::vector<std::string> value;
};

class Line {
public:
	std::vector<GameObject> values;

	Line(std::string key , std::vector<GameObject> values) : key(key) , values(values) {}

	void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos);
private :
	std::string key;
};

class Form {






};
#include "plate.h"

class Plate3D : public Plate {
public:
	// 空方法
	virtual void DrawText(TextRenderer& textRenderer) override {};
	virtual bool isChosen(float mouseX, float mouseY) override { return false; };
	virtual void Draw(SpriteRenderer& renderer) override;
};
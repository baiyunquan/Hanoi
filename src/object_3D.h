#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "object_2D.h"

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within Object2D.
class Object3D : Object2D
{
public:
	enum ObjectType {
		BOX , CYLINDER , CIRCLESLICE
	};

	// Getter for PosZ
	int getPosZ() const {
		return PosZ;
	}

	// Setter for PosZ
	void setPosZ(int z) {
		PosZ = z;
	}

	ObjectType getObjectType() {
		return objectType;
	}

	void setObjectType(ObjectType oj) {
		objectType = oj;
	}

	void Draw(SpriteRenderer renderer);
private :
	ObjectType objectType = BOX;
	int PosZ = 0;
};

#endif

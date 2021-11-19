#pragma once
#include "Mesh.h"
class HeightMap :
	public Mesh
{
public:
	HeightMap(const std::string& name);
	~HeightMap(void) {};

	void DrawType(GLuint type);

	Vector3 GetHeightmapSize() const { return heightmapSize; }
protected:
	Vector3 heightmapSize;
};


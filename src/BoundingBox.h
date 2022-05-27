#pragma once

#include "Vertex.h"

#include <glm/glm.hpp>

#include <vector>

class BoundingBox{
public:
	float minX, maxX, minY, maxY, minZ, maxZ;
	bool initialized = false;

	BoundingBox();
	BoundingBox(Vertex& v);
	BoundingBox(std::vector<Vertex>& vertices);
	BoundingBox(glm::vec3 v);
	BoundingBox(std::vector<glm::vec3>& vertices);

	void Insert(Vertex& v);
	void Insert(std::vector<Vertex>& vertices);
	void Insert(glm::vec3 v);
	void Insert(std::vector<glm::vec3>& vertices);
	void Union(BoundingBox& bb);
	float GetDiagonalLength();
	glm::vec3 GetCenter();
};
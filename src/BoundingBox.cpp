#include "BoundingBox.h"

BoundingBox::BoundingBox() : initialized(false) {}

BoundingBox::BoundingBox(Vertex& v) :
	minX(v.Position.x),
	maxX(v.Position.x),
	minY(v.Position.y),
	maxY(v.Position.y),
	minZ(v.Position.z),
	maxZ(v.Position.z),
	initialized(true)
{}

BoundingBox::BoundingBox(std::vector<Vertex>& vertices) :
	minX(vertices[0].Position.x),
	maxX(vertices[0].Position.x),
	minY(vertices[0].Position.y),
	maxY(vertices[0].Position.y),
	minZ(vertices[0].Position.z),
	maxZ(vertices[0].Position.z),
	initialized(true)
{
	for (int i = 1; i < vertices.size(); i++)
		Insert(vertices[i]);
}

BoundingBox::BoundingBox(glm::vec3 v) :
	minX(v.x),
	maxX(v.x),
	minY(v.y),
	maxY(v.y),
	minZ(v.z),
	maxZ(v.z),
	initialized(true)
{}

BoundingBox::BoundingBox(std::vector<glm::vec3>& vertices) :
	minX(vertices[0].x),
	maxX(vertices[0].x),
	minY(vertices[0].y),
	maxY(vertices[0].y),
	minZ(vertices[0].z),
	maxZ(vertices[0].z),
	initialized(true)
{
	for (int i = 1; i < vertices.size(); i++)
		Insert(vertices[i]);
}

void BoundingBox::Insert(Vertex& v)
{
	if (!initialized) {
		minX = v.Position.x;
		maxX = v.Position.x;
		minY = v.Position.y;
		maxY = v.Position.y;
		minZ = v.Position.z;
		maxZ = v.Position.z;
		initialized = true;
		return;
	}

	if (v.Position.x < minX)
		minX = v.Position.x;
	else if (v.Position.x > maxX)
		maxX = v.Position.x;

	if (v.Position.y < minY)
		minY = v.Position.y;
	else if (v.Position.y > maxY)
		maxY = v.Position.y;

	if (v.Position.z < minZ)
		minZ = v.Position.z;
	else if (v.Position.z > maxZ)
		maxZ = v.Position.z;
}

void BoundingBox::Insert(std::vector<Vertex>& vertices)
{
	for (Vertex& v : vertices)
		Insert(v);
}

void BoundingBox::Insert(glm::vec3 v)
{
	if (!initialized) {
		minX = v.x;
		maxX = v.x;
		minY = v.y;
		maxY = v.y;
		minZ = v.z;
		maxZ = v.z;
		initialized = true;
		return;
	}

	if (v.x < minX)
		minX = v.x;
	else if (v.x > maxX)
		maxX = v.x;

	if (v.y < minY)
		minY = v.y;
	else if (v.y > maxY)
		maxY = v.y;

	if (v.z < minZ)
		minZ = v.z;
	else if (v.z > maxZ)
		maxZ = v.z;
}

void BoundingBox::Insert(std::vector<glm::vec3>& vertices)
{
	for (glm::vec3& v : vertices)
		Insert(v);
}

void BoundingBox::Union(BoundingBox& bb)
{
	if (!initialized) {
		*this = bb;
		return;
	}

	minX = std::min(minX, bb.minX);
	minY = std::min(minY, bb.minY);
	minZ = std::min(minZ, bb.minZ);

	maxX = std::max(maxX, bb.maxX);
	maxY = std::max(maxY, bb.maxY);
	maxZ = std::max(maxZ, bb.maxZ);
}

float BoundingBox::GetDiagonalLength()
{
	assert(initialized);
	float lenZ = maxZ - minZ;
	float lenX = maxX - minX;
	float lenY = maxY - minY;
	return sqrt(lenX * lenX + lenY * lenY + lenZ * lenZ);
}

glm::vec3 BoundingBox::GetCenter()
{
	assert(initialized);
	return glm::vec3(minX + maxX, minY + maxY, minZ + maxZ)*0.5f;
}


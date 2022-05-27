#pragma once

#include "Vertex.h"

#include <vector>

enum ChangeType
{
	ChangeNormals,
	ChangeOffset
};

class Change {
public:
	glm::vec3 offset;
	float displacement = 0.0f;
	ChangeType changeType;

	Change(std::vector<Vertex*>& changedVertices, ChangeType type);
	void Redo();
	void Undo();
	void Modify(glm::vec3 newoffset);
	void Modify(float newDisplacement);
	void End();
	//void Reskin(std::vector<glm::mat4>& matrices);
private:
	std::vector<Vertex*> changedVertices;

	void ApplyOffset();
	void ApplyDisplacement();
	void RedoOffset();
	void RedoDisplacement();
	void UndoOffset();
	void UndoDisplacement();
	void EndOffset();
	void EndDisplacement();
};
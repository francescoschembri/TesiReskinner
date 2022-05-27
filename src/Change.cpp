#include "Change.h"
#include "eigen_glm_helpers.h"

Change::Change(std::vector<Vertex*>& changedVertices, ChangeType type)
	:
	changedVertices(changedVertices),
	offset(glm::vec3(0.0f, 0.0f, 0.0f)),
	displacement(0.0f),
	changeType(type)
{}

void Change::Redo() {
	if (changeType == ChangeNormals)
		RedoDisplacement();
	else
		RedoOffset();
}

void Change::Undo() {
	if (changeType == ChangeNormals)
		UndoDisplacement();
	else
		UndoOffset();
}

void Change::ApplyOffset()
{
	for (auto&& v : changedVertices) {
		v->Position += offset;
	}
}

void Change::RedoOffset() {
	ApplyOffset();
	EndOffset();
}

void Change::UndoOffset() {
	for (auto&& v : changedVertices) {
		v->Position -= offset;
		glm::mat4 inverse = glm::inverse(v->associatedWeightMatrix);
		v->originalVertex->Position -= glm::vec3((inverse * glm::vec4(offset, 0.0f)));
	}
}

void Change::Modify(glm::vec3 newoffset)
{
	offset = newoffset - offset;
	ApplyOffset();
	offset = newoffset;
}

void Change::ApplyDisplacement()
{
	for (auto&& v : changedVertices) {
		v->Position += v->Normal * displacement;
	}
}

void Change::RedoDisplacement() {
	ApplyDisplacement();
	EndDisplacement();
}


void Change::UndoDisplacement() {
	for (auto&& v : changedVertices) {
		glm::vec3 nOffset = v->Normal * displacement;
		v->Position -= nOffset;
		glm::mat4 inverse = glm::inverse(v->associatedWeightMatrix);
		v->originalVertex->Position -= glm::vec3((inverse * glm::vec4(nOffset, 0.0f)));
	}
}

void Change::EndOffset()
{
	for (auto&& v : changedVertices) {
		glm::mat4 inverse = glm::inverse(v->associatedWeightMatrix);
		v->originalVertex->Position += glm::vec3((inverse * glm::vec4(offset, 0.0f)));
	}
}

void Change::EndDisplacement()
{
	for (auto&& v : changedVertices) {
		glm::mat4 inverse = glm::inverse(v->associatedWeightMatrix);
		v->originalVertex->Position += glm::vec3((inverse * glm::vec4(v->Normal * displacement, 0.0f)));
	}
}

void Change::Modify(float newDisplacement)
{
	displacement = newDisplacement - displacement;
	ApplyDisplacement();
	displacement = newDisplacement;
}

void Change::End()
{
	if (changeType == ChangeNormals)
		EndDisplacement();
	else
		EndOffset();
}

//
//void Change::Reskin(std::vector<glm::mat4>& matrices) {
//	for (auto&& v : changedVertices) {
//		std::vector<glm::vec3> positions(matrices.size());
//		for (int i = 0; i < matrices.size(); i++) {
//			positions.push_back(glm::vec3(matrices[i] * glm::vec4(v->originalVertex->Position, 1.0f)));
//		}
//		Eigen::MatrixXf mat = MakeEigenMatrixWithGLMVec3Cols(positions);
//		Eigen::Vector3f finalPos = ConvertGLMVec3ToEigenVec3(v->Position);
//		Eigen::VectorXf weights = mat.colPivHouseholderQr().solve(finalPos);
//
//		Vertex* original = v->originalVertex;
//		original->BoneData.NumBones = 0;
//		for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
//			if (weights(i) > FLT_EPSILON || weights(i) < -FLT_EPSILON) //!= 0 for floating point values
//			{
//				original->BoneData.BoneIDs[original->BoneData.NumBones] = i;
//				original->BoneData.Weights[original->BoneData.NumBones++] = weights(i);
//			}
//		}
//	}
//}
#pragma once

#include "Animation.h"
#include "Bone.h"

#include <map>
#include <vector>
#include <algorithm>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>


class Animator
{
public:
	std::vector<Animation> animations;
	int currentAnimationIndex = 0;
	int playmode = 0;
	int playdir = 1;

	Animator();
	void UpdateAnimation(float dt);
	void PlayAnimation(Animation* animation);
	void PlayAnimationIndex(int index);
	void PlayNextAnimation();
	void PlayPrevAnimation();
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4>& GetFinalBoneMatrices();
	void AddAnimation(Animation animation);

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
};


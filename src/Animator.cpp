#include  "Animator.h"

float clamp(float v, float min, float max) {
	if(v<=min) return min;
	else if(v>=max) return max;
	else return v;
}

Animator::Animator() : currentAnimationIndex(0), m_FinalBoneMatrices(100, glm::mat4(1.0f)), playdir(1), playmode(0)
{
	animations.reserve(5);
}

void Animator::UpdateAnimation(float dt)
{
	Animation& currentAnimation = animations[currentAnimationIndex];
	if (&currentAnimation)
	{
		if (!playmode)
			playdir = 1;
		currentAnimation.currentTime += currentAnimation.GetTicksPerSecond() * dt * currentAnimation.speed * playdir;
		if (playmode) { //boomerang
			if (currentAnimation.currentTime >= currentAnimation.endAt)
			{
				currentAnimation.currentTime = 2 * currentAnimation.endAt - currentAnimation.currentTime;
				playdir *= -1;
			}
			else if (currentAnimation.currentTime <= currentAnimation.startFrom)
			{
				currentAnimation.currentTime = 2 * currentAnimation.startFrom - currentAnimation.currentTime;
				playdir *= -1;
			}
		}
		else { //loop
			currentAnimation.currentTime = clamp(fmod(currentAnimation.currentTime, currentAnimation.endAt), currentAnimation.startFrom, currentAnimation.endAt);
		}
		CalculateBoneTransform(&currentAnimation.GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* animation)
{
	if (animation) {
		currentAnimationIndex = animations.size();
		AddAnimation(*animation);
		UpdateAnimation(0.0f);
	}
}

void Animator::PlayAnimationIndex(int index)
{
	currentAnimationIndex = (index + animations.size()) % animations.size();
	UpdateAnimation(0.0f);
}

void Animator::PlayNextAnimation()
{
	PlayAnimationIndex(currentAnimationIndex + 1);
}

void Animator::PlayPrevAnimation()
{
	PlayAnimationIndex(currentAnimationIndex - 1);
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{ 
	std::string nodeName = node->name;
	Animation currentAnimation = animations[currentAnimationIndex];
	glm::mat4 nodeTransform = currentAnimation.GetNodeTransform(node);

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = currentAnimation.GetBoneInfoMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4>& Animator::GetFinalBoneMatrices()
{
	return m_FinalBoneMatrices;
}

void Animator::AddAnimation(Animation animation)
{
	if (&animation)
	{
		animations.push_back(animation);
	}
}

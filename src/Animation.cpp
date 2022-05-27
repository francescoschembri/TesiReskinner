#include "Animation.h"


Animation::Animation(const std::string& animationPath, Model& model) : speed(1.0f), currentTime(0.0f)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	int start = animationPath.find_last_of("/") + 1;
	name = animationPath.substr(start, animationPath.find_last_of(".") - start);
	endAt = m_Duration = animation->mDuration;
	m_TicksPerSecond = animation->mTicksPerSecond;
	ReadHeirarchyData(m_RootNode, scene->mRootNode);
	ReadMissingBones(animation, model);
}

Bone* Animation::FindBone(const std::string& name)
{
	auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.GetBoneName() == name;
		}
	);
	if (iter == m_Bones.end()) return nullptr;
	else return &(*iter);
}

float Animation::GetTicksPerSecond() { return m_TicksPerSecond; }
float Animation::GetDuration() { return m_Duration; }
const AssimpNodeData& Animation::GetRootNode() { return m_RootNode; }
const std::map<std::string, BoneInfo>& Animation::GetBoneInfoMap() { return m_BoneInfoMap; }

glm::mat4 Animation::GetNodeTransform(const AssimpNodeData* node)
{
	auto iter = std::find_if(std::begin(m_Bones), std::end(m_Bones), [&node](const auto& bone) {
		return bone.GetBoneName() == node->name;
		});
	if (iter == std::end(m_Bones))
		return node->transformation;
	iter->Update(currentTime);
	return iter->GetLocalTransform();
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		auto boneName = channel->mNodeName.data;

		m_Bones.push_back(Bone(boneName, model.AddBoneInfo(std::move(boneName), glm::mat4(1.0f)), channel));
	}

	m_BoneInfoMap = model.GetBoneInfoMap();
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHeirarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
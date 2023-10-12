#pragma once

struct pqCompare
{
	bool operator () (std::pair<float, int> a, std::pair<float, int> b)
	{
		if (a.first != b.first)
			return a.first > b.first;
		else
			return a.second > b.second;
	}
};

struct pqWrapper
{
	std::priority_queue < std::pair<float, int>, std::vector<std::pair<float, int>>, pqCompare > pq;

	void push(std::pair<float, int> inValue)
	{
		const int pqMaxSize = 5;
		if (pq.size() < pqMaxSize)
		{
			pq.push(inValue);
		}
		else if (pq.size() == pqMaxSize)
		{
			pq.push(inValue);
			pq.pop();
		}
		else
		{
			pq.push(inValue);
			while (pq.size() > pqMaxSize)
			{
				pq.pop();
			}
		}
	}
};

struct BoneInfo
{
	BoneInfo(FbxNode* InNode, int inIndex, int inParentIndex, const char* inJointName)
		:
		Index(inIndex),
		ParentIndex(inParentIndex),
		Name(inJointName)
	{}

	const char* Name;
	int Index;
	int ParentIndex;
	Vector3 Transform;
};

struct SkeletonInfo
{
	std::vector<BoneInfo> BoneInfoVector;

	BoneInfo& GetTargetBoneInfo(const char* JointName)
	{
		for (int i = 0; i < BoneInfoVector.size(); ++i)
		{
			BoneInfo& TargetBone = BoneInfoVector[i];
			if (strcmp(JointName, TargetBone.Name) == 0)
			{

				return TargetBone;
			}
		}

		assert(false);
		return BoneInfoVector[0];
	}
};

struct AnimationInfo
{
	AnimationInfo(std::string AnimationName)
		: AnimationName(AnimationName)
	{}

	const std::string AnimationName;

	SkeletonInfo Skeleton;
	std::vector<std::vector<Vector3>> FrameTranslation;
};
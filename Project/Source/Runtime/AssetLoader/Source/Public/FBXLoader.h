#pragma once

enum class NormalMapType : UINT32
{
	Normal = 0,
	Binormal,
	Tangent
};
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
			if(strcmp(JointName, TargetBone.Name) == 0)
			{

				return TargetBone;
			}
		}

		assert(false);
		return BoneInfoVector[0];
	}
};

struct BoneFrameInfo
{
	BoneFrameInfo()
		: BoneName()
	{}

	const char* BoneName;
	std::vector<FbxAMatrix> GlobalTransform;
};

struct AnimationInfo
{
	AnimationInfo(std::string AnimationName)
		: AnimationName(AnimationName)
	{}

	const std::string AnimationName;
	std::vector<BoneFrameInfo> Frames;
};

class FBXLoader
{
private:
	FBXLoader()
		: _IsInitialized(false)
		, _FbxManager(nullptr)
	{
	}

	FBXLoader(const FBXLoader& ref) 
		: _IsInitialized(ref.IsInitialized())
		, _FbxManager(nullptr)
	{}

	FBXLoader& operator=(const FBXLoader& ref) {}
	~FBXLoader() {}

	void Init();
	
public:
	static FBXLoader& get()
	{
		static FBXLoader _FBXLoader;
		_FBXLoader.Init();

		return _FBXLoader;
	}

	bool IsInitialized() const;

	// ĳ���� �ε��ؼ� �޸𸮿� �÷��� �ϴ� �� 
	// 1. �޽� - Vertices, Indices, UV
	//		ĳ���� ���� ������ŭ ���ؼ�, Offset �����ؼ�, ��ġ��
	// 
	// 2. png (Material)
	// 
	// 3. �� Ʈ��
	//		�� �̸� - GameEngine::���̸� ���ε�, Transform ����
	//		Parent ���� 
	//		�޽ÿ� ����?
	
	void LoadCharacterFBX(const std::string& AssetName, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT std::string& TexturePath, OUT SkeletonInfo& InSkeletonInfo, OUT std::vector<pqWrapper>& InWeightInfo);
	void LoadMesh(FbxNode* InNode, OUT unsigned int& StartVIndex, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT SkeletonInfo& InSkeletonInfo, OUT std::vector<pqWrapper>& InWeightInfo);
	void LoadSkeleton(FbxNode* InRootBoneNode, OUT SkeletonInfo& InSkeletonInfo);
	void GetBoneInfoRecursive(OUT SkeletonInfo& InSkeletonInfo, FbxNode* InNode, int InDepth, int MyIndex, int ParentIndex);

	FbxAMatrix GetGeometryTransformation(FbxNode* InNode);

	void LoadAnimation();
	void LoadAnimationWithName(const std::string& AnimationName);
	void GetBoneAnimationRecursive(FbxNode* InNode, const FbxLongLong& InStartIndex, const FbxLongLong& InEndIndex);


	// Texture Related
	Vector2 ReadUV(FbxMesh* InMesh, int ControllPointIndex, int VertexCounter);




private:
	bool _IsInitialized;
	FbxManager* _FbxManager;
};
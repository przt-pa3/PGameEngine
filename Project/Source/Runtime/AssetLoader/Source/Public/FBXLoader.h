#pragma once

enum class NormalMapType : UINT32
{
	Normal = 0,
	Binormal,
	Tangent
};

struct BoneInfo
{
	BoneInfo(FbxNode* InNode, int inIndex, int inParentIndex)
		: 
		Index(inIndex), 
		ParentIndex(inParentIndex),
		Name(InNode->GetName())
	{}

	const char* Name;
	int Index;
	int ParentIndex;
};

struct SkeletonInfo
{
	std::vector<BoneInfo> BoneInfoVector;
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
	
	void LoadCharacterFBX(const std::string& AssetName, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT std::string& TexturePath);
	void LoadMesh(FbxNode* InNode, OUT unsigned int& StartVIndex, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT std::vector<BoneInfo>& InSkeleton);
	void LoadSkeleton(FbxNode* InRootBoneNode, OUT std::vector<BoneInfo>& InSkeleton);
	void GetBoneInfoRecursive(OUT std::vector<BoneInfo>& InSkeleton, FbxNode* InNode, int InDepth, int MyIndex, int ParentIndex);

	FbxAMatrix GetGeometryTransformation(FbxNode* InNode);
	unsigned int FindBoneIndexByName(const std::vector<BoneInfo>& InSkeleton, const char* BoneName) const;

	void LoadAnimation();
	void LoadAnimationWithName(const std::string& AnimationName);
	void GetBoneAnimationRecursive(FbxNode* InNode, const FbxLongLong& InStartIndex, const FbxLongLong& InEndIndex);


	// Texture Related
	Vector2 ReadUV(FbxMesh* InMesh, int ControllPointIndex, int VertexCounter);




private:
	bool _IsInitialized;
	FbxManager* _FbxManager;
};
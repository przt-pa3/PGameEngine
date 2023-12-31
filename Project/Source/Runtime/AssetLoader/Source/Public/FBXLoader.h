#pragma once

enum class NormalMapType : UINT32
{
	Normal = 0,
	Binormal,
	Tangent
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

	// 캐릭터 로딩해서 메모리에 올려야 하는 것 
	// 1. 메시 - Vertices, Indices, UV
	//		캐릭터 파츠 개수만큼 구해서, Offset 설정해서, 합치기
	// 
	// 2. png (Material)
	// 
	// 3. 릭 트리
	//		본 이름 - GameEngine::본이름 바인딩, Transform 연결
	//		Parent 정보 
	//		메시에 리깅?
	
	void LoadCharacterFBX(const std::string& AssetName, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT std::string& TexturePath, OUT SkeletonInfo& InSkeletonInfo, OUT std::vector<std::vector<std::pair<std::string, float>>>& InWeightInfo);
	void LoadMesh(FbxNode* InNode, OUT unsigned int& StartVIndex, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT SkeletonInfo& InSkeletonInfo, OUT std::vector<std::vector<std::pair<std::string, float>>>& InWeightInfo);
	void LoadSkeleton(FbxNode* InRootBoneNode, OUT SkeletonInfo& InSkeletonInfo);
	void GetBoneInfoRecursive(OUT SkeletonInfo& InSkeletonInfo, FbxNode* InNode, int InDepth, int MyIndex, int ParentIndex);

	FbxAMatrix GetGeometryTransformation(FbxNode* InNode);

	void LoadAnimation();
	void LoadAnimationWithName(const std::string& AnimationName, const std::vector<std::string>& SKBones, OUT std::vector<std::string>& BoneNames, OUT std::vector<bool>& BoneUsage, std::vector<std::vector<Vector3>>& FTranslations, OUT std::vector <std::vector<Quaternion>>& FRotations);
	void GetBoneAnimationRecursive(FbxNode* InNode, const FbxLongLong& InStartIndex, const FbxLongLong& InEndIndex, const std::vector<std::string>& SKBones, 
		OUT std::vector<std::string>& BoneNames, OUT std::vector<bool>& BoneUsage, OUT std::vector<std::vector<Vector3>>& FTranslations, OUT std::vector <std::vector<Quaternion>>& FRotations);


	// Texture Related
	Vector2 ReadUV(FbxMesh* InMesh, int ControllPointIndex, int VertexCounter);




private:
	bool _IsInitialized;
	FbxManager* _FbxManager;
};
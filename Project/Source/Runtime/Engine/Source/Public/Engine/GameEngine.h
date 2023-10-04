#pragma once

namespace PGE
{
namespace DDD
{

struct GameObjectCompare
{
	bool operator()(const std::unique_ptr<GameObject>& lhs, std::size_t rhs)
	{
		return lhs->GetHash() < rhs;
	}
};

class GameEngine : public EngineInterface
{
public:
	GameEngine()
		: _GameMode(std::make_unique<GameMode>())
		, _Factory(std::make_unique<GameObjectTypeFactory>())
	{}

public:
	// ���� �������̽�
	virtual bool Init() override;
	virtual bool IsInitialized() override { return _IsInitialized; }
	virtual void OnScreenResize(const ScreenPoint& InScreenSize) override;
	virtual InputManager& GetInputManager() override { return _InputManager; }
	virtual std::vector<PLogs>& GetLogs() override { return _EngineLogs; }
	virtual PLogs& GetTargetLog(const std::string& LogHeader) override;

	// ���� ���� �뵵 
	const InputManager& GetInputManager() const { return _InputManager; }
	const std::vector<PLogs>& GetLogs() const { return _EngineLogs; }

	// Logging
	void InitLogChannels();
	int NewLogChannel(const std::string& NewLogHeader);

	// Load Asset
	void LoadCube();
	void LoadAsset();


	// ���ҽ� ����
	Mesh& CreateMesh(const std::size_t& InKey);
	SKMesh& CreateSKMesh(const std::size_t& InKey);
	Texture& CreateTexture(const std::size_t& InKey);

	// ���� ������Ʈ
	const std::vector<std::unique_ptr<GameObject>>& GetScene() const { return _Scene; }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneEnd() const { return _Scene.end(); }
	GameObject& CreateNewGameObject(const std::string& InName);
	GameObject& GetGameObject(const std::string& InName);

	void RegisterGameObjectFactories();
	GameObject& CreateNewGameObjectOfClass(const std::string& InName, const std::string& InTypeName);

	// �޽�
	Mesh& GetMesh(const std::size_t& InMeshKey) { return *_Meshes.at(InMeshKey).get(); }
	const Mesh& GetMesh(const std::size_t& InMeshKey) const { return *_Meshes.at(InMeshKey).get(); }

	// ī�޶� 
	FORCEINLINE CameraObject& GetMainCamera() { return _MainCamera; }
	FORCEINLINE const CameraObject& GetMainCamera() const { return _MainCamera; }

	// ���� �ؽ�ó
	FORCEINLINE const Texture& GetTexture(const std::size_t& InTextureKey) const { return *_Textures.at(InTextureKey).get(); }

	//// ���� �׸��� ���� ���
	//std::unordered_map<std::string, GameObject*> GetBoneObjectPtrs() { return _BoneGameObjectPtrs; }

private:
	bool LoadResources();

public: // �ֿ� Ű ��
//	// ��
	static const std::string GameEngine::RootBone;
	static const std::string GameEngine::HipsBone;
	static const std::string GameEngine::SpineBone;
//	static const std::string PelvisBone;
//	static const std::string SpineBone;
//	static const std::string LeftArmBone;
//	static const std::string RightArmBone;
//	static const std::string NeckBone;
//	static const std::string LeftLegBone;
//	static const std::string RightLegBone;
//
	// �޽�
	static const std::size_t CubeMesh;
	static const std::size_t CharacterMesh;
	static const std::size_t ArrowMesh;
//	static const std::size_t PlaneMesh;
//
//	// �ؽ�ó
	static const std::size_t BaseTexture;
	static const std::size_t CharacterTexture;
//	static const std::string CharacterTexturePath;

	// GameObject Factories
	static PGameObjectFactory PFactory;
	static ActorFactory ActorFactory;
	static CharacterFactory CharacterFactory;

private:
	bool _IsInitialized = false;

	std::unique_ptr<GameMode> _GameMode;
	std::unique_ptr<GameObjectTypeFactory> _Factory;

	ScreenPoint _ScreenSize;
	InputManager _InputManager;
	CameraObject _MainCamera;

	std::vector<PLogs> _EngineLogs;
	std::unordered_map<std::string, int> _LogIndexMap;

	std::vector<std::unique_ptr<GameObject>> _Scene;
	std::unordered_map<std::size_t, std::unique_ptr<Mesh>> _Meshes;
	std::unordered_map<std::size_t, std::unique_ptr<Texture>> _Textures;
	std::unordered_map<std::string, GameObject*> _BoneGameObjectOtrs;
	
	
};



}
}
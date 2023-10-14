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
	// 공용 인터페이스
	virtual bool Init() override;
	virtual bool IsInitialized() override { return _IsInitialized; }
	virtual void OnScreenResize(const ScreenPoint& InScreenSize) override;
	virtual InputManager& GetInputManager() override { return _InputManager; }
	virtual std::vector<PLogs>& GetLogs() override { return _EngineLogs; }
	virtual PLogs& GetTargetLog(const std::string& LogHeader) override;

	//// TODO : Temp
	//virtual void InterfaceTestFunction() override;

	// 게임 로직 용도 
	const InputManager& GetInputManager() const { return _InputManager; }
	const std::vector<PLogs>& GetLogs() const { return _EngineLogs; }

	// Logging
	void InitLogChannels();
	int NewLogChannel(const std::string& NewLogHeader);

	// Load Asset
	void LoadCube();
	void LoadAsset();


	// 리소스 관리
	Mesh& CreateMesh(const std::size_t& InKey);
	SKMesh& CreateSKMesh(const std::size_t& InKey);
	Texture& CreateTexture(const std::size_t& InKey);
	Animation& CreateAnimation(const std::size_t& InKey);

	// 게임 오브젝트
	const std::vector<std::unique_ptr<GameObject>>& GetScene() const { return _Scene; }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneBegin() const { return _Scene.begin(); }
	std::vector< std::unique_ptr<GameObject>>::const_iterator SceneEnd() const { return _Scene.end(); }
	GameObject& CreateNewGameObject(const std::string& InName);
	GameObject& GetGameObject(const std::string& InName);

	void RegisterGameObjectFactories();
	GameObject& CreateNewGameObjectOfClass(const std::string& InName, const std::string& InTypeName);

	// 메시
	Mesh& GetMesh(const std::size_t& InMeshKey) { return *_Meshes.at(InMeshKey).get(); }
	const Mesh& GetMesh(const std::size_t& InMeshKey) const { return *_Meshes.at(InMeshKey).get(); }

	// Animation
	Animation& GetAnimation(const std::size_t& InAnimKey) { return *_Anims.at(InAnimKey).get(); }
	const Animation& GetAnimation(const std::size_t& InAnimKey) const { return *_Anims.at(InAnimKey).get(); }
	
	// 카메라 
	FORCEINLINE CameraObject& GetMainCamera() { return _MainCamera; }
	FORCEINLINE const CameraObject& GetMainCamera() const { return _MainCamera; }

	// 메인 텍스처
	FORCEINLINE const Texture& GetTexture(const std::size_t& InTextureKey) const { return *_Textures.at(InTextureKey).get(); }

	//// 본을 그리기 위한 목록
	//std::unordered_map<std::string, GameObject*> GetBoneObjectPtrs() { return _BoneGameObjectPtrs; }

private:
	bool LoadResources();

public: // 주요 키 값
//	 본
	static const std::string GameEngine::RootBone;
	static const std::string GameEngine::HipsBone;
	static const std::string GameEngine::SpineBone;
	static const std::string GameEngine::Spine1Bone;
	static const std::string GameEngine::Spine2Bone;
	static const std::string GameEngine::NeckBone;
	static const std::string GameEngine::HeadBone;
	static const std::string GameEngine::HeadTop_EndBone;
	static const std::string GameEngine::LeftEyeBone;
	static const std::string GameEngine::RightEyeBone;
	static const std::string GameEngine::LeftShoulderBone;
	static const std::string GameEngine::LeftArmBone;
	static const std::string GameEngine::LeftForeArmBone;
	static const std::string GameEngine::LeftHandBone;
	static const std::string GameEngine::LeftHandThumb1Bone;
	static const std::string GameEngine::LeftHandThumb2Bone;
	static const std::string GameEngine::LeftHandThumb3Bone;
	static const std::string GameEngine::LeftHandThumb4Bone;
	static const std::string GameEngine::LeftHandIndex1Bone;
	static const std::string GameEngine::LeftHandIndex2Bone;
	static const std::string GameEngine::LeftHandIndex3Bone;
	static const std::string GameEngine::LeftHandIndex4Bone;
	static const std::string GameEngine::LeftHandMiddle1Bone;
	static const std::string GameEngine::LeftHandMiddle2Bone;
	static const std::string GameEngine::LeftHandMiddle3Bone;
	static const std::string GameEngine::LeftHandMiddle4Bone;
	static const std::string GameEngine::LeftHandRing1Bone;
	static const std::string GameEngine::LeftHandRing2Bone;
	static const std::string GameEngine::LeftHandRing3Bone;
	static const std::string GameEngine::LeftHandRing4Bone;
	static const std::string GameEngine::LeftHandPinky1Bone;
	static const std::string GameEngine::LeftHandPinky2Bone;
	static const std::string GameEngine::LeftHandPinky3Bone;
	static const std::string GameEngine::LeftHandPinky4Bone;
	static const std::string GameEngine::Shield_jointBone;
	static const std::string GameEngine::RightShoulderBone;
	static const std::string GameEngine::RightArmBone;
	static const std::string GameEngine::RightForeArmBone;
	static const std::string GameEngine::RightHandBone;
	static const std::string GameEngine::RightHandThumb1Bone;
	static const std::string GameEngine::RightHandThumb2Bone;
	static const std::string GameEngine::RightHandThumb3Bone;
	static const std::string GameEngine::RightHandThumb4Bone;
	static const std::string GameEngine::RightHandIndex1Bone;
	static const std::string GameEngine::RightHandIndex2Bone;
	static const std::string GameEngine::RightHandIndex3Bone;
	static const std::string GameEngine::RightHandIndex4Bone;
	static const std::string GameEngine::RightHandMiddle1Bone;
	static const std::string GameEngine::RightHandMiddle2Bone;
	static const std::string GameEngine::RightHandMiddle3Bone;
	static const std::string GameEngine::RightHandMiddle4Bone;
	static const std::string GameEngine::RightHandRing1Bone;
	static const std::string GameEngine::RightHandRing2Bone;
	static const std::string GameEngine::RightHandRing3Bone;
	static const std::string GameEngine::RightHandRing4Bone;
	static const std::string GameEngine::RightHandPinky1Bone;
	static const std::string GameEngine::RightHandPinky2Bone;
	static const std::string GameEngine::RightHandPinky3Bone;
	static const std::string GameEngine::RightHandPinky4Bone;
	static const std::string GameEngine::Sword_jointBone;
	static const std::string GameEngine::LeftUpLegBone;
	static const std::string GameEngine::LeftLegBone;
	static const std::string GameEngine::LeftFootBone;
	static const std::string GameEngine::LeftToeBaseBone;
	static const std::string GameEngine::LeftToe_EndBone;
	static const std::string GameEngine::RightUpLegBone;
	static const std::string GameEngine::RightLegBone;
	static const std::string GameEngine::RightFootBone;
	static const std::string GameEngine::RightToeBaseBone;
	static const std::string GameEngine::RightToe_EndBone;
// ~ 본

	// 메시
	static const std::size_t CubeMesh;
	static const std::size_t CharacterMesh;
	static const std::size_t ArrowMesh;


	// 텍스처
	static const std::size_t BaseTexture;
	static const std::size_t CharacterTexture;

	// Animation
	static const std::size_t SaluteAnimation;

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
	std::unordered_map<std::size_t, std::unique_ptr<Animation>> _Anims;
	
	
};



}
}
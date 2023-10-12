#include "Precompiled.h"
using namespace PGE::DDD;

// GameObject Factories
PGameObjectFactory GameEngine::PFactory;
ActorFactory GameEngine::ActorFactory;
CharacterFactory GameEngine::CharacterFactory;

// Meshes
const std::size_t GameEngine::ArrowMesh = std::hash<std::string>()("SM_Arrow");
const std::size_t GameEngine::CharacterMesh = std::hash<std::string>()("SK_Character");
const std::size_t GameEngine::CubeMesh = std::hash<std::string>()("M_Cube");

// Textures
const std::size_t GameEngine::BaseTexture = std::hash<std::string>()("T_Base");
const std::size_t GameEngine::CharacterTexture = std::hash<std::string>()("T_Character");

// Bones
const std::string GameEngine::RootBone("RootBone");
const std::string GameEngine::HipsBone("HipsBone");
const std::string GameEngine::SpineBone("SpineBone");
const std::string GameEngine::Spine1Bone("Spine1Bone");
const std::string GameEngine::Spine2Bone("Spine2Bone");
const std::string GameEngine::NeckBone("NeckBone");
const std::string GameEngine::HeadBone("HeadBone");
const std::string GameEngine::HeadTop_EndBone("HeadTop_EndBone");
const std::string GameEngine::LeftEyeBone("LeftEyeBone");
const std::string GameEngine::RightEyeBone("RightEyeBone");
const std::string GameEngine::LeftShoulderBone("LeftShoulderBone");
const std::string GameEngine::LeftArmBone("LeftArmBone");
const std::string GameEngine::LeftForeArmBone("LeftForeArmBone");
const std::string GameEngine::LeftHandBone("LeftHandBone");
const std::string GameEngine::LeftHandThumb1Bone("LeftHandThumb1Bone");
const std::string GameEngine::LeftHandThumb2Bone("LeftHandThumb2Bone");
const std::string GameEngine::LeftHandThumb3Bone("LeftHandThumb3Bone");
const std::string GameEngine::LeftHandThumb4Bone("LeftHandThumb4Bone");
const std::string GameEngine::LeftHandIndex1Bone("LeftHandIndex1Bone");
const std::string GameEngine::LeftHandIndex2Bone("LeftHandIndex2Bone");
const std::string GameEngine::LeftHandIndex3Bone("LeftHandIndex3Bone");
const std::string GameEngine::LeftHandIndex4Bone("LeftHandIndex4Bone");
const std::string GameEngine::LeftHandMiddle1Bone("LeftHandMiddle1Bone");
const std::string GameEngine::LeftHandMiddle2Bone("LeftHandMiddle2Bone");
const std::string GameEngine::LeftHandMiddle3Bone("LeftHandMiddle3Bone");
const std::string GameEngine::LeftHandMiddle4Bone("LeftHandMiddle4Bone");
const std::string GameEngine::LeftHandRing1Bone("LeftHandRing1Bone");
const std::string GameEngine::LeftHandRing2Bone("LeftHandRing2Bone");
const std::string GameEngine::LeftHandRing3Bone("LeftHandRing3Bone");
const std::string GameEngine::LeftHandRing4Bone("LeftHandRing4Bone");
const std::string GameEngine::LeftHandPinky1Bone("LeftHandPinky1Bone");
const std::string GameEngine::LeftHandPinky2Bone("LeftHandPinky2Bone");
const std::string GameEngine::LeftHandPinky3Bone("LeftHandPinky3Bone");
const std::string GameEngine::LeftHandPinky4Bone("LeftHandPinky4Bone");
const std::string GameEngine::Shield_jointBone("Shield_jointBone");
const std::string GameEngine::RightShoulderBone("RightShoulderBone");
const std::string GameEngine::RightArmBone("RightArmBone");
const std::string GameEngine::RightForeArmBone("RightForeArmBone");
const std::string GameEngine::RightHandBone("RightHandBone");
const std::string GameEngine::RightHandThumb1Bone("RightHandThumb1Bone");
const std::string GameEngine::RightHandThumb2Bone("RightHandThumb2Bone");
const std::string GameEngine::RightHandThumb3Bone("RightHandThumb3Bone");
const std::string GameEngine::RightHandThumb4Bone("RightHandThumb4Bone");
const std::string GameEngine::RightHandIndex1Bone("RightHandIndex1Bone");
const std::string GameEngine::RightHandIndex2Bone("RightHandIndex2Bone");
const std::string GameEngine::RightHandIndex3Bone("RightHandIndex3Bone");
const std::string GameEngine::RightHandIndex4Bone("RightHandIndex4Bone");
const std::string GameEngine::RightHandMiddle1Bone("RightHandMiddle1Bone");
const std::string GameEngine::RightHandMiddle2Bone("RightHandMiddle2Bone");
const std::string GameEngine::RightHandMiddle3Bone("RightHandMiddle3Bone");
const std::string GameEngine::RightHandMiddle4Bone("RightHandMiddle4Bone");
const std::string GameEngine::RightHandRing1Bone("RightHandRing1Bone");
const std::string GameEngine::RightHandRing2Bone("RightHandRing2Bone");
const std::string GameEngine::RightHandRing3Bone("RightHandRing3Bone");
const std::string GameEngine::RightHandRing4Bone("RightHandRing4Bone");
const std::string GameEngine::RightHandPinky1Bone("RightHandPinky1Bone");
const std::string GameEngine::RightHandPinky2Bone("RightHandPinky2Bone");
const std::string GameEngine::RightHandPinky3Bone("RightHandPinky3Bone");
const std::string GameEngine::RightHandPinky4Bone("RightHandPinky4Bone");
const std::string GameEngine::Sword_jointBone("Sword_jointBone");
const std::string GameEngine::LeftUpLegBone("LeftUpLegBone");
const std::string GameEngine::LeftLegBone("LeftLegBone");
const std::string GameEngine::LeftFootBone("LeftFootBone");
const std::string GameEngine::LeftToeBaseBone("LeftToeBaseBone");
const std::string GameEngine::LeftToe_EndBone("LeftToe_EndBone");
const std::string GameEngine::RightUpLegBone("RightUpLegBone");
const std::string GameEngine::RightLegBone("RightLegBone");
const std::string GameEngine::RightFootBone("RightFootBone");
const std::string GameEngine::RightToeBaseBone("RightToeBaseBone");
const std::string GameEngine::RightToe_EndBone("RightToe_EndBone");

void GameEngine::OnScreenResize(const ScreenPoint& InScreenSize)
{
	_ScreenSize = InScreenSize;
	_MainCamera.SetViewportSize(_ScreenSize);
}

bool GameEngine::Init()
{
	if (_IsInitialized)
	{
		return true;
	}

	// Check Things

	if (_ScreenSize.HasZero())
	{
		return false;
	}

	if (!_InputManager.IsInputReady())
	{
		return false;
	}

	if (!LoadResources())
	{
		return false;
	}

	InitLogChannels();
	RegisterGameObjectFactories();

	//LoadCube();
	LoadAsset();

	_IsInitialized = true;
	return _IsInitialized;
}

// Logging
PLogs& GameEngine::GetTargetLog(const std::string& LogHeader)
{
	auto MapValue = _LogIndexMap.find(LogHeader);
	
	if (MapValue != _LogIndexMap.end())
	{
		return _EngineLogs[MapValue->second];
	}

	int NewIndex = NewLogChannel(LogHeader);

	return _EngineLogs[NewIndex];
}

void GameEngine::InitLogChannels()
{
	PLogs EngineLog;
	EngineLog.SetLogHeader("EngineLOG");
	_EngineLogs.push_back(EngineLog);
	_LogIndexMap.insert(std::make_pair("EngineLOG", 0));

	NewLogChannel("InputLOG");
}

int GameEngine::NewLogChannel(const std::string& NewLogHeader)
{
	PLogs NewLogs;
	NewLogs.SetLogHeader(NewLogHeader);
	_EngineLogs.push_back(NewLogs);

	int NewIndex = static_cast<int>(_EngineLogs.size()) - 1;
	_LogIndexMap.insert(std::make_pair(NewLogHeader, NewIndex));

	return NewIndex;
}
// ~ Logging

Mesh& GameEngine::CreateMesh(const std::size_t& InKey)
{
	auto meshPtr = std::make_unique<Mesh>();
	_Meshes.insert({ InKey, std::move(meshPtr) });
	return *_Meshes.at(InKey).get();
}

SKMesh& GameEngine::CreateSKMesh(const std::size_t& InKey)
{
	auto meshPtr = std::make_unique<SKMesh>();
	_Meshes.insert({ InKey, std::move(meshPtr) });
	return static_cast<SKMesh&>(*_Meshes.at(InKey).get());
}

Texture& GameEngine::CreateTexture(const std::size_t& InKey)
{
	auto texturePtr = std::make_unique<Texture>();
	_Textures.insert({ InKey, std::move(texturePtr) });
	return *_Textures.at(InKey).get();
}

GameObject& GameEngine::CreateNewGameObject(const std::string& InName)
{
	std::size_t inHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());

	auto newGameObject = std::make_unique<GameObject>(InName);
	if (it != _Scene.end())
	{
		std::size_t targetHash = (*it)->GetHash();
		if (targetHash == inHash)
		{
			// 중복된 키 발생. 무시.
			assert(false);
			return GameObject::Invalid;
		}
		else if (targetHash < inHash)
		{
			_Scene.insert(it + 1, std::move(newGameObject));
		}
		else
		{
			_Scene.insert(it, std::move(newGameObject));
		}
	}
	else
	{
		_Scene.push_back(std::move(newGameObject));
	}

	return GetGameObject(InName);
}

GameObject& GameEngine::GetGameObject(const std::string& InName)
{
	std::size_t targetHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), targetHash, GameObjectCompare());

	return (it != _Scene.end()) ? *(*it).get() : GameObject::Invalid;
}

void GameEngine::RegisterGameObjectFactories()
{
	_Factory.get()->RegisterFactory("PObject", &PFactory);
	_Factory.get()->RegisterFactory("Actor", &ActorFactory);
	_Factory.get()->RegisterFactory("Character", &CharacterFactory);
}

GameObject& GameEngine::CreateNewGameObjectOfClass(const std::string& InName, const std::string& InTypeName)
{
	GameObject* PotentialNewGameObject = _Factory.get()->CreateIfRegistered(InName, InTypeName);
	assert(PotentialNewGameObject);
	// PGameObject* Test = static_cast<PGameObject*>(newGameObject.get());

	std::size_t inHash = std::hash<std::string>()(InName);
	const auto it = std::lower_bound(SceneBegin(), SceneEnd(), inHash, GameObjectCompare());
	auto newGameObject = std::unique_ptr<GameObject>(PotentialNewGameObject);
	
	if (it != _Scene.end())
	{
		std::size_t targetHash = (*it)->GetHash();
		if (targetHash == inHash)
		{
			// 중복된 키 발생. 무시.
			assert(false);
			return GameObject::Invalid;
		}
		else if (targetHash < inHash)
		{
			_Scene.insert(it + 1, std::move(newGameObject));
		}
		else
		{
			_Scene.insert(it, std::move(newGameObject));
		}
	}
	else
	{
		_Scene.push_back(std::move(newGameObject));
	}

	return GetGameObject(InName);
}

void GameEngine::LoadCube()
{
	constexpr Vector3 CubeSize(0.5f, 0.5f, 0.5f);
	Mesh& SimpleCubeMesh = CreateMesh(GameEngine::CubeMesh);
	auto& v = SimpleCubeMesh.GetVertices();
	auto& i = SimpleCubeMesh.GetIndices();
	
	int PointSize = cubeMeshPositions.size();

	for (int p = 0; p < PointSize; ++p)
	{
		v.push_back(cubeMeshPositions[p]);
	}
	
	int IndexSize = cubeMeshIndice.size();

	for (int vi = 0; vi < IndexSize; ++vi)
	{
		i.push_back(cubeMeshIndice[vi]);
	}

	SimpleCubeMesh.SetMeshType(MeshType::Normal);
	SimpleCubeMesh.CalculateBounds();

	Texture& TextureBase = CreateTexture(GameEngine::BaseTexture);

	OutputDebugString("LoadCube Complete");
}

void GameEngine::LoadAsset()
{
	FBXLoader& TestLoader = FBXLoader::get();

	SKMesh& CharacterMesh = CreateSKMesh(GameEngine::CharacterMesh);
	auto& v = CharacterMesh.GetVertices();
	auto& i = CharacterMesh.GetIndices();
	auto& uv = CharacterMesh.GetUVs();

	SkeletonInfo CharacterSkeleton;
	std::vector<pqWrapper> WeightInfo;

	std::string TexturePath;
	TestLoader.LoadCharacterFBX("Paladin", OUT v, OUT i, OUT uv, OUT TexturePath, OUT CharacterSkeleton, OUT WeightInfo);

	Texture& CharacterTexture = CreateTexture(GameEngine::CharacterTexture);
	CharacterTexture.LoadFromPath(TexturePath);

	CharacterMesh.SetMeshType(MeshType::Normal);
	CharacterMesh.CalculateBounds();

	// Rigging 
	auto& cb = CharacterMesh.GetConnectedBones();
	auto& w = CharacterMesh.GetWeights();
	auto& bones = CharacterMesh.GetBones();

	bones = {
		{ GameEngine::RootBone, Bone(GameEngine::RootBone, Transform())},
		{ GameEngine::HipsBone, Bone(GameEngine::HipsBone, Transform(CharacterSkeleton.GetTargetBoneInfo("Hips").Transform)) },
		{ GameEngine::SpineBone, Bone(GameEngine::SpineBone, Transform(CharacterSkeleton.GetTargetBoneInfo("Spine").Transform)) },
		{ GameEngine::Spine1Bone, Bone(GameEngine::Spine1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("Spine1").Transform)) },
		{ GameEngine::Spine2Bone, Bone(GameEngine::Spine2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("Spine2").Transform)) },
		{ GameEngine::NeckBone, Bone(GameEngine::NeckBone, Transform(CharacterSkeleton.GetTargetBoneInfo("Neck").Transform)) },
		{ GameEngine::HeadBone, Bone(GameEngine::HeadBone, Transform(CharacterSkeleton.GetTargetBoneInfo("Head").Transform)) },
		{ GameEngine::HeadTop_EndBone, Bone(GameEngine::HeadTop_EndBone, Transform(CharacterSkeleton.GetTargetBoneInfo("HeadTop_End").Transform)) },
		{ GameEngine::LeftEyeBone, Bone(GameEngine::LeftEyeBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftEye").Transform)) },
		{ GameEngine::RightEyeBone, Bone(GameEngine::RightEyeBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightEye").Transform)) },
		{ GameEngine::LeftShoulderBone, Bone(GameEngine::LeftShoulderBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftShoulder").Transform)) },
		{ GameEngine::LeftArmBone, Bone(GameEngine::LeftArmBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftArm").Transform)) },
		{ GameEngine::LeftForeArmBone, Bone(GameEngine::LeftForeArmBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftForeArm").Transform)) },
		{ GameEngine::LeftHandBone, Bone(GameEngine::LeftHandBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHand").Transform)) },
		{ GameEngine::LeftHandThumb1Bone, Bone(GameEngine::LeftHandThumb1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandThumb1").Transform)) },
		{ GameEngine::LeftHandThumb2Bone, Bone(GameEngine::LeftHandThumb2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandThumb2").Transform)) },
		{ GameEngine::LeftHandThumb3Bone, Bone(GameEngine::LeftHandThumb3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandThumb3").Transform)) },
		{ GameEngine::LeftHandThumb4Bone, Bone(GameEngine::LeftHandThumb4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandThumb4").Transform)) },
		{ GameEngine::LeftHandIndex1Bone, Bone(GameEngine::LeftHandIndex1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandIndex1").Transform)) },
		{ GameEngine::LeftHandIndex2Bone, Bone(GameEngine::LeftHandIndex2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandIndex2").Transform)) },
		{ GameEngine::LeftHandIndex3Bone, Bone(GameEngine::LeftHandIndex3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandIndex3").Transform)) },
		{ GameEngine::LeftHandIndex4Bone, Bone(GameEngine::LeftHandIndex4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandIndex4").Transform)) },
		{ GameEngine::LeftHandMiddle1Bone, Bone(GameEngine::LeftHandMiddle1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandMiddle1").Transform)) },
		{ GameEngine::LeftHandMiddle2Bone, Bone(GameEngine::LeftHandMiddle2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandMiddle2").Transform)) },
		{ GameEngine::LeftHandMiddle3Bone, Bone(GameEngine::LeftHandMiddle3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandMiddle3").Transform)) },
		{ GameEngine::LeftHandMiddle4Bone, Bone(GameEngine::LeftHandMiddle4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandMiddle4").Transform)) },
		{ GameEngine::LeftHandRing1Bone, Bone(GameEngine::LeftHandRing1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandRing1").Transform)) },
		{ GameEngine::LeftHandRing2Bone, Bone(GameEngine::LeftHandRing2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandRing2").Transform)) },
		{ GameEngine::LeftHandRing3Bone, Bone(GameEngine::LeftHandRing3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandRing3").Transform)) },
		{ GameEngine::LeftHandRing4Bone, Bone(GameEngine::LeftHandRing4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandRing4").Transform)) },
		{ GameEngine::LeftHandPinky1Bone, Bone(GameEngine::LeftHandPinky1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandPinky1").Transform)) },
		{ GameEngine::LeftHandPinky2Bone, Bone(GameEngine::LeftHandPinky2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandPinky2").Transform)) },
		{ GameEngine::LeftHandPinky3Bone, Bone(GameEngine::LeftHandPinky3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandPinky3").Transform)) },
		{ GameEngine::LeftHandPinky4Bone, Bone(GameEngine::LeftHandPinky4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftHandPinky4").Transform)) },
		{ GameEngine::Shield_jointBone, Bone(GameEngine::Shield_jointBone, Transform(CharacterSkeleton.GetTargetBoneInfo("Shield_joint").Transform)) },
		{ GameEngine::RightShoulderBone, Bone(GameEngine::RightShoulderBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightShoulder").Transform)) },
		{ GameEngine::RightArmBone, Bone(GameEngine::RightArmBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightArm").Transform)) },
		{ GameEngine::RightForeArmBone, Bone(GameEngine::RightForeArmBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightForeArm").Transform)) },
		{ GameEngine::RightHandBone, Bone(GameEngine::RightHandBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHand").Transform)) },
		{ GameEngine::RightHandThumb1Bone, Bone(GameEngine::RightHandThumb1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandThumb1").Transform)) },
		{ GameEngine::RightHandThumb2Bone, Bone(GameEngine::RightHandThumb2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandThumb2").Transform)) },
		{ GameEngine::RightHandThumb3Bone, Bone(GameEngine::RightHandThumb3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandThumb3").Transform)) },
		{ GameEngine::RightHandThumb4Bone, Bone(GameEngine::RightHandThumb4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandThumb4").Transform)) },
		{ GameEngine::RightHandIndex1Bone, Bone(GameEngine::RightHandIndex1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandIndex1").Transform)) },
		{ GameEngine::RightHandIndex2Bone, Bone(GameEngine::RightHandIndex2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandIndex2").Transform)) },
		{ GameEngine::RightHandIndex3Bone, Bone(GameEngine::RightHandIndex3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandIndex3").Transform)) },
		{ GameEngine::RightHandIndex4Bone, Bone(GameEngine::RightHandIndex4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandIndex4").Transform)) },
		{ GameEngine::RightHandMiddle1Bone, Bone(GameEngine::RightHandMiddle1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandMiddle1").Transform)) },
		{ GameEngine::RightHandMiddle2Bone, Bone(GameEngine::RightHandMiddle2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandMiddle2").Transform)) },
		{ GameEngine::RightHandMiddle3Bone, Bone(GameEngine::RightHandMiddle3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandMiddle3").Transform)) },
		{ GameEngine::RightHandMiddle4Bone, Bone(GameEngine::RightHandMiddle4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandMiddle4").Transform)) },
		{ GameEngine::RightHandRing1Bone, Bone(GameEngine::RightHandRing1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandRing1").Transform)) },
		{ GameEngine::RightHandRing2Bone, Bone(GameEngine::RightHandRing2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandRing2").Transform)) },
		{ GameEngine::RightHandRing3Bone, Bone(GameEngine::RightHandRing3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandRing3").Transform)) },
		{ GameEngine::RightHandRing4Bone, Bone(GameEngine::RightHandRing4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandRing4").Transform)) },
		{ GameEngine::RightHandPinky1Bone, Bone(GameEngine::RightHandPinky1Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandPinky1").Transform)) },
		{ GameEngine::RightHandPinky2Bone, Bone(GameEngine::RightHandPinky2Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandPinky2").Transform)) },
		{ GameEngine::RightHandPinky3Bone, Bone(GameEngine::RightHandPinky3Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandPinky3").Transform)) },
		{ GameEngine::RightHandPinky4Bone, Bone(GameEngine::RightHandPinky4Bone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightHandPinky4").Transform)) },
		{ GameEngine::Sword_jointBone, Bone(GameEngine::Sword_jointBone, Transform(CharacterSkeleton.GetTargetBoneInfo("Sword_joint").Transform)) },
		{ GameEngine::LeftUpLegBone, Bone(GameEngine::LeftUpLegBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftUpLeg").Transform)) },
		{ GameEngine::LeftLegBone, Bone(GameEngine::LeftLegBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftLeg").Transform)) },
		{ GameEngine::LeftFootBone, Bone(GameEngine::LeftFootBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftFoot").Transform)) },
		{ GameEngine::LeftToeBaseBone, Bone(GameEngine::LeftToeBaseBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftToeBase").Transform)) },
		{ GameEngine::LeftToe_EndBone, Bone(GameEngine::LeftToe_EndBone, Transform(CharacterSkeleton.GetTargetBoneInfo("LeftToe_End").Transform)) },
		{ GameEngine::RightUpLegBone, Bone(GameEngine::RightUpLegBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightUpLeg").Transform)) },
		{ GameEngine::RightLegBone, Bone(GameEngine::RightLegBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightLeg").Transform)) },
		{ GameEngine::RightFootBone, Bone(GameEngine::RightFootBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightFoot").Transform)) },
		{ GameEngine::RightToeBaseBone, Bone(GameEngine::RightToeBaseBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightToeBase").Transform)) },
		{ GameEngine::RightToe_EndBone, Bone(GameEngine::RightToe_EndBone, Transform(CharacterSkeleton.GetTargetBoneInfo("RightToe_End").Transform)) }
	};

	std::vector<std::string> boneOrder = {
		GameEngine::RootBone,
		GameEngine::HipsBone,
		GameEngine::SpineBone,
		GameEngine::Spine1Bone,
		GameEngine::Spine2Bone,
		GameEngine::NeckBone,
		GameEngine::HeadBone,
		GameEngine::HeadTop_EndBone,
		GameEngine::LeftEyeBone,
		GameEngine::RightEyeBone,
		GameEngine::LeftShoulderBone,
		GameEngine::LeftArmBone,
		GameEngine::LeftForeArmBone,
		GameEngine::LeftHandBone,
		GameEngine::LeftHandThumb1Bone,
		GameEngine::LeftHandThumb2Bone,
		GameEngine::LeftHandThumb3Bone,
		GameEngine::LeftHandThumb4Bone,
		GameEngine::LeftHandIndex1Bone,
		GameEngine::LeftHandIndex2Bone,
		GameEngine::LeftHandIndex3Bone,
		GameEngine::LeftHandIndex4Bone,
		GameEngine::LeftHandMiddle1Bone,
		GameEngine::LeftHandMiddle2Bone,
		GameEngine::LeftHandMiddle3Bone,
		GameEngine::LeftHandMiddle4Bone,
		GameEngine::LeftHandRing1Bone,
		GameEngine::LeftHandRing2Bone,
		GameEngine::LeftHandRing3Bone,
		GameEngine::LeftHandRing4Bone,
		GameEngine::LeftHandPinky1Bone,
		GameEngine::LeftHandPinky2Bone,
		GameEngine::LeftHandPinky3Bone,
		GameEngine::LeftHandPinky4Bone,
		GameEngine::Shield_jointBone,
		GameEngine::RightShoulderBone,
		GameEngine::RightArmBone,
		GameEngine::RightForeArmBone,
		GameEngine::RightHandBone,
		GameEngine::RightHandThumb1Bone,
		GameEngine::RightHandThumb2Bone,
		GameEngine::RightHandThumb3Bone,
		GameEngine::RightHandThumb4Bone,
		GameEngine::RightHandIndex1Bone,
		GameEngine::RightHandIndex2Bone,
		GameEngine::RightHandIndex3Bone,
		GameEngine::RightHandIndex4Bone,
		GameEngine::RightHandMiddle1Bone,
		GameEngine::RightHandMiddle2Bone,
		GameEngine::RightHandMiddle3Bone,
		GameEngine::RightHandMiddle4Bone,
		GameEngine::RightHandRing1Bone,
		GameEngine::RightHandRing2Bone,
		GameEngine::RightHandRing3Bone,
		GameEngine::RightHandRing4Bone,
		GameEngine::RightHandPinky1Bone,
		GameEngine::RightHandPinky2Bone,
		GameEngine::RightHandPinky3Bone,
		GameEngine::RightHandPinky4Bone,
		GameEngine::Sword_jointBone,
		GameEngine::LeftUpLegBone,
		GameEngine::LeftLegBone,
		GameEngine::LeftFootBone,
		GameEngine::LeftToeBaseBone,
		GameEngine::LeftToe_EndBone,
		GameEngine::RightUpLegBone,
		GameEngine::RightLegBone,
		GameEngine::RightFootBone,
		GameEngine::RightToeBaseBone,
		GameEngine::RightToe_EndBone,
	};

	auto a = CharacterSkeleton.GetTargetBoneInfo("Spine").Transform;
	auto& b = CharacterMesh.GetBone("SpineBone").GetTransform();

	// Connecting Bones
	for (auto boneIdx = 1; boneIdx < boneOrder.size(); boneIdx++)
	{
		int ParentIdx = CharacterSkeleton.BoneInfoVector[boneIdx - 1].ParentIndex + 1;
		if (ParentIdx < 0) continue;

		Bone& TargetChild = CharacterMesh.GetBone(boneOrder[boneIdx]);
		Bone& TargetParent = CharacterMesh.GetBone(boneOrder[CharacterSkeleton.BoneInfoVector[boneIdx - 1].ParentIndex + 1]);
		TargetChild.SetParent(TargetParent);
	}

	cb.resize(v.size());
	std::fill(cb.begin(), cb.end(), 1);

	// Moving Weight Information
	w.resize(v.size());
	for (auto idx = 0; idx < WeightInfo.size(); ++idx)
	{
		auto& targetpq = WeightInfo[idx].pq;
		while(!targetpq.empty())
		{
			w[idx].Bones.push_back(boneOrder[targetpq.top().second + 1]);
			w[idx].Values.push_back(targetpq.top().first);
			targetpq.pop();
		}
		
	}

	TestLoader.LoadAnimation();

	Mesh& arrow = CreateMesh(GameEngine::ArrowMesh);
	arrow.GetVertices().resize(arrowPositions.size());
	arrow.GetIndices().resize(arrowIndice.size());
	arrow.GetColors().resize(arrowPositions.size());
	std::copy(arrowPositions.begin(), arrowPositions.end(), arrow.GetVertices().begin());
	std::copy(arrowIndice.begin(), arrowIndice.end(), arrow.GetIndices().begin());
	std::fill(arrow.GetColors().begin(), arrow.GetColors().end(), LinearColor::Gray);

	OutputDebugString("LoadAsset Complete");
}

bool GameEngine::LoadResources()
{
	return true;
}

// TODO : Temp, Should use Dynamic Casting
void GameEngine::InterfaceTestFunction()
{
	static bool flag = true;

	GameObject& goPlayer = GetGameObject("Player");
	Mesh& m = GetMesh(goPlayer.GetMeshKey());
	SKMesh& skm = static_cast<SKMesh&>(m);
	Bone& NeckBone = skm.GetBone(GameEngine::NeckBone);

	if (flag)
	{
		NeckBone.GetTransform().SetLocalRotation(Rotator(50.f, 0.f, 0.f));
		flag = false;
	}
	else
	{
		NeckBone.GetTransform().SetLocalRotation(Rotator(-50.f, 0.f, 0.f));
		flag = true;
	}
	
}
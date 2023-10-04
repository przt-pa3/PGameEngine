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

	std::string TexturePath;
	TestLoader.LoadCharacterFBX("Paladin", OUT v, OUT i, OUT uv, OUT TexturePath);

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
		{ GameEngine::HipsBone, Bone(GameEngine::HipsBone, Transform(Vector3(100.0f, -100.0f, 0.f)))},
		{ GameEngine::SpineBone, Bone(GameEngine::SpineBone, Transform(Vector3(0.0f, 200.25f, 0.f)))}
	};

	Bone& root = CharacterMesh.GetBone(GameEngine::RootBone);
	Bone& hip = CharacterMesh.GetBone(GameEngine::HipsBone);
	hip.SetParent(root);
	Bone& spine = CharacterMesh.GetBone(GameEngine::SpineBone);
	spine.SetParent(hip);

	cb.resize(v.size());
	w.resize(v.size());
	std::fill(cb.begin(), cb.end(), 1);

	static std::array<std::string, 2> boneOrder = {
		GameEngine::HipsBone, GameEngine::SpineBone
	};

	for (size_t part = 0; part < 2; part++)
	{
		Weight weight; 
		weight.Bones = { boneOrder[part] };
		weight.Values = { 1.f };
		auto startIt = w.begin() + part * 4;
		std::fill(startIt, startIt + 4, weight);
	}


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

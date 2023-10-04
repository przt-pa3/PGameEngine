#pragma once

namespace PGE
{
namespace DDD
{

class GameObjectFactory;
class GameObjectTypeFactory;

class GameObject
{
public:
	GameObject(const std::string& InName) 
		: _Name(InName)
	{
		_Hash = std::hash<std::string>()(_Name);
	}

	~GameObject() {}

protected:
	void OnInit();
	virtual void NativeOnInit();

public:
	// 종류
	void SetGameObjectType(const GameObjectType& _InGameObjectType) { _GameObjectType = _InGameObjectType; }
	FORCEINLINE bool IsGizmoObject() const { return _GameObjectType == GameObjectType::Gizmo; }

	// 트랜스폼
	TransformComponent& GetTransform() { return _Transform; }
	const TransformComponent& GetTransform() const { return _Transform; }
	void SetParent(GameObject& InGameObject) { _Transform.SetParent(InGameObject.GetTransform()); }

	// 메시
	void SetMesh(const std::size_t& InMeshKey) { _MeshKey = InMeshKey; }
	bool HasMesh() const { return _MeshKey != Math::InvalidHash; }
	FORCEINLINE std::size_t GetMeshKey() const { return _MeshKey; }

	// 색상
	void SetColor(const LinearColor& InColor) { _Color = InColor; }
	FORCEINLINE const LinearColor& GetColor() const { return _Color; }

	// 키 관련
	const std::string& GetName() const { return _Name; }
	std::size_t GetHash() const { return _Hash; }

	// 검색 관련
	bool IsValid() const { return _Hash != Math::InvalidHash; }
	FORCEINLINE bool operator==(const GameObject& InGameObject) const;
	FORCEINLINE bool operator!=(const GameObject& InGameObject) const;
	static GameObject Invalid;

	// 렌더링 설정
	bool IsVisible() const { return _IsVisible; }
	void SetVisible(bool InVisible) { _IsVisible = InVisible; }

protected:
	GameObjectType _GameObjectType = GameObjectType::Normal;
	bool _IsVisible = true;
	std::size_t _Hash = Math::InvalidHash;
	std::string _Name;
	std::size_t _MeshKey = Math::InvalidHash;
	TransformComponent _Transform;
	LinearColor _Color = LinearColor::Error;
};

FORCEINLINE bool GameObject::operator==(const GameObject& InGameObject) const
{
	return _Hash == InGameObject.GetHash();
}

FORCEINLINE bool GameObject::operator!=(const GameObject& InGameObject) const
{
	return _Hash == InGameObject.GetHash();
}

class GameObjectFactory
{
public:
	virtual ~GameObjectFactory() {}
	virtual GameObject* Create(const std::string& InName) = 0;
};

class GameObjectTypeFactory
{
public:
	void RegisterFactory(const std::string& GameObjectType, GameObjectFactory* TargetFactory)
	{
		_Factories.insert(std::pair<std::string, GameObjectFactory*>(GameObjectType, TargetFactory));
	}

	GameObject* Create(const std::string& InName, const std::string& GameObjectType)
	{

		return _Factories.at(GameObjectType)->Create(InName);
	}

	GameObject* CreateIfRegistered(const std::string& InName, const std::string& GameObjectType)
	{
		auto a = _Factories.find(GameObjectType);
		if (a != _Factories.end())
		{
			return Create(InName, GameObjectType);
		}
		return nullptr;
	}
private:
	std::unordered_map<std::string, GameObjectFactory*> _Factories;
};

}
}
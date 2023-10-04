#pragma once

namespace PGE
{
namespace DDD
{

class PGameObject : public GameObject
{

public:
	PGameObject(const std::string& InName) 
		: GameObject(InName)
	{}

};

class PGameObjectFactory : public GameObjectFactory
{
public:
	virtual GameObject* Create(const std::string& InName) override
	{
		return new PGameObject(InName);
	}
};



}
}
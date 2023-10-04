#pragma once

namespace PGE
{
namespace DDD
{

class Actor : public GameObject
{

public:
	Actor(const std::string& InName)
		: GameObject(InName)
	{}

};

class ActorFactory : public GameObjectFactory
{
public:
	virtual GameObject* Create(const std::string& InName) override
	{
		return new Actor(InName);
	}
};

}
}
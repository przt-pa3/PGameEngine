#pragma once

namespace PGE
{
namespace DDD
{

class Character : public Actor
{

public:
	Character(const std::string& InName)
		: Actor(InName)
	{}

public:

protected:


};

class CharacterFactory : public ActorFactory
{
public:
	virtual GameObject* Create(const std::string& InName) override
	{
		return new Character(InName);
	}
};

}
}
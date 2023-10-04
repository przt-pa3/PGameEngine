#include "Precompiled.h"
using namespace PGE::DDD;

GameObject GameObject::Invalid(Math::InvalidHashName);

void GameObject::OnInit()
{
	NativeOnInit();
}

void GameObject::NativeOnInit()
{

}
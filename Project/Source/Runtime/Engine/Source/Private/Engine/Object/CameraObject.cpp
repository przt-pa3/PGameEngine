#include "Precompiled.h"

using namespace PGE::DDD;

void CameraObject::SetLookAtRotation(const GameObject& InGameObject, const Vector3& InUp)
{
	// TODO
	SetLookAtRotation(InGameObject.GetTransform().GetWorldPosition(), InUp);
}

void CameraObject::SetLookAtRotation(const Vector3& InTargetPosition, const Vector3& InUp)
{
	_Transform.SetWorldRotation(Quaternion(InTargetPosition - _Transform.GetWorldPosition()));
}
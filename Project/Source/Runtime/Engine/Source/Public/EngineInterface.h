#pragma once

namespace PGE
{

enum class GameObjectType : UINT32
{
	Normal = 0,
	Gizmo
};

enum class MeshType : UINT32
{
	Normal = 0,
	Skinned
};

enum class TextureFilterMode : UINT32
{
	Nearest = 0,
	Linear,
	Cubic
};

enum class TextureAddressMode : UINT32
{
	Repeat = 0,
	Clamp
};

class EngineInterface
{
public:
	virtual bool Init() = 0;
	virtual bool IsInitialized() = 0;
	virtual void OnScreenResize(const ScreenPoint& InScreenSize) = 0;
	virtual InputManager& GetInputManager() = 0;
	virtual std::vector<PLogs>& GetLogs() = 0;
	virtual PLogs& GetTargetLog(const std::string& LogHeader) = 0;

	//virtual void InterfaceTestFunction() = 0;
};

}
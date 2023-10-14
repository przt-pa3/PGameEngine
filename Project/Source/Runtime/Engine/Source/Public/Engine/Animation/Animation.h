#pragma once

namespace PGE
{
namespace DDD
{

class Animation
{
public:
	Animation() = default;

public:
	std::vector<std::vector<Vector3>>& GetFrameTranslantions() { return _FrameTranslations; }
	const std::vector<std::vector<Vector3>>& GetFrameTranslantions() const { return _FrameTranslations; }
	
	std::vector<std::vector<Quaternion>>& GetFrameQuaternions() { return _FrameQuaternions; }
	const std::vector<std::vector<Quaternion>>& GetFrameQuaternions() const { return _FrameQuaternions; }

	std::vector<std::string>& GetBoneNames() { return _BoneNames; }
	const std::vector<std::string>& GetBoneNames() const { return _BoneNames; }

	std::vector<bool>& GetBoneUsage() { return _BoneUsage; }
	const std::vector<bool>& GetBoneUsage() const { return _BoneUsage; }
private:
	std::vector<std::string> _BoneNames;
	std::vector<bool> _BoneUsage;
	std::vector<std::vector<Vector3>> _FrameTranslations;
	std::vector<std::vector<Quaternion>> _FrameQuaternions;
};




}
}
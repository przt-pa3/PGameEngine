#pragma once

namespace PGE
{
namespace DDD
{

struct Weight
{
	std::vector<std::string> Bones;
	std::vector<float> Values;
};

class SKMesh : public Mesh
{
public:

	virtual bool IsSKMesh() const override { return true; }

	// 스켈레탈 애니메이션 관련 함수
	FORCEINLINE bool IsSkinnedMesh() const { return _MeshType == MeshType::Skinned; }
	FORCEINLINE bool HasBone(const std::string& InBoneName) const { return _Bones.find(InBoneName) != _Bones.end(); }

	std::vector<Weight>& GetWeights() { return _Weights; }
	const std::vector<Weight>& GetWeights() const { return _Weights; }

	std::vector<BYTE>& GetConnectedBones() { return _ConnectedBones; }
	const std::vector<BYTE>& GetConnectedBones() const { return _ConnectedBones; }

	Bone& GetBone(const std::string& InBoneName) { return _Bones.at(InBoneName); } 
	const Bone& GetBone(const std::string& InBoneName) const { return _Bones.at(InBoneName); }

	std::unordered_map<std::string, Bone>& GetBones() { return _Bones; }
	const std::unordered_map<std::string, Bone>& GetBones() const { return _Bones; }

	const Transform& GetBindPose(const std::string& InBoneName) const { return _Bones.at(InBoneName).GetBindPose(); }

protected:
	std::vector<BYTE> _ConnectedBones;
	std::vector<Weight> _Weights;
	std::unordered_map<std::string, Bone> _Bones;
};

}
}
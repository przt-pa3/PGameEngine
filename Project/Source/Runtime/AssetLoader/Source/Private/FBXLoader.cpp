
#include "Precompiled.h"


void FBXLoader::Init()
{
	if (_IsInitialized)
	{
		return;
	}

	_FbxManager = FbxManager::Create();

	_IsInitialized = true;
}

bool FBXLoader::IsInitialized() const
{
	return _IsInitialized;
}

void FBXLoader::LoadCharacterFBX(const std::string& AssetName, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT std::string& TexturePath, OUT SkeletonInfo& InSkeletonInfo, OUT std::vector<std::vector<std::pair<std::string, float>>>& InWeightInfo)
{
	assert(_FbxManager);
	
	const std::string InFilePath = AssetFolderPath + "Character/" + AssetName + ".fbx";
	const std::string TextureFilePath = AssetFolderPath + "Character/" + AssetName + ".fbm/" + AssetName + "_";
	TexturePath = TextureFilePath;

	const char* filepath = InFilePath.c_str();

	FbxIOSettings* ios = FbxIOSettings::Create(_FbxManager, IOSROOT);
	_FbxManager->SetIOSettings(ios);
	FbxImporter* importer = FbxImporter::Create(_FbxManager, "");
	bool status = importer->Initialize(filepath, -1, _FbxManager->GetIOSettings());

	assert(status);

	FbxScene* scene = FbxScene::Create(_FbxManager, "scene");
	importer->Import(scene);
	importer->Destroy();

	// Check Axis System
	FbxAxisSystem ASR = scene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem MyAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);

	FbxNode* rootNode = scene->GetRootNode();
	
	const int childCount = rootNode->GetChildCount();
	unsigned int MeshVertexIndex = 0;

	std::vector<BoneInfo>& sk = InSkeletonInfo.BoneInfoVector;

	for (auto i = 0; i < childCount; ++i)
	{
		FbxNode* childNode = rootNode->GetChild(i);
		FbxNodeAttribute* childAttribute = childNode->GetNodeAttribute();
		if (childAttribute)
		{
			if (childAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
			{
				LoadSkeleton(childNode, OUT InSkeletonInfo);
				break;
			}
		}
	}

	for (auto i = 0; i < childCount; ++i)
	{
		FbxNode* childNode = rootNode->GetChild(i);
		FbxNodeAttribute* childAttribute = childNode->GetNodeAttribute();
		if (childAttribute)
		{
			if (childAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				char buffer[50];
				auto childName = childAttribute->GetName();
				sprintf(buffer, "Node %d : %s \n\n", i, childName);
				OutputDebugString(buffer);

				//	for Paladin i==2
				//	for Mannequin i==0

				if (i == 2)
				{
					LoadMesh(childNode, OUT MeshVertexIndex, OUT Vertices, OUT Indices, OUT Uvs, OUT InSkeletonInfo, OUT InWeightInfo);
				}
			}
		}
	}

}

void FBXLoader::LoadMesh(FbxNode* InNode, OUT unsigned int& StartVIndex, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT SkeletonInfo& InSkeletonInfo, OUT std::vector<std::vector<std::pair<std::string, float>>>& InWeightInfo)
{
	char buffer[50];
	sprintf(buffer, "%s - LoadMesh Called \n", InNode->GetName());
	OutputDebugString(buffer);

	// Triangulate Mesh (Node/Mesh -> Converted Node/Mesh)
	FbxGeometryConverter* geometryConverter = new FbxGeometryConverter(_FbxManager);
	FbxNodeAttribute* OriginalAttribute = InNode->GetNodeAttribute();
	FbxNodeAttribute* ConvertedAttribute = geometryConverter->Triangulate(OriginalAttribute, false);
	delete geometryConverter;

	FbxNode* ConvertedNode = ConvertedAttribute->GetNode();
	FbxMesh* ConvertedMesh = ConvertedNode->GetMesh();

	// Read Vertices and Indices
	unsigned int PointCount = ConvertedMesh->GetControlPointsCount();
	unsigned int PolyCount = ConvertedMesh->GetPolygonCount();
	FbxGeometryElementUV* vUV = ConvertedMesh->GetElementUV(0);

	// One file can be consist of multiple Meshes
	unsigned int VertexCount = StartVIndex;

	// vi (vertex index in .fbx) and Vertices (my version of vertex index) is diff.
	// therefore, map them with vector -> TODO : Later use better datastructure
	int viSize = ConvertedMesh->GetControlPointsCount();
	std::vector<std::vector<int>> viToVertices(viSize);

	for (auto p = 0; p < PolyCount; ++p)
	{
		unsigned int vCountInPolygon = ConvertedMesh->GetPolygonSize(p);
		assert(vCountInPolygon == 3);

		for (auto v = 0; v < 3; ++v)
		{
			int vi = ConvertedMesh->GetPolygonVertex(p, v);
			float x = static_cast<float>(ConvertedMesh->GetControlPointAt(vi).mData[0]);
			float y = static_cast<float>(ConvertedMesh->GetControlPointAt(vi).mData[1]);
			float z = static_cast<float>(ConvertedMesh->GetControlPointAt(vi).mData[2]);

			Vertices.push_back(PGE::Vector3(x, y, z));
			Indices.push_back(VertexCount);

			// link vi to Vertices
			viToVertices[vi].push_back(VertexCount);

			// Read .fbx related Texture file
			Vector2 VertexUV;
			VertexUV = ReadUV(ConvertedMesh, vi, ConvertedMesh->GetTextureUVIndex(p, v));
			Uvs.push_back(VertexUV);
			VertexCount++;
		}
	}
	
	// Update StartVIndex for next Mesh to Load
	StartVIndex = VertexCount;

	// Getting Rig
	InWeightInfo.resize(VertexCount);

	unsigned int numOfDeformers = ConvertedMesh->GetDeformerCount();
	
	//	Not Used for Now - 추후 gpu Library 올릴 때 사용
	// FbxAMatrix GeometryTransform = GetGeometryTransformation(InNode);

	// for now, Only one deformer is considered
	assert(numOfDeformers == 1);

	for (unsigned int DeformerIndex = 0; DeformerIndex < numOfDeformers; ++DeformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(ConvertedMesh->GetDeformer(DeformerIndex, FbxDeformer::eSkin));
		if (!currSkin) continue;

		unsigned int numOfClusters = currSkin->GetClusterCount();

		for (unsigned int ClusterIndex = 0; ClusterIndex < numOfClusters; ++ClusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(ClusterIndex);

			// convert fbx joint name to my form
			// e.g.)  "mixamorig:Hips" -> "HipsBone"
			const char* currJointFullName = (currCluster->GetLink()->GetName());
			char* cpy = strdup(currJointFullName);
			char* currJointName = NULL;
			char* temp = strtok_s(cpy, ":", &currJointName);

			std::string currJointString(currJointName);
			currJointString.append("Bone");
			//~ convert name

			FbxAMatrix TransformMatrix;
			FbxAMatrix TransformLinkMatrix;
			FbxAMatrix GlobalBindPoseInverseMatrix;

			FbxTransform ClusterTransform = currCluster->GetLink()->GetTransform();
			FbxAMatrix ClusterGlobalTransform = currCluster->GetLink()->EvaluateGlobalTransform();	
			FbxVector4 ClusterTranslation = ClusterGlobalTransform.GetT();
			FbxQuaternion ClusterQuaternion = ClusterGlobalTransform.GetQ();

			BoneInfo& currBoneInfo = InSkeletonInfo.GetTargetBoneInfo(currJointName);
			// free(cpy);

			int currBoneIdx = currBoneInfo.Index;

			// Pass Translation and Quaternion
			currBoneInfo.Transform = Vector3(ClusterTranslation.mData[0], ClusterTranslation.mData[1], ClusterTranslation.mData[2]);
			currBoneInfo.Quat = Quaternion(ClusterQuaternion.mData[0], ClusterQuaternion.mData[1], ClusterQuaternion.mData[2], ClusterQuaternion.mData[3]);
			
			//	Not Used for Now - 추후 gpu Library 올릴 때 사용
			//currCluster->GetTransformMatrix(TransformMatrix);
			//currCluster->GetTransformLinkMatrix(TransformLinkMatrix);
			//GlobalBindPoseInverseMatrix = TransformLinkMatrix.Inverse() * TransformMatrix * GeometryTransform;

			// Update Informations to Joint (Skeleton vector)
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();

			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				//Blending ClusterIndex - WeightPair
				double currWeight = currCluster->GetControlPointWeights()[i];

				const std::vector<int>& targetVertices = viToVertices[currCluster->GetControlPointIndices()[i]];
				for (auto targetVertice : targetVertices)
				{
					InWeightInfo[targetVertice].push_back(std::make_pair(currJointString, currWeight));
				}
			}
		}
	}
	// ~ Getting Rig
}

void FBXLoader::LoadSkeleton(FbxNode* InRootBoneNode, OUT SkeletonInfo& InSkeletonInfo)
{
	assert(InRootBoneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);
	
	GetBoneInfoRecursive(OUT InSkeletonInfo, InRootBoneNode, 0, 0, -1);
}

void FBXLoader::GetBoneInfoRecursive(OUT SkeletonInfo& InSkeletonInfo, FbxNode* InNode, int Depth, int MyIndex, int ParentIndex)
{
	assert(InNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);

	const char* currJointFullName = InNode->GetName();
	char* cpy = strdup(currJointFullName);

	char* currJointName = NULL;
	char* temp = strtok_s(cpy, ":", &currJointName);
	InSkeletonInfo.BoneInfoVector.push_back(BoneInfo(InNode, MyIndex, ParentIndex, currJointName));
	//free(cpy);

	for (int childIndex = 0; childIndex < InNode->GetChildCount(); ++childIndex)
	{
		FbxNode* childNode = InNode->GetChild(childIndex);
		GetBoneInfoRecursive(InSkeletonInfo, childNode, Depth + 1, InSkeletonInfo.BoneInfoVector.size(), MyIndex);
	}
}

FbxAMatrix FBXLoader::GetGeometryTransformation(FbxNode* InNode)
{
	assert(InNode);

	const FbxVector4 lT = InNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = InNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = InNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

// No-Use Function
void FBXLoader::LoadAnimation()
{
	//LoadAnimationWithName("Salute");
}

void FBXLoader::LoadAnimationWithName(const std::string& AnimationName, const std::vector<std::string>& SKBones, OUT std::vector<std::string>& BoneNames, OUT std::vector<bool>& BoneUsage, OUT std::vector<std::vector<Vector3>>& FTranslations, OUT std::vector <std::vector<Quaternion>>& FRotations)
{
	assert(_FbxManager);
	const std::string InFilePath = AssetFolderPath + "Character/Animation/" + AnimationName + ".fbx";

	OutputDebugString("LoadAnim With Name Called \n");

	const char* filepath = InFilePath.c_str();

	FbxIOSettings* ios = FbxIOSettings::Create(_FbxManager, IOSROOT);
	_FbxManager->SetIOSettings(ios);
	FbxImporter* importer = FbxImporter::Create(_FbxManager, "");

	bool status = importer->Initialize(filepath, -1, _FbxManager->GetIOSettings());

	assert(status);

	FbxScene* scene = FbxScene::Create(_FbxManager, "scene");
	importer->Import(scene);

	importer->Destroy();

	FbxAnimStack* TargetAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
	auto AnimStackName = TargetAnimStack->GetName();

	FbxTakeInfo* takeInfo = scene->GetTakeInfo(AnimStackName);
	FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime endTime = takeInfo->mLocalTimeSpan.GetStop();
	
	FbxLongLong timeStartIndex = startTime.GetFrameCount(FbxTime::eFrames24);
	FbxLongLong timeEndIndex = endTime.GetFrameCount(FbxTime::eFrames24);

	FbxNode* sceneRoot = scene->GetRootNode();
	const int childCount = sceneRoot->GetChildCount();

	FbxNode* SkeletonNode = nullptr;

	for (unsigned int i = 0; i < childCount; ++i)
	{
		FbxNode* childNode = sceneRoot->GetChild(i);
		FbxNodeAttribute* childNodeAttribute = childNode->GetNodeAttribute();

		if (childNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			SkeletonNode = childNode;
		}

	}

	assert(SkeletonNode);

	GetBoneAnimationRecursive(SkeletonNode, timeStartIndex, timeEndIndex, SKBones, BoneNames, BoneUsage, FTranslations, FRotations);

}

void FBXLoader::GetBoneAnimationRecursive(FbxNode* InNode, const FbxLongLong& InStartIndex, const FbxLongLong& InEndIndex, const std::vector<std::string>& SKBones, OUT std::vector<std::string>& BoneNames, OUT std::vector<bool>& BoneUsage, OUT std::vector<std::vector<Vector3>>& FTranslations, OUT std::vector <std::vector<Quaternion>>& FRotations)
{
	const char* currClusterName = InNode->GetName();
	std::string CutString = std::string(currClusterName).substr(10).append("Bone");
	
	int SKIndex = -1;
	for (auto i = 0; i < SKBones.size(); ++i)
	{
		if (CutString.compare(SKBones[i]) == 0)
		{
			SKIndex = i;
			BoneUsage[i] = true;
			break;
		}
	}
	assert(SKIndex >= 0 && SKIndex < SKBones.size());

	BoneNames[SKIndex] = CutString;
	int FrameSize = InEndIndex - InStartIndex + 1;

	FTranslations[SKIndex].resize(FrameSize);
	FRotations[SKIndex].resize(FrameSize);

	for (FbxLongLong i = InStartIndex; i <= InEndIndex; ++i)
	{
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eFrames24);
		FbxAMatrix currentGlobalTransformOffset = InNode->EvaluateGlobalTransform(currTime);
		FbxAMatrix currentLocalTransformOffset = InNode->EvaluateLocalTransform(currTime);

		FbxVector4 LClusterTranslation = currentLocalTransformOffset.GetT();
		FbxVector4 LClusterRotation = currentLocalTransformOffset.GetR();
		FbxQuaternion LClusterQuat = currentLocalTransformOffset.GetQ();
		
		FTranslations[SKIndex][i] = Vector3(LClusterTranslation.mData[0], LClusterTranslation.mData[1], LClusterTranslation.mData[2]);
		FRotations[SKIndex][i] = Quaternion(LClusterQuat.mData[0], LClusterQuat.mData[1], LClusterQuat.mData[2], LClusterQuat.mData[3]);
	}

	for (int childIndex = 0; childIndex < InNode->GetChildCount(); ++childIndex)
	{
		FbxNode* childNode = InNode->GetChild(childIndex);
		GetBoneAnimationRecursive(childNode, InStartIndex, InEndIndex, SKBones, BoneNames, BoneUsage, FTranslations, FRotations);
	}
}

Vector2 FBXLoader::ReadUV(FbxMesh* InMesh, int ControllPointIndex, int VertexCounter)
{
	Vector2 Result;
	FbxGeometryElementUV* vertexUV = InMesh->GetElementUV(0);
	int idx(-1);

	switch (vertexUV->GetMappingMode())
	{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (vertexUV->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				idx = ControllPointIndex;
				break;
			case FbxGeometryElement::eIndexToDirect:
				idx = vertexUV->GetIndexArray().GetAt(ControllPointIndex);
				break;
			default:
				assert(false);
			}
			break;
		}

		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (vertexUV->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				idx = VertexCounter;
				break;
			case FbxGeometryElement::eIndexToDirect: // HERE
				idx = VertexCounter;
				//idx = vertexUV->GetIndexArray().GetAt(VertexCounter);
				break;
			default:
				assert(false);
			}
			break;
		}

		default:
		{
			assert(false);
		}
	}

	assert(idx >= 0);
	Result.X = static_cast<float>(vertexUV->GetDirectArray().GetAt(idx).mData[0]);
	Result.Y = static_cast<float>(vertexUV->GetDirectArray().GetAt(idx).mData[1]);
	return Result;
}



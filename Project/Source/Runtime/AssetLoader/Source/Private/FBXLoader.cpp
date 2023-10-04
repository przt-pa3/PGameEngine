
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



void FBXLoader::LoadCharacterFBX(const std::string& AssetName, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT std::string& TexturePath)
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

	FbxNode* rootNode = scene->GetRootNode();

	const int childCount = rootNode->GetChildCount();
	unsigned int MeshVertexIndex = 0;

	// TODO..
	std::vector<BoneInfo> Skeleton;

	for (auto i = 0; i < childCount; ++i)
	{
		FbxNode* childNode = rootNode->GetChild(i);
		FbxNodeAttribute* childAttribute = childNode->GetNodeAttribute();
		if (childAttribute)
		{
			if (childAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
			{
				LoadSkeleton(childNode, OUT Skeleton);
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
					LoadMesh(childNode, OUT MeshVertexIndex, OUT Vertices, OUT Indices, OUT Uvs, OUT Skeleton);
				}

			}
		}
	}

}

void FBXLoader::LoadMesh(FbxNode* InNode, OUT unsigned int& StartVIndex, OUT std::vector<Vector3>& Vertices, OUT std::vector<size_t>& Indices, OUT std::vector<Vector2>& Uvs, OUT std::vector<BoneInfo>& InSkeleton)
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

	// Getting Rig
	unsigned int numOfDeformers = ConvertedMesh->GetDeformerCount();
	FbxAMatrix GeometryTransform = GetGeometryTransformation(InNode);

	for (unsigned int DeformerIndex = 0; DeformerIndex < numOfDeformers; ++DeformerIndex)
	{
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(ConvertedMesh->GetDeformer(DeformerIndex, FbxDeformer::eSkin));
		if (!currSkin) continue;

		unsigned int numOfClusters = currSkin->GetClusterCount();

		for (unsigned int ClusterIndex = 0; ClusterIndex < numOfClusters; ++ClusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(ClusterIndex);
			const char* currJointName = currCluster->GetLink()->GetName();

			FbxAMatrix TransformMatrix;
			FbxAMatrix TransformLinkMatrix;
			FbxAMatrix GlobalBindPoseInverseMatrix;

// TODO : Do Something With Bones
			FbxTransform ClusterTransform = currCluster->GetLink()->GetTransform();
			FbxAMatrix ClusterLocalTransform = currCluster->GetLink()->EvaluateLocalTransform();
			FbxAMatrix ClusterGlobalTransform = currCluster->GetLink()->EvaluateGlobalTransform();

			FbxVector4 ClusterTranslation = ClusterLocalTransform.GetT();
			FbxQuaternion ClusterRotation = ClusterLocalTransform.GetQ();
			FbxVector4 ClusterScale = ClusterLocalTransform.GetS();
// ~ Bones

			currCluster->GetTransformMatrix(TransformMatrix);
			currCluster->GetTransformLinkMatrix(TransformLinkMatrix);
			GlobalBindPoseInverseMatrix = TransformLinkMatrix.Inverse() * TransformMatrix * GeometryTransform;

			unsigned int currJointIndex = FindBoneIndexByName(InSkeleton, currJointName);

			// Update Informations to Joint (Skeleton vector)
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();

			char buffer[100];
			sprintf(buffer, "JointName : %s, Index : %d, numOfIndicies : %d \n", currJointName, currJointIndex, numOfIndices);
			OutputDebugString(buffer);

			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				//BlendingIndexWeightPair
				currJointIndex;
				double currWeight = currCluster->GetControlPointWeights()[i];

				//char buffer2[100];
				//sprintf(buffer2, "JointName : %s -> Affect Index : %d (weight : %f) \n", currJointName, currCluster->GetControlPointIndices()[i], currWeight);
				//OutputDebugString(buffer2);
			}
		}
	}
	// ~ Getting Rig

	unsigned int PointCount = ConvertedMesh->GetControlPointsCount();
	unsigned int PolyCount = ConvertedMesh->GetPolygonCount();
	FbxGeometryElementUV* vUV = ConvertedMesh->GetElementUV(0);
	
	unsigned int VertexCount = StartVIndex;

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
			
			Vector2 VertexUV;
			VertexUV = ReadUV(ConvertedMesh, vi, ConvertedMesh->GetTextureUVIndex(p, v));
			Uvs.push_back(VertexUV);

			VertexCount++;
		}
	}

	StartVIndex = VertexCount;
}



void FBXLoader::LoadSkeleton(FbxNode* InRootBoneNode, OUT std::vector<BoneInfo>& InSkeleton)
{
	assert(InRootBoneNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);
	
	//Skeleton.push_back(BoneInfo(InRootBoneNode));
	GetBoneInfoRecursive(OUT InSkeleton, InRootBoneNode, 0, 0, -1);
}

void FBXLoader::GetBoneInfoRecursive(OUT std::vector<BoneInfo>& InSkeleton, FbxNode* InNode, int Depth, int MyIndex, int ParentIndex)
{
	assert(InNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton);

	//char buffer[100];
	//auto MyName = InNode->GetName();
	//sprintf(buffer, "%s : Depth %d, Index %d, Parent %d \n", MyName, Depth, MyIndex, ParentIndex);
	//OutputDebugString(buffer);
	
	InSkeleton.push_back(BoneInfo(InNode, MyIndex, ParentIndex));

	for (int childIndex = 0; childIndex < InNode->GetChildCount(); ++childIndex)
	{
		FbxNode* childNode = InNode->GetChild(childIndex);
		GetBoneInfoRecursive(InSkeleton, childNode, Depth + 1, InSkeleton.size(), MyIndex);
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

unsigned int FBXLoader::FindBoneIndexByName(const std::vector<BoneInfo>& InSkeleton, const char* BoneName) const
{
	for (unsigned int i = 0; i < InSkeleton.size(); ++i)
	{
		if (InSkeleton[i].Name == BoneName)
		{
			return i;
		}
	}

	assert(false);
	return 0;
}

void FBXLoader::LoadAnimation()
{

}

void FBXLoader::LoadAnimationWithName(const std::string& AnimationName)
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

	GetBoneAnimationRecursive(SkeletonNode, timeStartIndex, timeEndIndex);

}

void FBXLoader::GetBoneAnimationRecursive(FbxNode* InNode, const FbxLongLong& InStartIndex, const FbxLongLong& InEndIndex)
{
	//FbxAMatrix currentTransformOffset = InNode->EvaluateGlobalTransform(InTime);

	for (FbxLongLong i = InStartIndex; i <= InEndIndex; ++i)
	{
		FbxTime currTime;
		currTime.SetFrame(i, FbxTime::eFrames24);
		FbxAMatrix currentGlobalTransformOffset = InNode->EvaluateGlobalTransform(currTime);
		FbxAMatrix currentLocalTransformOffset = InNode->EvaluateLocalTransform(currTime);

		FbxVector4 ClusterTranslation = currentLocalTransformOffset.GetT();
		FbxQuaternion ClusterRotation = currentLocalTransformOffset.GetQ();
		FbxVector4 ClusterScale = currentLocalTransformOffset.GetS();
	}

	for (int childIndex = 0; childIndex < InNode->GetChildCount(); ++childIndex)
	{
		FbxNode* childNode = InNode->GetChild(childIndex);
		GetBoneAnimationRecursive(childNode, InStartIndex, InEndIndex);
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



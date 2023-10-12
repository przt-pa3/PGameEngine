#include "Precompiled.h"
#include "SoftRenderer.h"
#include <random>

using namespace PGE::DDD;

const std::string MainPlayer("Player");
const std::string MainCameraTarget("MainCameraTarget");
const std::string SimpleCube("SimpleCube");

void SoftRenderer::LoadScene()
{
	GameEngine& g = GetDirectGameEngine();

	// Making Character
	GameObject& goPlayer = g.CreateNewGameObjectOfClass(MainPlayer, "Character");
	constexpr float playerScale = 1.0f;

	goPlayer.SetMesh(GameEngine::CharacterMesh);
	goPlayer.SetColor(LinearColor::White);
	goPlayer.GetTransform().SetWorldScale(Vector3::One * playerScale);

	// Making Camera
	GameObject& goCameraTarget = g.CreateNewGameObject(MainCameraTarget);
	goCameraTarget.GetTransform().SetWorldPosition(Vector3(0.0f, 0.0f, 0.0f));
	goCameraTarget.SetParent(goPlayer);

	CameraObject& mainCamera = g.GetMainCamera();
	mainCamera.GetTransform().SetWorldPosition(Vector3(-100.0f, 100.0f, 100.0f));
	mainCamera.SetLookAtRotation(goCameraTarget);

	return;
}

void SoftRenderer::LateUpdate(float InDeltaSeconds)
{

}

void SoftRenderer::Render()
{

	RenderWorld();
	RenderUI();

}

void SoftRenderer::RenderWorld()
{
	const GameEngine& g = GetDirectGameEngine();
	auto& r = GetRenderer();
	const CameraObject& mainCamera = g.GetMainCamera();

	const Matrix4x4 pvMatrix = mainCamera.GetPerspectiveViewMatrix();

	size_t totalObjects = g.GetScene().size();
	size_t culledObjects = 0;
	size_t intersectedObjects = 0;
	size_t renderedObjects = 0;

	for (auto it = g.SceneBegin(); it != g.SceneEnd(); ++it)
	{
		const GameObject& gameObject = *(*it);
		if (!gameObject.HasMesh() || !gameObject.IsVisible())
		{
			continue;
		}

		// 렌더링에 필요한 게임 오브젝트의 주요 레퍼런스를 얻기
		const Mesh& mesh = g.GetMesh(gameObject.GetMeshKey());
		const TransformComponent& transform = gameObject.GetTransform();

		// 최종 행렬 계산
		Matrix4x4 finalMatrix = pvMatrix * transform.GetWorldMatrix();
		LinearColor finalColor = gameObject.GetColor();

		// 최종 변환 행렬로부터 평면의 방정식과 절두체 생성
		Matrix4x4 finalTransposedMatrix = finalMatrix.Transpose();
		std::array<Plane, 6> frustumPlanesFromMatrix = {
			Plane(-(finalTransposedMatrix[3] - finalTransposedMatrix[1])), // up
			Plane(-(finalTransposedMatrix[3] + finalTransposedMatrix[1])), // bottom
			Plane(-(finalTransposedMatrix[3] - finalTransposedMatrix[0])), // right
			Plane(-(finalTransposedMatrix[3] + finalTransposedMatrix[0])), // left 
			Plane(-(finalTransposedMatrix[3] - finalTransposedMatrix[2])),  // far
			Plane(-(finalTransposedMatrix[3] + finalTransposedMatrix[2])), // near
		};
		Frustum frustumFromMatrix(frustumPlanesFromMatrix);

		// 바운딩 영역을 사용해 절두체 컬링을 구현
		Box boxBound = mesh.GetBoxBound();
		auto checkResult = frustumFromMatrix.CheckBound(boxBound);
		if (checkResult == BoundCheckResult::Outside)
		{
			culledObjects++;
			continue;
		}
		else if (checkResult == BoundCheckResult::Intersect)
		{
			// 겹친 게임 오브젝트를 통계에 포함
			intersectedObjects++;
		}

		// 스키닝이고 WireFrame인 경우 본을 그리기
		if (mesh.IsSKMesh() && IsWireframeDrawing())
		{
			const Mesh& boneMesh = g.GetMesh(GameEngine::ArrowMesh);
			Mesh& currMesh = const_cast<Mesh&>(mesh);
			SKMesh& currSKMesh = dynamic_cast<SKMesh&>(currMesh);

			if (currSKMesh.GetBones().size() > 0)
			{
				for (const auto& b : currSKMesh.GetBones())
				{
					if (!b.second.HasParent())
					{
						continue;
					}
					const Bone& bone = b.second;

					// Skip Drawing Root Bone
					if (bone.GetName().compare("HipsBone") == 0)
					{
						continue;
					}

					const Bone& parentBone = currSKMesh.GetBone(bone.GetParentName());
					const Transform& tGameObject = transform.GetWorldTransform();

					// 모델링 공간에서의 본의 위치
					const Transform& t1 = parentBone.GetTransform().GetWorldTransform();
					const Transform& t2 = bone.GetTransform().GetWorldTransform();

					// 게임 월드 공간에서의 본의 위치
					const Transform& wt1 = t1.LocalToWorld(tGameObject);
					const Transform& wt2 = t2.LocalToWorld(tGameObject);

					Vector3 boneVector = wt2.GetPosition() - wt1.GetPosition();
					Transform tboneObject(wt1.GetPosition(), Quaternion(boneVector), Vector3(10.f, 10.f, boneVector.Size()));
					Matrix4x4 boneMatrix = pvMatrix * tboneObject.GetMatrix();

					

					DrawMesh3D(boneMesh, boneMatrix, _BoneWireframeColor);
				}
			}
			
		}

		DrawMesh3D(mesh, finalMatrix, gameObject.GetColor());

		renderedObjects++;

	}

	r.PushStatisticText("Camera Position : " + mainCamera.GetTransform().GetWorldTransform().GetPosition().ToString());
	r.PushStatisticText("Camera Rotation : " + mainCamera.GetTransform().GetLocalRotation().ToString());
}

void SoftRenderer::DrawMesh3D(const DDD::Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor)
{

	size_t vertexCount = InMesh.GetVertices().size();
	size_t indexCount = InMesh.GetIndices().size();
	size_t triangleCount = indexCount / 3;

	// 렌더러가 사용할 정점 버퍼와 인덱스 버퍼로 변환
	std::vector<Vertex3D> vertices(vertexCount);
	std::vector<size_t> indice(InMesh.GetIndices());
	for (size_t vi = 0; vi < vertexCount; ++vi)
	{
		vertices[vi].Position = Vector4(InMesh.GetVertices()[vi]);

		// TODO : should Change Casting
		if (InMesh.IsSKMesh())
		{
			Vector4 totalPosition = Vector4::Zero;
			Weight w = static_cast<const DDD::SKMesh&>(InMesh).GetWeights()[vi];
			for (size_t wi = 0; wi < static_cast<const DDD::SKMesh&>(InMesh).GetConnectedBones()[vi]; ++wi)
			{
				std::string boneName = w.Bones[wi];
				if (static_cast<const DDD::SKMesh&>(InMesh).HasBone(boneName))
				{
					const Transform& bindPoseTransform = static_cast<const DDD::SKMesh&>(InMesh).GetBindPose(boneName);
					const Transform& boneTransform = static_cast<const DDD::SKMesh&>(InMesh).GetBone(boneName).GetTransform().GetWorldTransform();
					Vector4 localPosition = boneTransform.GetMatrix() * bindPoseTransform.Inverse().GetMatrix() * vertices[vi].Position;

					totalPosition += localPosition * w.Values[wi];
				}
			}

			vertices[vi].Position = totalPosition;

		}

		if (InMesh.HasColor())
		{
			vertices[vi].Color = InMesh.GetColors()[vi];
		}

		if (InMesh.HasUV())
		{
			vertices[vi].UV = InMesh.GetUVs()[vi];
		}
	}

	// 정점 변환 진행
	VertexShader3D(vertices, InMatrix);

	// 삼각형 별로 그리기
	for (int ti = 0; ti < triangleCount; ++ti)
	{
		int bi0 = ti * 3, bi1 = ti * 3 + 1, bi2 = ti * 3 + 2;
		std::vector<Vertex3D> tvs = { vertices[indice[bi0]] , vertices[indice[bi1]] , vertices[indice[bi2]] };

		// 동차좌표계에서 클리핑을 위한 설정
		std::vector<PerspectiveTest> testPlanes = {
			{ TestFuncW0, EdgeFuncW0 },
			{ TestFuncNY, EdgeFuncNY },
			{ TestFuncPY, EdgeFuncPY },
			{ TestFuncNX, EdgeFuncNX },
			{ TestFuncPX, EdgeFuncPX },
			{ TestFuncFar, EdgeFuncFar },
			{ TestFuncNear, EdgeFuncNear }
		};

		// 동차좌표계에서 클리핑 진행
		for (auto& p : testPlanes)
		{
			p.ClipTriangles(tvs);
		}

		size_t triangles = tvs.size() / 3;
		for (size_t ti = 0; ti < triangles; ++ti)
		{
			size_t si = ti * 3;
			std::vector<Vertex3D> sub(tvs.begin() + si, tvs.begin() + si + 3);
			DrawTriangle3D(sub, InColor, FillMode::Color);
		}
	}
}

void SoftRenderer::DrawTriangle3D(std::vector<DDD::Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode)
{
	auto& r = GetRenderer();
	const GameEngine& g = GetDirectGameEngine();
	const CameraObject& mainCamera = g.GetMainCamera();

	// 카메라의 근평면과 원평면 값
	float n = mainCamera.GetNearZ();
	float f = mainCamera.GetFarZ();

	// 클립 좌표를 NDC 좌표로 변경
	for (auto& v : InVertices)
	{
		// 무한 원점인 경우, 약간 보정해준다.
		if (v.Position.W == 0.f) v.Position.W = SMALL_NUMBER;

		float invW = 1.f / v.Position.W;
		v.Position.X *= invW;
		v.Position.Y *= invW;
		v.Position.Z *= invW;
	}

	// 백페이스 컬링
	Vector3 edge1 = (InVertices[1].Position - InVertices[0].Position).ToVector3();
	Vector3 edge2 = (InVertices[2].Position - InVertices[0].Position).ToVector3();
	Vector3 faceNormal = -edge1.Cross(edge2);
	Vector3 viewDirection = Vector3::UnitZ;
	if (faceNormal.Dot(viewDirection) >= 0.f)
	{
		return;
	}

	// NDC 좌표를 화면 좌표로 늘리기
	for (auto& v : InVertices)
	{
		v.Position.X *= _ScreenSize.X * 0.5f;
		v.Position.Y *= _ScreenSize.Y * 0.5f;
	}

	if (IsWireframeDrawing())
	{
		LinearColor finalColor = _WireframeColor;
		if (InColor == _BoneWireframeColor)
		{
			finalColor = InColor;
		}

		r.DrawLine(InVertices[0].Position, InVertices[1].Position, finalColor);
		r.DrawLine(InVertices[0].Position, InVertices[2].Position, finalColor);
		r.DrawLine(InVertices[1].Position, InVertices[2].Position, finalColor);
	}
	else
	{
		// TODO : 다른 Mesh의 InVertice 면 다른 Texture 불러와서 그리도록 
		//const Texture& mainTexture = g.GetTexture(GameEngine::BaseTexture);
		const Texture& mainTexture = g.GetTexture(GameEngine::CharacterTexture);

		// 삼각형 칠하기
		// 삼각형의 영역 설정
		Vector2 minPos(Math::Min3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Min3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));
		Vector2 maxPos(Math::Max3(InVertices[0].Position.X, InVertices[1].Position.X, InVertices[2].Position.X), Math::Max3(InVertices[0].Position.Y, InVertices[1].Position.Y, InVertices[2].Position.Y));

		// 무게중심좌표를 위해 점을 벡터로 변환
		Vector2 u = InVertices[1].Position.ToVector2() - InVertices[0].Position.ToVector2();
		Vector2 v = InVertices[2].Position.ToVector2() - InVertices[0].Position.ToVector2();

		// 공통 분모 값 ( uu * vv - uv * uv )
		float udotv = u.Dot(v);
		float vdotv = v.Dot(v);
		float udotu = u.Dot(u);
		float denominator = udotv * udotv - vdotv * udotu;

		// 퇴화 삼각형 판정.
		if (denominator == 0.f)
		{
			return;
		}

		float invDenominator = 1.f / denominator;

		ScreenPoint lowerLeftPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, minPos);
		ScreenPoint upperRightPoint = ScreenPoint::ToScreenCoordinate(_ScreenSize, maxPos);

		// 두 점이 화면 밖을 벗어나는 경우 클리핑 처리
		lowerLeftPoint.X = Math::Max(0, lowerLeftPoint.X);
		lowerLeftPoint.Y = Math::Min(_ScreenSize.Y, lowerLeftPoint.Y);
		upperRightPoint.X = Math::Min(_ScreenSize.X, upperRightPoint.X);
		upperRightPoint.Y = Math::Max(0, upperRightPoint.Y);

		// 각 정점마다 보존된 뷰 공간의 z값
		float invZ0 = 1.f / InVertices[0].Position.W;
		float invZ1 = 1.f / InVertices[1].Position.W;
		float invZ2 = 1.f / InVertices[2].Position.W;

		// 삼각형 영역 내 모든 점을 점검하고 색칠
		for (int x = lowerLeftPoint.X; x <= upperRightPoint.X; ++x)
		{
			for (int y = upperRightPoint.Y; y <= lowerLeftPoint.Y; ++y)
			{
				ScreenPoint fragment = ScreenPoint(x, y);
				Vector2 pointToTest = fragment.ToCartesianCoordinate(_ScreenSize);
				Vector2 w = pointToTest - InVertices[0].Position.ToVector2();
				float wdotu = w.Dot(u);
				float wdotv = w.Dot(v);

				float s = (wdotv * udotv - wdotu * vdotv) * invDenominator;
				float t = (wdotu * udotv - wdotv * udotu) * invDenominator;
				float oneMinusST = 1.f - s - t;

				// 투영보정에 사용할 공통 분모
				float z = invZ0 * oneMinusST + invZ1 * s + invZ2 * t;
				float invZ = 1.f / z;

				if (((s >= 0.f) && (s <= 1.f)) && ((t >= 0.f) && (t <= 1.f)) && ((oneMinusST >= 0.f) && (oneMinusST <= 1.f)))
				{
					// 깊이 테스팅
					float newDepth = InVertices[0].Position.Z * oneMinusST + InVertices[1].Position.Z * s + InVertices[2].Position.Z * t;
					float prevDepth = r.GetDepthBufferValue(fragment);
					if (newDepth < prevDepth)
					{
						// 픽셀을 처리하기 전 깊이 값을 버퍼에 보관
						r.SetDepthBufferValue(fragment, newDepth);
					}
					else
					{
						// 이미 앞에 무언가 그려져있으므로 픽셀그리기는 생략
						continue;
					}

					if (IsDepthBufferDrawing())
					{
						// 카메라로부터의 거리에 따라 균일하게 증감하는 흑백 값으로 변환
						float grayScale = (invZ - n) / (f - n);

						// 뎁스 버퍼 그리기
						r.DrawPoint(fragment, LinearColor::White * grayScale);
					}
					else
					{
						// 최종 보정보간된 UV 좌표
						Vector2 targetUV = (InVertices[0].UV * oneMinusST * invZ0 + InVertices[1].UV * s * invZ1 + InVertices[2].UV * t * invZ2) * invZ;
						r.DrawPoint(fragment, FragmentShader3D(mainTexture.GetSample(targetUV), InColor));
					}
				}
			}
		}
	}
}

void SoftRenderer::RenderUI()
{
	auto& r = GetRenderer();

	
	r.PushStatisticText("ElapsedTime : " + std::to_string(_ElapsedTime));
	UpdateLogs();
}

void SoftRenderer::UpdateLogs()
{
	auto& l = GetGameEngine().GetLogs();
	auto& r = GetRenderer();
	
	std::vector<std::pair<std::size_t, std::string>> LogsToUpdate;
	std::vector<std::size_t> LogHashToDelete;
	
	for (auto& IndLog : l)
	{
		IndLog.UpdateLogs(GetElapsedTime(), OUT LogsToUpdate, OUT LogHashToDelete);
	}
	
	r.DeleteLogTexts(LogHashToDelete);
	r.PushLogTexts(LogsToUpdate);
}

void SoftRenderer::TestPermLog()
{
	auto& l = GetGameEngine().GetLogs()[0];
	l.PushLog("PermLogTest", true);
}

void SoftRenderer::TestTempLog()
{
	auto& l = GetGameEngine().GetLogs()[0];
	l.PushLog("TempLogTest", 5000.0f);
}

void SoftRenderer::TestFunc()
{
	auto& g = GetDirectGameEngine();
}


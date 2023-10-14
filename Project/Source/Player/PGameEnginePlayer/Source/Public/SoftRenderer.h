#pragma once

enum class DrawMode : UINT32
{
	Normal = 0,
	Wireframe,
	DepthBuffer,
	OnlyBone
};

enum class FillMode : UINT32
{
	None = 0x00,
	Color = 0x01,
	Texture = 0x02
};

FORCEINLINE FillMode operator|(FillMode InLhs, FillMode InRhs)
{
	return static_cast<FillMode> (
		static_cast<std::underlying_type<FillMode>::type>(InLhs) |
		static_cast<std::underlying_type<FillMode>::type>(InRhs)
		);
}

FORCEINLINE FillMode& operator|=(FillMode& InLhs, FillMode InRhs)
{
	InLhs = InLhs | InRhs;
	return InLhs;
}

FORCEINLINE bool operator&(FillMode InLhs, FillMode InRhs)
{
	BYTE rhsValue = static_cast<std::underlying_type<FillMode>::type>(InRhs);
	return ((static_cast<std::underlying_type<FillMode>::type>(InLhs) & rhsValue) == rhsValue);
}

class SoftRenderer
{
public:
	// ������
	SoftRenderer(RendererInterface* InRSI);

	// ������ �̺�Ʈ ó��
	void OnInit();
	void OnTick();
	void OnResize(const ScreenPoint& InNewScreenSize);
	void OnShutdown();

	// ���α׷� �⺻ ����
	SystemInputManager& GetSystemInput() { return _SystemInputManager; }
	const ScreenPoint& GetScreenSize() { return _ScreenSize; }
	float GetFrameFPS() const { return _FrameFPS; }
	FORCEINLINE float GetElapsedTime() const { return _ElapsedTime; }

	// ���� ����
	std::function<float()> _PerformanceInitFunc;
	std::function<INT64()> _PerformanceMeasureFunc;
	std::function<void(InputManager&)> _InputBindingFunc;

	// ���� ���� ���۷��� 
	FORCEINLINE EngineInterface& GetGameEngine() { return static_cast<EngineInterface&>(_GameEngine); }
	FORCEINLINE DDD::GameEngine& GetDirectGameEngine() { return _GameEngine; }
private:
	bool InitSubsystems();

	// �⺻ ���� �Լ�
	void PreUpdate();
	void Update(float InDeltaSeconds);
	void LateUpdate(float InDeltaSeconds);
	void PostRender();

	// ������ ���۷���
	FORCEINLINE RendererInterface& GetRenderer() { return *_RSIPtr.get(); }
	FORCEINLINE void SetBackgroundColor(const LinearColor& InLinearColor) { _BackgroundColor = InLinearColor; }
	FORCEINLINE void SetWireframeColor(const LinearColor& InLinearColor) { _WireframeColor = InLinearColor; }

	// Graphics
	void LoadScene();
	void Render();

	//void DrawMesh2D(const class DD::Mesh& InMesh, const Matrix3x3& InMatrix, const LinearColor& InColor);
	//void DrawTriangle2D(std::vector<DD::Vertex2D>& InVertices, const LinearColor& InColor, FillMode InFillMode);

	void DrawMesh3D(const class DDD::Mesh& InMesh, const Matrix4x4& InMatrix, const LinearColor& InColor);
	void DrawTriangle3D(std::vector<DDD::Vertex3D>& InVertices, const LinearColor& InColor, FillMode InFillMode);

	void RenderWorld();
	void RenderUI();

	// Drawing Modes
	bool IsDepthBufferDrawing() const { return _CurrentDrawMode == DrawMode::DepthBuffer; }
	bool IsWireframeDrawing() const { return _CurrentDrawMode == DrawMode::Wireframe; }
	bool IsOnlyBoneDrawing() const { return _CurrentDrawMode == DrawMode::OnlyBone; }
	DrawMode GetDrawMode() const { return _CurrentDrawMode; }
	void SetDrawMode(DrawMode InDrawMode) { _CurrentDrawMode = InDrawMode; }
	DrawMode _CurrentDrawMode = DrawMode::Normal;

	// Log
	void UpdateLogs();
	void TestPermLog();
	void TestTempLog();
	void TestFunc();

	void PushLog(const std::string& LogString);
	void PushLog(const std::string& LogHeader, const std::string& LogString);
	void PushLog(const std::string& LogHeader, const std::string& LogString, bool IsOnScreen);
	void PushLog(const std::string& LogHeader, const std::string& LogString, float LifeTime);

private:
	// �ʱ�ȭ ���� ����
	bool _PerformanceCheckInitialized = false;
	bool _RendererInitialized = false;
	bool _GameEngineInitialized = false;
	bool _TickEnabled = false;
	bool _AllInitialized = false;

	// ȭ�� ũ��
	ScreenPoint _ScreenSize;

	// ��� ����
	LinearColor _BackgroundColor = LinearColor::WhiteSmoke;

	// ���̾������� ����
	LinearColor _WireframeColor = LinearColor::DimGray;

	// ���� �׸� ����
	LinearColor _BoneWireframeColor = LinearColor::Red;

	// ���� ����
	long long _StartTimeStamp = 0;
	long long _FrameTimeStamp = 0;
	long _FrameCount = 0;
	float _CyclesPerMilliSeconds = 0.f;
	float _FrameTime = 0.f;
	float _ElapsedTime = 0.f;
	float _AverageFPS = 0.f;
	float _FrameFPS = 0.f;

	// ������ �������̽�
	std::unique_ptr<RendererInterface> _RSIPtr;

	// ���� ����
	DDD::GameEngine _GameEngine;

	// ���� ���α׷� �Է�
	SystemInputManager _SystemInputManager;
};
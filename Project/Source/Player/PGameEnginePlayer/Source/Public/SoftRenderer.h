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
	// 생성자
	SoftRenderer(RendererInterface* InRSI);

	// 윈도우 이벤트 처리
	void OnInit();
	void OnTick();
	void OnResize(const ScreenPoint& InNewScreenSize);
	void OnShutdown();

	// 프로그램 기본 정보
	SystemInputManager& GetSystemInput() { return _SystemInputManager; }
	const ScreenPoint& GetScreenSize() { return _ScreenSize; }
	float GetFrameFPS() const { return _FrameFPS; }
	FORCEINLINE float GetElapsedTime() const { return _ElapsedTime; }

	// 성능 측정
	std::function<float()> _PerformanceInitFunc;
	std::function<INT64()> _PerformanceMeasureFunc;
	std::function<void(InputManager&)> _InputBindingFunc;

	// 게임 엔진 레퍼런스 
	FORCEINLINE EngineInterface& GetGameEngine() { return static_cast<EngineInterface&>(_GameEngine); }
	FORCEINLINE DDD::GameEngine& GetDirectGameEngine() { return _GameEngine; }
private:
	bool InitSubsystems();

	// 기본 루프 함수
	void PreUpdate();
	void Update(float InDeltaSeconds);
	void LateUpdate(float InDeltaSeconds);
	void PostRender();

	// 렌더러 레퍼런스
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
	// 초기화 점검 변수
	bool _PerformanceCheckInitialized = false;
	bool _RendererInitialized = false;
	bool _GameEngineInitialized = false;
	bool _TickEnabled = false;
	bool _AllInitialized = false;

	// 화면 크기
	ScreenPoint _ScreenSize;

	// 배경 색상
	LinearColor _BackgroundColor = LinearColor::WhiteSmoke;

	// 와이어프레임 색상
	LinearColor _WireframeColor = LinearColor::DimGray;

	// 본을 그릴 색상
	LinearColor _BoneWireframeColor = LinearColor::Red;

	// 성능 측정
	long long _StartTimeStamp = 0;
	long long _FrameTimeStamp = 0;
	long _FrameCount = 0;
	float _CyclesPerMilliSeconds = 0.f;
	float _FrameTime = 0.f;
	float _ElapsedTime = 0.f;
	float _AverageFPS = 0.f;
	float _FrameFPS = 0.f;

	// 렌더러 인터페이스
	std::unique_ptr<RendererInterface> _RSIPtr;

	// 게임 엔진
	DDD::GameEngine _GameEngine;

	// 응용 프로그램 입력
	SystemInputManager _SystemInputManager;
};
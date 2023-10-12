#include "Precompiled.h"
#include "SoftRenderer.h"

using namespace PGE::DDD;

SoftRenderer::SoftRenderer(RendererInterface* InRSI) : _RSIPtr(InRSI)
{
}

void SoftRenderer::OnInit()
{
	::MessageBox(nullptr, "Begin to Tick", "SoftRenderer Init Finished", MB_OK);
}

void SoftRenderer::OnTick()
{
	if (!_AllInitialized)
	{
		InitSubsystems();
		return;
	}

	// Check Condition + TickEnabled
	assert(_RSIPtr != nullptr && GetRenderer().IsInitialized() && !_ScreenSize.HasZero());

	if (_TickEnabled)
	{
		PreUpdate();

		Update(_FrameTime / 1000.0f);
		LateUpdate(_FrameTime / 1000.0f);
		Render();

		PostUpdate();
	}
}

void SoftRenderer::OnResize(const ScreenPoint& InNewScreenSize)
{
	_ScreenSize = InNewScreenSize;

	if (_RendererInitialized)
	{
		GetRenderer().Init(_ScreenSize);
	}

	if (_GameEngineInitialized)
	{
		GetGameEngine().OnScreenResize(_ScreenSize);
	}
}

void SoftRenderer::OnShutdown()
{
	GetRenderer().Shutdown();
}

bool SoftRenderer::InitSubsystems()
{
	// Check Screen Size
	if (_ScreenSize.HasZero())
	{
		::MessageBox(nullptr, "ScreenSize HasZero", "Error", MB_OK);
		return false;
	}

	// Init Performance counter
	if (!_PerformanceCheckInitialized)
	{
		if (_PerformanceInitFunc && _PerformanceMeasureFunc)
		{
			_CyclesPerMilliSeconds = _PerformanceInitFunc();
			_PerformanceCheckInitialized = true;
		}
		else
		{
			assert(false);
			return false;
		}
	}

	if (!_RendererInitialized)
	{
		_RendererInitialized = GetRenderer().Init(_ScreenSize);
		if (!_RendererInitialized)
		{
			assert(false);
			return false;
		}
	}

	_GameEngineInitialized = GetGameEngine().IsInitialized();
	if (!_GameEngineInitialized)
	{
		GetGameEngine().OnScreenResize(_ScreenSize);
		InputManager& input = GetGameEngine().GetInputManager();
		if (!input.IsInputReady())
		{
			_InputBindingFunc(input);
		}

		_GameEngineInitialized = GetGameEngine().Init();
		if (!_GameEngineInitialized)
		{
			assert(false);
			return false;
		}

		LoadScene();
	}

	_AllInitialized = _PerformanceCheckInitialized && _RendererInitialized && _GameEngineInitialized;

	if (_AllInitialized)
	{
		_TickEnabled = true;
		OnInit();
	}

	return _TickEnabled;
}

void SoftRenderer::PreUpdate()
{
	// Performance Check
	_FrameTimeStamp = _PerformanceMeasureFunc();
	if (_FrameCount == 0)
	{
		_StartTimeStamp = _FrameTimeStamp;
	}

	GetRenderer().Clear(_BackgroundColor);

	const SystemInputManager& sinput = GetSystemInput();

	if (sinput.IsReleased(SystemInputButton::F1)) { SetDrawMode(DrawMode::Normal); }
	if (sinput.IsReleased(SystemInputButton::F2)) { SetDrawMode(DrawMode::Wireframe); }
	if (sinput.IsReleased(SystemInputButton::F8)) { TestPermLog(); }
	if (sinput.IsReleased(SystemInputButton::F9)) { TestTempLog(); }
	if (sinput.IsReleased(SystemInputButton::F7)) { TestFunc(); }
}

void SoftRenderer::Update(float InDeltaSeconds)
{
	GameEngine& g = GetDirectGameEngine();
	const InputManager& input = g.GetInputManager();


	float CameraRotateSpeed = 180.0f;
	float CameraMoveSpeed = 500.0f;

	CameraObject& camera = g.GetMainCamera();

	// TODO - Improve Camera Rotation
	camera.GetTransform().AddLocalYawRotation(-input.GetAxis(InputAxis::XAxis) * CameraRotateSpeed * InDeltaSeconds);
	camera.GetTransform().AddLocalPitchRotation(-input.GetAxis(InputAxis::YAxis) * CameraRotateSpeed * InDeltaSeconds);
	Vector3 CameraMovement(input.GetAxis(InputAxis::X2Axis) * CameraMoveSpeed * InDeltaSeconds, input.GetAxis(InputAxis::Y2Axis) * CameraMoveSpeed * InDeltaSeconds, input.GetAxis(InputAxis::Z2Axis) * CameraMoveSpeed * InDeltaSeconds);
	camera.GetTransform().AddWorldPosition(CameraMovement);

	if (input.IsReleased(InputButton::Z))
	{
		// TODO - Do Something
		auto& g = GetGameEngine();
		auto& l = g.GetLogs()[0];
		l.PushLog("Input :: Z", 5000.0f);

		g.InterfaceTestFunction();

		
	}
}

void SoftRenderer::PostUpdate()
{
	GetRenderer().EndFrame();

	// 입력 상태 업데이트
	GetGameEngine().GetInputManager().UpdateInput();
	GetSystemInput().UpdateSystemInput();

	// 성능 측정 마무리.
	_FrameCount++;
	INT64 currentTimeStamp = _PerformanceMeasureFunc();
	INT64 frameCycles = currentTimeStamp - _FrameTimeStamp;
	INT64 elapsedCycles = currentTimeStamp - _StartTimeStamp;
	_FrameTime = frameCycles / _CyclesPerMilliSeconds;
	_ElapsedTime = elapsedCycles / _CyclesPerMilliSeconds;
	_FrameFPS = _FrameTime == 0.f ? 0.f : 1000.f / _FrameTime;
	_AverageFPS = _ElapsedTime == 0.f ? 0.f : 1000.f / _ElapsedTime * _FrameCount;
}

void SoftRenderer::PushLog(const std::string& LogString)
{
	auto& g = GetGameEngine();
	auto& l = g.GetLogs()[0];

	l.PushLog(LogString, 5000.0f);
}

void SoftRenderer::PushLog(const std::string& LogHeader, const std::string& LogString)
{
	auto& g = GetGameEngine();
	auto& l = g.GetTargetLog(LogString);

	l.PushLog(LogString, 5000.0f);
}

void SoftRenderer::PushLog(const std::string& LogHeader, const std::string& LogString, bool IsOnScreen)
{
	auto& g = GetGameEngine();
	auto& l = g.GetTargetLog(LogString);

	l.PushLog(LogString, IsOnScreen);
}

void SoftRenderer::PushLog(const std::string& LogHeader, const std::string& LogString, float LifeTime)
{
	auto& g = GetGameEngine();
	auto& l = g.GetTargetLog(LogHeader);

	l.PushLog(LogString, LifeTime);
}


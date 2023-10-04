#pragma once

#if defined(PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include "Res/resource.h"

#endif

#include <sstream>

#include "MathHeaders.h"
#include "AssetLoaderHeaders.h"
#include "EngineHeaders.h"
#include "RendererHeaders.h"
#include "SystemInputManager.h"

using namespace PGE;
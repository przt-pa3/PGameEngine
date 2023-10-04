#pragma once

#include <functional>

//#include "2D/Vertex.h"
//#include "2D/Shader.h"
#include "Graphics/Vertex.h"
#include "Graphics/Shader.h"
#include "Graphics/PerspectiveTest.h"

#include "RendererInterface.h"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include "Windows/WindowsGDI.h"
#include "Windows/WindowsRSI.h"
#endif

using namespace PGE;
#include "Precompiled.h"

#define STB_IMAGE_IMPLEMENTATION
#pragma warning( push )
#pragma warning( disable : 26541 )
#include "Stb/stb_image.h"
#pragma warning( pop )

void Texture::Release()
{
	_Width = 0;
	_Height = 0;
	_BaseColor.clear();
	_Normal.clear();
	_Specular.clear();
}

void PGE::Texture::LoadFromPath(const std::string& InFilePath)
{
	std::string TargetFilePath = InFilePath + "diffuse.png";

	FILE* f = NULL;
	if (0 != fopen_s(&f, TargetFilePath.c_str(), "rb"))
	{
		return;
	}
	LoadFromFile(f);
}

void PGE::Texture::LoadFromFile(FILE* InFilePtr)
{
	if (InFilePtr == NULL)
	{
		return;
	}

	Release();

	int width = 0, height = 0, channel = 0;
	stbi_uc* pixelsPtr = stbi_load_from_file(InFilePtr, &width, &height, &channel, STBI_rgb_alpha);
	if (pixelsPtr == NULL)
	{
		return;
	}

	_Width = static_cast<UINT32>(width);
	_Height = static_cast<UINT32>(height);
	size_t pixelNumbers = static_cast<size_t>(_Width) * static_cast<size_t>(_Height);
	_BaseColor.reserve(pixelNumbers);
	for (size_t j = 0; j < _Height; j++)
	{
		for (size_t i = 0; i < _Width; i++)
		{
			size_t ix = (j * _Width + i) * 4;
			Color32 c(pixelsPtr[ix], pixelsPtr[ix + 1], pixelsPtr[ix + 2], pixelsPtr[ix + 3]);
			_BaseColor.push_back(LinearColor(c));
		}
	}

	OutputDebugString("Texture Load Completed \n");
	char buffer[50];
	sprintf(buffer, "Texture %d x %d \n", _Width, _Height);
	OutputDebugString(buffer);
	return;
}

LinearColor Texture::GetSample(Vector2 InUV) const
{
	if (!IsInitialized())
	{
		return LinearColor::Error;
	}

	int x = Math::FloorToInt(InUV.X * _Width) % _Width;
	int y = Math::FloorToInt(InUV.Y * _Height) % _Height;
	int index = _Width * (_Height - (1 + y)) + x;

	if (index >= _BaseColor.size())
	{
		return LinearColor::Error;
	}

	return _BaseColor[index];
}

#pragma once

namespace PGE
{

class Texture
{
public:
	Texture() = default;
	~Texture() { }

public:
	void Release();
	void LoadFromPath(const std::string& InFilePath);
	void LoadFromFile(FILE* InFilePtr);
	bool IsInitialized() const { return (_BaseColor.size() > 0); }
	UINT32 GetWidth() const { return _Width; }
	UINT32 GetHeight() const { return _Height; }
	UINT32 GetSize() const { return _Width * _Height; }
	LinearColor GetSample(Vector2 InUV) const;
	TextureFilterMode GetMagFilterMode() const { return _MagFilter; }
	TextureFilterMode GetMinFilterMode() const { return _MinFilter; }
	TextureAddressMode GetAddressMode() const { return _AddressMode; }
	bool IsAnisotropyEnabled() const { return _AnisotropyEnable; }
	float GetMaxAnisotropy() const { return _MaxAnisotropy; }

	std::vector<LinearColor>& GetBaseColor() { return _BaseColor; }
	std::vector<LinearColor>& GetNormal() { return _Normal; }
	std::vector<float>& GetSpecular() { return _Specular; }

private:
	static constexpr BYTE _Channel = 4;
	std::vector<LinearColor> _BaseColor;
	std::vector<LinearColor> _Normal;
	std::vector<float> _Specular;

	UINT32 _Width = 0;
	UINT32 _Height = 0;

	TextureFilterMode _MagFilter = TextureFilterMode::Nearest;
	TextureFilterMode _MinFilter = TextureFilterMode::Nearest;
	TextureAddressMode _AddressMode = TextureAddressMode::Repeat;
	bool _AnisotropyEnable = false;
	float _MaxAnisotropy = 16.0f;
};

}
#pragma once

class SerializerIO;

namespace Tool
{
	enum class ResolutionType : int
	{
		R2560x1440, 
		R1920x1080,
		R1600x900,
		R1280x720,
		R1024x768,
		R800x600
	};

	class Config
	{
	public:
		Config();
		~Config();

		static void SetResolution(ResolutionType type) noexcept;
		static ResolutionType GetResolution() noexcept;
		static XMUINT2 GetResolutionInCoordinate() noexcept;

		void ProcessIO(SerializerIO& serializer);

	private:
		static ResolutionType m_resolutionType;
		static Config* m_Instance;
	};
}

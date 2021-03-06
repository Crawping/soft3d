#pragma once

namespace soft3d
{

	class Texture
	{
	public:
		Texture();
		~Texture();
		void CopyFromBuffer(const uint32* buf, int width, int height);
		Color Sampler2D(const vmath::vec2* uv) const;

		enum FILTER_MODE
		{
			NEAREST,
			BILINEAR,
		};
		FILTER_MODE filter_mode = BILINEAR;

	private:
		uint32* m_data;
		uint16 m_width;
		uint16 m_height;

		Color Sampler2D_nearest(const vmath::vec2* uv) const;
		Color Sampler2D_bilinear(const vmath::vec2* uv) const;
	};

}
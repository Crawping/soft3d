#pragma once
#include <memory>
#include <algorithm>
#include "vmath.h"

namespace soft3d
{

	typedef unsigned short uint16;
	typedef unsigned int uint32;

	inline void uC2fC(uint32 color, vmath::vec4* colorf)
	{
		(*colorf)[0] = ((color & 0xff0000) >> 16) / 255.0f;
		(*colorf)[1] = ((color & 0xff00) >> 8) / 255.0f;
		(*colorf)[2] = (color & 0xff) / 255.0f;
		(*colorf)[3] = (color & 0xff000000) / 255.0f;
	}

	inline void uC2fC(uint32 color, vmath::vec3* colorf)
	{
		(*colorf)[0] = ((color & 0xff0000) >> 16) / 255.0f;
		(*colorf)[1] = ((color & 0xff00) >> 8) / 255.0f;
		(*colorf)[2] = (color & 0xff) / 255.0f;
	}

	inline uint32 fC2uC(const vmath::vec4& colorf)
	{
		//return 0xfff;
		uint32 color = 0;
		color += (uint32)(0xff * colorf[2]) << 16;
		color += (uint32)(0xff * colorf[1]) << 8;
		color += (uint32)(0xff * colorf[0]);
		color += (uint32)(0xf0) << 24;

		return color;
	}

	inline uint32 fC2uC(const vmath::vec3& colorf)
	{
		//return 0xfff;
		uint32 color = 0;
		color += (uint32)(0xff * colorf[2]) << 16;
		color += (uint32)(0xff * colorf[1]) << 8;
		color += (uint32)(0xff * colorf[0]);
		color += (uint32)(0xf0) << 24;

		return color;
	}

	inline uint32 colorMultiFloat(uint32 color, float ratio)
	{
		uint32 cc = 0;
		cc += uint32((color & 0xff0000) * ratio) & 0xff0000;
		cc += uint32((color & 0xff00) * ratio) & 0xff00;
		cc += uint32((color & 0xff) * ratio) & 0xff;
		return cc;
	}

	struct Color
	{
		unsigned char B;
		unsigned char G;
		unsigned char R;
		unsigned char A;

		Color()
		{
			*((uint32*)this) = 0;
		}

		Color(uint32 color)
		{
			memcpy(this, &color, sizeof(uint32));
		}

		void operator=(uint32 color)
		{
			memcpy(this, &color, sizeof(uint32));
		}

		operator uint32() const
		{
			return *((uint32*)(this));
		}

		inline Color& operator*(const vmath::vec3* v)
		{
			vmath::vec3 color;
			uC2fC(*this, &color);

			color[0] *= (*v)[0];
			color[1] *= (*v)[1];
			color[2] *= (*v)[2];

			this->R = std::min<float>(color[0], 1.0f) * 255;
			this->G = std::min<float>(color[1], 1.0f) * 255;
			this->B = std::min<float>(color[2], 1.0f) * 255;
			return *this;
		}
	};

	inline Color operator*(const Color& lf, float ratio)
	{
		Color cc;
		cc.B = (unsigned char)(lf.B * ratio);
		cc.G = (unsigned char)(lf.G * ratio);
		cc.R = (unsigned char)(lf.R * ratio);
		cc.A = (unsigned char)(lf.A * ratio);
		return cc;
	}

	inline Color operator+(const Color& lf, const Color& rf)
	{
		Color cc;
		cc.B = lf.B + rf.B;
		cc.G = lf.G + rf.G;
		cc.R = lf.R + rf.R;
		cc.A = lf.A + rf.A;
		return cc;
	}

}
#include "Soft3dPipeline.h"
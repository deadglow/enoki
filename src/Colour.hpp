#pragma once
#include "CommonTypes.hpp"
#include "Math.h"

namespace Enoki
{
	struct Colour
	{
		enum Channel : uint8
		{
			C_RED,
			C_GREEN,
			C_BLUE,
			C_ALPHA
		};

		inline Colour FromVec4(const vec4_t& floatColour)
		{
			return Colour {
				.r = (uint8)(floatColour.x * 255),
				.g = (uint8)(floatColour.y * 255),
				.b = (uint8)(floatColour.z * 255),
				.a = (uint8)(floatColour.w * 255),
			};
		}

		inline Colour FromPacked(const uint32 colour)
		{
			return Colour {.packed = colour};
		}

		inline vec4_t GetVec4() const
		{
			return vec4(GetChannel<float>(C_RED),
			            GetChannel<float>(C_GREEN),
			            GetChannel<float>(C_BLUE),
			            GetChannel<float>(C_ALPHA));
		}

		template <typename T>
		inline T GetChannel(const Channel channel) const;

		template <>
		inline uint32 GetChannel<uint32>(const Channel channel) const
		{
			return channels[channel];
		}

		template <>
		inline float GetChannel<float>(const Channel channel) const
		{
			return (float)channels[channel] / 255.f;
		}

		union
		{
			struct
			{
				uint8 r;
				uint8 g;
				uint8 b;
				uint8 a;
			};
			uint8 channels[4];
			uint32 packed;
		};
	};
} // namespace Enoki
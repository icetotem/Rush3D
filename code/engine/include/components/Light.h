#pragma once

namespace rush
{

	enum class LightType
	{
		Directional,
		Point,
		Spot,
	};

	struct Light
	{
		LightType Type;

	};

}
#pragma once

#include <DirectXMath.h>

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHTY_TYPE_SPOT			2

struct Light
{
	int Type;						//Which type of light
	DirectX::XMFLOAT3 Direction;	//directional/spot lights have direction
	float Range;					// Point/Spot have max range
	DirectX::XMFLOAT3 Position;		//Point/spot light position in space
	float Intensity;				//All lights - light intensity
	DirectX::XMFLOAT3 Color;		//All lights - color
	float SpotFalloff;				//Spot lights - value to restrict cone
	DirectX::XMFLOAT3 Padding;		//Purposefully padding to hit 16-byte boundary
};

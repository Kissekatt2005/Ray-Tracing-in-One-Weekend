#pragma once
#include<random>
#include "Vector3.h"

class random
{
public:
	inline float random_float()
	{
		static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		static std::mt19937 generator;
		return distribution(generator);
	}

	inline vec3 random_in_unit_sphere()
	{
		vec3 p;
		do {
			p = 2.0 * vec3(random_float(), random_float(), random_float()) - vec3(1, 1, 1);
		} while (p.squared_lenght() >= 1.0);
		return p;
	}
};


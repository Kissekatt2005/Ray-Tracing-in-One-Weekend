#pragma once
#ifndef CAMERAH
#define CAMERAH
#include "Ray.h"
#include <math.h>

class camera
{
public:
	camera(vec3 lookfrom, vec3 lookat, vec3 vup,float vfov, float aspect) { // vfov is top to bottom in degrees
		vec3 u, v, w;
		float theta = static_cast<float>(vfov * std::_Pi_val / 180);
		float half_height = std::tan(theta / 2);
		float half_width = aspect * half_height;
		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup,w));
		v = cross(w, u);
		lower_left_corner = vec3(-half_width, -half_height, -1.f);
		lower_left_corner = origin - half_width * u - half_height * v - w;
		horizontal = 2 * half_width * u;
		vertical = 2 * half_height * v;
	}
	ray get_rey(float u, float v) { return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin); }

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};

#endif 

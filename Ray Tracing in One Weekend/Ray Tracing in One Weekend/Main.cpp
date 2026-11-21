#include "pch.h"
#include <iostream>
#include <fstream>
#include "Sphere.h"
#include "Random.h"
#include"Hitable_list.h"
#include"Camera.h"
#include"float.h"
#include "Material.h"



vec3 color(const ray& r, hitable* world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return vec3(0.f, 0.f, 0.f);
		}
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f * (unit_direction.y() + 1);
		return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}

}


int main() {
	int nx = 200;
	int ny = 100;
	int ns = 100;

	std::ofstream out("image.ppm");
	out << "P3\n" << nx << " " << ny << "\n255\n";

	constexpr int list_size = 4;
	hitable* list[list_size];
	list[0] = new sphere(vec3(0.f, 0.f, -1.f), 0.5f, new lambertian(vec3(0.8f,0.3f,0.3f)));
	list[1] = new sphere(vec3(0.f, -100.5f, -1.f), 100.f, new lambertian(vec3(0.8f,0.8f,0.0f)));
	list[2] = new sphere(vec3(1.f, 0.f, -1.f), 0.5f, new metal(vec3(0.8f,0.6f,0.2f)));
	list[3] = new sphere(vec3(-1.f, 0.f, -1.f), 0.5f, new metal(vec3(0.8f, 0.8f, 0.8f)));
	hitable* world = new hitable_list(list, list_size);
	camera cam;
	random rand;

	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			vec3 col = vec3(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + rand.random_float()) / float(nx);
				float v = float(j + rand.random_float()) / float(ny);
				ray r = cam.get_rey(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}

			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);

			out << ir << " " << ig << " " << ib << "\n";
		}
	}

	out.close();
	return 0;
}
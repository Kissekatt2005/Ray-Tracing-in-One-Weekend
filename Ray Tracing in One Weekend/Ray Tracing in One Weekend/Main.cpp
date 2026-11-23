#include "pch.h"
#include <iostream>
#include <fstream>
#include "Sphere.h"
#include "Random.h"
#include"Hitable_list.h"
#include"Camera.h"
#include"float.h"
#include "Material.h"
#include <chrono>
#include <thread>
#include <array>
#include<vector>

constexpr int nx = 800;
constexpr int ny = 400;
constexpr int ns = 100;

using Image = std::array<vec3, nx* ny>;   // image[row][column]
static Image image;

hitable* random_scene()
{
	random rand;
	int n = 500;
	hitable** list = new hitable * [n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5f, 0.5f, 0.5f)));
	int i = 1;
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float choose_mat = rand.random_float();
			vec3 center(a + 0.9f * rand.random_float(), 0.2f, b + 0.9f * rand.random_float());
			if ((center - vec3(4.f, 0.2f, 0.f)).lenght() > 0.9f)
			{
				if (choose_mat < 0.8f)
				{
					list[i++] = new sphere(center, 0.2f, new lambertian(vec3(rand.random_float() * rand.random_float(), rand.random_float() * rand.random_float(), rand.random_float() * rand.random_float())));
				}
				else if (choose_mat < 0.95f)
				{
					list[i++] = new sphere(center, 0.2f,
						new metal(vec3(0.5f * (1 + rand.random_float()), 0.5f * (1 + rand.random_float()), 0.5f * (1 + rand.random_float())), 0.5f * rand.random_float()));
				}
				else
				{
					list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
				}
			}

		}
	}

	list[i++] = new sphere(vec3(0.f, 1.f, 0.f), 1.f, new dielectric(1.5f));
	list[i++] = new sphere(vec3(-4.f, 1.f, 0.f), 1.f, new lambertian(vec3(0.4f, 0.2f, 0.1f)));
	list[i++] = new sphere(vec3(4.f, 1.f, 0.f), 1.f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));

	return new hitable_list(list, i);
}

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

void compute(const int id, const int thredCount, hitable*& in_world, const camera& cam, Image& img)
{
	random rand;
	camera camera = cam;
	//hitable* world = in_world;
	int loop_zize = (nx * ny) / thredCount;
	int start_index = loop_zize * id;

	for (int j = start_index + loop_zize - 1; j >= start_index; j--) {
		int x = j % nx;
		int y = j / nx;

		vec3 col = vec3(0, 0, 0);
		for (int s = 0; s < ns; s++)
		{
			float u = float(x + rand.random_float()) / float(nx);
			float v = 1.0f - (float(y + rand.random_float()) / float(ny));
			ray r = camera.get_rey(u, v);
			vec3 p = r.point_at_parameter(2.0);
			col += color(r, in_world, 0);
		}

		col /= float(ns);
		col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

		img[j] = col;

	}
}

int main() {
	auto start = std::chrono::steady_clock::now();

	std::ofstream out("image.ppm");
	out << "P3\n" << nx << " " << ny << "\n255\n";


	hitable* world = random_scene();
	vec3 lookfrom(13.0f, 2.0f, 3.0f);
	vec3 lookat(0.0f, 0.0f, 0.0f);
	constexpr float dist_to_focus = 10.0f;
	constexpr float aperture = 0.1f;
	camera cam(lookfrom, lookat, vec3(0.0f, 1.0f, 0.0f), 20.0f,
		float(nx) / float(ny), aperture, dist_to_focus);

	int threadCount = std::thread::hardware_concurrency();
	if (threadCount == 0) threadCount = 4;

	std::vector<std::jthread> threads;
	for (int t = 0; t < threadCount; t++)
		threads.emplace_back(compute, t, threadCount, std::ref(world), std::cref(cam), std::ref(image));


	// Wait for all threads to finish
	for (auto& thread : threads)
	{
		thread.join();
	}

	for (size_t i = 0; i < image.size(); i++)
	{
		out << int(255.99f * image[i].x()) << " "
			<< int(255.99f * image[i].y()) << " "
			<< int(255.99f * image[i].z()) << "\n";
	}

	out.close();

	auto end = std::chrono::steady_clock::now();
	double seconds = std::chrono::duration<double>(end - start).count();

	int minutes = int(seconds) / 60;
	std::cout << "Render time: " << minutes << " min "
		<< (seconds - minutes * 60) << " s\n";
	return 0;
}
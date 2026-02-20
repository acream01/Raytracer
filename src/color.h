#ifndef COLOR_H
#define COLOR_H

#include <vector>
#include <cstdint>

#include "interval.h"
#include "vec3.h"


using color = vec3;

inline double linear_to_gamma(double linear_component) {
	if (linear_component > 0)
		return std::sqrt(linear_component);

	return 0;
}

void write_color(std::vector<uint8_t>& pixels, const color& pixel_color, int x, int y, int img_width) {
	//write color pixel data to an allocated pixels vector
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	//Apply a linear to gamma transform for gamma 2
	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	// Translate the [0,1] component values top the byte range [0, 255]
	static const interval intensity(0.000, 0.999);

	int rbyte = int(256 * intensity.clamp(r));
	int gbyte = int(256 * intensity.clamp(g));
	int bbyte = int(256 * intensity.clamp(b));


	// Write out the pixel color components
	//out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
	//pixels.push_back(static_cast<uint8_t>(rbyte));
	//pixels.push_back(static_cast<uint8_t>(gbyte));
	//pixels.push_back(static_cast<uint8_t>(bbyte));
	
	//Write to correct part of the image for the thread
	int index = (y * img_width + x) * 3;
	
	pixels[index + 0] = static_cast<uint8_t>(rbyte);
	pixels[index + 1] = static_cast<uint8_t>(gbyte);
	pixels[index + 2] = static_cast<uint8_t>(bbyte);
	 
}

#endif
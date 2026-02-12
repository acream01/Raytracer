#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>
color ray_color(const ray& r) {
    return color(0, 0, 0);
}


int main() {
     
    //Define Image
    auto aspect_ratio = 16.0 / 9.0;
    int img_width = 400;
    // Calculate image height and ensure its at least one
    int img_height = int (img_width / aspect_ratio);
    img_height = (img_height < 1) ? 1 : img_height;

    //Camera

    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(img_width) / img_height);
    auto camera_center = point3(0, 0, 0);

    //Calculate the vectors across image space, horizantal and verticle viewport edges
    auto viewport_u = vec3(viewport_width, 0, 0);
    auto viewport_v = vec3(0, -viewport_height, 0);

    //Calculate the horizontal and vertical delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / img_width;
    auto pixel_delta_v = viewport_v / img_height;

    //Calculate the location of the upper left pixel
    auto viewport_upper_left = camera_center
        - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);



    //Render

    std::cout << "P3\n" << img_width << ' ' << img_height << "\n255\n";

    for (int j = 0; j < img_height; j++) {
        std::clog << "\rScanlines Remaining: " << (img_height - j) << ' ' << std::flush;
        for (int i= 0; i < img_width; i++){
            auto pixel_center = pixel100_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }

    }

    std::clog << "\rDone!                       \n";
}
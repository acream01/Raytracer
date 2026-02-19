#ifndef CAMERA_H
#define CAMERA_H

#include "vector"
#include "thread"       // for multithreading
#include <cstdint>      // for uint8_t
#include <iostream>     // for std::clog


#include "color.h"
#include "hittable.h"
#include "material.h"
#include "ray.h"
#include <chrono>

class camera {
public:
    //Define Image
    double aspect_ratio = 1.0;
    int img_width = 100;
    int samples_per_pixel = 10;
    int max_depth = 10; //max number of ray bounces into scene

    double vfov = 90; //Verticle field of view
    point3 lookfrom = point3(0, 0, 0); //Cam pos
    point3 lookat = point3(0, 0, -1); 
    vec3 up = vec3(0, 1, 0);

    double defocus_angle = 0; //Variation angle of rays through each pixel
    double focus_dist = 10; //Distance form camera look from point to plane of perfect focus



    void render_rows(int y_start, int y_end, const hittable& world, std::vector<uint8_t>& pixels) {
        //Render specified rows for multithreading/parralelization
        for (int y = y_start; y < y_end; ++y) {
            for (int x = 0; x < img_width; ++x) {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(x, y);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(pixels, pixel_samples_scale * pixel_color, x, y, img_width);
            }
        }
    }

	void render(const hittable& world, char* outfile) {
        //Start render timer
        auto start_time = std::chrono::steady_clock::now();

        //Render Loop
        initialize();
        std::vector<uint8_t> pixels(img_height * img_width * 3);
        
        
        int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        int rows_per_thread = img_height / num_threads; 

        for (int i = 0; i < num_threads; i++) {
            int y_start = i * rows_per_thread;
            int y_end = (i == num_threads - 1) ? img_height : y_start + rows_per_thread;
            threads.emplace_back([this, y_start, y_end, &world, &pixels]() {
                render_rows(y_start, y_end, world, pixels);
                });

        }
        //render_rows(0, img_height, world, pixels);

        for (auto& t : threads) t.join();

        //Output into render directory
        std::string outdirectory = "out/Renders/" + std::string(outfile);

        stbi_write_png(outdirectory.c_str(), img_width, img_height, 3, pixels.data(), img_width * 3);
        std::clog << "\rDone!                       \n";
        
        //Tracks render time and prints
        auto end_time = std::chrono::steady_clock::now();
        print_render_time(end_time - start_time);
	}


private:
    int img_height;
    double pixel_samples_scale; //Color scale factor for a sum of pixel samples
    point3 center;          //Camera Center
    point3 pixel100_loc; //0,0
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w; //Camera frame Basis vectors
    vec3 defocus_disk_u; //Defocus disk horizontal radius
    vec3 defocus_disk_v; //Defocus disk vertical radius



	void initialize() {
        // Calculate image height and ensure its at least one
        img_height = int(img_width / aspect_ratio);
        img_height = (img_height < 1) ? 1 : img_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        //Viewport Vars
        //auto focal_length = (lookfrom - lookat).length();
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(img_width) / img_height);
	    
        //Calculate the u,v,w unit basis vectors for the camera coordinate frame
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(up, w));
        v = cross(w, u);

        
        //Calculate the vectors across image space, horizantal and verticle viewport edges
        auto viewport_u = viewport_width * u; //Vector accross viewport horizontal edge
        auto viewport_v = viewport_height * -v; //Vector down viewport verticle edge


        //Calculate the horizontal and vertical delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / img_width;
        pixel_delta_v = viewport_v / img_height;

        //Calculate the location of the upper left pixel
        auto viewport_upper_left = center
            - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        //Construct a camera ray originating from the origin and directed at randomly sampled
        //point around the pixel location i, j

        auto offset = sample_square();
        auto pixel_sample = pixel100_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const{
        // Returns the vector to a random point in the [-0.5, -0.5] - [+0.5, +0.5] unit square
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        // Return a random point in the camera defocus disk
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        //If we exceed the ray bounce limit no more light is gathered
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;
        
        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    void print_render_time(std::chrono::duration<double> duration) {
        double remainder = duration.count();
        std::cout << "Render time :";
        
        //Seperate hours
        int hours = (int)remainder / (3600);
        if (hours > 0)
            std::cout << ' ' << hours << " hour";
        if (hours > 1)
            std::cout << "s";
        remainder = remainder - 3600 * hours;

        //Seperate minutes
        int minutes = (int)remainder / 60;
        if (minutes > 0)
            std::cout << ' ' << minutes << " minute";
        if (minutes > 1)
            std::cout << "s";
        remainder = remainder - 60 * minutes;
        
        //Print remainding seconds
        std::cout << ' ' << remainder << " seconds" << std::endl;
    }
};

#endif
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
    color background;   //Scene background color

    double vfov = 90; //Verticle field of view
    point3 lookfrom = point3(0, 0, 0); //Cam pos
    point3 lookat = point3(0, 0, -1); 
    vec3 up = vec3(0, 1, 0);

    double atmos_perspective = 0.0;

    double defocus_angle = 0; //Variation angle of rays through each pixel
    double focus_dist = 10; //Distance form camera look from point to plane of perfect focus



    void render_rows(int y_start, int y_end, const hittable& world, std::vector<uint8_t>& pixels, std::atomic_int& rows_done) {
        //Render specified rows for multithreading/parralelization
        for (int y = y_start; y < y_end; ++y) {
            for (int x = 0; x < img_width; ++x) {
                color pixel_color(0, 0, 0);
                for (int s_j = 0; s_j < sqrt_spp; s_j++) { //Stratified samples
                    for (int s_i = 0; s_i < sqrt_spp; s_i++) { //Stratified samples
                        ray r = get_ray(x, y, s_i, s_j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                }

            write_color(pixels, pixel_samples_scale * pixel_color, x, y, img_width);
            }
            rows_done++;
            std::clog << '\r' << "Rows Remaining: " << img_height - rows_done << "              " << std::flush;
        }
    }

	void render(const hittable& world, char* outfile) {
        //Start render timer
        auto start_time = std::chrono::steady_clock::now();

        //Render Loop
        std::clog << "Starting Render..." << std::flush;
        initialize();
        std::atomic_int rows_done = 0; //Counter reported to by all threads
        std::vector<uint8_t> pixels(img_height * img_width * 3); //Pixel Buffer 
        
        
        int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        int rows_per_thread = img_height / num_threads; 

        for (int i = 0; i < num_threads; i++) {
            int y_start = i * rows_per_thread;
            int y_end = (i == num_threads - 1) ? img_height : y_start + rows_per_thread;
            threads.emplace_back([this, y_start, y_end, &world, &pixels, &rows_done]() {
                render_rows(y_start, y_end, world, pixels, rows_done);
                });

        }
        //render_rows(0, img_height, world, pixels);

        for (auto& t : threads) {
            t.join();
            num_threads--;
            //std::clog << '\r' << num_threads << " threads remaining to return" << std::flush;
        }

        //Output into render directory
        std::string outdirectory = "out/Renders/" + std::string(outfile);

        stbi_write_png(outdirectory.c_str(), img_width, img_height, 3, pixels.data(), img_width * 3);
        std::clog << "\rDone!                                \n";
        std::cout << "Render Output To ./" << outdirectory << '\n';
        
        //Tracks render time and prints
        auto end_time = std::chrono::steady_clock::now();
        print_render_time(end_time - start_time);
	}


private:
    int img_height;
    double pixel_samples_scale; //Color scale factor for a sum of pixel samples
    int sqrt_spp; //Square root of samples per pixel
    double recip_sqrt_spp; //1 / sqrt_spp

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

        sqrt_spp = int(std::sqrt(samples_per_pixel));
        pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
        recip_sqrt_spp = 1.0 / sqrt_spp;

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

    ray get_ray(int i, int j, int s_i, int s_j) const {
        //Construct a camera ray originating from the origin and directed at randomly sampled
        //point around the pixel location i, j for stratified sample square s_i, s_j

        auto offset = sample_square_stratified(s_i, s_j);
        auto pixel_sample = pixel100_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double(); //Rays are taken accross the course of a second [0s - 1s] by random distribution

        return ray(ray_origin, ray_direction, ray_time);
    }

    vec3 sample_square_stratified(int s_i, int s_j) const {
        //Returns the vector to a random point in the square sub-pixel specified by grid
        //indices s_i and s_j, for an idealized unit square pixel [-0.5, -0.5] to [+0.5, +0.5]

        auto px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5;
        auto py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5;

        return vec3(px, py, 0);
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
        
    //If the ray hits nothing, return then background color.
        if (!world.hit(r, interval(0.001, infinity), rec)) 
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
        color color_from_scatter;
        double scatterChance = random_double() * (rec.p.length() - r.origin().length());
        
        if (this->atmos_perspective && (scatterChance / this->atmos_perspective > 0.5)){
            // atmos_perspective is the distance where there is a 50% scatter chance
            //If ray travels the atmospheric perspective distance have a chance to scatter
           atmos_perspective_scatter(r, ((rec.p - r.origin())/2), attenuation, scattered);
                
            // It just needs a bigger chance the farther it is to scatter randomly along its path
            color_from_scatter = ray_color(scattered, depth - 1, world);
        } else {
        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;      

        color_from_scatter = attenuation * ray_color(scattered, depth-1, world);
        }
        
        return color_from_emission + color_from_scatter;
    }

    bool atmos_perspective_scatter(const ray& r_in, const point3 p, color& attenuation, ray& scattered)
        const {
        vec3 debris_normal = vec3(0, 1, 0);
        auto scatter_direction = debris_normal + random_unit_vector();
        
        //Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = debris_normal;

        scattered = ray(p, debris_normal, r_in.time());
        //attenuation = ;
        return true;
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
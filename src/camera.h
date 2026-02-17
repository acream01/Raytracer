#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

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



	void render(const hittable& world) {
        initialize();

        std::cout << "P3\n" << img_width << ' ' << img_height << "\n255\n";
        
        for (int j = 0; j < img_height; j++) {
            std::clog << "\rScanlines Remaining: " << (img_height - j) << ' ' << std::flush;
            for (int i = 0; i < img_width; i++) {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }

        }

        std::clog << "\rDone!                       \n";

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
};

#endif
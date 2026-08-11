#include "rtweekend.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#include "bvh.h"
#include "camera.h"
#include "constant_medium.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "quad.h"
#include "triangle.h"
#include "texture.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "model_loader.h"

// Scenes
void bouncing_spheres(hittable_list& world, camera& cam) {
    //Bouncing Sphere Scene
    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    //diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared <lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, 0.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    //metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    //Glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }

            }

        }
    }
    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    //BVH to help with preformance
    world = hittable_list(make_shared<bvh_node>(world));

    //Camera Settings
    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
    cam.background = color(0.70, 0.80, 1.00);
}

void sphere_example(hittable_list& world, camera& cam) {
    auto material_ground = make_shared<lambertian>(color(0.0, 0.8, 0.8));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(point3(0, 0, -8), 7, material_center));
    world.add(make_shared<sphere>(point3(-4.0, 0, -1.0), 1.0, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0, -1.0), 0.4, material_bubble));
    world.add(make_shared<sphere>(point3(1.0, 0, -1.0), 0.5, material_right));
    

    //Camera Settings
    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;
    cam.background = color(0.70, 0.80, 1.00);
}

void checkered_spheres(hittable_list& world, camera& cam) {
    auto checker = make_shared<checker_texture>(0.32, color(.2,.3,.1), color(0.9, 0.9, 0.9));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));

    //Camera Settings
    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);
 }

void earth(hittable_list& world, camera& cam) {
    auto earth_texture = make_shared<image_texture>("world.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0), 2, earth_surface);

    world.add(globe);

    //Camera Settings
    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);

}

void perlin_sphere(hittable_list& world, camera& cam) {
    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    
    //Camera Settings
    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);

}

void quads(hittable_list& world, camera& cam) {
   
    // Materials
    auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));
    auto map = make_shared<image_texture>("world.jpg");
    auto globe_surface = make_shared<lambertian>(map);

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), globe_surface));
    world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), globe_surface));
    world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), globe_surface));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));

    cam.aspect_ratio = 1.0;
    cam.img_width = 400;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);
}

void simple_light(hittable_list& world, camera& cam) {
    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<quad>(point3(3,1, -2), vec3(2,0,0), vec3(0, 2, 0), difflight));
    
    auto earth_texture = make_shared<image_texture>("world.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(4, 1, 4), 1, earth_surface);

    world.add(globe);

    //Camera Settings
    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 1600;
    cam.samples_per_pixel = 500;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(26, 2, 3);
    cam.lookat = point3(0, 2, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    //cam.background = color(0.70, 0.80, 1.00);
    
}

void cornell_box(hittable_list& world, hittable_list& lights, camera& cam) {

    auto red   = make_shared<lambertian>(color(0.65 , 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));

    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));
    
    //shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = box(point3(0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    //shared_ptr<hittable> box2 = box(point3(0), point3(165, 165, 165), white);
    //box2 = make_shared<rotate_y>(box2, -18);
    //box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    //world.add(box2);
    auto glass = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));
   
    //Light sources (Invisible areas for PDF to direct to)
    auto empty_material = shared_ptr<material>();
    lights.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), empty_material));
    lights.add(make_shared<sphere>(point3(190, 90, 190), 90, empty_material));

    //Camera Settings
    cam.aspect_ratio = 1.0;
    cam.img_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = color(0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
}

void cornell_smoke(hittable_list& world, camera& cam) {

    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    shared_ptr<hittable> box1 = box(point3(0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = box(point3(0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    world.add(make_shared<constant_medium>(box1, 0.01, color (0)));
    world.add(make_shared<constant_medium>(box2, 0.01, color (1)));

    //Camera Settings
    cam.aspect_ratio = 1.0;
    cam.img_width = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth = 50;
    cam.background = color(0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
}

void final_scene(hittable_list& world, hittable_list& lights, camera& cam, int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("world.jpg"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
        vec3(-100, 270, 395)
    )
    );

    //Light sources (Invisible areas for PDF to direct to)
    auto empty_material = shared_ptr<material>();
    lights.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), empty_material));
    //lights.add(make_shared<sphere>(point3(260, 150, 45), 50, empty_material));
    //Theory- Need one on metal sphere

    cam.aspect_ratio = 1.0;
    cam.img_width = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth = max_depth;
    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat = point3(278, 278, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    
    }

void cornell_triangle(hittable_list& world, camera& cam) {

    //Scene
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));

    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

    //Light
    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));

    
    world.add(make_shared<triangle>(point3(300, 178, -100), point3(400, 300, 300), point3(200, 100, 0),  white));
    

    //Camera Settings
    cam.aspect_ratio = 1.0;
    cam.img_width = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
}

void triangles(hittable_list& world, camera& cam) {
   
    // Materials
    auto red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
 

    // Triangle

    world.add(make_shared<triangle>(point3(6, -5, -1), point3(0, 6, 3), point3(-6, -5, 4), red));


    cam.aspect_ratio = 1.0;
    cam.img_width = 400;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;

    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, -9);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);
}





void model_render(hittable_list& world, camera& cam, std::string objfilename, std::string texturefilename) {
    //Renders a model normalized along the x axis and centered at (0,0,0)
    if (!texturefilename.empty()) {
        // Materials
        auto light = make_shared<diffuse_light>(color(5));


        //if texture is provided
        auto texture = make_shared<image_texture>(texturefilename);
        auto tex_mat = make_shared<lambertian>(texture);
        
        
        // Model
        hittable_list model_object_space;
        load_obj(model_object_space, tex_mat, MODELS_PATH + objfilename);
        //BVH to help with preformance
        shared_ptr<hittable> model_world_space = make_shared<bvh_node>(model_object_space);

        model_world_space = make_shared<normalize_bbox_x>(model_world_space);
        model_world_space = make_shared<move_center_to>(model_world_space, point3(0, 0, 0));

        //sphere Light
        world.add(make_shared<sphere>(point3(1, 1, 1), 0.5, light));
        
        
        world.add(model_world_space);
        world = hittable_list(make_shared<bvh_node>(world));
    }
    else {
    // Materials
    auto light_grey = make_shared<lambertian>(color(0.83, 0.83, 0.83));
    auto light = make_shared<diffuse_light>(color(5));
    // Model
    hittable_list model_object_space;
    load_obj(model_object_space, light_grey, MODELS_PATH + objfilename);
    //BVH to help with preformance
    shared_ptr<hittable> model_world_space = make_shared<bvh_node>(model_object_space);

    model_world_space = make_shared<normalize_bbox_x>(model_world_space);
    model_world_space = make_shared<move_center_to>(model_world_space, point3(0, 0, 0));

    //sphere light
    world.add(make_shared<sphere>(point3(1,1,1), 0.5, light));


    world.add(model_world_space);
    world = hittable_list(make_shared<bvh_node>(world));
    }

    //Camera Settings
    cam.aspect_ratio = 1.0;
    cam.img_width = 500;
    cam.samples_per_pixel = 5000;
    cam.max_depth = 50;

    cam.vfov = 50;
    cam.lookfrom = point3(0, 0, 1.5);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);
}

void model_render(hittable_list& world, camera& cam, std::string objfilename){
    //Overload without texture
    model_render(world, cam, objfilename, "");
}

void cornell_mesh(hittable_list& world, camera& cam) {

    //Scene
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto metalic = make_shared<metal>(color(0.7), 1.0);
    auto glass = make_shared<dielectric>(1.5);

    auto texture = make_shared<image_texture>(TEXTURES_PATH + "agamer.jpg");
    auto tex_mat = make_shared<lambertian>(texture);

    //Light
    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
    


    world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));


    shared_ptr<hittable> box1 = box(point3(0), point3(165, 330, 165), tex_mat);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    world.add(box1);

    hittable_list model_object_space;
    load_obj(model_object_space, white, MODELS_PATH + "utah_teapot.obj");
    //BVH to help with preformance
    shared_ptr<hittable> model_world_space = make_shared<bvh_node>(model_object_space);

    model_world_space = make_shared<normalize_bbox_x>(model_world_space);
    model_world_space = make_shared<scale>(model_world_space, 200);

    model_world_space = make_shared<move_bottom_to>(model_world_space, point3(190 , 0, 222.5));
    ////model_world_space = make_shared<rotate_z>(model_world_space, 6.0);
    //model_world_space = make_shared<rotate_y>(model_world_space, 180.0);
    //model_world_space = make_shared<translate>(model_world_space, vec3(150, 10, 405));
    
    world.add(model_world_space);


    world = hittable_list(make_shared<bvh_node>(world));

    //Camera Settings
    cam.aspect_ratio = 1.0;
    cam.img_width = 500; //600
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
}

void sphere_mesh_example(hittable_list& world, camera& cam) {
    auto material_ground = make_shared<lambertian>(color(0.0, 0.8, 0.8));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto metalic = make_shared<metal>(color(0.7), 0.0);
    auto glass = make_shared<dielectric>(1.5);

    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_ground));
    world.add(make_shared<sphere>(point3(0, 0, -8), 7, material_center));
    world.add(make_shared<sphere>(point3(-4.0, 0, -1.0), 1.0, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0, -1.0), 0.4, material_bubble));
    world.add(make_shared<sphere>(point3(4.0, 0, 2.0), 0.5, material_right));
    
    hittable_list model_object_space;
    load_obj(model_object_space, metalic, MODELS_PATH + "bunny.obj");
    //BVH to help with preformance
    //Bunny 

    shared_ptr<hittable> model_world_space = make_shared<bvh_node>(model_object_space);
    model_world_space = make_shared<scale>(model_world_space, 3.0);
    //model_world_space = make_shared<rotate_z>(model_world_space, 6.0);
    model_world_space = make_shared<rotate_y>(model_world_space, 150.0);
    model_world_space = make_shared<translate>(model_world_space, vec3(-2, -0.3, 2.5));
    world.add(model_world_space);
    

    world = hittable_list(make_shared<bvh_node>(world));

    //Camera Settings
    cam.aspect_ratio = 16.0 / 9.0;
    cam.img_width = 1080;
    cam.samples_per_pixel = 1000;
    cam.max_depth = 50;

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 15.0;
    cam.background = color(0.70, 0.80, 1.00);
}

void log_and_spheres(hittable_list& world, camera & cam) {
    // Materials
    auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
    auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
    auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
    auto glass = make_shared<dielectric>(1.5);

    //Ground plane
    world.add(make_shared<quad>(point3(-500, 0, -500), vec3(1000, 0, 0), vec3(0, 0, 1000), white));

    //Spheres
    world.add(make_shared<sphere>(point3(-4, 10, -15), 10, green));
    world.add(make_shared<sphere>(point3(14, 6, 15), 6, glass));
    
    // Model
    auto earth_texture = make_shared<image_texture>("treestumptex.png");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    hittable_list model_object_space;
    load_obj(model_object_space, earth_surface, MODELS_PATH + "treestump.obj");

    //BVH to help with preformance
    shared_ptr<hittable> log = make_shared<bvh_node>(model_object_space);
    log = make_shared<normalize_bbox_x>(log);
    log = make_shared<scale>(log, 7);
    log = make_shared<translate>(log, vec3(9, -1.28, -3));
    log->bounding_box().print_bbox();
    world.add(log);

    log = make_shared<scale>(log, 1.4);
    log = make_shared<rotate_y>(log, 15);
    log = make_shared<translate>(log, vec3(9, -2.28, -3));
    world.add(log);

    //model_world_space = make_shared<translate>(model_world_space, vec3(0, 0, 0));
    world = hittable_list(make_shared<bvh_node>(world));
    


    cam.aspect_ratio = 1.0;
    cam.img_width = 500;
    cam.samples_per_pixel = 10;
    cam.max_depth = 50;

    cam.vfov = 80;
    cam.lookfrom = point3(-14, 14, 35);
    cam.lookat = point3(0, 1, 0);
    cam.up = vec3(0, 1, 0);

    cam.defocus_angle = 0;
    cam.background = color(0.70, 0.80, 1.00);
}

int main(int argc, char* argv[]) {
    hittable_list world;
    hittable_list lights;
    camera cam;

    cornell_mesh(world, cam);

    
    if (argc > 1 && check_file_extention(argv[1])) {
        cam.render(world, lights, argv[1]);
    }
    else {
        cam.render(world, lights, "output.png");
    }

} 
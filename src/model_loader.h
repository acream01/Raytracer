#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include "tiny_obj_loader.h"

	bool load_obj(hittable_list& model_primatives, shared_ptr<material> mat, std::string inputfile) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        //Vertex Coordinates that make up one triangle (x, y, z)
        tinyobj::real_t x0 = 0.0, x1 = 0.0, x2 = 0.0;
        tinyobj::real_t y0 = 0.0, y1 = 0.0, y2 = 0.0;
        tinyobj::real_t z0 = 0.0, z1 = 0.0, z2 = 0.0;

        //Texture Coordinate 
        bool texture_coords = false;
        tinyobj::real_t xt0 = -1, xt1 = -1, xt2 = -1;
        tinyobj::real_t yt0 = -1, yt1 = -1, yt2 = -1;

        //Vertex Normals
        bool vertex_normals = false;
        vec3 normalsV0 = vec3(0);
        vec3 normalsV1 = vec3(0);
        vec3 normalsV2 = vec3(0);


        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

        if (!warn.empty()) {
            std::cout << warn << std::endl;
        }

        if (!err.empty()) {
            std::cerr << err << std::endl;
        }

        if (!ret) {
            exit(1);
        }


        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    if (v == 0){
                        x0 = vx;
                        y0 = vy;
                        z0 = vz;

                        if (idx.texcoord_index >= 0) {
                            //If texcoords are defined, include them in the vertex data
                            texture_coords = true;
                            xt0 = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                            yt0 = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        } else texture_coords = false;
                        if (idx.normal_index >= 0) {
                            //If vertex normals are defined, include them in the vertex data
                            vertex_normals = true;
                            normalsV0 = vec3(
                                attrib.normals[3 * size_t(idx.normal_index) + 0],
                                attrib.normals[3 * size_t(idx.normal_index) + 1],
                                attrib.normals[3 * size_t(idx.normal_index) + 2]);
                        } else vertex_normals = false;
                    }
                    if (v == 1){
                        x1 = vx;
                        y1 = vy;
                        z1 = vz;
                        if (idx.texcoord_index >= 0) {
                            //If texcoords are defined, include them in the vertex data
                            texture_coords = true;
                            xt1 = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                            yt1 = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        } else texture_coords = false;
                        if (idx.normal_index >= 0) {
                            //If vertex normals are defined, include them in the vertex data
                            vertex_normals = true;
                            normalsV1 = vec3(
                                attrib.normals[3 * size_t(idx.normal_index) + 0],
                                attrib.normals[3 * size_t(idx.normal_index) + 1],
                                attrib.normals[3 * size_t(idx.normal_index) + 2]);
                        } else vertex_normals = false;
                    }
                    if (v == 2){
                        x2 = vx;
                        y2 = vy;
                        z2 = vz;
                        if (idx.texcoord_index >= 0) {
                            //If texcoords are defined, include them in the vertex data
                            texture_coords = true;
                            xt2 = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                            yt2 = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        } else texture_coords = false;
                        if (idx.normal_index >= 0) {
                            //If vertex normals are defined, include them in the vertex data
                            vertex_normals = true;
                            normalsV2 = vec3(
                                attrib.normals[3 * size_t(idx.normal_index) + 0],
                                attrib.normals[3 * size_t(idx.normal_index) + 1],
                                attrib.normals[3 * size_t(idx.normal_index) + 2]);
                        } else vertex_normals = false;
                    }
                }

                //Create a triangle primative with 3 verteces, textures, vertex normals, and materials
                if (texture_coords && vertex_normals){
                    model_primatives.add(make_shared<triangle>(
                        point3(x0, y0, z0), xt0, yt0, normalsV0,
                        point3(x1, y1, z1), xt1, yt1, normalsV1,
                        point3(x2, y2, z2), xt2, yt2, normalsV2,
                        mat));
                    
                }
                else if (!texture_coords && vertex_normals) {
                    model_primatives.add(make_shared<triangle>(
                        point3(x0, y0, z0), normalsV0,
                        point3(x1, y1, z1), normalsV1,
                        point3(x2, y2, z2), normalsV2,
                        mat));
                }
                else  if (texture_coords && !vertex_normals) {
                    model_primatives.add(make_shared<triangle>(
                        point3(x0, y0, z0), xt0, yt0,
                        point3(x1, y1, z1), xt1, yt1,
                        point3(x2, y2, z2), xt2, yt2,
                        mat));
                }
                else {
                    model_primatives.add(make_shared<triangle>(
                        point3(x0, y0, z0),
                        point3(x1, y1, z1),
                        point3(x2, y2, z2),
                        mat));
                }

                index_offset += fv;
            }
        }

	}

#endif
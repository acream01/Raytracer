#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

class triangle : public hittable {
public:
	triangle(point3 v0, point3 v1, point3 v2, shared_ptr<material> mat) : A(v0), B(v1), C(v2), mat(mat) {
		//Define Normal
		auto n = cross(v1 - v0, v2 - v0);
		normal = unit_vector(n);
		vnA = normal;
		vnB = normal;
		vnC = normal;

		D = dot(normal, A);

		area = n.length();
		set_bounding_box();
	}

	//Triangle Constructor with Texture Coordinates and Normals
	triangle(point3 v0, double xtA, double ytA, vec3 vnA,
		point3 v1, double xtB, double ytB, vec3 vnB,
		point3 v2, double xtC, double ytC, vec3 vnC,
		shared_ptr<material> mat)
		: A(v0), B(v1), C(v2), mat(mat),
		xtA(xtA), ytA(ytA), vnA(vnA),
		xtB(xtB), ytB(ytB), vnB(vnB),
		xtC(xtC), ytC(ytC), vnC(vnC) 
	{

		//Define Normal
		auto n = cross(v1 - v0, v2 - v0);
		normal = unit_vector(n);
		D = dot(normal, A);

		area = n.length();
		set_bounding_box();
	}

	//Triangle Constructor with Texture Coordinates
	triangle(point3 v0, double xtA, double ytA,
			 point3 v1, double xtB, double ytB,
			 point3 v2, double xtC, double ytC,
			 shared_ptr<material> mat)
		: A(v0), B(v1), C(v2), mat(mat),
		xtA(xtA), ytA(ytA),
		xtB(xtB), ytB(ytB),
		xtC(xtC), ytC(ytC)
	{


		//Define Normal
		auto n = cross(v1 - v0, v2 - v0);
		normal = unit_vector(n);
		vnA = normal;
		vnB = normal;
		vnC = normal;

		D = dot(normal, A);


		area = n.length();
		set_bounding_box();
	}

	//Triangle Constructor with Normals
	triangle(point3 v0, vec3 vnA,
		point3 v1, vec3 vnB,
		point3 v2, vec3 vnC,
		shared_ptr<material> mat)
		: A(v0), B(v1), C(v2), mat(mat),
		vnA(vnA),
		vnB(vnB),
		vnC(vnC)
	{

		//Define Normal
		auto n = cross(v1 - v0, v2 - v0);
		normal = unit_vector(n);
		D = dot(normal, A);

		area = n.length();
		set_bounding_box();
	}

	virtual void set_bounding_box() {
		//compute the bounding box of the three verteces
		auto bbox_AB = aabb(A, B);
		auto bbox_BC = aabb(B, C);
		bbox = aabb(bbox_AB, bbox_BC);
	}

	aabb bounding_box() const override {return bbox;}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
		//Return false if hits outside of triangle
		auto denom = dot(normal, r.direction());

		// No hit if the ray is parallel to the plane.
		if (std::fabs(denom) < 1e-8)
			return false;

		auto t = (D - dot(normal, r.origin())) / denom;
		if (!ray_t.contains(t))
			return false;

		// Determine if the hit point lies within the triangle shape using its plane coordinates.
		auto intersection = r.at(t);

		// Inside - outside Test
		vec3 Ne; //Vector perpendicular to triangles plane

		//Test Sidedness of P w.r.t edge AB
		vec3 AP = intersection - A;
		Ne = cross((B - A), (AP));
		if (dot(normal, Ne) < 0) return false; //P is on the Right side

		//Test Sidedness of P w.r.t edge BC
		vec3 BP = intersection - B;
		Ne = cross((C - B), (BP));
		if (dot(normal, Ne) < 0) return false; //P is on the Right side

		//Test Sidedness of P w.r.t edge CA
		vec3 CP = intersection - C;
		Ne = cross((A - C), (CP));
		if (dot(normal, Ne) < 0) return false; //P is on the Right side


		rec.t = t;
		rec.p = intersection;
		
		//Find hitpoint normal based on barycentric linear combination of vertex normals. 
		// Vertex normals will be face normals if no vertex normals are provided at construction
		if (!(equals(vnA, vnB) && equals(vnC, normal))) {
			vec3 hitpoint_normal = normal;
			find_hitpoint_normal(rec.p, hitpoint_normal, A, B, C, vnA, vnB, vnC);
			rec.set_face_normal(r, hitpoint_normal);
		}
		else {
			//A little messy since it works without this if statement, but slightly optimized since
			//the equals operation should be slightly faster than the dot opperators for the barycentric coords
			rec.set_face_normal(r, normal);
		}
		get_triangle_uv(rec.p, rec.u, rec.v, A, B, C, xtA, xtB, xtC, ytA, ytB, ytC);		
		rec.mat = mat;

		return true;
	}


private:
	point3 A, B, C;
	aabb bbox;
	shared_ptr<material> mat;
	vec3 normal;
	double D; //result of the plane equation
	double area;
	//Texture coordinates, If not defined init as A: UV(0, 0), B: UV(1, 0), C: UV(0, 1)
	double xtA = 0.0, ytA = 0.0;
	double xtB = 1.0, ytB = 0.0;
	double xtC = 0.0, ytC = 1.0;
	//Vertex normals, if not defined init as face normals
	vec3 vnA;
	vec3 vnB;
	vec3 vnC;

	static void calculate_barycentric(point3 p, point3 a, point3 b, point3 c, float &alpha, float &beta, float &gamma){
		vec3 v0 = b - a;
		vec3 v1 = c - a;
		vec3 v2 = p - a;

		float d00 = dot(v0, v0);
		float d01 = dot(v0, v1);
		float d11 = dot(v1, v1);
		float d20 = dot(v2, v0);
		float d21 = dot(v2, v1);

		float denom = d00 * d11 - d01 * d01;

		beta = (d11 * d20 - d01 * d21) / denom;
		gamma = (d00 * d21 - d01 * d20) / denom;
		alpha = 1.0f - beta - gamma;
	}

	static void get_triangle_uv(const point3& p, double& u, double& v, 
		point3 A, point3 B, point3 C
		, double xtA, double xtB, double xtC,
		double ytA, double ytB, double ytC
	){
		//If no tex data, use a genaric triangle uv mapping
		//if tex data -> barycentric coordinates, linear addition to find the UV values
		//Add to the hit rec. u and v are passed as rec.u and rec.v
		//We know P is in the triangle
		float alpha, beta, gamma;
		calculate_barycentric(p, A, B, C, alpha, beta, gamma);
		
		u = alpha * xtA + beta * xtB + gamma * xtC;
		v = alpha * ytA + beta * ytB + gamma * ytC;

	}
	static void find_hitpoint_normal(const point3& p, vec3& hitpoint_normal,
		point3 A, point3 B, point3 C
		, vec3 vnA, vec3 vnB, vec3 vnC
		//Uses Barycentric Coodinates to get a smooth normal transition accross the triangle with vertex normals
	) {
		//If no normal data data, use face normal
		//if normal data -> barycentric coordinates, linear addition to find the hitpoint normal
		//Add to the hit rec. hitpoint normal
		//We know P is in the triangle
		float alpha, beta, gamma;
		calculate_barycentric(p, A, B, C, alpha, beta, gamma);

		hitpoint_normal = vec3(
			alpha * vnA.x() + beta * vnB.x() + gamma * vnC.x(),
			alpha * vnA.y() + beta * vnB.y() + gamma * vnC.y(),
			alpha * vnA.z() + beta * vnB.z() + gamma * vnC.z()
			);

	}

};
#endif
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

class triangle : public hittable {
public:
	triangle(point3 v0, point3 v1, point3 v2, shared_ptr<material> mat) : A(v0), B(v1), C(v2), mat(mat) {
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
		rec.mat = mat;
		rec.set_face_normal(r, normal);

		return true;
	}


private:
	point3 A, B, C;
	aabb bbox;
	shared_ptr<material> mat;
	vec3 normal;
	double D; //result of the plane equation
	double area;

};

#endif
#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"

class material;

class hit_record {
	public:
		point3 p;
		vec3 normal;
		shared_ptr<material> mat;
		double t;
		double u;
		double v;
		bool front_face;

		void set_face_normal(const ray& r, const vec3& outward_normal) {
		// Sets the hit record normal vector
		//NOTE: the parameter 'outward_normal' is assumed to have unit length

		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}

};

class hittable {
	public:
		virtual ~hittable() = default;

		virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
	
		virtual aabb bounding_box() const = 0;
};

//Tranformation Operators
class translate : public hittable {
	public:
		translate(shared_ptr<hittable> object, const vec3& offset)
			: object(object), offset(offset)
		{
			bbox = object->bounding_box() + offset;
		}

		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			//Move the ray backwards by the offset
			ray offset_r(r.origin() - offset, r.direction(), r.time());

			//Determine whether an intersection exists along the offset ray (and if so where)
			if (!object->hit(offset_r, ray_t, rec))
				return false;

			//Move the intersection point forwards by the offset
			rec.p += offset;

			return true;
		}
	
		aabb bounding_box() const override { return bbox; }
	
	private:
		shared_ptr<hittable> object;
		vec3 offset;
		aabb bbox;

};

class rotate_y : public hittable {
	public:
		rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
			auto radians = degrees_to_radians(angle);
			sin_theta = std::sin(radians);
			cos_theta = std::cos(radians);
			bbox = object-> bounding_box();

			point3 min( infinity, infinity, infinity);
			point3 max( -infinity, -infinity, -infinity);

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++){
					for (int k = 0; k < 2; k++) {
					auto x = i * bbox.x.max + (1-i)*bbox.x.min;
					auto y = j * bbox.y.max + (1-j)*bbox.y.min;
					auto z = k * bbox.z.max + (1-k)*bbox.z.min;
				
					auto newx = cos_theta*x + sin_theta*z;
					auto newz = -sin_theta*x + cos_theta*z;

					vec3 tester(newx, y, newz);

					for (int c = 0; c < 3; c++){
						min[c] = std::fmin(min[c], tester[c]);
						max[c] = std::fmax(max[c], tester[c]);
					}

					}
				}
			}

			bbox = aabb(max, min);
		}

		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			//Transform the ray from world space to object space

			auto origin = point3(
				(cos_theta * r.origin().x()) - (sin_theta * r.origin().z()),
				r.origin().y(),
				(sin_theta * r.origin().x()) + (cos_theta * r.origin().z())
			);

			auto direction = vec3(
				(cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
				r.direction().y(),
				(sin_theta * r.direction().x()) + (cos_theta * r.direction().z())
			);

			ray rotated_r(origin, direction, r.time());

			//Determine if a intersection exists in object space

			if (!object->hit(rotated_r, ray_t, rec))
				return false;
		
			//Transform the intersection from object space back into world space

			rec.p = point3(
				(cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
				rec.p.y(),
				(-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
			);
			
			rec.normal = vec3(
				(cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
				rec.normal.y(),
				(-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
			);
		
			return true;
		}

		aabb bounding_box() const override {return bbox;}

	private:
		shared_ptr<hittable> object;
		double sin_theta;
		double cos_theta;
		aabb bbox;
};

class rotate_x : public hittable {
	public:
		rotate_x(shared_ptr<hittable> object, double angle) : object(object) {
			auto radians = degrees_to_radians(angle);
			sin_theta = std::sin(radians);
			cos_theta = std::cos(radians);
			bbox = object->bounding_box();

			point3 min(infinity, infinity, infinity);
			point3 max(-infinity, -infinity, -infinity);

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
						auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
						auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

						auto newy = cos_theta * y + sin_theta * z;
						auto newz = -sin_theta * y + cos_theta * z;

						vec3 tester(x, newy, newz);

						for (int c = 0; c < 3; c++) {
							min[c] = std::fmin(min[c], tester[c]);
							max[c] = std::fmax(max[c], tester[c]);
						}

					}
				}
			}

			bbox = aabb(min, max);
		}

		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			//Transform the ray from world space to object space

			auto origin = point3(
				r.origin().x(),
				cos_theta * r.origin().y() + sin_theta * r.origin().z(),
				(-sin_theta * r.origin().y()) + (cos_theta * r.origin().z())
			);

			auto direction = vec3(
				r.direction().x(),
				(cos_theta * r.direction().y()) - (sin_theta * r.direction().z()),
				(-sin_theta * r.direction().y()) + (cos_theta * r.direction().z())
			);

			ray rotated_r(origin, direction, r.time());

			//Determine if a intersection exists in object space

			if (!object->hit(rotated_r, ray_t, rec))
				return false;

			//Transform the intersection from object space back into world space

			rec.p = point3(
				rec.p.x(),
				(cos_theta * rec.p.y()) + (sin_theta * rec.p.z()),
				(-sin_theta * rec.p.y()) + (cos_theta * rec.p.z())
			);

			rec.normal = vec3(
				rec.normal.x(),
				(cos_theta * rec.normal.y()) + (sin_theta * rec.normal.z()),
				(-sin_theta * rec.normal.y()) + (cos_theta * rec.normal.z())
			);

			return true;
		}

		aabb bounding_box() const override { return bbox; }

	private:
		shared_ptr<hittable> object;
		double sin_theta;
		double cos_theta;
		aabb bbox;
};

class rotate_z : public hittable {
	public:
		rotate_z(shared_ptr<hittable> object, double angle) : object(object) {
			auto radians = degrees_to_radians(angle);
			sin_theta = std::sin(radians);
			cos_theta = std::cos(radians);
			bbox = object->bounding_box();

			point3 min(infinity, infinity, infinity);
			point3 max(-infinity, -infinity, -infinity);

			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
						auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
						auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

						auto newx = cos_theta * x + sin_theta * y;
						auto newy = sin_theta * x + cos_theta * y;

						vec3 tester(newx, newy, z);

						for (int c = 0; c < 3; c++) {
							min[c] = std::fmin(min[c], tester[c]);
							max[c] = std::fmax(max[c], tester[c]);
						}

					}
				}
			}

			bbox = aabb(min, max);
		}

		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			//Transform the ray from world space to object space

			auto origin = point3(
				cos_theta * r.origin().x() + sin_theta * r.origin().y(),
				(-sin_theta * r.origin().x()) + (cos_theta * r.origin().y()),
				r.origin().z()
			);

			auto direction = vec3(
				(cos_theta * r.direction().x()) + (sin_theta * r.direction().y()),
				(-sin_theta * r.direction().x()) + (cos_theta * r.direction().y()),
				r.direction().z()
			);

			ray rotated_r(origin, direction, r.time());

			//Determine if a intersection exists in object space

			if (!object->hit(rotated_r, ray_t, rec))
				return false;

			//Transform the intersection from object space back into world space

			rec.p = point3(
				(cos_theta * rec.p.x()) - (sin_theta * rec.p.y()),
				(sin_theta * rec.p.x()) + (cos_theta * rec.p.y()),
				rec.p.z()
			);

			rec.normal = vec3(
				(cos_theta * rec.normal.x()) - (sin_theta * rec.normal.y()),
				(-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.y()),
				rec.normal.z()
			);

			return true;
		}

		aabb bounding_box() const override { return bbox; }

	private:
		shared_ptr<hittable> object;
		double sin_theta;
		double cos_theta;
		aabb bbox;
};

class scale : public hittable {
	public:
		scale(shared_ptr<hittable> object, double s)
			: scale(object, vec3(s, s, s)) {}

		scale(shared_ptr<hittable> object, const vec3& s)
			: object(object), scale_factor(s), inv_scale(1.0/s.x(), 1.0/s.y(), 1.0/s.z())
		{
			auto b = object->bounding_box();
			//scale all 8 corners of the AABB
			point3 min ( infinity, infinity, infinity);
			point3 max ( -infinity, -infinity, -infinity);
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 2; k++) {
						auto x = (i ? b.x.max : b.x.min) * scale_factor.x();
						auto y = (j ? b.y.max : b.y.min) * scale_factor.y();
						auto z = (k ? b.z.max : b.z.min) * scale_factor.z();
						min[0] = std::fmin(min[0], x); max[0] = std::fmax(max[0], x); 
						min[1] = std::fmin(min[1], y); max[1] = std::fmax(max[1], y); 
						min[2] = std::fmin(min[2], z); max[2] = std::fmax(max[2], z); 

					}
				}
			}
			bbox = aabb(min, max);
		}

		bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
			//Scale ray into object space
			ray scaled_r(
				point3(r.origin().x()*inv_scale.x(), r.origin().y()*inv_scale.y(), r.origin().z()*inv_scale.z()),
				vec3(r.direction().x()*inv_scale.x(), r.direction().y()*inv_scale.y(), r.direction().z()*inv_scale.z()),
				r.time()
			);

			if (!object->hit(scaled_r, ray_t, rec))
				return false;

			//scale hit point back to world space
			rec.p = point3(rec.p.x()*scale_factor.x(), rec.p.y()*scale_factor.y(), rec.p.z()*scale_factor.z());
			
			// Normals transform by the inverse transpose (inv_scale for uniform,
			// inv_scale again for non uniform since inv-transpose of diag = 1/s)
			rec.normal = unit_vector(vec3(
				rec.normal.x() * inv_scale.x(),
				rec.normal.y() * inv_scale.y(),
				rec.normal.z() * inv_scale.z()
			));

			return true;
		}

		aabb bounding_box() const override {return bbox;}

	private: 
		shared_ptr<hittable> object;
		vec3 scale_factor;
		vec3 inv_scale;
		aabb bbox;
};

class normalize_bbox_x : public hittable {
public:
	//Uniformly scales the object so the length of the x direction of the bounding box is 1
	normalize_bbox_x(shared_ptr<hittable> object)
		: object(object) {

		auto b = object->bounding_box();
		inv_scale = b.x.max - b.x.min;
		scale_factor = 1 / (inv_scale);

		//scale all 8 corners of the AABB
		point3 min(infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = (i ? b.x.max : b.x.min) * scale_factor;
					auto y = (j ? b.y.max : b.y.min) * scale_factor;
					auto z = (k ? b.z.max : b.z.min) * scale_factor;
					min[0] = std::fmin(min[0], x); max[0] = std::fmax(max[0], x);
					min[1] = std::fmin(min[1], y); max[1] = std::fmax(max[1], y);
					min[2] = std::fmin(min[2], z); max[2] = std::fmax(max[2], z);

				}
			}
		}
		bbox = aabb(min, max);
	
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		//Scale ray into object space
		ray scaled_r(
			point3(r.origin().x() * inv_scale, r.origin().y() * inv_scale, r.origin().z() * inv_scale),
			vec3(r.direction().x() * inv_scale, r.direction().y() * inv_scale, r.direction().z() * inv_scale),
			r.time()
		);

		if (!object->hit(scaled_r, ray_t, rec))
			return false;

		//scale hit point back to world space
		rec.p = point3(rec.p.x() * scale_factor, rec.p.y() * scale_factor, rec.p.z() * scale_factor);

		// Normals transform by the inverse transpose (inv_scale for uniform,
		// inv_scale again for non uniform since inv-transpose of diag = 1/s)
		rec.normal = unit_vector(vec3(
			rec.normal.x() * inv_scale,
			rec.normal.y() * inv_scale,
			rec.normal.z() * inv_scale
		));

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<hittable> object;
	double scale_factor;
	double inv_scale;
	aabb bbox;
};

class normalize_bbox_y : public hittable {
	//Uniformly scales the object so the length of the y direction of the bounding box is 1
public:
	normalize_bbox_y(shared_ptr<hittable> object)
		: object(object) {

		auto b = object->bounding_box();
		inv_scale = b.y.max - b.y.min;
		scale_factor = 1 / (inv_scale);

		//scale all 8 corners of the AABB
		point3 min(infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = (i ? b.x.max : b.x.min) * scale_factor;
					auto y = (j ? b.y.max : b.y.min) * scale_factor;
					auto z = (k ? b.z.max : b.z.min) * scale_factor;
					min[0] = std::fmin(min[0], x); max[0] = std::fmax(max[0], x);
					min[1] = std::fmin(min[1], y); max[1] = std::fmax(max[1], y);
					min[2] = std::fmin(min[2], z); max[2] = std::fmax(max[2], z);

				}
			}
		}
		bbox = aabb(min, max);

	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		//Scale ray into object space
		ray scaled_r(
			point3(r.origin().x() * inv_scale, r.origin().y() * inv_scale, r.origin().z() * inv_scale),
			vec3(r.direction().x() * inv_scale, r.direction().y() * inv_scale, r.direction().z() * inv_scale),
			r.time()
		);

		if (!object->hit(scaled_r, ray_t, rec))
			return false;

		//scale hit point back to world space
		rec.p = point3(rec.p.x() * scale_factor, rec.p.y() * scale_factor, rec.p.z() * scale_factor);

		// Normals transform by the inverse transpose (inv_scale for uniform,
		// inv_scale again for non uniform since inv-transpose of diag = 1/s)
		rec.normal = unit_vector(vec3(
			rec.normal.x() * inv_scale,
			rec.normal.y() * inv_scale,
			rec.normal.z() * inv_scale
		));

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<hittable> object;
	double scale_factor;
	double inv_scale;
	aabb bbox;
};

class normalize_bbox_z : public hittable {
	//Uniformly scales the object so the length of the x direction of the bounding box is 1
public:
	normalize_bbox_z(shared_ptr<hittable> object)
		: object(object) {

		auto b = object->bounding_box();
		inv_scale = b.z.max - b.z.min;
		scale_factor = 1 / (inv_scale);

		//scale all 8 corners of the AABB
		point3 min(infinity, infinity, infinity);
		point3 max(-infinity, -infinity, -infinity);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 2; k++) {
					auto x = (i ? b.x.max : b.x.min) * scale_factor;
					auto y = (j ? b.y.max : b.y.min) * scale_factor;
					auto z = (k ? b.z.max : b.z.min) * scale_factor;
					min[0] = std::fmin(min[0], x); max[0] = std::fmax(max[0], x);
					min[1] = std::fmin(min[1], y); max[1] = std::fmax(max[1], y);
					min[2] = std::fmin(min[2], z); max[2] = std::fmax(max[2], z);

				}
			}
		}
		bbox = aabb(min, max);

	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		//Scale ray into object space
		ray scaled_r(
			point3(r.origin().x() * inv_scale, r.origin().y() * inv_scale, r.origin().z() * inv_scale),
			vec3(r.direction().x() * inv_scale, r.direction().y() * inv_scale, r.direction().z() * inv_scale),
			r.time()
		);

		if (!object->hit(scaled_r, ray_t, rec))
			return false;

		//scale hit point back to world space
		rec.p = point3(rec.p.x() * scale_factor, rec.p.y() * scale_factor, rec.p.z() * scale_factor);

		// Normals transform by the inverse transpose (inv_scale for uniform,
		// inv_scale again for non uniform since inv-transpose of diag = 1/s)
		rec.normal = unit_vector(vec3(
			rec.normal.x() * inv_scale,
			rec.normal.y() * inv_scale,
			rec.normal.z() * inv_scale
		));

		return true;
	}

	aabb bounding_box() const override { return bbox; }

private:
	shared_ptr<hittable> object;
	double scale_factor;
	double inv_scale;
	aabb bbox;
};

#endif
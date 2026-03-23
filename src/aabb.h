#ifndef AABB_H
#define AABB_H

class aabb {
public:
	interval x, y, z;

	aabb() {} //default AABB is empty

	
	aabb(const interval& x, const interval& y, const interval& z)
		: x(x), y(y), z(z) 
	{
		pad_to_mininums();
	}

	aabb(const point3& a, const point3& b) {
		x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
		y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
		z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);

		pad_to_mininums();
	}

	aabb(const aabb bbox1, const aabb bbox2) {
		x = interval(bbox1.x, bbox2.x);
		y = interval(bbox1.y, bbox2.y);
		z = interval(bbox1.z, bbox2.z);
	}

	const interval& axis_interval(int n) const {
		if (n == 1) return y;
		if (n == 2) return z;
		return x;

	}

	bool hit(const ray& r, interval ray_t) const {
		const point3&	ray_orig = r.origin();
		const vec3&		ray_dir  = r.direction();

		for (int axis = 0; axis < 3; axis++) {
			const interval& ax = axis_interval(axis);
			const double adinv = 1.0 / ray_dir[axis];

			auto t0 = (ax.min - ray_orig[axis]) * adinv;
			auto t1 = (ax.max - ray_orig[axis]) * adinv;

			if (t0 < t1) {
				if (t0 > ray_t.min) ray_t.min = t0;
				if (t1 < ray_t.max) ray_t.max = t1;
			}
			else {
				if (t1 > ray_t.min) ray_t.min = t1;
				if (t1 < ray_t.max) ray_t.max = t0;

			}

			if (ray_t.max <= ray_t.min)
				return false;
		
		}
		return true;
	}

	int longest_axis() const {
		//returns the index of the longest axis of the bbox x:0 y:1 z:2

		if (x.size() > y.size())
			return x.size() > z.size() ? 0 : 2;
		else
			return y.size() > z.size() ? 1 : 2;
	}

	static const aabb empty, universe;
private:
	void pad_to_mininums() {
		// Adjust the AABB so that no side is narrower than some delta, padding if neccesary


		double delta = 0.0001;
		if (x.size() < delta) x = x.expand(delta);
		if (y.size() < delta) y = y.expand(delta);
		if (z.size() < delta) z = z.expand(delta);
	}
};
	const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
	const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

	aabb operator+(const aabb& bbox, const vec3& offset) {
		return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
	}

	aabb operator+(const vec3& offset, const aabb& bbox) {
		return bbox + offset;
	}

#endif
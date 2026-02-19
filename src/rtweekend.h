#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

//C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//util

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

inline double random_double() {
	//Returns a random real in [0,1)
	return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
	//Returns a random real in [min, max)
	return min + (max - min) * random_double();
}

bool check_file_extention(char* outfile) {
    std::string fileExt = std::string(outfile);
    int len = fileExt.length();

    if (len > 4 && fileExt.compare((len - 4), 4, ".png") == 0) {
        return true;
    }
    std::cout << "Unaccepted file extention. Writing to output.png...\n";
    return false;
}

//common headers

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
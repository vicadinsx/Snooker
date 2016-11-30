#ifndef MATHF_H
#define MATHF_H

#include <cmath>
constexpr float M_PI = 3.1415926;

namespace math {

    float floatThreshold = 1.0e-10;

    inline float toRadians(float degrees){
        return (degrees * M_PI) / 180.0f;
    }

    inline float toDegrees(float radians){
        return (radians * 180.0f) / M_PI;
    }

    inline float cotangent(float value){
        float angle = toRadians(value);
        return (1.0f / ((float)tan(angle)));
    }

    inline float compareEqual(float x, float y) {
        float difference = x - y;
        return difference < floatThreshold && -difference < floatThreshold;
    }

};

#endif

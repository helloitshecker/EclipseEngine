#include "Maths.hpp"

using namespace e;

float maths::clamp(float value, float min, float max) {
    return min + ((value - min) > 0 ? (value - min) : 0) - ((value - max) > 0 ? (value - max) : 0);
}

double maths::clamp(double value, double min, double max) {
    return min + ((value - min) > 0 ? (value - min) : 0) - ((value - max) > 0 ? (value - max) : 0);
}

int maths::clamp(int value, int min, int max) {
    return min + ((value - min) > 0 ? (value - min) : 0) - ((value - max) > 0 ? (value - max) : 0);
}

int maths::ceil(float value) {
    int intValue = (int)value;
    return (value > intValue) ? intValue + 1 : intValue;
}
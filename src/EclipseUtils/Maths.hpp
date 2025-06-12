#pragma once

namespace e {
	namespace maths {
		float clamp(float value, float min, float max);
		double clamp(double value, double min, double max);
		int clamp(int value, int min, int max);
		int ceil(float value);
	};
};
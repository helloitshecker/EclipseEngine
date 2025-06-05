#pragma once

namespace ee {
	namespace Time {
		double GetCurrentTimeInSeconds();
		double GetCurrentTimeInMillseconds();
		double GetCurrentTimeInMicroseconds();
		double GetCurrentTimeInNanoseconds();
		double GetDelta();
		void Sleep(double time);
	};
};
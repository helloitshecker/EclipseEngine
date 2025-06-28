#pragma once

namespace Eclipse::Time {
    struct TimeData {
        double delta_time;
        double current_time;
    };

    void Update();
    const TimeData& Get();
}

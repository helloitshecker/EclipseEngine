#pragma once

#include <tbb/task_group.h>
#include <functional>

namespace Eclipse {
      class JobSystem {
      public:
            void submit(const std::function<void()>& job) {
                  group.run(job);
            }

            void wait() {
                  group.wait();
            }

      private:
            tbb::task_group group;
      };
}
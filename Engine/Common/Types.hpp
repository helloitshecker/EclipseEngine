#pragma once

namespace Eclipse {
  union IntVec2 {
      struct {int x, y;};
      struct {int w, h;};
      int val[2];
  };
};

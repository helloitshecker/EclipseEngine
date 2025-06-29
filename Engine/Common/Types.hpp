#pragma once

namespace Eclipse {
  union IntVec2 {
      struct {int x, y;};
      struct {int w, h;};
      int val[2];
  };
  union FloatVec2 {
      struct {float x, y;};
      struct {float w, h;};
      float val[2];
  };
  union FloatVec4 {
      struct {float x, y, z, zp;};
      struct {float r, g, b, a;};
      float val[4];
  };
};

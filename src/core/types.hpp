#pragma once

namespace ee {

	// VECS
		// VEC 2
		union UVec2
		{
			struct { size_t x, y; };
			struct { size_t w, h; };
			size_t pos[2];
		};
		union IVec2
		{
			struct { int x, y; };
			struct { int w, h; };
			int pos[2];
		};
		union FVec2
		{
			struct { float x, y; };
			struct { float w, h; };
			float pos[2];
		};



		// VEC 3
		union UVec3
		{
			struct { size_t x, y, z; };
			size_t pos[3];
		};
		union IVec3
		{
			struct { int x, y, z; };
			int pos[3];
		};
		union FVec3
		{
			struct { float x, y, z; };
			float pos[3];
		};
		


		// VEC 4
		union UVec4
		{
			struct { size_t x, y, z, r; };
			size_t pos[4];
		};
		union IVec4
		{
			struct { int x, y, z, r; };
			int pos[4];
		};
		union FVec4
		{
			struct { float x, y, z, r; };
			float pos[4];
		};
};
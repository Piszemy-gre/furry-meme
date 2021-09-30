#pragma once
#include <glm/vec3.hpp>

namespace voxel_utility
{

	template<typename Iter, typename F>
	Iter insertVec3(Iter iter, F x, F y, F z)
	{
	    *iter++ = x;
	    *iter++ = y;
	    *iter++ = z;
		return iter;
	}

    template<typename Iter, typename F>
	Iter insertQuad(Iter iter, F x1, F y1, F z1, F x2, F y2, F z2, F x3, F y3, F z3, F x4, F y4, F z4)
	{
		//1st triangle
	    iter = insertVec3(iter, x1, y1, z1);
	    iter = insertVec3(iter, x2, y2, z2);
	    iter = insertVec3(iter, x4, y4, z4);
		//2nd triangle
	    iter = insertVec3(iter, x2, y2, z2);
	    iter = insertVec3(iter, x3, y3, z3);
	    iter = insertVec3(iter, x4, y4, z4);

		return iter;
	}


	template<typename Iter, typename F, typename Z>
    Iter insertCube(Iter iter, F x, F y, F z, Z w)
	{
		//bottom face
	    iter = insertQuad(iter, 
			x, y, z,
			x, y, z + w,
			x + w, y, z + w,
			x + w, y, z);

		//top face
		iter = insertQuad(iter, 
			x, y + w , z,
			x, y + w , z + w,
			x + w, y + w, z + w,
			x + w, y + w, z);

		//front face
		iter = insertQuad(iter, 
			x, y, z,
			x, y + w, z,
			x + w, y + w, z,
			x + w, y, z);

		//back face
		iter = insertQuad(iter, 
			x, y, z + w,
			x, y + w, z + w,
			x + w, y + w, z + w,
			x + w, y, z + w);

		//left face
		iter = insertQuad(iter, 
			x, y, z,
			x, y, z + w,
			x, y + w, z + w,
			x, y + w, z);

		//right face
		iter = insertQuad(iter, 
			x + w, y, z,
			x + w, y, z + w,
			x + w, y + w, z + w,
			x + w, y + w, z);

		return iter;
	}

	template<typename Iter, typename F>
    void translateVertices(Iter start, Iter end, F x, F y, F z)
	{
        while (start != end)
        {
            *start++ += x;
            *start++ += y;
            *start++ += z;
        }
	}
}


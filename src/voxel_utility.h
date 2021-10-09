#pragma once
#include <glm/vec3.hpp>

namespace voxel_utility
{

    template<typename Iter, typename F>
    Iter insertQuad(Iter iter, F v1, F v2, F v3, F v4)
	{
		//1st triangle
	    *iter++ = v1;
	    *iter++ = v2;
	    *iter++ = v3;

		//2nd triangle
        *iter++ = v1;
        *iter++ = v3;
        *iter++ = v4;

		return iter;
	}

    template<typename Iter, typename F>
    Iter insertQuad(Iter iter, F x1, F y1, F z1, F x2, F y2, F z2, F x3, F y3, F z3, F x4, F y4, F z4)
    {
        return insertQuad(iter, glm::vec<3, F>(x1, y1, z1), glm::vec<3, F>(x2, y2, z2), glm::vec<3, F>(x3, y3, z3), glm::vec<3, F>(x4, y4, z4));
    }


	template <typename Iter, typename F, typename Z>
    Iter insertCube(Iter iter, glm::vec<3, F> v, Z w)
    {
        const auto x = v.x;
        const auto y = v.y;
        const auto z = v.z;
        //bottom face
        iter = insertQuad(iter,
                            x, y, z,
                            x + w, y, z,
                            x + w, y, z + w,
                            x, y, z + w);

        //top face
        iter = insertQuad(iter,
                            x, y + w, z + w,
                            x + w, y + w, z + w,
                            x + w, y + w, z,
                            x, y + w, z);

        //front face
        iter = insertQuad(iter,
                          x, y, z + w,
                            x + w, y, z + w,
                            x + w, y + w, z + w,
                            x, y + w, z + w);

        //back face
        iter = insertQuad(iter,
                        x + w, y, z,
                        x, y, z,
                        x, y + w, z,
                        x + w, y + w, z);

        //left face
        iter = insertQuad(iter,
                            x, y, z,
                            x, y, z + w,
                            x, y + w, z + w,
                            x, y + w, z);

        //right face
        iter = insertQuad(iter,
                          x + w, y, z + w,
                        x + w, y, z,
                        x + w, y + w, z,
                        x + w, y + w, z + w);

        return iter;
    }

    template <typename Iter, typename Array>
    Iter insertCubeTextures(Iter iter, const Array& faces)
    {
        //bottom face
        iter = insertQuad(iter,
            faces[0][0], 
            faces[0][1], 
            faces[0][2], 
            faces[0][3]);

        //top face
        iter = insertQuad(iter,
                          faces[1][0],
                          faces[1][1],
                          faces[1][2],
                          faces[1][3]);

        //front face
        //back face
        //left face
        //right face
        for (size_t i = 2; i < faces.size(); i++)
            iter = insertQuad(iter,
                              faces[i][0],
                              faces[i][1],
                              faces[i][2],
                              faces[i][3]);

        return iter;
    }

	template<typename Iter, typename F>
    void translateVertices(Iter start, Iter end, glm::vec<3, F> v)
	{
        while (start != end)
        {
            *start++ += v;
        }
	}
}


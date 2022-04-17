#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Amethyst
{
	namespace Math
	{
		bool Decompose(const glm::mat4& matrix, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
		{
			// This code is from glm::decompose in the file matrix_decompose.inl

			using namespace glm;
			using T = float;

			mat4 LocalMatrix(matrix);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], static_cast<T>(0), epsilon<T>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
				epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
			{
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of translation (easy).
			position = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);// v3Length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));

			// Now, compute Y scale and normalize 2nd row.
			scale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));

			// Next, get Z scale and normalize 3rd row.
			scale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
#if 0
			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					Scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}
#endif

			// Now, get the rotations out, as described in the gem.

			// FIXME - Add the ability to return either quaternions (which are
			// easier to recompose with) or Euler angles (rx, ry, rz), which
			// are easier for authors to deal with. The latter will only be useful
			// when we fix https://bugs.webkit.org/show_bug.cgi?id=23799, so I
			// will leave the Euler angle code here for now.

			 rotation.y = asin(-Row[0][2]);
			 if (cos(rotation.x) != 0) 
			 {
				 rotation.x = atan2(Row[1][2], Row[2][2]);
				 rotation.z = atan2(Row[0][1], Row[0][0]);
			 } 
			 else 
			 {
				 rotation.x = atan2(-Row[2][0], Row[1][1]);
				 rotation.z = 0;
			 }

			return true;
		}

		bool AABBIntersects(const AABB& aabb, const glm::vec3& nearPlanePos, const glm::vec3& farPlanePos)
		{
			glm::vec3 direction = farPlanePos - nearPlanePos;
			float length = glm::length(direction);

			float invLen = 1.0f / length;
			direction *= invLen;
			float tNear = 0.0f;
			float tFar = length;

			direction = glm::normalize(direction);

			// Line x
			if (glm::epsilonEqual(direction.x, 0.0f, glm::epsilon<float>()))
			{
				float recipDir = 1.0f / direction.x;

				float t1 = (aabb.min.x - nearPlanePos.x) * recipDir;
				float t2 = (aabb.max.x - nearPlanePos.x) * recipDir;

				if (t1 < t2)
				{
					tNear = glm::max(t1, tNear);
					tFar = glm::min(t2, tFar);
				}
				else
				{
					tNear = glm::max(t2, tNear);
					tFar = glm::min(t1, tFar);
				}

				if (tNear > tFar)
					return false;
			}
			else if (nearPlanePos.x < aabb.min.x || nearPlanePos.x > aabb.max.x)
				return false;

			// Line y
			if (glm::epsilonEqual(direction.y, 0.0f, glm::epsilon<float>()))
			{
				float recipDir = 1.0f / direction.y;

				float t1 = (aabb.min.y - nearPlanePos.y) * recipDir;
				float t2 = (aabb.max.y - nearPlanePos.y) * recipDir;

				if (t1 < t2)
				{
					tNear = glm::max(t1, tNear);
					tFar = glm::min(t2, tFar);
				}
				else
				{
					tNear = glm::max(t2, tNear);
					tFar = glm::min(t1, tFar);
				}

				if (tNear > tFar)
					return false;
			}
			else if (nearPlanePos.y < aabb.min.y || nearPlanePos.y > aabb.max.y)
				return false;

			// Line z
			if (glm::epsilonEqual(direction.z, 0.0f, glm::epsilon<float>()))
			{
				float recipDir = 1.0f / direction.z;

				float t1 = (aabb.min.z - nearPlanePos.z) * recipDir;
				float t2 = (aabb.max.z - nearPlanePos.z) * recipDir;

				if (t1 < t2)
				{
					tNear = glm::max(t1, tNear);
					tFar = glm::min(t2, tFar);
				}
				else
				{
					tNear = glm::max(t2, tNear);
					tFar = glm::min(t1, tFar);
				}

				if (tNear > tFar)
					return false;
			}
			else if (nearPlanePos.z < aabb.min.z || nearPlanePos.z > aabb.max.z)
				return false;

			return tNear <= tFar;
		}
	}
}
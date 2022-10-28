//
// Created by rfaye on 27/10/2022.
//

#pragma once

namespace spring
{
    namespace math
    {
        template<typename T>
        class vec3
        {
        public:
            vec3() = default;
            vec3(T x, T y, T z): x(x), y(y), z(z) {}

            bool operator==(const vec3 &rhs) const {
                return x == rhs.x &&
                       y == rhs.y &&
                       z == rhs.z;
            }

            bool operator!=(const vec3 &rhs) const {
                return !(rhs == *this);
            }

            bool operator<(const vec3 &rhs) const {
                if (x < rhs.x)
                    return true;
                if (rhs.x < x)
                    return false;
                if (y < rhs.y)
                    return true;
                if (rhs.y < y)
                    return false;
                return z < rhs.z;
            }

            bool operator>(const vec3 &rhs) const {
                return rhs < *this;
            }

            bool operator<=(const vec3 &rhs) const {
                return !(rhs < *this);
            }

            bool operator>=(const vec3 &rhs) const {
                return !(*this < rhs);
            };

        protected:
            T x;
            T y;
            T z;
        };

        template <typename T>
        vec3<T> operator+(vec3<T>& left, vec3<T>& right)
        {
            return vec3(left.x+right.x, left.y+right.y, left.z+right.z);
        }
    }
}
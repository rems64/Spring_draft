//
// Created by rfaye on 27/10/2022.
//

#pragma once

#include <spdlog/fmt/bundled/format.h>

namespace spring::math {
    template<typename T>
    class vec3 {
    public:
        constexpr vec3() = default;

        constexpr vec3(T x, T y, T z) : x(x), y(y), z(z) {};

        template<typename U>
        constexpr explicit vec3(const vec3 <U> &v) : x((T) v.x), y((T) v.y), z((T) v.z) {};

        bool operator==(const vec3 &rhs) const {
            return x == rhs.x &&
                   y == rhs.y &&
                   z == rhs.z;
        }

        bool operator!=(const vec3 &rhs) const {
            return rhs != *this;
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
            return rhs >= *this;
        }

        bool operator>=(const vec3 &rhs) const {
            return *this >= rhs;
        };

        vec3 <T> &operator+=(const vec3 <T> &right) {
            x += right.x;
            y += right.y;
            z += right.z;
            return this;
        }

        friend std::ostream &operator<<(std::ostream &os, const vec3 &v);

    public:
        T x;
        T y;
        T z;
    };

    template<typename T>
    vec3<T> operator+(const vec3<T> &left, const vec3<T> &right) {
        return vec3(left.x + right.x, left.y + right.y, left.z + right.z);
    }

    template<typename T>
    vec3<T> operator-(const vec3<T> &left, const vec3<T> &right) {
        return vec3(left.x - right.x, left.y - right.y, left.z - right.z);
    }

    template<typename T>
    vec3<T> operator-(const vec3<T> &right) {
        return vec3(-right.x, -right.y, -right.z);
    }

    template<typename T>
    std::ostream &operator<<(std::ostream &os, const vec3<T> &v) {
        os << v.x << ", " << v.y << ", " << v.z;
        return os;
    }

    using vec3f = vec3<float>;
    using vec3d = vec3<double>;
    using vec3i = vec3<int32_t>;
    using vec3ui = vec3<uint32_t>;
    using vec3i_16 = vec3<int16_t>;
    using vec3ui_16 = vec3<int16_t>;
    using vec3i_64 = vec3<int64_t>;
    using vec3ui_64 = vec3<uint64_t>;
}

template<typename T>
struct fmt::formatter<spring::math::vec3<T>> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template <typename FormatContext>
    auto format(const spring::math::vec3<T>& v, FormatContext& ctx) -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(),
                         "({} {} {})",
                         v.x, v.y, v.z);
    }
};
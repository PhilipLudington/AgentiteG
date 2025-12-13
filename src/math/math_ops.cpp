/**
 * MathOps Implementation
 */

#include "math_ops.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>

namespace godot {

void MathOps::_bind_methods() {
    // Normalization
    ClassDB::bind_static_method("MathOps", D_METHOD("normalize_batch_2d", "vectors"), &MathOps::normalize_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("normalize_batch_3d", "vectors"), &MathOps::normalize_batch_3d);

    // Dot products
    ClassDB::bind_static_method("MathOps", D_METHOD("dot_batch_2d", "a", "b"), &MathOps::dot_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("dot_batch_3d", "a", "b"), &MathOps::dot_batch_3d);

    // Cross products
    ClassDB::bind_static_method("MathOps", D_METHOD("cross_batch", "a", "b"), &MathOps::cross_batch);
    ClassDB::bind_static_method("MathOps", D_METHOD("cross_batch_2d", "a", "b"), &MathOps::cross_batch_2d);

    // Lengths
    ClassDB::bind_static_method("MathOps", D_METHOD("length_batch_2d", "vectors"), &MathOps::length_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("length_batch_3d", "vectors"), &MathOps::length_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("length_squared_batch_2d", "vectors"), &MathOps::length_squared_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("length_squared_batch_3d", "vectors"), &MathOps::length_squared_batch_3d);

    // Distances
    ClassDB::bind_static_method("MathOps", D_METHOD("distance_batch_2d", "a", "b"), &MathOps::distance_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("distance_batch_3d", "a", "b"), &MathOps::distance_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("distance_squared_batch_2d", "a", "b"), &MathOps::distance_squared_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("distance_squared_batch_3d", "a", "b"), &MathOps::distance_squared_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("distance_matrix_2d", "a", "b"), &MathOps::distance_matrix_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("distance_matrix_3d", "a", "b"), &MathOps::distance_matrix_3d);

    // Interpolation
    ClassDB::bind_static_method("MathOps", D_METHOD("lerp_batch_2d", "a", "b", "t"), &MathOps::lerp_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("lerp_batch_3d", "a", "b", "t"), &MathOps::lerp_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("lerp_batch_2d_weights", "a", "b", "t"), &MathOps::lerp_batch_2d_weights);
    ClassDB::bind_static_method("MathOps", D_METHOD("lerp_batch_3d_weights", "a", "b", "t"), &MathOps::lerp_batch_3d_weights);

    // Arithmetic
    ClassDB::bind_static_method("MathOps", D_METHOD("add_batch_2d", "a", "b"), &MathOps::add_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("add_batch_3d", "a", "b"), &MathOps::add_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("sub_batch_2d", "a", "b"), &MathOps::sub_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("sub_batch_3d", "a", "b"), &MathOps::sub_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("scale_batch_2d", "vectors", "scalar"), &MathOps::scale_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("scale_batch_3d", "vectors", "scalar"), &MathOps::scale_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("scale_batch_2d_weights", "vectors", "scalars"), &MathOps::scale_batch_2d_weights);
    ClassDB::bind_static_method("MathOps", D_METHOD("scale_batch_3d_weights", "vectors", "scalars"), &MathOps::scale_batch_3d_weights);

    // Direction
    ClassDB::bind_static_method("MathOps", D_METHOD("direction_batch_2d", "from", "to"), &MathOps::direction_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("direction_batch_3d", "from", "to"), &MathOps::direction_batch_3d);

    // Transforms
    ClassDB::bind_static_method("MathOps", D_METHOD("transform_2d_batch", "xform", "points"), &MathOps::transform_2d_batch);
    ClassDB::bind_static_method("MathOps", D_METHOD("transform_3d_batch", "xform", "points"), &MathOps::transform_3d_batch);
    ClassDB::bind_static_method("MathOps", D_METHOD("transform_2d_batch_inverse", "xform", "points"), &MathOps::transform_2d_batch_inverse);
    ClassDB::bind_static_method("MathOps", D_METHOD("transform_3d_batch_inverse", "xform", "points"), &MathOps::transform_3d_batch_inverse);

    // Angles
    ClassDB::bind_static_method("MathOps", D_METHOD("angle_batch_2d", "vectors"), &MathOps::angle_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("angle_between_batch_2d", "a", "b"), &MathOps::angle_between_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("angle_between_batch_3d", "a", "b"), &MathOps::angle_between_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("from_angle_batch", "angles"), &MathOps::from_angle_batch);

    // Projection & Reflection
    ClassDB::bind_static_method("MathOps", D_METHOD("project_batch_2d", "a", "b"), &MathOps::project_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("project_batch_3d", "a", "b"), &MathOps::project_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("reflect_batch_2d", "vectors", "normals"), &MathOps::reflect_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("reflect_batch_3d", "vectors", "normals"), &MathOps::reflect_batch_3d);

    // Clamping
    ClassDB::bind_static_method("MathOps", D_METHOD("clamp_length_batch_2d", "vectors", "max_length"), &MathOps::clamp_length_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("clamp_length_batch_3d", "vectors", "max_length"), &MathOps::clamp_length_batch_3d);
    ClassDB::bind_static_method("MathOps", D_METHOD("clamp_length_range_batch_2d", "vectors", "min_length", "max_length"), &MathOps::clamp_length_range_batch_2d);
    ClassDB::bind_static_method("MathOps", D_METHOD("clamp_length_range_batch_3d", "vectors", "min_length", "max_length"), &MathOps::clamp_length_range_batch_3d);
}

// ========== NORMALIZATION ==========

PackedVector2Array MathOps::normalize_batch_2d(const PackedVector2Array& vectors) {
    int32_t count = vectors.size();
    PackedVector2Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float len_sq = src[i].x * src[i].x + src[i].y * src[i].y;
        if (len_sq > 0.0f) {
            float inv_len = 1.0f / std::sqrt(len_sq);
            dst[i] = Vector2(src[i].x * inv_len, src[i].y * inv_len);
        } else {
            dst[i] = Vector2(0.0f, 0.0f);
        }
    }

    return result;
}

PackedVector3Array MathOps::normalize_batch_3d(const PackedVector3Array& vectors) {
    int32_t count = vectors.size();
    PackedVector3Array result;
    result.resize(count);

    const Vector3* src = vectors.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float len_sq = src[i].x * src[i].x + src[i].y * src[i].y + src[i].z * src[i].z;
        if (len_sq > 0.0f) {
            float inv_len = 1.0f / std::sqrt(len_sq);
            dst[i] = Vector3(src[i].x * inv_len, src[i].y * inv_len, src[i].z * inv_len);
        } else {
            dst[i] = Vector3(0.0f, 0.0f, 0.0f);
        }
    }

    return result;
}

// ========== DOT PRODUCTS ==========

PackedFloat32Array MathOps::dot_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = pa[i].x * pb[i].x + pa[i].y * pb[i].y;
    }

    return result;
}

PackedFloat32Array MathOps::dot_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = pa[i].x * pb[i].x + pa[i].y * pb[i].y + pa[i].z * pb[i].z;
    }

    return result;
}

// ========== CROSS PRODUCTS ==========

PackedVector3Array MathOps::cross_batch(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector3(
            pa[i].y * pb[i].z - pa[i].z * pb[i].y,
            pa[i].z * pb[i].x - pa[i].x * pb[i].z,
            pa[i].x * pb[i].y - pa[i].y * pb[i].x
        );
    }

    return result;
}

PackedFloat32Array MathOps::cross_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = pa[i].x * pb[i].y - pa[i].y * pb[i].x;
    }

    return result;
}

// ========== VECTOR LENGTHS ==========

PackedFloat32Array MathOps::length_batch_2d(const PackedVector2Array& vectors) {
    int32_t count = vectors.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = std::sqrt(src[i].x * src[i].x + src[i].y * src[i].y);
    }

    return result;
}

PackedFloat32Array MathOps::length_batch_3d(const PackedVector3Array& vectors) {
    int32_t count = vectors.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = vectors.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = std::sqrt(src[i].x * src[i].x + src[i].y * src[i].y + src[i].z * src[i].z);
    }

    return result;
}

PackedFloat32Array MathOps::length_squared_batch_2d(const PackedVector2Array& vectors) {
    int32_t count = vectors.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = src[i].x * src[i].x + src[i].y * src[i].y;
    }

    return result;
}

PackedFloat32Array MathOps::length_squared_batch_3d(const PackedVector3Array& vectors) {
    int32_t count = vectors.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = vectors.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = src[i].x * src[i].x + src[i].y * src[i].y + src[i].z * src[i].z;
    }

    return result;
}

// ========== DISTANCE CALCULATIONS ==========

PackedFloat32Array MathOps::distance_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dx = pb[i].x - pa[i].x;
        float dy = pb[i].y - pa[i].y;
        dst[i] = std::sqrt(dx * dx + dy * dy);
    }

    return result;
}

PackedFloat32Array MathOps::distance_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dx = pb[i].x - pa[i].x;
        float dy = pb[i].y - pa[i].y;
        float dz = pb[i].z - pa[i].z;
        dst[i] = std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    return result;
}

PackedFloat32Array MathOps::distance_squared_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dx = pb[i].x - pa[i].x;
        float dy = pb[i].y - pa[i].y;
        dst[i] = dx * dx + dy * dy;
    }

    return result;
}

PackedFloat32Array MathOps::distance_squared_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dx = pb[i].x - pa[i].x;
        float dy = pb[i].y - pa[i].y;
        float dz = pb[i].z - pa[i].z;
        dst[i] = dx * dx + dy * dy + dz * dz;
    }

    return result;
}

PackedFloat32Array MathOps::distance_matrix_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count_a = a.size();
    int32_t count_b = b.size();
    PackedFloat32Array result;
    result.resize(count_a * count_b);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            float dx = pb[j].x - pa[i].x;
            float dy = pb[j].y - pa[i].y;
            dst[i * count_b + j] = std::sqrt(dx * dx + dy * dy);
        }
    }

    return result;
}

PackedFloat32Array MathOps::distance_matrix_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count_a = a.size();
    int32_t count_b = b.size();
    PackedFloat32Array result;
    result.resize(count_a * count_b);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            float dx = pb[j].x - pa[i].x;
            float dy = pb[j].y - pa[i].y;
            float dz = pb[j].z - pa[i].z;
            dst[i * count_b + j] = std::sqrt(dx * dx + dy * dy + dz * dz);
        }
    }

    return result;
}

// ========== INTERPOLATION ==========

PackedVector2Array MathOps::lerp_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b, float t) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector2Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    Vector2* dst = result.ptrw();

    float one_minus_t = 1.0f - t;
    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector2(
            pa[i].x * one_minus_t + pb[i].x * t,
            pa[i].y * one_minus_t + pb[i].y * t
        );
    }

    return result;
}

PackedVector3Array MathOps::lerp_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b, float t) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    Vector3* dst = result.ptrw();

    float one_minus_t = 1.0f - t;
    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector3(
            pa[i].x * one_minus_t + pb[i].x * t,
            pa[i].y * one_minus_t + pb[i].y * t,
            pa[i].z * one_minus_t + pb[i].z * t
        );
    }

    return result;
}

PackedVector2Array MathOps::lerp_batch_2d_weights(const PackedVector2Array& a, const PackedVector2Array& b, const PackedFloat32Array& t) {
    int32_t count = std::min({a.size(), b.size(), t.size()});
    PackedVector2Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    const float* pt = t.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float ti = pt[i];
        float one_minus_t = 1.0f - ti;
        dst[i] = Vector2(
            pa[i].x * one_minus_t + pb[i].x * ti,
            pa[i].y * one_minus_t + pb[i].y * ti
        );
    }

    return result;
}

PackedVector3Array MathOps::lerp_batch_3d_weights(const PackedVector3Array& a, const PackedVector3Array& b, const PackedFloat32Array& t) {
    int32_t count = std::min({a.size(), b.size(), t.size()});
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    const float* pt = t.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float ti = pt[i];
        float one_minus_t = 1.0f - ti;
        dst[i] = Vector3(
            pa[i].x * one_minus_t + pb[i].x * ti,
            pa[i].y * one_minus_t + pb[i].y * ti,
            pa[i].z * one_minus_t + pb[i].z * ti
        );
    }

    return result;
}

// ========== VECTOR ARITHMETIC ==========

PackedVector2Array MathOps::add_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector2Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector2(pa[i].x + pb[i].x, pa[i].y + pb[i].y);
    }

    return result;
}

PackedVector3Array MathOps::add_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector3(pa[i].x + pb[i].x, pa[i].y + pb[i].y, pa[i].z + pb[i].z);
    }

    return result;
}

PackedVector2Array MathOps::sub_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector2Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector2(pa[i].x - pb[i].x, pa[i].y - pb[i].y);
    }

    return result;
}

PackedVector3Array MathOps::sub_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector3(pa[i].x - pb[i].x, pa[i].y - pb[i].y, pa[i].z - pb[i].z);
    }

    return result;
}

PackedVector2Array MathOps::scale_batch_2d(const PackedVector2Array& vectors, float scalar) {
    int32_t count = vectors.size();
    PackedVector2Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector2(src[i].x * scalar, src[i].y * scalar);
    }

    return result;
}

PackedVector3Array MathOps::scale_batch_3d(const PackedVector3Array& vectors, float scalar) {
    int32_t count = vectors.size();
    PackedVector3Array result;
    result.resize(count);

    const Vector3* src = vectors.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector3(src[i].x * scalar, src[i].y * scalar, src[i].z * scalar);
    }

    return result;
}

PackedVector2Array MathOps::scale_batch_2d_weights(const PackedVector2Array& vectors, const PackedFloat32Array& scalars) {
    int32_t count = std::min(vectors.size(), scalars.size());
    PackedVector2Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    const float* s = scalars.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector2(src[i].x * s[i], src[i].y * s[i]);
    }

    return result;
}

PackedVector3Array MathOps::scale_batch_3d_weights(const PackedVector3Array& vectors, const PackedFloat32Array& scalars) {
    int32_t count = std::min(vectors.size(), scalars.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* src = vectors.ptr();
    const float* s = scalars.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector3(src[i].x * s[i], src[i].y * s[i], src[i].z * s[i]);
    }

    return result;
}

// ========== DIRECTION CALCULATIONS ==========

PackedVector2Array MathOps::direction_batch_2d(const PackedVector2Array& from, const PackedVector2Array& to) {
    int32_t count = std::min(from.size(), to.size());
    PackedVector2Array result;
    result.resize(count);

    const Vector2* pf = from.ptr();
    const Vector2* pt = to.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dx = pt[i].x - pf[i].x;
        float dy = pt[i].y - pf[i].y;
        float len_sq = dx * dx + dy * dy;
        if (len_sq > 0.0f) {
            float inv_len = 1.0f / std::sqrt(len_sq);
            dst[i] = Vector2(dx * inv_len, dy * inv_len);
        } else {
            dst[i] = Vector2(0.0f, 0.0f);
        }
    }

    return result;
}

PackedVector3Array MathOps::direction_batch_3d(const PackedVector3Array& from, const PackedVector3Array& to) {
    int32_t count = std::min(from.size(), to.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pf = from.ptr();
    const Vector3* pt = to.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dx = pt[i].x - pf[i].x;
        float dy = pt[i].y - pf[i].y;
        float dz = pt[i].z - pf[i].z;
        float len_sq = dx * dx + dy * dy + dz * dz;
        if (len_sq > 0.0f) {
            float inv_len = 1.0f / std::sqrt(len_sq);
            dst[i] = Vector3(dx * inv_len, dy * inv_len, dz * inv_len);
        } else {
            dst[i] = Vector3(0.0f, 0.0f, 0.0f);
        }
    }

    return result;
}

// ========== TRANSFORM OPERATIONS ==========

PackedVector2Array MathOps::transform_2d_batch(const Transform2D& xform, const PackedVector2Array& points) {
    int32_t count = points.size();
    PackedVector2Array result;
    result.resize(count);

    const Vector2* src = points.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = xform.xform(src[i]);
    }

    return result;
}

PackedVector3Array MathOps::transform_3d_batch(const Transform3D& xform, const PackedVector3Array& points) {
    int32_t count = points.size();
    PackedVector3Array result;
    result.resize(count);

    const Vector3* src = points.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = xform.xform(src[i]);
    }

    return result;
}

PackedVector2Array MathOps::transform_2d_batch_inverse(const Transform2D& xform, const PackedVector2Array& points) {
    int32_t count = points.size();
    PackedVector2Array result;
    result.resize(count);

    Transform2D inv = xform.affine_inverse();
    const Vector2* src = points.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = inv.xform(src[i]);
    }

    return result;
}

PackedVector3Array MathOps::transform_3d_batch_inverse(const Transform3D& xform, const PackedVector3Array& points) {
    int32_t count = points.size();
    PackedVector3Array result;
    result.resize(count);

    Transform3D inv = xform.affine_inverse();
    const Vector3* src = points.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = inv.xform(src[i]);
    }

    return result;
}

// ========== ANGLE OPERATIONS ==========

PackedFloat32Array MathOps::angle_batch_2d(const PackedVector2Array& vectors) {
    int32_t count = vectors.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = std::atan2(src[i].y, src[i].x);
    }

    return result;
}

PackedFloat32Array MathOps::angle_between_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dot = pa[i].x * pb[i].x + pa[i].y * pb[i].y;
        float len_a = std::sqrt(pa[i].x * pa[i].x + pa[i].y * pa[i].y);
        float len_b = std::sqrt(pb[i].x * pb[i].x + pb[i].y * pb[i].y);
        float denom = len_a * len_b;
        if (denom > 0.0f) {
            float cos_angle = std::fmax(-1.0f, std::fmin(1.0f, dot / denom));
            dst[i] = std::acos(cos_angle);
        } else {
            dst[i] = 0.0f;
        }
    }

    return result;
}

PackedFloat32Array MathOps::angle_between_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dot = pa[i].x * pb[i].x + pa[i].y * pb[i].y + pa[i].z * pb[i].z;
        float len_a = std::sqrt(pa[i].x * pa[i].x + pa[i].y * pa[i].y + pa[i].z * pa[i].z);
        float len_b = std::sqrt(pb[i].x * pb[i].x + pb[i].y * pb[i].y + pb[i].z * pb[i].z);
        float denom = len_a * len_b;
        if (denom > 0.0f) {
            float cos_angle = std::fmax(-1.0f, std::fmin(1.0f, dot / denom));
            dst[i] = std::acos(cos_angle);
        } else {
            dst[i] = 0.0f;
        }
    }

    return result;
}

PackedVector2Array MathOps::from_angle_batch(const PackedFloat32Array& angles) {
    int32_t count = angles.size();
    PackedVector2Array result;
    result.resize(count);

    const float* src = angles.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector2(std::cos(src[i]), std::sin(src[i]));
    }

    return result;
}

// ========== PROJECTION & REFLECTION ==========

PackedVector2Array MathOps::project_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector2Array result;
    result.resize(count);

    const Vector2* pa = a.ptr();
    const Vector2* pb = b.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float b_len_sq = pb[i].x * pb[i].x + pb[i].y * pb[i].y;
        if (b_len_sq > 0.0f) {
            float dot = pa[i].x * pb[i].x + pa[i].y * pb[i].y;
            float scale = dot / b_len_sq;
            dst[i] = Vector2(pb[i].x * scale, pb[i].y * scale);
        } else {
            dst[i] = Vector2(0.0f, 0.0f);
        }
    }

    return result;
}

PackedVector3Array MathOps::project_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b) {
    int32_t count = std::min(a.size(), b.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pa = a.ptr();
    const Vector3* pb = b.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float b_len_sq = pb[i].x * pb[i].x + pb[i].y * pb[i].y + pb[i].z * pb[i].z;
        if (b_len_sq > 0.0f) {
            float dot = pa[i].x * pb[i].x + pa[i].y * pb[i].y + pa[i].z * pb[i].z;
            float scale = dot / b_len_sq;
            dst[i] = Vector3(pb[i].x * scale, pb[i].y * scale, pb[i].z * scale);
        } else {
            dst[i] = Vector3(0.0f, 0.0f, 0.0f);
        }
    }

    return result;
}

PackedVector2Array MathOps::reflect_batch_2d(const PackedVector2Array& vectors, const PackedVector2Array& normals) {
    int32_t count = std::min(vectors.size(), normals.size());
    PackedVector2Array result;
    result.resize(count);

    const Vector2* pv = vectors.ptr();
    const Vector2* pn = normals.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dot = pv[i].x * pn[i].x + pv[i].y * pn[i].y;
        float scale = 2.0f * dot;
        dst[i] = Vector2(pv[i].x - pn[i].x * scale, pv[i].y - pn[i].y * scale);
    }

    return result;
}

PackedVector3Array MathOps::reflect_batch_3d(const PackedVector3Array& vectors, const PackedVector3Array& normals) {
    int32_t count = std::min(vectors.size(), normals.size());
    PackedVector3Array result;
    result.resize(count);

    const Vector3* pv = vectors.ptr();
    const Vector3* pn = normals.ptr();
    Vector3* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        float dot = pv[i].x * pn[i].x + pv[i].y * pn[i].y + pv[i].z * pn[i].z;
        float scale = 2.0f * dot;
        dst[i] = Vector3(
            pv[i].x - pn[i].x * scale,
            pv[i].y - pn[i].y * scale,
            pv[i].z - pn[i].z * scale
        );
    }

    return result;
}

// ========== CLAMPING ==========

PackedVector2Array MathOps::clamp_length_batch_2d(const PackedVector2Array& vectors, float max_length) {
    int32_t count = vectors.size();
    PackedVector2Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    Vector2* dst = result.ptrw();
    float max_len_sq = max_length * max_length;

    for (int32_t i = 0; i < count; i++) {
        float len_sq = src[i].x * src[i].x + src[i].y * src[i].y;
        if (len_sq > max_len_sq && len_sq > 0.0f) {
            float scale = max_length / std::sqrt(len_sq);
            dst[i] = Vector2(src[i].x * scale, src[i].y * scale);
        } else {
            dst[i] = src[i];
        }
    }

    return result;
}

PackedVector3Array MathOps::clamp_length_batch_3d(const PackedVector3Array& vectors, float max_length) {
    int32_t count = vectors.size();
    PackedVector3Array result;
    result.resize(count);

    const Vector3* src = vectors.ptr();
    Vector3* dst = result.ptrw();
    float max_len_sq = max_length * max_length;

    for (int32_t i = 0; i < count; i++) {
        float len_sq = src[i].x * src[i].x + src[i].y * src[i].y + src[i].z * src[i].z;
        if (len_sq > max_len_sq && len_sq > 0.0f) {
            float scale = max_length / std::sqrt(len_sq);
            dst[i] = Vector3(src[i].x * scale, src[i].y * scale, src[i].z * scale);
        } else {
            dst[i] = src[i];
        }
    }

    return result;
}

PackedVector2Array MathOps::clamp_length_range_batch_2d(const PackedVector2Array& vectors, float min_length, float max_length) {
    int32_t count = vectors.size();
    PackedVector2Array result;
    result.resize(count);

    const Vector2* src = vectors.ptr();
    Vector2* dst = result.ptrw();
    float min_len_sq = min_length * min_length;
    float max_len_sq = max_length * max_length;

    for (int32_t i = 0; i < count; i++) {
        float len_sq = src[i].x * src[i].x + src[i].y * src[i].y;
        if (len_sq < min_len_sq && len_sq > 0.0f) {
            float scale = min_length / std::sqrt(len_sq);
            dst[i] = Vector2(src[i].x * scale, src[i].y * scale);
        } else if (len_sq > max_len_sq) {
            float scale = max_length / std::sqrt(len_sq);
            dst[i] = Vector2(src[i].x * scale, src[i].y * scale);
        } else {
            dst[i] = src[i];
        }
    }

    return result;
}

PackedVector3Array MathOps::clamp_length_range_batch_3d(const PackedVector3Array& vectors, float min_length, float max_length) {
    int32_t count = vectors.size();
    PackedVector3Array result;
    result.resize(count);

    const Vector3* src = vectors.ptr();
    Vector3* dst = result.ptrw();
    float min_len_sq = min_length * min_length;
    float max_len_sq = max_length * max_length;

    for (int32_t i = 0; i < count; i++) {
        float len_sq = src[i].x * src[i].x + src[i].y * src[i].y + src[i].z * src[i].z;
        if (len_sq < min_len_sq && len_sq > 0.0f) {
            float scale = min_length / std::sqrt(len_sq);
            dst[i] = Vector3(src[i].x * scale, src[i].y * scale, src[i].z * scale);
        } else if (len_sq > max_len_sq) {
            float scale = max_length / std::sqrt(len_sq);
            dst[i] = Vector3(src[i].x * scale, src[i].y * scale, src[i].z * scale);
        } else {
            dst[i] = src[i];
        }
    }

    return result;
}

}

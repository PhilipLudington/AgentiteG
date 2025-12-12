/**
 * ArrayOps Implementation
 */

#include "array_ops.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <algorithm>
#include <numeric>
#include <cmath>
#include <cstring>
#include <limits>

namespace godot {

void ArrayOps::_bind_methods() {
    // Filter operations - Float
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_gt_float", "values", "threshold"), &ArrayOps::filter_gt_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_gte_float", "values", "threshold"), &ArrayOps::filter_gte_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_lt_float", "values", "threshold"), &ArrayOps::filter_lt_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_lte_float", "values", "threshold"), &ArrayOps::filter_lte_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_eq_float", "values", "target", "epsilon"), &ArrayOps::filter_eq_float, DEFVAL(0.0001f));
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_range_float", "values", "min_val", "max_val"), &ArrayOps::filter_range_float);

    // Filter operations - Int
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_gt_int", "values", "threshold"), &ArrayOps::filter_gt_int);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_gte_int", "values", "threshold"), &ArrayOps::filter_gte_int);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_lt_int", "values", "threshold"), &ArrayOps::filter_lt_int);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_lte_int", "values", "threshold"), &ArrayOps::filter_lte_int);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_eq_int", "values", "target"), &ArrayOps::filter_eq_int);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_range_int", "values", "min_val", "max_val"), &ArrayOps::filter_range_int);

    // Filter operations - Vector
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_in_rect", "positions", "rect"), &ArrayOps::filter_in_rect);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("filter_in_radius", "positions", "origin", "radius"), &ArrayOps::filter_in_radius);

    // Sort operations
    ClassDB::bind_static_method("ArrayOps", D_METHOD("sort_floats", "values", "descending"), &ArrayOps::sort_floats, DEFVAL(false));
    ClassDB::bind_static_method("ArrayOps", D_METHOD("sort_ints", "values", "descending"), &ArrayOps::sort_ints, DEFVAL(false));
    ClassDB::bind_static_method("ArrayOps", D_METHOD("argsort_floats", "values", "descending"), &ArrayOps::argsort_floats, DEFVAL(false));
    ClassDB::bind_static_method("ArrayOps", D_METHOD("argsort_ints", "values", "descending"), &ArrayOps::argsort_ints, DEFVAL(false));
    ClassDB::bind_static_method("ArrayOps", D_METHOD("argsort_by_distance", "positions", "origin", "descending"), &ArrayOps::argsort_by_distance, DEFVAL(false));
    ClassDB::bind_static_method("ArrayOps", D_METHOD("argsort_by_distance_3d", "positions", "origin", "descending"), &ArrayOps::argsort_by_distance_3d, DEFVAL(false));

    // Reorder operations
    ClassDB::bind_static_method("ArrayOps", D_METHOD("reorder_floats", "values", "indices"), &ArrayOps::reorder_floats);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("reorder_ints", "values", "indices"), &ArrayOps::reorder_ints);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("reorder_vector2", "values", "indices"), &ArrayOps::reorder_vector2);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("reorder_vector3", "values", "indices"), &ArrayOps::reorder_vector3);

    // Reduce operations
    ClassDB::bind_static_method("ArrayOps", D_METHOD("sum_floats", "values"), &ArrayOps::sum_floats);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("sum_ints", "values"), &ArrayOps::sum_ints);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("sum_vector2", "values"), &ArrayOps::sum_vector2);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("sum_vector3", "values"), &ArrayOps::sum_vector3);

    ClassDB::bind_static_method("ArrayOps", D_METHOD("min_float", "values"), &ArrayOps::min_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("max_float", "values"), &ArrayOps::max_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("min_int", "values"), &ArrayOps::min_int);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("max_int", "values"), &ArrayOps::max_int);

    ClassDB::bind_static_method("ArrayOps", D_METHOD("argmin_floats", "values"), &ArrayOps::argmin_floats);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("argmax_floats", "values"), &ArrayOps::argmax_floats);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("argmin_ints", "values"), &ArrayOps::argmin_ints);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("argmax_ints", "values"), &ArrayOps::argmax_ints);

    ClassDB::bind_static_method("ArrayOps", D_METHOD("mean_floats", "values"), &ArrayOps::mean_floats);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("mean_vector2", "values"), &ArrayOps::mean_vector2);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("mean_vector3", "values"), &ArrayOps::mean_vector3);

    // Select operations
    ClassDB::bind_static_method("ArrayOps", D_METHOD("select_floats", "values", "indices"), &ArrayOps::select_floats);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("select_ints", "values", "indices"), &ArrayOps::select_ints);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("select_vector2", "values", "indices"), &ArrayOps::select_vector2);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("select_vector3", "values", "indices"), &ArrayOps::select_vector3);

    // Find operations
    ClassDB::bind_static_method("ArrayOps", D_METHOD("find_first_gt_float", "values", "threshold"), &ArrayOps::find_first_gt_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("find_first_eq_int", "values", "target"), &ArrayOps::find_first_eq_int);

    // Count operations
    ClassDB::bind_static_method("ArrayOps", D_METHOD("count_gt_float", "values", "threshold"), &ArrayOps::count_gt_float);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("count_eq_int", "values", "target"), &ArrayOps::count_eq_int);
    ClassDB::bind_static_method("ArrayOps", D_METHOD("count_in_radius", "positions", "origin", "radius"), &ArrayOps::count_in_radius);
}

// ========== FILTER OPERATIONS - FLOAT ==========

PackedInt32Array ArrayOps::filter_gt_float(const PackedFloat32Array& values, float threshold) {
    PackedInt32Array result;
    const float* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] > threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_gte_float(const PackedFloat32Array& values, float threshold) {
    PackedInt32Array result;
    const float* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] >= threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_lt_float(const PackedFloat32Array& values, float threshold) {
    PackedInt32Array result;
    const float* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] < threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_lte_float(const PackedFloat32Array& values, float threshold) {
    PackedInt32Array result;
    const float* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] <= threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_eq_float(const PackedFloat32Array& values, float target, float epsilon) {
    PackedInt32Array result;
    const float* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (std::abs(data[i] - target) <= epsilon) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_range_float(const PackedFloat32Array& values, float min_val, float max_val) {
    PackedInt32Array result;
    const float* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] >= min_val && data[i] <= max_val) {
            result.push_back(i);
        }
    }
    return result;
}

// ========== FILTER OPERATIONS - INT ==========

PackedInt32Array ArrayOps::filter_gt_int(const PackedInt32Array& values, int32_t threshold) {
    PackedInt32Array result;
    const int32_t* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] > threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_gte_int(const PackedInt32Array& values, int32_t threshold) {
    PackedInt32Array result;
    const int32_t* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] >= threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_lt_int(const PackedInt32Array& values, int32_t threshold) {
    PackedInt32Array result;
    const int32_t* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] < threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_lte_int(const PackedInt32Array& values, int32_t threshold) {
    PackedInt32Array result;
    const int32_t* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] <= threshold) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_eq_int(const PackedInt32Array& values, int32_t target) {
    PackedInt32Array result;
    const int32_t* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] == target) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_range_int(const PackedInt32Array& values, int32_t min_val, int32_t max_val) {
    PackedInt32Array result;
    const int32_t* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] >= min_val && data[i] <= max_val) {
            result.push_back(i);
        }
    }
    return result;
}

// ========== FILTER OPERATIONS - VECTOR ==========

PackedInt32Array ArrayOps::filter_in_rect(const PackedVector2Array& positions, const Rect2& rect) {
    PackedInt32Array result;
    const Vector2* data = positions.ptr();
    int size = positions.size();

    for (int i = 0; i < size; i++) {
        if (rect.has_point(data[i])) {
            result.push_back(i);
        }
    }
    return result;
}

PackedInt32Array ArrayOps::filter_in_radius(const PackedVector2Array& positions, const Vector2& origin, float radius) {
    PackedInt32Array result;
    const Vector2* data = positions.ptr();
    int size = positions.size();
    float radius_sq = radius * radius;

    for (int i = 0; i < size; i++) {
        if (origin.distance_squared_to(data[i]) <= radius_sq) {
            result.push_back(i);
        }
    }
    return result;
}

// ========== SORT OPERATIONS ==========

PackedFloat32Array ArrayOps::sort_floats(const PackedFloat32Array& values, bool descending) {
    int size = values.size();
    PackedFloat32Array result;
    result.resize(size);

    // Copy data
    const float* src = values.ptr();
    float* dst = result.ptrw();
    memcpy(dst, src, size * sizeof(float));

    if (descending) {
        std::sort(dst, dst + size, std::greater<float>());
    } else {
        std::sort(dst, dst + size);
    }
    return result;
}

PackedInt32Array ArrayOps::sort_ints(const PackedInt32Array& values, bool descending) {
    int size = values.size();
    PackedInt32Array result;
    result.resize(size);

    // Copy data
    const int32_t* src = values.ptr();
    int32_t* dst = result.ptrw();
    memcpy(dst, src, size * sizeof(int32_t));

    if (descending) {
        std::sort(dst, dst + size, std::greater<int32_t>());
    } else {
        std::sort(dst, dst + size);
    }
    return result;
}

PackedInt32Array ArrayOps::argsort_floats(const PackedFloat32Array& values, bool descending) {
    int size = values.size();
    std::vector<int32_t> indices(size);
    std::iota(indices.begin(), indices.end(), 0);

    const float* data = values.ptr();

    if (descending) {
        std::sort(indices.begin(), indices.end(),
            [data](int32_t a, int32_t b) { return data[a] > data[b]; });
    } else {
        std::sort(indices.begin(), indices.end(),
            [data](int32_t a, int32_t b) { return data[a] < data[b]; });
    }

    PackedInt32Array result;
    result.resize(size);
    memcpy(result.ptrw(), indices.data(), size * sizeof(int32_t));
    return result;
}

PackedInt32Array ArrayOps::argsort_ints(const PackedInt32Array& values, bool descending) {
    int size = values.size();
    std::vector<int32_t> indices(size);
    std::iota(indices.begin(), indices.end(), 0);

    const int32_t* data = values.ptr();

    if (descending) {
        std::sort(indices.begin(), indices.end(),
            [data](int32_t a, int32_t b) { return data[a] > data[b]; });
    } else {
        std::sort(indices.begin(), indices.end(),
            [data](int32_t a, int32_t b) { return data[a] < data[b]; });
    }

    PackedInt32Array result;
    result.resize(size);
    memcpy(result.ptrw(), indices.data(), size * sizeof(int32_t));
    return result;
}

PackedInt32Array ArrayOps::argsort_by_distance(const PackedVector2Array& positions, const Vector2& origin, bool descending) {
    int size = positions.size();
    std::vector<std::pair<float, int32_t>> dist_idx(size);

    const Vector2* data = positions.ptr();
    for (int i = 0; i < size; i++) {
        dist_idx[i] = {origin.distance_squared_to(data[i]), i};
    }

    if (descending) {
        std::sort(dist_idx.begin(), dist_idx.end(),
            [](const auto& a, const auto& b) { return a.first > b.first; });
    } else {
        std::sort(dist_idx.begin(), dist_idx.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
    }

    PackedInt32Array result;
    result.resize(size);
    int32_t* result_data = result.ptrw();
    for (int i = 0; i < size; i++) {
        result_data[i] = dist_idx[i].second;
    }
    return result;
}

PackedInt32Array ArrayOps::argsort_by_distance_3d(const PackedVector3Array& positions, const Vector3& origin, bool descending) {
    int size = positions.size();
    std::vector<std::pair<float, int32_t>> dist_idx(size);

    const Vector3* data = positions.ptr();
    for (int i = 0; i < size; i++) {
        dist_idx[i] = {origin.distance_squared_to(data[i]), i};
    }

    if (descending) {
        std::sort(dist_idx.begin(), dist_idx.end(),
            [](const auto& a, const auto& b) { return a.first > b.first; });
    } else {
        std::sort(dist_idx.begin(), dist_idx.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
    }

    PackedInt32Array result;
    result.resize(size);
    int32_t* result_data = result.ptrw();
    for (int i = 0; i < size; i++) {
        result_data[i] = dist_idx[i].second;
    }
    return result;
}

// ========== REORDER OPERATIONS ==========

PackedFloat32Array ArrayOps::reorder_floats(const PackedFloat32Array& values, const PackedInt32Array& indices) {
    PackedFloat32Array result;
    int size = indices.size();
    result.resize(size);

    const float* src = values.ptr();
    const int32_t* idx = indices.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

PackedInt32Array ArrayOps::reorder_ints(const PackedInt32Array& values, const PackedInt32Array& indices) {
    PackedInt32Array result;
    int size = indices.size();
    result.resize(size);

    const int32_t* src = values.ptr();
    const int32_t* idx = indices.ptr();
    int32_t* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

PackedVector2Array ArrayOps::reorder_vector2(const PackedVector2Array& values, const PackedInt32Array& indices) {
    PackedVector2Array result;
    int size = indices.size();
    result.resize(size);

    const Vector2* src = values.ptr();
    const int32_t* idx = indices.ptr();
    Vector2* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

PackedVector3Array ArrayOps::reorder_vector3(const PackedVector3Array& values, const PackedInt32Array& indices) {
    PackedVector3Array result;
    int size = indices.size();
    result.resize(size);

    const Vector3* src = values.ptr();
    const int32_t* idx = indices.ptr();
    Vector3* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

// ========== REDUCE OPERATIONS ==========

float ArrayOps::sum_floats(const PackedFloat32Array& values) {
    const float* data = values.ptr();
    int size = values.size();

    float result = 0.0f;
    for (int i = 0; i < size; i++) {
        result += data[i];
    }
    return result;
}

int64_t ArrayOps::sum_ints(const PackedInt32Array& values) {
    const int32_t* data = values.ptr();
    int size = values.size();

    int64_t result = 0;
    for (int i = 0; i < size; i++) {
        result += data[i];
    }
    return result;
}

Vector2 ArrayOps::sum_vector2(const PackedVector2Array& values) {
    const Vector2* data = values.ptr();
    int size = values.size();

    Vector2 result(0, 0);
    for (int i = 0; i < size; i++) {
        result += data[i];
    }
    return result;
}

Vector3 ArrayOps::sum_vector3(const PackedVector3Array& values) {
    const Vector3* data = values.ptr();
    int size = values.size();

    Vector3 result(0, 0, 0);
    for (int i = 0; i < size; i++) {
        result += data[i];
    }
    return result;
}

float ArrayOps::min_float(const PackedFloat32Array& values) {
    if (values.is_empty()) return 0.0f;

    const float* data = values.ptr();
    int size = values.size();

    float result = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] < result) result = data[i];
    }
    return result;
}

float ArrayOps::max_float(const PackedFloat32Array& values) {
    if (values.is_empty()) return 0.0f;

    const float* data = values.ptr();
    int size = values.size();

    float result = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] > result) result = data[i];
    }
    return result;
}

int32_t ArrayOps::min_int(const PackedInt32Array& values) {
    if (values.is_empty()) return 0;

    const int32_t* data = values.ptr();
    int size = values.size();

    int32_t result = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] < result) result = data[i];
    }
    return result;
}

int32_t ArrayOps::max_int(const PackedInt32Array& values) {
    if (values.is_empty()) return 0;

    const int32_t* data = values.ptr();
    int size = values.size();

    int32_t result = data[0];
    for (int i = 1; i < size; i++) {
        if (data[i] > result) result = data[i];
    }
    return result;
}

int32_t ArrayOps::argmin_floats(const PackedFloat32Array& values) {
    if (values.is_empty()) return -1;

    const float* data = values.ptr();
    int size = values.size();

    int32_t min_idx = 0;
    float min_val = data[0];

    for (int i = 1; i < size; i++) {
        if (data[i] < min_val) {
            min_val = data[i];
            min_idx = i;
        }
    }
    return min_idx;
}

int32_t ArrayOps::argmax_floats(const PackedFloat32Array& values) {
    if (values.is_empty()) return -1;

    const float* data = values.ptr();
    int size = values.size();

    int32_t max_idx = 0;
    float max_val = data[0];

    for (int i = 1; i < size; i++) {
        if (data[i] > max_val) {
            max_val = data[i];
            max_idx = i;
        }
    }
    return max_idx;
}

int32_t ArrayOps::argmin_ints(const PackedInt32Array& values) {
    if (values.is_empty()) return -1;

    const int32_t* data = values.ptr();
    int size = values.size();

    int32_t min_idx = 0;
    int32_t min_val = data[0];

    for (int i = 1; i < size; i++) {
        if (data[i] < min_val) {
            min_val = data[i];
            min_idx = i;
        }
    }
    return min_idx;
}

int32_t ArrayOps::argmax_ints(const PackedInt32Array& values) {
    if (values.is_empty()) return -1;

    const int32_t* data = values.ptr();
    int size = values.size();

    int32_t max_idx = 0;
    int32_t max_val = data[0];

    for (int i = 1; i < size; i++) {
        if (data[i] > max_val) {
            max_val = data[i];
            max_idx = i;
        }
    }
    return max_idx;
}

float ArrayOps::mean_floats(const PackedFloat32Array& values) {
    if (values.is_empty()) return 0.0f;
    return sum_floats(values) / static_cast<float>(values.size());
}

Vector2 ArrayOps::mean_vector2(const PackedVector2Array& values) {
    if (values.is_empty()) return Vector2(0, 0);
    return sum_vector2(values) / static_cast<float>(values.size());
}

Vector3 ArrayOps::mean_vector3(const PackedVector3Array& values) {
    if (values.is_empty()) return Vector3(0, 0, 0);
    return sum_vector3(values) / static_cast<float>(values.size());
}

// ========== SELECT OPERATIONS ==========

PackedFloat32Array ArrayOps::select_floats(const PackedFloat32Array& values, const PackedInt32Array& indices) {
    PackedFloat32Array result;
    const float* src = values.ptr();
    const int32_t* idx = indices.ptr();
    int size = indices.size();

    result.resize(size);
    float* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

PackedInt32Array ArrayOps::select_ints(const PackedInt32Array& values, const PackedInt32Array& indices) {
    PackedInt32Array result;
    const int32_t* src = values.ptr();
    const int32_t* idx = indices.ptr();
    int size = indices.size();

    result.resize(size);
    int32_t* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

PackedVector2Array ArrayOps::select_vector2(const PackedVector2Array& values, const PackedInt32Array& indices) {
    PackedVector2Array result;
    const Vector2* src = values.ptr();
    const int32_t* idx = indices.ptr();
    int size = indices.size();

    result.resize(size);
    Vector2* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

PackedVector3Array ArrayOps::select_vector3(const PackedVector3Array& values, const PackedInt32Array& indices) {
    PackedVector3Array result;
    const Vector3* src = values.ptr();
    const int32_t* idx = indices.ptr();
    int size = indices.size();

    result.resize(size);
    Vector3* dst = result.ptrw();

    for (int i = 0; i < size; i++) {
        dst[i] = src[idx[i]];
    }
    return result;
}

// ========== FIND OPERATIONS ==========

int32_t ArrayOps::find_first_gt_float(const PackedFloat32Array& values, float threshold) {
    const float* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] > threshold) {
            return i;
        }
    }
    return -1;
}

int32_t ArrayOps::find_first_eq_int(const PackedInt32Array& values, int32_t target) {
    const int32_t* data = values.ptr();
    int size = values.size();

    for (int i = 0; i < size; i++) {
        if (data[i] == target) {
            return i;
        }
    }
    return -1;
}

// ========== COUNT OPERATIONS ==========

int32_t ArrayOps::count_gt_float(const PackedFloat32Array& values, float threshold) {
    const float* data = values.ptr();
    int size = values.size();

    int32_t count = 0;
    for (int i = 0; i < size; i++) {
        if (data[i] > threshold) {
            count++;
        }
    }
    return count;
}

int32_t ArrayOps::count_eq_int(const PackedInt32Array& values, int32_t target) {
    const int32_t* data = values.ptr();
    int size = values.size();

    int32_t count = 0;
    for (int i = 0; i < size; i++) {
        if (data[i] == target) {
            count++;
        }
    }
    return count;
}

int32_t ArrayOps::count_in_radius(const PackedVector2Array& positions, const Vector2& origin, float radius) {
    const Vector2* data = positions.ptr();
    int size = positions.size();
    float radius_sq = radius * radius;

    int32_t count = 0;
    for (int i = 0; i < size; i++) {
        if (origin.distance_squared_to(data[i]) <= radius_sq) {
            count++;
        }
    }
    return count;
}

}

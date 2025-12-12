/**
 * ArrayOps - Fast operations on Godot PackedArrays
 *
 * Provides high-performance filter, sort, and reduce operations
 * that avoid the overhead of GDScript loops and intermediate arrays.
 *
 * Usage:
 *   # Filter: returns indices where condition is true
 *   var indices = ArrayOps.filter_gt(values, threshold)
 *
 *   # Sort: returns sorted array or sorted indices
 *   var sorted = ArrayOps.sort_floats(values)
 *   var indices = ArrayOps.argsort_floats(values)
 *
 *   # Reduce: aggregation operations
 *   var total = ArrayOps.sum_floats(values)
 *   var min_idx = ArrayOps.argmin_floats(values)
 */

#ifndef AGENTITE_ARRAY_OPS_HPP
#define AGENTITE_ARRAY_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_float64_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_int64_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

namespace godot {

class ArrayOps : public RefCounted {
    GDCLASS(ArrayOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // ========== FILTER OPERATIONS ==========
    // All filter operations return PackedInt32Array of indices

    // Float comparisons
    static PackedInt32Array filter_gt_float(const PackedFloat32Array& values, float threshold);
    static PackedInt32Array filter_gte_float(const PackedFloat32Array& values, float threshold);
    static PackedInt32Array filter_lt_float(const PackedFloat32Array& values, float threshold);
    static PackedInt32Array filter_lte_float(const PackedFloat32Array& values, float threshold);
    static PackedInt32Array filter_eq_float(const PackedFloat32Array& values, float target, float epsilon = 0.0001f);
    static PackedInt32Array filter_range_float(const PackedFloat32Array& values, float min_val, float max_val);

    // Int comparisons
    static PackedInt32Array filter_gt_int(const PackedInt32Array& values, int32_t threshold);
    static PackedInt32Array filter_gte_int(const PackedInt32Array& values, int32_t threshold);
    static PackedInt32Array filter_lt_int(const PackedInt32Array& values, int32_t threshold);
    static PackedInt32Array filter_lte_int(const PackedInt32Array& values, int32_t threshold);
    static PackedInt32Array filter_eq_int(const PackedInt32Array& values, int32_t target);
    static PackedInt32Array filter_range_int(const PackedInt32Array& values, int32_t min_val, int32_t max_val);

    // Vector2 filters (by component or distance)
    static PackedInt32Array filter_in_rect(const PackedVector2Array& positions, const Rect2& rect);
    static PackedInt32Array filter_in_radius(const PackedVector2Array& positions, const Vector2& origin, float radius);

    // ========== SORT OPERATIONS ==========

    // Sort and return sorted copy
    static PackedFloat32Array sort_floats(const PackedFloat32Array& values, bool descending = false);
    static PackedInt32Array sort_ints(const PackedInt32Array& values, bool descending = false);

    // Argsort: return indices that would sort the array
    static PackedInt32Array argsort_floats(const PackedFloat32Array& values, bool descending = false);
    static PackedInt32Array argsort_ints(const PackedInt32Array& values, bool descending = false);

    // Sort by distance from origin
    static PackedInt32Array argsort_by_distance(const PackedVector2Array& positions, const Vector2& origin, bool descending = false);
    static PackedInt32Array argsort_by_distance_3d(const PackedVector3Array& positions, const Vector3& origin, bool descending = false);

    // Reorder array by indices (apply argsort result)
    static PackedFloat32Array reorder_floats(const PackedFloat32Array& values, const PackedInt32Array& indices);
    static PackedInt32Array reorder_ints(const PackedInt32Array& values, const PackedInt32Array& indices);
    static PackedVector2Array reorder_vector2(const PackedVector2Array& values, const PackedInt32Array& indices);
    static PackedVector3Array reorder_vector3(const PackedVector3Array& values, const PackedInt32Array& indices);

    // ========== REDUCE OPERATIONS ==========

    // Sum
    static float sum_floats(const PackedFloat32Array& values);
    static int64_t sum_ints(const PackedInt32Array& values);
    static Vector2 sum_vector2(const PackedVector2Array& values);
    static Vector3 sum_vector3(const PackedVector3Array& values);

    // Min/Max value
    static float min_float(const PackedFloat32Array& values);
    static float max_float(const PackedFloat32Array& values);
    static int32_t min_int(const PackedInt32Array& values);
    static int32_t max_int(const PackedInt32Array& values);

    // Argmin/Argmax (index of min/max)
    static int32_t argmin_floats(const PackedFloat32Array& values);
    static int32_t argmax_floats(const PackedFloat32Array& values);
    static int32_t argmin_ints(const PackedInt32Array& values);
    static int32_t argmax_ints(const PackedInt32Array& values);

    // Average
    static float mean_floats(const PackedFloat32Array& values);
    static Vector2 mean_vector2(const PackedVector2Array& values);
    static Vector3 mean_vector3(const PackedVector3Array& values);

    // ========== UTILITY OPERATIONS ==========

    // Select elements by indices
    static PackedFloat32Array select_floats(const PackedFloat32Array& values, const PackedInt32Array& indices);
    static PackedInt32Array select_ints(const PackedInt32Array& values, const PackedInt32Array& indices);
    static PackedVector2Array select_vector2(const PackedVector2Array& values, const PackedInt32Array& indices);
    static PackedVector3Array select_vector3(const PackedVector3Array& values, const PackedInt32Array& indices);

    // Find first index matching condition (returns -1 if not found)
    static int32_t find_first_gt_float(const PackedFloat32Array& values, float threshold);
    static int32_t find_first_eq_int(const PackedInt32Array& values, int32_t target);

    // Count matching
    static int32_t count_gt_float(const PackedFloat32Array& values, float threshold);
    static int32_t count_eq_int(const PackedInt32Array& values, int32_t target);
    static int32_t count_in_radius(const PackedVector2Array& positions, const Vector2& origin, float radius);
};

}

#endif // AGENTITE_ARRAY_OPS_HPP

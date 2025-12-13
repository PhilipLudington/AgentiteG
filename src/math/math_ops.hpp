/**
 * MathOps - SIMD-accelerated math operations
 *
 * Provides high-performance batch vector and matrix operations
 * that avoid the overhead of GDScript loops.
 *
 * Usage:
 *   # Normalize many vectors at once
 *   var normals = MathOps.normalize_batch_2d(vectors)
 *
 *   # Batch dot products
 *   var dots = MathOps.dot_batch_2d(vectors_a, vectors_b)
 *
 *   # Transform many points
 *   var transformed = MathOps.transform_2d_batch(xform, points)
 */

#ifndef AGENTITE_MATH_OPS_HPP
#define AGENTITE_MATH_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/transform2d.hpp>
#include <godot_cpp/variant/transform3d.hpp>

namespace godot {

class MathOps : public RefCounted {
    GDCLASS(MathOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // ========== VECTOR NORMALIZATION ==========
    // Normalize vectors (safe: zero vectors become zero)
    static PackedVector2Array normalize_batch_2d(const PackedVector2Array& vectors);
    static PackedVector3Array normalize_batch_3d(const PackedVector3Array& vectors);

    // ========== DOT PRODUCTS ==========
    // Pairwise dot products: result[i] = a[i].dot(b[i])
    static PackedFloat32Array dot_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedFloat32Array dot_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b);

    // ========== CROSS PRODUCTS ==========
    // 3D cross products: result[i] = a[i].cross(b[i])
    static PackedVector3Array cross_batch(const PackedVector3Array& a, const PackedVector3Array& b);
    // 2D "cross" (perpendicular dot / z-component of 3D cross)
    static PackedFloat32Array cross_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);

    // ========== VECTOR LENGTHS ==========
    // Length of each vector
    static PackedFloat32Array length_batch_2d(const PackedVector2Array& vectors);
    static PackedFloat32Array length_batch_3d(const PackedVector3Array& vectors);
    // Length squared (faster, no sqrt)
    static PackedFloat32Array length_squared_batch_2d(const PackedVector2Array& vectors);
    static PackedFloat32Array length_squared_batch_3d(const PackedVector3Array& vectors);

    // ========== DISTANCE CALCULATIONS ==========
    // Pairwise distances: result[i] = a[i].distance_to(b[i])
    static PackedFloat32Array distance_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedFloat32Array distance_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b);
    // Squared distances (faster)
    static PackedFloat32Array distance_squared_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedFloat32Array distance_squared_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b);

    // Distance matrices: result[i*b.size() + j] = a[i].distance_to(b[j])
    // Returns flat array in row-major order
    static PackedFloat32Array distance_matrix_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedFloat32Array distance_matrix_3d(const PackedVector3Array& a, const PackedVector3Array& b);

    // ========== INTERPOLATION ==========
    // Lerp with uniform t: result[i] = a[i].lerp(b[i], t)
    static PackedVector2Array lerp_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b, float t);
    static PackedVector3Array lerp_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b, float t);
    // Lerp with per-element t: result[i] = a[i].lerp(b[i], t[i])
    static PackedVector2Array lerp_batch_2d_weights(const PackedVector2Array& a, const PackedVector2Array& b, const PackedFloat32Array& t);
    static PackedVector3Array lerp_batch_3d_weights(const PackedVector3Array& a, const PackedVector3Array& b, const PackedFloat32Array& t);

    // ========== VECTOR ARITHMETIC ==========
    // Element-wise addition
    static PackedVector2Array add_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedVector3Array add_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b);
    // Element-wise subtraction
    static PackedVector2Array sub_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedVector3Array sub_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b);
    // Scalar multiplication
    static PackedVector2Array scale_batch_2d(const PackedVector2Array& vectors, float scalar);
    static PackedVector3Array scale_batch_3d(const PackedVector3Array& vectors, float scalar);
    // Per-element scalar multiplication
    static PackedVector2Array scale_batch_2d_weights(const PackedVector2Array& vectors, const PackedFloat32Array& scalars);
    static PackedVector3Array scale_batch_3d_weights(const PackedVector3Array& vectors, const PackedFloat32Array& scalars);

    // ========== DIRECTION CALCULATIONS ==========
    // Direction from a to b (normalized): result[i] = (b[i] - a[i]).normalized()
    static PackedVector2Array direction_batch_2d(const PackedVector2Array& from, const PackedVector2Array& to);
    static PackedVector3Array direction_batch_3d(const PackedVector3Array& from, const PackedVector3Array& to);

    // ========== TRANSFORM OPERATIONS ==========
    // Apply Transform2D to all points
    static PackedVector2Array transform_2d_batch(const Transform2D& xform, const PackedVector2Array& points);
    // Apply Transform3D to all points
    static PackedVector3Array transform_3d_batch(const Transform3D& xform, const PackedVector3Array& points);
    // Apply inverse transform
    static PackedVector2Array transform_2d_batch_inverse(const Transform2D& xform, const PackedVector2Array& points);
    static PackedVector3Array transform_3d_batch_inverse(const Transform3D& xform, const PackedVector3Array& points);

    // ========== ANGLE OPERATIONS ==========
    // Angle of each 2D vector (atan2)
    static PackedFloat32Array angle_batch_2d(const PackedVector2Array& vectors);
    // Angle between pairs of vectors
    static PackedFloat32Array angle_between_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedFloat32Array angle_between_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b);
    // Create unit vectors from angles
    static PackedVector2Array from_angle_batch(const PackedFloat32Array& angles);

    // ========== PROJECTION & REFLECTION ==========
    // Project a onto b: result[i] = a[i].project(b[i])
    static PackedVector2Array project_batch_2d(const PackedVector2Array& a, const PackedVector2Array& b);
    static PackedVector3Array project_batch_3d(const PackedVector3Array& a, const PackedVector3Array& b);
    // Reflect a around normal n: result[i] = a[i].reflect(n[i])
    static PackedVector2Array reflect_batch_2d(const PackedVector2Array& vectors, const PackedVector2Array& normals);
    static PackedVector3Array reflect_batch_3d(const PackedVector3Array& vectors, const PackedVector3Array& normals);

    // ========== CLAMPING ==========
    // Clamp vector lengths
    static PackedVector2Array clamp_length_batch_2d(const PackedVector2Array& vectors, float max_length);
    static PackedVector3Array clamp_length_batch_3d(const PackedVector3Array& vectors, float max_length);
    // Clamp vector lengths with min and max
    static PackedVector2Array clamp_length_range_batch_2d(const PackedVector2Array& vectors, float min_length, float max_length);
    static PackedVector3Array clamp_length_range_batch_3d(const PackedVector3Array& vectors, float min_length, float max_length);
};

}

#endif // AGENTITE_MATH_OPS_HPP

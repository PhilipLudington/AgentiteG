/**
 * InterpolationOps - Batch interpolation and curve operations
 *
 * High-performance easing functions, bezier curves, and spline evaluation.
 * All operations work on arrays for batch processing.
 *
 * Ideal for:
 * - Animation systems
 * - Particle interpolation
 * - Camera paths
 * - Procedural motion
 *
 * Usage:
 *   # Batch easing
 *   var t_values = PackedFloat32Array([0.0, 0.25, 0.5, 0.75, 1.0])
 *   var eased = InterpolationOps.ease_out_quad(t_values)
 *
 *   # Bezier curves
 *   var points = InterpolationOps.bezier_cubic(p0, p1, p2, p3, t_values)
 */

#ifndef AGENTITE_INTERPOLATION_OPS_HPP
#define AGENTITE_INTERPOLATION_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class InterpolationOps : public RefCounted {
    GDCLASS(InterpolationOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // === Easing Functions (batch) ===
    // All take t values in [0,1] and return eased values

    // Quadratic
    static PackedFloat32Array ease_in_quad(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_quad(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_quad(const PackedFloat32Array& t);

    // Cubic
    static PackedFloat32Array ease_in_cubic(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_cubic(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_cubic(const PackedFloat32Array& t);

    // Quartic
    static PackedFloat32Array ease_in_quart(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_quart(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_quart(const PackedFloat32Array& t);

    // Quintic
    static PackedFloat32Array ease_in_quint(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_quint(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_quint(const PackedFloat32Array& t);

    // Sine
    static PackedFloat32Array ease_in_sine(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_sine(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_sine(const PackedFloat32Array& t);

    // Exponential
    static PackedFloat32Array ease_in_expo(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_expo(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_expo(const PackedFloat32Array& t);

    // Circular
    static PackedFloat32Array ease_in_circ(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_circ(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_circ(const PackedFloat32Array& t);

    // Back (overshoots)
    static PackedFloat32Array ease_in_back(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_back(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_back(const PackedFloat32Array& t);

    // Elastic
    static PackedFloat32Array ease_in_elastic(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_elastic(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_elastic(const PackedFloat32Array& t);

    // Bounce
    static PackedFloat32Array ease_in_bounce(const PackedFloat32Array& t);
    static PackedFloat32Array ease_out_bounce(const PackedFloat32Array& t);
    static PackedFloat32Array ease_in_out_bounce(const PackedFloat32Array& t);

    // === Bezier Curves ===

    // Quadratic bezier (3 control points)
    static PackedVector2Array bezier_quadratic_2d(const Vector2& p0, const Vector2& p1,
                                                   const Vector2& p2, const PackedFloat32Array& t);
    static PackedVector3Array bezier_quadratic_3d(const Vector3& p0, const Vector3& p1,
                                                   const Vector3& p2, const PackedFloat32Array& t);

    // Cubic bezier (4 control points)
    static PackedVector2Array bezier_cubic_2d(const Vector2& p0, const Vector2& p1,
                                               const Vector2& p2, const Vector2& p3,
                                               const PackedFloat32Array& t);
    static PackedVector3Array bezier_cubic_3d(const Vector3& p0, const Vector3& p1,
                                               const Vector3& p2, const Vector3& p3,
                                               const PackedFloat32Array& t);

    // === Splines ===

    // Catmull-Rom spline (smooth curve through control points)
    static PackedVector2Array catmull_rom_2d(const PackedVector2Array& control_points,
                                              int32_t samples_per_segment);
    static PackedVector3Array catmull_rom_3d(const PackedVector3Array& control_points,
                                              int32_t samples_per_segment);

    // B-spline
    static PackedVector2Array bspline_2d(const PackedVector2Array& control_points,
                                          int32_t degree, int32_t total_samples);

    // === Utility Functions ===

    // Remap values from one range to another
    static PackedFloat32Array remap(const PackedFloat32Array& values,
                                     float in_min, float in_max,
                                     float out_min, float out_max);

    // Clamp values to a range
    static PackedFloat32Array clamp_array(const PackedFloat32Array& values,
                                           float min_val, float max_val);

    // Smoothstep (Hermite interpolation)
    static PackedFloat32Array smoothstep(const PackedFloat32Array& t);

    // Smoother step (Ken Perlin's improved version)
    static PackedFloat32Array smootherstep(const PackedFloat32Array& t);

    // Linear interpolation batch
    static PackedFloat32Array lerp_floats(const PackedFloat32Array& a, const PackedFloat32Array& b,
                                           float t);
    static PackedFloat32Array lerp_floats_array(const PackedFloat32Array& a, const PackedFloat32Array& b,
                                                 const PackedFloat32Array& t);

    // Generate evenly spaced t values
    static PackedFloat32Array linspace(float start, float end, int32_t count);
};

}

#endif // AGENTITE_INTERPOLATION_OPS_HPP

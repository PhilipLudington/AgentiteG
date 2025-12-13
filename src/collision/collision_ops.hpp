/**
 * CollisionOps - Batch collision detection primitives
 *
 * Provides high-performance collision detection for common game scenarios.
 * All operations work with PackedArrays and return indices of colliding elements.
 *
 * Usage:
 *   # Find which points are inside a circle
 *   var inside = CollisionOps.points_in_circle(positions, center, radius)
 *
 *   # Check for circle-circle collisions
 *   var pairs = CollisionOps.circles_vs_circles(centers_a, radii_a, centers_b, radii_b)
 *   # pairs is [i0, j0, i1, j1, ...] where i is from set A, j is from set B
 *
 *   # Self-collision within a group
 *   var pairs = CollisionOps.circles_self_collision(centers, radii)
 */

#ifndef AGENTITE_COLLISION_OPS_HPP
#define AGENTITE_COLLISION_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/aabb.hpp>

namespace godot {

class CollisionOps : public RefCounted {
    GDCLASS(CollisionOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // ========== POINT CONTAINMENT TESTS ==========
    // Returns indices of points inside the shape

    // Points inside a 2D rectangle
    static PackedInt32Array points_in_rect(const PackedVector2Array& points, const Rect2& rect);

    // Points inside a circle
    static PackedInt32Array points_in_circle(const PackedVector2Array& points, const Vector2& center, float radius);

    // Points inside a convex or concave polygon (uses ray casting)
    static PackedInt32Array points_in_polygon(const PackedVector2Array& points, const PackedVector2Array& polygon);

    // Points inside a 3D AABB
    static PackedInt32Array points_in_aabb(const PackedVector3Array& points, const AABB& box);

    // Points inside a sphere
    static PackedInt32Array points_in_sphere(const PackedVector3Array& points, const Vector3& center, float radius);

    // ========== SHAPE VS SHAPE COLLISION ==========
    // Returns collision pairs as flat array [i0, j0, i1, j1, ...]
    // where i is index from first set, j is index from second set

    // Circle vs circle (different radii per circle)
    static PackedInt32Array circles_vs_circles(
        const PackedVector2Array& centers_a, const PackedFloat32Array& radii_a,
        const PackedVector2Array& centers_b, const PackedFloat32Array& radii_b);

    // Circle vs circle (uniform radius for each set)
    static PackedInt32Array circles_vs_circles_uniform(
        const PackedVector2Array& centers_a, float radius_a,
        const PackedVector2Array& centers_b, float radius_b);

    // 2D AABB vs AABB
    static PackedInt32Array aabb_vs_aabb_2d(
        const PackedVector2Array& mins_a, const PackedVector2Array& maxs_a,
        const PackedVector2Array& mins_b, const PackedVector2Array& maxs_b);

    // 3D AABB vs AABB
    static PackedInt32Array aabb_vs_aabb_3d(
        const PackedVector3Array& mins_a, const PackedVector3Array& maxs_a,
        const PackedVector3Array& mins_b, const PackedVector3Array& maxs_b);

    // Sphere vs sphere (different radii)
    static PackedInt32Array spheres_vs_spheres(
        const PackedVector3Array& centers_a, const PackedFloat32Array& radii_a,
        const PackedVector3Array& centers_b, const PackedFloat32Array& radii_b);

    // Sphere vs sphere (uniform radius)
    static PackedInt32Array spheres_vs_spheres_uniform(
        const PackedVector3Array& centers_a, float radius_a,
        const PackedVector3Array& centers_b, float radius_b);

    // ========== SELF-COLLISION ==========
    // Returns collision pairs within the same set
    // Each pair appears once (not duplicated as [i,j] and [j,i])

    // Circle self-collision (different radii)
    static PackedInt32Array circles_self_collision(
        const PackedVector2Array& centers, const PackedFloat32Array& radii);

    // Circle self-collision (uniform radius)
    static PackedInt32Array circles_self_collision_uniform(
        const PackedVector2Array& centers, float radius);

    // Sphere self-collision (different radii)
    static PackedInt32Array spheres_self_collision(
        const PackedVector3Array& centers, const PackedFloat32Array& radii);

    // Sphere self-collision (uniform radius)
    static PackedInt32Array spheres_self_collision_uniform(
        const PackedVector3Array& centers, float radius);

    // ========== RAY INTERSECTION ==========

    // Ray vs circles: returns distance to each circle (INF if no hit)
    static PackedFloat32Array ray_vs_circles(
        const Vector2& origin, const Vector2& direction,
        const PackedVector2Array& centers, const PackedFloat32Array& radii);

    // Ray vs circles with uniform radius
    static PackedFloat32Array ray_vs_circles_uniform(
        const Vector2& origin, const Vector2& direction,
        const PackedVector2Array& centers, float radius);

    // Ray vs 2D AABBs: returns distance to each AABB (INF if no hit)
    static PackedFloat32Array ray_vs_aabbs_2d(
        const Vector2& origin, const Vector2& direction,
        const PackedVector2Array& mins, const PackedVector2Array& maxs);

    // Ray vs spheres: returns distance to each sphere (INF if no hit)
    static PackedFloat32Array ray_vs_spheres(
        const Vector3& origin, const Vector3& direction,
        const PackedVector3Array& centers, const PackedFloat32Array& radii);

    // Find first circle hit by ray (returns index, or -1 if none)
    static int ray_first_circle(
        const Vector2& origin, const Vector2& direction, float max_distance,
        const PackedVector2Array& centers, const PackedFloat32Array& radii);

    // Find first sphere hit by ray (returns index, or -1 if none)
    static int ray_first_sphere(
        const Vector3& origin, const Vector3& direction, float max_distance,
        const PackedVector3Array& centers, const PackedFloat32Array& radii);

    // ========== SEGMENT INTERSECTION ==========

    // Check which segment pairs intersect
    // Returns pairs [i0, j0, i1, j1, ...] where segments i from A and j from B intersect
    static PackedInt32Array segments_intersect(
        const PackedVector2Array& starts_a, const PackedVector2Array& ends_a,
        const PackedVector2Array& starts_b, const PackedVector2Array& ends_b);

    // Get intersection points for intersecting segment pairs
    // Returns points in same order as segments_intersect pairs
    static PackedVector2Array segment_intersection_points(
        const PackedVector2Array& starts_a, const PackedVector2Array& ends_a,
        const PackedVector2Array& starts_b, const PackedVector2Array& ends_b);

    // Segment self-intersection (within same set)
    static PackedInt32Array segments_self_intersect(
        const PackedVector2Array& starts, const PackedVector2Array& ends);

    // ========== CLOSEST POINT QUERIES ==========

    // Find closest point on each segment to each query point
    // Returns points array of size points.size() * segments.size() in row-major order
    static PackedVector2Array closest_points_on_segments(
        const PackedVector2Array& points,
        const PackedVector2Array& seg_starts, const PackedVector2Array& seg_ends);

    // Find index of closest circle to each point
    static PackedInt32Array closest_circle_indices(
        const PackedVector2Array& points,
        const PackedVector2Array& centers, const PackedFloat32Array& radii);

    // Find index of closest circle (uniform radius)
    static PackedInt32Array closest_circle_indices_uniform(
        const PackedVector2Array& points,
        const PackedVector2Array& centers, float radius);
};

}

#endif // AGENTITE_COLLISION_OPS_HPP

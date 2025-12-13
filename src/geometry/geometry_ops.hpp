/**
 * GeometryOps - Computational geometry algorithms
 *
 * Provides high-performance geometry operations for common game scenarios.
 * Operations include convex hull, triangulation, polygon analysis, and simplification.
 *
 * Usage:
 *   # Compute convex hull of points
 *   var hull = GeometryOps.convex_hull(points)
 *
 *   # Triangulate a polygon
 *   var triangles = GeometryOps.triangulate(polygon)
 *   # triangles is [i0, i1, i2, i3, i4, i5, ...] - indices into polygon
 *
 *   # Simplify a polyline
 *   var simplified = GeometryOps.simplify_rdp(polyline, epsilon)
 */

#ifndef AGENTITE_GEOMETRY_OPS_HPP
#define AGENTITE_GEOMETRY_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/rect2.hpp>

namespace godot {

class GeometryOps : public RefCounted {
    GDCLASS(GeometryOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // ========== CONVEX HULL ==========

    // Returns points on the convex hull in counter-clockwise order
    static PackedVector2Array convex_hull(const PackedVector2Array& points);

    // Returns indices of points on the convex hull
    static PackedInt32Array convex_hull_indices(const PackedVector2Array& points);

    // ========== TRIANGULATION ==========

    // Ear clipping triangulation of a simple polygon
    // Returns indices [i0, i1, i2, i3, i4, i5, ...] forming triangles
    static PackedInt32Array triangulate(const PackedVector2Array& polygon);

    // Delaunay triangulation of a point set
    // Returns indices [i0, i1, i2, ...] forming triangles
    static PackedInt32Array delaunay(const PackedVector2Array& points);

    // ========== VORONOI ==========

    // Returns Array of PackedVector2Array, one polygon per input point
    static Array voronoi_cells(const PackedVector2Array& points, const Rect2& bounds);

    // Returns edges as flat array [x1, y1, x2, y2, ...]
    static PackedFloat32Array voronoi_edges(const PackedVector2Array& points, const Rect2& bounds);

    // ========== POLYGON PROPERTIES ==========

    // Signed area (positive = counter-clockwise, negative = clockwise)
    static float polygon_signed_area(const PackedVector2Array& polygon);

    // Absolute area
    static float polygon_area(const PackedVector2Array& polygon);

    // Centroid (center of mass)
    static Vector2 polygon_centroid(const PackedVector2Array& polygon);

    // Bounding box
    static Rect2 polygon_bounds(const PackedVector2Array& polygon);

    // Perimeter length
    static float polygon_perimeter(const PackedVector2Array& polygon);

    // ========== POLYGON TESTS ==========

    // Check if polygon is convex
    static bool polygon_is_convex(const PackedVector2Array& polygon);

    // Check if polygon is clockwise (negative signed area)
    static bool polygon_is_clockwise(const PackedVector2Array& polygon);

    // Check if polygon is simple (no self-intersections)
    static bool polygon_is_simple(const PackedVector2Array& polygon);

    // Check if point is inside polygon
    static bool point_in_polygon(const Vector2& point, const PackedVector2Array& polygon);

    // ========== POLYGON OPERATIONS ==========

    // Reverse winding order
    static PackedVector2Array polygon_reverse(const PackedVector2Array& polygon);

    // Ensure counter-clockwise winding
    static PackedVector2Array polygon_make_ccw(const PackedVector2Array& polygon);

    // Ensure clockwise winding
    static PackedVector2Array polygon_make_cw(const PackedVector2Array& polygon);

    // ========== POLYLINE SIMPLIFICATION ==========

    // Ramer-Douglas-Peucker simplification
    // epsilon: maximum distance from original line allowed
    static PackedVector2Array simplify_rdp(const PackedVector2Array& polyline, float epsilon);

    // Returns indices of kept points
    static PackedInt32Array simplify_rdp_indices(const PackedVector2Array& polyline, float epsilon);

    // Visvalingam-Whyatt simplification (area-based)
    // target_count: number of points to keep
    static PackedVector2Array simplify_visvalingam(const PackedVector2Array& polyline, int target_count);

    // ========== POLYGON OFFSET / BUFFER ==========

    // Offset polygon by distance (positive = outward, negative = inward)
    // Works best for convex polygons
    static PackedVector2Array offset_polygon(const PackedVector2Array& polygon, float distance);

    // ========== MINKOWSKI OPERATIONS ==========

    // Minkowski sum of two convex polygons
    static PackedVector2Array minkowski_sum(const PackedVector2Array& a, const PackedVector2Array& b);

    // Minkowski difference (A - B = A + (-B))
    static PackedVector2Array minkowski_diff(const PackedVector2Array& a, const PackedVector2Array& b);

    // ========== DISTANCE QUERIES ==========

    // Distance from point to line segment
    static float point_to_segment_distance(const Vector2& point, const Vector2& seg_start, const Vector2& seg_end);

    // Distance from point to polygon boundary
    static float point_to_polygon_distance(const Vector2& point, const PackedVector2Array& polygon);

    // Batch: distances from each point to polygon boundary
    static PackedFloat32Array distances_to_polygon(const PackedVector2Array& points, const PackedVector2Array& polygon);

    // Closest point on polygon boundary to query point
    static Vector2 closest_point_on_polygon(const Vector2& point, const PackedVector2Array& polygon);

    // ========== LINE / SEGMENT UTILITIES ==========

    // Side of point relative to line (positive = left, negative = right, 0 = on line)
    static float point_line_side(const Vector2& point, const Vector2& line_start, const Vector2& line_end);

    // Project point onto line (returns parameter t, 0=start, 1=end)
    static float project_point_to_line(const Vector2& point, const Vector2& line_start, const Vector2& line_end);

    // Batch project points onto line
    static PackedFloat32Array project_points_to_line(const PackedVector2Array& points, const Vector2& line_start, const Vector2& line_end);
};

}

#endif // AGENTITE_GEOMETRY_OPS_HPP

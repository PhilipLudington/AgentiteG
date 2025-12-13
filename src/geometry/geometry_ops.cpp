/**
 * GeometryOps Implementation
 */

#include "geometry_ops.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <cmath>
#include <vector>
#include <queue>
#include <limits>

namespace godot {

void GeometryOps::_bind_methods() {
    // Convex hull
    ClassDB::bind_static_method("GeometryOps", D_METHOD("convex_hull", "points"), &GeometryOps::convex_hull);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("convex_hull_indices", "points"), &GeometryOps::convex_hull_indices);

    // Triangulation
    ClassDB::bind_static_method("GeometryOps", D_METHOD("triangulate", "polygon"), &GeometryOps::triangulate);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("delaunay", "points"), &GeometryOps::delaunay);

    // Voronoi
    ClassDB::bind_static_method("GeometryOps", D_METHOD("voronoi_cells", "points", "bounds"), &GeometryOps::voronoi_cells);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("voronoi_edges", "points", "bounds"), &GeometryOps::voronoi_edges);

    // Polygon properties
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_signed_area", "polygon"), &GeometryOps::polygon_signed_area);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_area", "polygon"), &GeometryOps::polygon_area);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_centroid", "polygon"), &GeometryOps::polygon_centroid);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_bounds", "polygon"), &GeometryOps::polygon_bounds);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_perimeter", "polygon"), &GeometryOps::polygon_perimeter);

    // Polygon tests
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_is_convex", "polygon"), &GeometryOps::polygon_is_convex);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_is_clockwise", "polygon"), &GeometryOps::polygon_is_clockwise);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_is_simple", "polygon"), &GeometryOps::polygon_is_simple);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("point_in_polygon", "point", "polygon"), &GeometryOps::point_in_polygon);

    // Polygon operations
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_reverse", "polygon"), &GeometryOps::polygon_reverse);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_make_ccw", "polygon"), &GeometryOps::polygon_make_ccw);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("polygon_make_cw", "polygon"), &GeometryOps::polygon_make_cw);

    // Simplification
    ClassDB::bind_static_method("GeometryOps", D_METHOD("simplify_rdp", "polyline", "epsilon"), &GeometryOps::simplify_rdp);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("simplify_rdp_indices", "polyline", "epsilon"), &GeometryOps::simplify_rdp_indices);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("simplify_visvalingam", "polyline", "target_count"), &GeometryOps::simplify_visvalingam);

    // Offset
    ClassDB::bind_static_method("GeometryOps", D_METHOD("offset_polygon", "polygon", "distance"), &GeometryOps::offset_polygon);

    // Minkowski
    ClassDB::bind_static_method("GeometryOps", D_METHOD("minkowski_sum", "a", "b"), &GeometryOps::minkowski_sum);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("minkowski_diff", "a", "b"), &GeometryOps::minkowski_diff);

    // Distance queries
    ClassDB::bind_static_method("GeometryOps", D_METHOD("point_to_segment_distance", "point", "seg_start", "seg_end"), &GeometryOps::point_to_segment_distance);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("point_to_polygon_distance", "point", "polygon"), &GeometryOps::point_to_polygon_distance);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("distances_to_polygon", "points", "polygon"), &GeometryOps::distances_to_polygon);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("closest_point_on_polygon", "point", "polygon"), &GeometryOps::closest_point_on_polygon);

    // Line utilities
    ClassDB::bind_static_method("GeometryOps", D_METHOD("point_line_side", "point", "line_start", "line_end"), &GeometryOps::point_line_side);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("project_point_to_line", "point", "line_start", "line_end"), &GeometryOps::project_point_to_line);
    ClassDB::bind_static_method("GeometryOps", D_METHOD("project_points_to_line", "points", "line_start", "line_end"), &GeometryOps::project_points_to_line);
}

// ========== HELPER FUNCTIONS ==========

// Cross product of 2D vectors (returns z component)
static inline float cross_2d(float ax, float ay, float bx, float by) {
    return ax * by - ay * bx;
}

// Cross product of vectors OA and OB
static inline float cross_oab(const Vector2& o, const Vector2& a, const Vector2& b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

// ========== CONVEX HULL ==========

PackedVector2Array GeometryOps::convex_hull(const PackedVector2Array& points) {
    PackedVector2Array result;
    int32_t n = points.size();

    if (n < 3) {
        return points;
    }

    // Copy and sort points
    std::vector<Vector2> pts(n);
    for (int32_t i = 0; i < n; i++) {
        pts[i] = points[i];
    }

    std::sort(pts.begin(), pts.end(), [](const Vector2& a, const Vector2& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });

    // Build lower hull
    std::vector<Vector2> hull;
    for (int32_t i = 0; i < n; i++) {
        while (hull.size() >= 2 && cross_oab(hull[hull.size()-2], hull[hull.size()-1], pts[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    // Build upper hull
    int32_t lower_size = hull.size();
    for (int32_t i = n - 2; i >= 0; i--) {
        while (hull.size() > lower_size && cross_oab(hull[hull.size()-2], hull[hull.size()-1], pts[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    hull.pop_back();  // Remove last point (duplicate of first)

    result.resize(hull.size());
    for (size_t i = 0; i < hull.size(); i++) {
        result[i] = hull[i];
    }

    return result;
}

PackedInt32Array GeometryOps::convex_hull_indices(const PackedVector2Array& points) {
    PackedInt32Array result;
    int32_t n = points.size();

    if (n < 3) {
        for (int32_t i = 0; i < n; i++) {
            result.push_back(i);
        }
        return result;
    }

    const Vector2* p = points.ptr();

    // Create index array and sort
    std::vector<int32_t> idx(n);
    for (int32_t i = 0; i < n; i++) {
        idx[i] = i;
    }

    std::sort(idx.begin(), idx.end(), [p](int32_t a, int32_t b) {
        return p[a].x < p[b].x || (p[a].x == p[b].x && p[a].y < p[b].y);
    });

    // Build lower hull
    std::vector<int32_t> hull;
    for (int32_t i = 0; i < n; i++) {
        while (hull.size() >= 2 && cross_oab(p[hull[hull.size()-2]], p[hull[hull.size()-1]], p[idx[i]]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(idx[i]);
    }

    // Build upper hull
    int32_t lower_size = hull.size();
    for (int32_t i = n - 2; i >= 0; i--) {
        while (hull.size() > lower_size && cross_oab(p[hull[hull.size()-2]], p[hull[hull.size()-1]], p[idx[i]]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(idx[i]);
    }

    hull.pop_back();

    result.resize(hull.size());
    for (size_t i = 0; i < hull.size(); i++) {
        result[i] = hull[i];
    }

    return result;
}

// ========== TRIANGULATION ==========

PackedInt32Array GeometryOps::triangulate(const PackedVector2Array& polygon) {
    PackedInt32Array result;
    int32_t n = polygon.size();

    if (n < 3) {
        return result;
    }

    const Vector2* p = polygon.ptr();

    // Create index list
    std::vector<int32_t> indices(n);
    for (int32_t i = 0; i < n; i++) {
        indices[i] = i;
    }

    // Ensure CCW winding
    float signed_area = 0.0f;
    for (int32_t i = 0; i < n; i++) {
        int32_t j = (i + 1) % n;
        signed_area += (p[j].x - p[i].x) * (p[j].y + p[i].y);
    }
    if (signed_area > 0) {
        std::reverse(indices.begin(), indices.end());
    }

    // Ear clipping
    while (indices.size() > 2) {
        bool found_ear = false;

        for (size_t i = 0; i < indices.size(); i++) {
            size_t prev = (i + indices.size() - 1) % indices.size();
            size_t next = (i + 1) % indices.size();

            Vector2 a = p[indices[prev]];
            Vector2 b = p[indices[i]];
            Vector2 c = p[indices[next]];

            // Check if ear (convex vertex)
            // For CCW polygon, convex vertices have positive cross product
            if (cross_oab(a, b, c) <= 0) {
                continue;  // Reflex vertex or collinear, not an ear
            }

            // Check if any other vertex is inside triangle
            bool is_ear = true;
            for (size_t j = 0; j < indices.size(); j++) {
                if (j == prev || j == i || j == next) continue;

                Vector2 pt = p[indices[j]];

                // Point in triangle test
                float d1 = cross_oab(a, b, pt);
                float d2 = cross_oab(b, c, pt);
                float d3 = cross_oab(c, a, pt);

                bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
                bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

                if (!(has_neg && has_pos)) {
                    is_ear = false;
                    break;
                }
            }

            if (is_ear) {
                result.push_back(indices[prev]);
                result.push_back(indices[i]);
                result.push_back(indices[next]);

                indices.erase(indices.begin() + i);
                found_ear = true;
                break;
            }
        }

        if (!found_ear) {
            break;  // Failed to find ear, polygon may be invalid
        }
    }

    return result;
}

PackedInt32Array GeometryOps::delaunay(const PackedVector2Array& points) {
    PackedInt32Array result;
    int32_t n = points.size();

    if (n < 3) {
        return result;
    }

    const Vector2* p = points.ptr();

    // Bowyer-Watson algorithm
    // Start with super-triangle containing all points
    float min_x = p[0].x, max_x = p[0].x;
    float min_y = p[0].y, max_y = p[0].y;
    for (int32_t i = 1; i < n; i++) {
        min_x = std::fmin(min_x, p[i].x);
        max_x = std::fmax(max_x, p[i].x);
        min_y = std::fmin(min_y, p[i].y);
        max_y = std::fmax(max_y, p[i].y);
    }

    float dx = max_x - min_x;
    float dy = max_y - min_y;
    float delta = std::fmax(dx, dy) * 2.0f;

    // Super-triangle vertices (indices n, n+1, n+2)
    Vector2 st0(min_x - delta, min_y - delta);
    Vector2 st1(min_x + dx / 2, max_y + delta * 2);
    Vector2 st2(max_x + delta, min_y - delta);

    // Store all vertices
    std::vector<Vector2> vertices(n + 3);
    for (int32_t i = 0; i < n; i++) {
        vertices[i] = p[i];
    }
    vertices[n] = st0;
    vertices[n + 1] = st1;
    vertices[n + 2] = st2;

    // Triangle: indices of 3 vertices
    struct Triangle {
        int32_t v[3];
        bool bad;
    };

    std::vector<Triangle> triangles;
    triangles.push_back({n, n + 1, n + 2, false});

    // Add points one at a time
    for (int32_t i = 0; i < n; i++) {
        Vector2 point = vertices[i];

        // Find triangles whose circumcircle contains the point
        for (auto& tri : triangles) {
            if (tri.bad) continue;

            // Compute circumcircle
            Vector2 a = vertices[tri.v[0]];
            Vector2 b = vertices[tri.v[1]];
            Vector2 c = vertices[tri.v[2]];

            float ax = a.x - point.x;
            float ay = a.y - point.y;
            float bx = b.x - point.x;
            float by = b.y - point.y;
            float cx = c.x - point.x;
            float cy = c.y - point.y;

            float det = (ax * ax + ay * ay) * (bx * cy - cx * by) -
                        (bx * bx + by * by) * (ax * cy - cx * ay) +
                        (cx * cx + cy * cy) * (ax * by - bx * ay);

            if (det > 0) {
                tri.bad = true;
            }
        }

        // Find boundary of bad triangles (polygon hole)
        std::vector<std::pair<int32_t, int32_t>> polygon;
        for (const auto& tri : triangles) {
            if (!tri.bad) continue;

            for (int e = 0; e < 3; e++) {
                int32_t v0 = tri.v[e];
                int32_t v1 = tri.v[(e + 1) % 3];

                // Check if edge is shared with another bad triangle
                bool shared = false;
                for (const auto& other : triangles) {
                    if (&tri == &other || !other.bad) continue;

                    for (int e2 = 0; e2 < 3; e2++) {
                        if ((other.v[e2] == v0 && other.v[(e2 + 1) % 3] == v1) ||
                            (other.v[e2] == v1 && other.v[(e2 + 1) % 3] == v0)) {
                            shared = true;
                            break;
                        }
                    }
                    if (shared) break;
                }

                if (!shared) {
                    polygon.push_back({v0, v1});
                }
            }
        }

        // Remove bad triangles
        triangles.erase(std::remove_if(triangles.begin(), triangles.end(),
                                        [](const Triangle& t) { return t.bad; }),
                        triangles.end());

        // Add new triangles
        for (const auto& edge : polygon) {
            triangles.push_back({edge.first, edge.second, i, false});
        }
    }

    // Remove triangles connected to super-triangle
    for (const auto& tri : triangles) {
        if (tri.v[0] >= n || tri.v[1] >= n || tri.v[2] >= n) {
            continue;
        }
        result.push_back(tri.v[0]);
        result.push_back(tri.v[1]);
        result.push_back(tri.v[2]);
    }

    return result;
}

// ========== VORONOI ==========

Array GeometryOps::voronoi_cells(const PackedVector2Array& points, const Rect2& bounds) {
    Array result;
    int32_t n = points.size();

    if (n == 0) {
        return result;
    }

    // Simple brute-force Voronoi using clipping
    // For each point, compute its cell by intersecting half-planes

    const Vector2* p = points.ptr();
    Vector2 bounds_min = bounds.position;
    Vector2 bounds_max = bounds.position + bounds.size;

    for (int32_t i = 0; i < n; i++) {
        // Start with bounding box as cell
        std::vector<Vector2> cell;
        cell.push_back(Vector2(bounds_min.x, bounds_min.y));
        cell.push_back(Vector2(bounds_max.x, bounds_min.y));
        cell.push_back(Vector2(bounds_max.x, bounds_max.y));
        cell.push_back(Vector2(bounds_min.x, bounds_max.y));

        // Clip by half-plane for each other point
        for (int32_t j = 0; j < n && cell.size() >= 3; j++) {
            if (i == j) continue;

            // Perpendicular bisector
            Vector2 mid((p[i].x + p[j].x) / 2, (p[i].y + p[j].y) / 2);
            Vector2 normal(p[j].x - p[i].x, p[j].y - p[i].y);

            // Clip cell by half-plane (keep points on i's side)
            std::vector<Vector2> new_cell;
            for (size_t k = 0; k < cell.size(); k++) {
                Vector2 a = cell[k];
                Vector2 b = cell[(k + 1) % cell.size()];

                float da = (a.x - mid.x) * normal.x + (a.y - mid.y) * normal.y;
                float db = (b.x - mid.x) * normal.x + (b.y - mid.y) * normal.y;

                if (da <= 0) {
                    new_cell.push_back(a);
                }

                if ((da < 0 && db > 0) || (da > 0 && db < 0)) {
                    // Intersection
                    float t = da / (da - db);
                    new_cell.push_back(Vector2(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y)));
                }
            }
            cell = new_cell;
        }

        PackedVector2Array cell_array;
        cell_array.resize(cell.size());
        for (size_t k = 0; k < cell.size(); k++) {
            cell_array[k] = cell[k];
        }
        result.push_back(cell_array);
    }

    return result;
}

PackedFloat32Array GeometryOps::voronoi_edges(const PackedVector2Array& points, const Rect2& bounds) {
    PackedFloat32Array result;

    Array cells = voronoi_cells(points, bounds);

    for (int i = 0; i < cells.size(); i++) {
        PackedVector2Array cell = cells[i];
        for (int j = 0; j < cell.size(); j++) {
            Vector2 a = cell[j];
            Vector2 b = cell[(j + 1) % cell.size()];
            result.push_back(a.x);
            result.push_back(a.y);
            result.push_back(b.x);
            result.push_back(b.y);
        }
    }

    return result;
}

// ========== POLYGON PROPERTIES ==========

float GeometryOps::polygon_signed_area(const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n < 3) return 0.0f;

    const Vector2* p = polygon.ptr();
    float area = 0.0f;

    for (int32_t i = 0; i < n; i++) {
        int32_t j = (i + 1) % n;
        area += p[i].x * p[j].y - p[j].x * p[i].y;
    }

    return area / 2.0f;
}

float GeometryOps::polygon_area(const PackedVector2Array& polygon) {
    return std::fabs(polygon_signed_area(polygon));
}

Vector2 GeometryOps::polygon_centroid(const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n == 0) return Vector2(0, 0);
    if (n == 1) return polygon[0];
    if (n == 2) return (polygon[0] + polygon[1]) / 2.0f;

    const Vector2* p = polygon.ptr();
    float cx = 0.0f, cy = 0.0f;
    float signed_area = 0.0f;

    for (int32_t i = 0; i < n; i++) {
        int32_t j = (i + 1) % n;
        float cross = p[i].x * p[j].y - p[j].x * p[i].y;
        signed_area += cross;
        cx += (p[i].x + p[j].x) * cross;
        cy += (p[i].y + p[j].y) * cross;
    }

    signed_area /= 2.0f;
    if (std::fabs(signed_area) < 1e-10f) {
        // Degenerate polygon, return average
        cx = 0.0f;
        cy = 0.0f;
        for (int32_t i = 0; i < n; i++) {
            cx += p[i].x;
            cy += p[i].y;
        }
        return Vector2(cx / n, cy / n);
    }

    cx /= (6.0f * signed_area);
    cy /= (6.0f * signed_area);

    return Vector2(cx, cy);
}

Rect2 GeometryOps::polygon_bounds(const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n == 0) return Rect2();

    const Vector2* p = polygon.ptr();
    float min_x = p[0].x, max_x = p[0].x;
    float min_y = p[0].y, max_y = p[0].y;

    for (int32_t i = 1; i < n; i++) {
        min_x = std::fmin(min_x, p[i].x);
        max_x = std::fmax(max_x, p[i].x);
        min_y = std::fmin(min_y, p[i].y);
        max_y = std::fmax(max_y, p[i].y);
    }

    return Rect2(min_x, min_y, max_x - min_x, max_y - min_y);
}

float GeometryOps::polygon_perimeter(const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n < 2) return 0.0f;

    const Vector2* p = polygon.ptr();
    float perimeter = 0.0f;

    for (int32_t i = 0; i < n; i++) {
        int32_t j = (i + 1) % n;
        float dx = p[j].x - p[i].x;
        float dy = p[j].y - p[i].y;
        perimeter += std::sqrt(dx * dx + dy * dy);
    }

    return perimeter;
}

// ========== POLYGON TESTS ==========

bool GeometryOps::polygon_is_convex(const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n < 3) return true;

    const Vector2* p = polygon.ptr();
    int sign = 0;

    for (int32_t i = 0; i < n; i++) {
        Vector2 a = p[i];
        Vector2 b = p[(i + 1) % n];
        Vector2 c = p[(i + 2) % n];

        float cross = cross_oab(a, b, c);

        if (cross != 0) {
            int new_sign = cross > 0 ? 1 : -1;
            if (sign == 0) {
                sign = new_sign;
            } else if (sign != new_sign) {
                return false;
            }
        }
    }

    return true;
}

bool GeometryOps::polygon_is_clockwise(const PackedVector2Array& polygon) {
    return polygon_signed_area(polygon) < 0;
}

bool GeometryOps::polygon_is_simple(const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n < 4) return true;

    const Vector2* p = polygon.ptr();

    // Check each pair of non-adjacent edges
    for (int32_t i = 0; i < n; i++) {
        for (int32_t j = i + 2; j < n; j++) {
            // Skip adjacent edges
            if (i == 0 && j == n - 1) continue;

            // Check intersection
            Vector2 a1 = p[i];
            Vector2 a2 = p[(i + 1) % n];
            Vector2 b1 = p[j];
            Vector2 b2 = p[(j + 1) % n];

            float d1 = cross_oab(b1, b2, a1);
            float d2 = cross_oab(b1, b2, a2);
            float d3 = cross_oab(a1, a2, b1);
            float d4 = cross_oab(a1, a2, b2);

            if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
                ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
                return false;
            }
        }
    }

    return true;
}

bool GeometryOps::point_in_polygon(const Vector2& point, const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n < 3) return false;

    const Vector2* p = polygon.ptr();
    bool inside = false;

    for (int32_t i = 0, j = n - 1; i < n; j = i++) {
        if (((p[i].y > point.y) != (p[j].y > point.y)) &&
            (point.x < (p[j].x - p[i].x) * (point.y - p[i].y) / (p[j].y - p[i].y) + p[i].x)) {
            inside = !inside;
        }
    }

    return inside;
}

// ========== POLYGON OPERATIONS ==========

PackedVector2Array GeometryOps::polygon_reverse(const PackedVector2Array& polygon) {
    PackedVector2Array result;
    int32_t n = polygon.size();
    result.resize(n);

    for (int32_t i = 0; i < n; i++) {
        result[i] = polygon[n - 1 - i];
    }

    return result;
}

PackedVector2Array GeometryOps::polygon_make_ccw(const PackedVector2Array& polygon) {
    if (polygon_is_clockwise(polygon)) {
        return polygon_reverse(polygon);
    }
    return polygon;
}

PackedVector2Array GeometryOps::polygon_make_cw(const PackedVector2Array& polygon) {
    if (!polygon_is_clockwise(polygon)) {
        return polygon_reverse(polygon);
    }
    return polygon;
}

// ========== POLYLINE SIMPLIFICATION ==========

static void rdp_recursive(const Vector2* p, int start, int end, float epsilon_sq,
                          std::vector<bool>& keep) {
    if (end <= start + 1) return;

    float max_dist_sq = 0.0f;
    int max_idx = start;

    float dx = p[end].x - p[start].x;
    float dy = p[end].y - p[start].y;
    float line_len_sq = dx * dx + dy * dy;

    for (int i = start + 1; i < end; i++) {
        float dist_sq;

        if (line_len_sq < 1e-10f) {
            // Degenerate line
            float px = p[i].x - p[start].x;
            float py = p[i].y - p[start].y;
            dist_sq = px * px + py * py;
        } else {
            float t = ((p[i].x - p[start].x) * dx + (p[i].y - p[start].y) * dy) / line_len_sq;
            t = std::fmax(0.0f, std::fmin(1.0f, t));
            float proj_x = p[start].x + t * dx;
            float proj_y = p[start].y + t * dy;
            float px = p[i].x - proj_x;
            float py = p[i].y - proj_y;
            dist_sq = px * px + py * py;
        }

        if (dist_sq > max_dist_sq) {
            max_dist_sq = dist_sq;
            max_idx = i;
        }
    }

    if (max_dist_sq > epsilon_sq) {
        keep[max_idx] = true;
        rdp_recursive(p, start, max_idx, epsilon_sq, keep);
        rdp_recursive(p, max_idx, end, epsilon_sq, keep);
    }
}

PackedVector2Array GeometryOps::simplify_rdp(const PackedVector2Array& polyline, float epsilon) {
    PackedVector2Array result;
    int32_t n = polyline.size();

    if (n <= 2) {
        return polyline;
    }

    const Vector2* p = polyline.ptr();
    std::vector<bool> keep(n, false);
    keep[0] = true;
    keep[n - 1] = true;

    rdp_recursive(p, 0, n - 1, epsilon * epsilon, keep);

    for (int32_t i = 0; i < n; i++) {
        if (keep[i]) {
            result.push_back(p[i]);
        }
    }

    return result;
}

PackedInt32Array GeometryOps::simplify_rdp_indices(const PackedVector2Array& polyline, float epsilon) {
    PackedInt32Array result;
    int32_t n = polyline.size();

    if (n <= 2) {
        for (int32_t i = 0; i < n; i++) {
            result.push_back(i);
        }
        return result;
    }

    const Vector2* p = polyline.ptr();
    std::vector<bool> keep(n, false);
    keep[0] = true;
    keep[n - 1] = true;

    rdp_recursive(p, 0, n - 1, epsilon * epsilon, keep);

    for (int32_t i = 0; i < n; i++) {
        if (keep[i]) {
            result.push_back(i);
        }
    }

    return result;
}

PackedVector2Array GeometryOps::simplify_visvalingam(const PackedVector2Array& polyline, int target_count) {
    PackedVector2Array result;
    int32_t n = polyline.size();

    if (n <= target_count) {
        return polyline;
    }

    const Vector2* p = polyline.ptr();

    // Track which points are still in the line
    std::vector<int32_t> prev(n), next(n);
    for (int32_t i = 0; i < n; i++) {
        prev[i] = i - 1;
        next[i] = i + 1;
    }
    next[n - 1] = -1;

    // Priority queue: (area, index)
    auto calc_area = [&](int32_t i) -> float {
        if (prev[i] < 0 || next[i] < 0) return std::numeric_limits<float>::infinity();
        return std::fabs(cross_oab(p[prev[i]], p[i], p[next[i]])) / 2.0f;
    };

    std::priority_queue<std::pair<float, int32_t>,
                        std::vector<std::pair<float, int32_t>>,
                        std::greater<std::pair<float, int32_t>>> pq;

    for (int32_t i = 1; i < n - 1; i++) {
        pq.push({calc_area(i), i});
    }

    int32_t remaining = n;
    std::vector<bool> removed(n, false);

    while (remaining > target_count && !pq.empty()) {
        auto [area, idx] = pq.top();
        pq.pop();

        if (removed[idx]) continue;
        if (std::fabs(calc_area(idx) - area) > 1e-10f) continue;  // Stale entry

        removed[idx] = true;
        remaining--;

        // Update neighbors
        int32_t p_idx = prev[idx];
        int32_t n_idx = next[idx];

        if (p_idx >= 0) {
            next[p_idx] = n_idx;
            if (p_idx > 0) {
                pq.push({calc_area(p_idx), p_idx});
            }
        }
        if (n_idx >= 0) {
            prev[n_idx] = p_idx;
            if (n_idx < n - 1) {
                pq.push({calc_area(n_idx), n_idx});
            }
        }
    }

    for (int32_t i = 0; i < n; i++) {
        if (!removed[i]) {
            result.push_back(p[i]);
        }
    }

    return result;
}

// ========== POLYGON OFFSET ==========

PackedVector2Array GeometryOps::offset_polygon(const PackedVector2Array& polygon, float distance) {
    PackedVector2Array result;
    int32_t n = polygon.size();

    if (n < 3) {
        return polygon;
    }

    const Vector2* p = polygon.ptr();
    result.resize(n);
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        int32_t prev = (i + n - 1) % n;
        int32_t next = (i + 1) % n;

        // Edge normals
        Vector2 e1(p[i].x - p[prev].x, p[i].y - p[prev].y);
        Vector2 e2(p[next].x - p[i].x, p[next].y - p[i].y);

        float len1 = std::sqrt(e1.x * e1.x + e1.y * e1.y);
        float len2 = std::sqrt(e2.x * e2.x + e2.y * e2.y);

        if (len1 < 1e-10f || len2 < 1e-10f) {
            dst[i] = p[i];
            continue;
        }

        // Outward normals (perpendicular to edges)
        // For CCW polygon, outward normal is right perpendicular: (ey, -ex)
        Vector2 n1(e1.y / len1, -e1.x / len1);
        Vector2 n2(e2.y / len2, -e2.x / len2);

        // Average normal at vertex
        Vector2 normal(n1.x + n2.x, n1.y + n2.y);
        float normal_len = std::sqrt(normal.x * normal.x + normal.y * normal.y);

        if (normal_len < 1e-10f) {
            dst[i] = p[i];
            continue;
        }

        normal.x /= normal_len;
        normal.y /= normal_len;

        // Scale by 1/cos(half angle) to maintain edge offset
        float dot = n1.x * n2.x + n1.y * n2.y;
        float scale = 1.0f / std::sqrt((1.0f + dot) / 2.0f);
        scale = std::fmin(scale, 10.0f);  // Limit for very sharp angles

        dst[i] = Vector2(p[i].x + normal.x * distance * scale,
                         p[i].y + normal.y * distance * scale);
    }

    return result;
}

// ========== MINKOWSKI OPERATIONS ==========

PackedVector2Array GeometryOps::minkowski_sum(const PackedVector2Array& a, const PackedVector2Array& b) {
    // Ensure both polygons are convex and CCW
    PackedVector2Array hull_a = polygon_make_ccw(convex_hull(a));
    PackedVector2Array hull_b = polygon_make_ccw(convex_hull(b));

    int32_t na = hull_a.size();
    int32_t nb = hull_b.size();

    if (na == 0) return hull_b;
    if (nb == 0) return hull_a;

    const Vector2* pa = hull_a.ptr();
    const Vector2* pb = hull_b.ptr();

    // Find bottom-most points
    int32_t ia = 0, ib = 0;
    for (int32_t i = 1; i < na; i++) {
        if (pa[i].y < pa[ia].y || (pa[i].y == pa[ia].y && pa[i].x < pa[ia].x)) {
            ia = i;
        }
    }
    for (int32_t i = 1; i < nb; i++) {
        if (pb[i].y < pb[ib].y || (pb[i].y == pb[ib].y && pb[i].x < pb[ib].x)) {
            ib = i;
        }
    }

    PackedVector2Array result;

    // Traverse both polygons
    int32_t count_a = 0, count_b = 0;
    while (count_a < na || count_b < nb) {
        result.push_back(Vector2(pa[ia].x + pb[ib].x, pa[ia].y + pb[ib].y));

        int32_t next_ia = (ia + 1) % na;
        int32_t next_ib = (ib + 1) % nb;

        Vector2 edge_a(pa[next_ia].x - pa[ia].x, pa[next_ia].y - pa[ia].y);
        Vector2 edge_b(pb[next_ib].x - pb[ib].x, pb[next_ib].y - pb[ib].y);

        float cross = edge_a.x * edge_b.y - edge_a.y * edge_b.x;

        if (count_a >= na) {
            ib = next_ib;
            count_b++;
        } else if (count_b >= nb) {
            ia = next_ia;
            count_a++;
        } else if (cross > 0) {
            ia = next_ia;
            count_a++;
        } else if (cross < 0) {
            ib = next_ib;
            count_b++;
        } else {
            ia = next_ia;
            ib = next_ib;
            count_a++;
            count_b++;
        }
    }

    return result;
}

PackedVector2Array GeometryOps::minkowski_diff(const PackedVector2Array& a, const PackedVector2Array& b) {
    // A - B = A + (-B)
    PackedVector2Array neg_b;
    neg_b.resize(b.size());
    for (int32_t i = 0; i < b.size(); i++) {
        neg_b[i] = Vector2(-b[i].x, -b[i].y);
    }
    return minkowski_sum(a, neg_b);
}

// ========== DISTANCE QUERIES ==========

float GeometryOps::point_to_segment_distance(const Vector2& point, const Vector2& seg_start, const Vector2& seg_end) {
    float dx = seg_end.x - seg_start.x;
    float dy = seg_end.y - seg_start.y;
    float len_sq = dx * dx + dy * dy;

    if (len_sq < 1e-10f) {
        float px = point.x - seg_start.x;
        float py = point.y - seg_start.y;
        return std::sqrt(px * px + py * py);
    }

    float t = ((point.x - seg_start.x) * dx + (point.y - seg_start.y) * dy) / len_sq;
    t = std::fmax(0.0f, std::fmin(1.0f, t));

    float proj_x = seg_start.x + t * dx;
    float proj_y = seg_start.y + t * dy;
    float px = point.x - proj_x;
    float py = point.y - proj_y;

    return std::sqrt(px * px + py * py);
}

float GeometryOps::point_to_polygon_distance(const Vector2& point, const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n == 0) return std::numeric_limits<float>::infinity();

    const Vector2* p = polygon.ptr();
    float min_dist = std::numeric_limits<float>::infinity();

    for (int32_t i = 0; i < n; i++) {
        int32_t j = (i + 1) % n;
        float dist = point_to_segment_distance(point, p[i], p[j]);
        min_dist = std::fmin(min_dist, dist);
    }

    return min_dist;
}

PackedFloat32Array GeometryOps::distances_to_polygon(const PackedVector2Array& points, const PackedVector2Array& polygon) {
    PackedFloat32Array result;
    int32_t count = points.size();
    result.resize(count);

    const Vector2* pts = points.ptr();
    float* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = point_to_polygon_distance(pts[i], polygon);
    }

    return result;
}

Vector2 GeometryOps::closest_point_on_polygon(const Vector2& point, const PackedVector2Array& polygon) {
    int32_t n = polygon.size();
    if (n == 0) return point;

    const Vector2* p = polygon.ptr();
    float min_dist_sq = std::numeric_limits<float>::infinity();
    Vector2 closest = p[0];

    for (int32_t i = 0; i < n; i++) {
        int32_t j = (i + 1) % n;

        float dx = p[j].x - p[i].x;
        float dy = p[j].y - p[i].y;
        float len_sq = dx * dx + dy * dy;

        Vector2 proj;
        if (len_sq < 1e-10f) {
            proj = p[i];
        } else {
            float t = ((point.x - p[i].x) * dx + (point.y - p[i].y) * dy) / len_sq;
            t = std::fmax(0.0f, std::fmin(1.0f, t));
            proj = Vector2(p[i].x + t * dx, p[i].y + t * dy);
        }

        float px = point.x - proj.x;
        float py = point.y - proj.y;
        float dist_sq = px * px + py * py;

        if (dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            closest = proj;
        }
    }

    return closest;
}

// ========== LINE UTILITIES ==========

float GeometryOps::point_line_side(const Vector2& point, const Vector2& line_start, const Vector2& line_end) {
    return cross_oab(line_start, line_end, point);
}

float GeometryOps::project_point_to_line(const Vector2& point, const Vector2& line_start, const Vector2& line_end) {
    float dx = line_end.x - line_start.x;
    float dy = line_end.y - line_start.y;
    float len_sq = dx * dx + dy * dy;

    if (len_sq < 1e-10f) {
        return 0.0f;
    }

    return ((point.x - line_start.x) * dx + (point.y - line_start.y) * dy) / len_sq;
}

PackedFloat32Array GeometryOps::project_points_to_line(const PackedVector2Array& points, const Vector2& line_start, const Vector2& line_end) {
    PackedFloat32Array result;
    int32_t count = points.size();
    result.resize(count);

    const Vector2* p = points.ptr();
    float* dst = result.ptrw();

    float dx = line_end.x - line_start.x;
    float dy = line_end.y - line_start.y;
    float len_sq = dx * dx + dy * dy;

    if (len_sq < 1e-10f) {
        for (int32_t i = 0; i < count; i++) {
            dst[i] = 0.0f;
        }
    } else {
        for (int32_t i = 0; i < count; i++) {
            dst[i] = ((p[i].x - line_start.x) * dx + (p[i].y - line_start.y) * dy) / len_sq;
        }
    }

    return result;
}

}

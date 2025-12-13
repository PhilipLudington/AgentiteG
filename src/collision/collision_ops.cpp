/**
 * CollisionOps Implementation
 */

#include "collision_ops.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <limits>

namespace godot {

void CollisionOps::_bind_methods() {
    // Point containment
    ClassDB::bind_static_method("CollisionOps", D_METHOD("points_in_rect", "points", "rect"), &CollisionOps::points_in_rect);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("points_in_circle", "points", "center", "radius"), &CollisionOps::points_in_circle);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("points_in_polygon", "points", "polygon"), &CollisionOps::points_in_polygon);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("points_in_aabb", "points", "box"), &CollisionOps::points_in_aabb);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("points_in_sphere", "points", "center", "radius"), &CollisionOps::points_in_sphere);

    // Shape vs shape
    ClassDB::bind_static_method("CollisionOps", D_METHOD("circles_vs_circles", "centers_a", "radii_a", "centers_b", "radii_b"), &CollisionOps::circles_vs_circles);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("circles_vs_circles_uniform", "centers_a", "radius_a", "centers_b", "radius_b"), &CollisionOps::circles_vs_circles_uniform);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("aabb_vs_aabb_2d", "mins_a", "maxs_a", "mins_b", "maxs_b"), &CollisionOps::aabb_vs_aabb_2d);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("aabb_vs_aabb_3d", "mins_a", "maxs_a", "mins_b", "maxs_b"), &CollisionOps::aabb_vs_aabb_3d);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("spheres_vs_spheres", "centers_a", "radii_a", "centers_b", "radii_b"), &CollisionOps::spheres_vs_spheres);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("spheres_vs_spheres_uniform", "centers_a", "radius_a", "centers_b", "radius_b"), &CollisionOps::spheres_vs_spheres_uniform);

    // Self-collision
    ClassDB::bind_static_method("CollisionOps", D_METHOD("circles_self_collision", "centers", "radii"), &CollisionOps::circles_self_collision);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("circles_self_collision_uniform", "centers", "radius"), &CollisionOps::circles_self_collision_uniform);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("spheres_self_collision", "centers", "radii"), &CollisionOps::spheres_self_collision);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("spheres_self_collision_uniform", "centers", "radius"), &CollisionOps::spheres_self_collision_uniform);

    // Ray intersection
    ClassDB::bind_static_method("CollisionOps", D_METHOD("ray_vs_circles", "origin", "direction", "centers", "radii"), &CollisionOps::ray_vs_circles);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("ray_vs_circles_uniform", "origin", "direction", "centers", "radius"), &CollisionOps::ray_vs_circles_uniform);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("ray_vs_aabbs_2d", "origin", "direction", "mins", "maxs"), &CollisionOps::ray_vs_aabbs_2d);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("ray_vs_spheres", "origin", "direction", "centers", "radii"), &CollisionOps::ray_vs_spheres);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("ray_first_circle", "origin", "direction", "max_distance", "centers", "radii"), &CollisionOps::ray_first_circle);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("ray_first_sphere", "origin", "direction", "max_distance", "centers", "radii"), &CollisionOps::ray_first_sphere);

    // Segment intersection
    ClassDB::bind_static_method("CollisionOps", D_METHOD("segments_intersect", "starts_a", "ends_a", "starts_b", "ends_b"), &CollisionOps::segments_intersect);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("segment_intersection_points", "starts_a", "ends_a", "starts_b", "ends_b"), &CollisionOps::segment_intersection_points);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("segments_self_intersect", "starts", "ends"), &CollisionOps::segments_self_intersect);

    // Closest point queries
    ClassDB::bind_static_method("CollisionOps", D_METHOD("closest_points_on_segments", "points", "seg_starts", "seg_ends"), &CollisionOps::closest_points_on_segments);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("closest_circle_indices", "points", "centers", "radii"), &CollisionOps::closest_circle_indices);
    ClassDB::bind_static_method("CollisionOps", D_METHOD("closest_circle_indices_uniform", "points", "centers", "radius"), &CollisionOps::closest_circle_indices_uniform);
}

// ========== POINT CONTAINMENT TESTS ==========

PackedInt32Array CollisionOps::points_in_rect(const PackedVector2Array& points, const Rect2& rect) {
    PackedInt32Array result;
    int32_t count = points.size();
    const Vector2* p = points.ptr();

    float min_x = rect.position.x;
    float min_y = rect.position.y;
    float max_x = min_x + rect.size.x;
    float max_y = min_y + rect.size.y;

    for (int32_t i = 0; i < count; i++) {
        if (p[i].x >= min_x && p[i].x <= max_x &&
            p[i].y >= min_y && p[i].y <= max_y) {
            result.push_back(i);
        }
    }

    return result;
}

PackedInt32Array CollisionOps::points_in_circle(const PackedVector2Array& points, const Vector2& center, float radius) {
    PackedInt32Array result;
    int32_t count = points.size();
    const Vector2* p = points.ptr();
    float radius_sq = radius * radius;

    for (int32_t i = 0; i < count; i++) {
        float dx = p[i].x - center.x;
        float dy = p[i].y - center.y;
        if (dx * dx + dy * dy <= radius_sq) {
            result.push_back(i);
        }
    }

    return result;
}

PackedInt32Array CollisionOps::points_in_polygon(const PackedVector2Array& points, const PackedVector2Array& polygon) {
    PackedInt32Array result;
    int32_t point_count = points.size();
    int32_t poly_count = polygon.size();

    if (poly_count < 3) {
        return result;
    }

    const Vector2* pts = points.ptr();
    const Vector2* poly = polygon.ptr();

    // Ray casting algorithm for point-in-polygon
    for (int32_t i = 0; i < point_count; i++) {
        float px = pts[i].x;
        float py = pts[i].y;
        bool inside = false;

        for (int32_t j = 0, k = poly_count - 1; j < poly_count; k = j++) {
            float xi = poly[j].x, yi = poly[j].y;
            float xj = poly[k].x, yj = poly[k].y;

            if (((yi > py) != (yj > py)) &&
                (px < (xj - xi) * (py - yi) / (yj - yi) + xi)) {
                inside = !inside;
            }
        }

        if (inside) {
            result.push_back(i);
        }
    }

    return result;
}

PackedInt32Array CollisionOps::points_in_aabb(const PackedVector3Array& points, const AABB& box) {
    PackedInt32Array result;
    int32_t count = points.size();
    const Vector3* p = points.ptr();

    Vector3 min = box.position;
    Vector3 max = box.position + box.size;

    for (int32_t i = 0; i < count; i++) {
        if (p[i].x >= min.x && p[i].x <= max.x &&
            p[i].y >= min.y && p[i].y <= max.y &&
            p[i].z >= min.z && p[i].z <= max.z) {
            result.push_back(i);
        }
    }

    return result;
}

PackedInt32Array CollisionOps::points_in_sphere(const PackedVector3Array& points, const Vector3& center, float radius) {
    PackedInt32Array result;
    int32_t count = points.size();
    const Vector3* p = points.ptr();
    float radius_sq = radius * radius;

    for (int32_t i = 0; i < count; i++) {
        float dx = p[i].x - center.x;
        float dy = p[i].y - center.y;
        float dz = p[i].z - center.z;
        if (dx * dx + dy * dy + dz * dz <= radius_sq) {
            result.push_back(i);
        }
    }

    return result;
}

// ========== SHAPE VS SHAPE COLLISION ==========

PackedInt32Array CollisionOps::circles_vs_circles(
    const PackedVector2Array& centers_a, const PackedFloat32Array& radii_a,
    const PackedVector2Array& centers_b, const PackedFloat32Array& radii_b) {

    PackedInt32Array result;
    int32_t count_a = std::min(centers_a.size(), radii_a.size());
    int32_t count_b = std::min(centers_b.size(), radii_b.size());

    const Vector2* ca = centers_a.ptr();
    const float* ra = radii_a.ptr();
    const Vector2* cb = centers_b.ptr();
    const float* rb = radii_b.ptr();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            float dx = ca[i].x - cb[j].x;
            float dy = ca[i].y - cb[j].y;
            float dist_sq = dx * dx + dy * dy;
            float combined_r = ra[i] + rb[j];
            if (dist_sq <= combined_r * combined_r) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::circles_vs_circles_uniform(
    const PackedVector2Array& centers_a, float radius_a,
    const PackedVector2Array& centers_b, float radius_b) {

    PackedInt32Array result;
    int32_t count_a = centers_a.size();
    int32_t count_b = centers_b.size();

    const Vector2* ca = centers_a.ptr();
    const Vector2* cb = centers_b.ptr();

    float combined_r = radius_a + radius_b;
    float combined_r_sq = combined_r * combined_r;

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            float dx = ca[i].x - cb[j].x;
            float dy = ca[i].y - cb[j].y;
            if (dx * dx + dy * dy <= combined_r_sq) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::aabb_vs_aabb_2d(
    const PackedVector2Array& mins_a, const PackedVector2Array& maxs_a,
    const PackedVector2Array& mins_b, const PackedVector2Array& maxs_b) {

    PackedInt32Array result;
    int32_t count_a = std::min(mins_a.size(), maxs_a.size());
    int32_t count_b = std::min(mins_b.size(), maxs_b.size());

    const Vector2* mina = mins_a.ptr();
    const Vector2* maxa = maxs_a.ptr();
    const Vector2* minb = mins_b.ptr();
    const Vector2* maxb = maxs_b.ptr();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            if (maxa[i].x >= minb[j].x && mina[i].x <= maxb[j].x &&
                maxa[i].y >= minb[j].y && mina[i].y <= maxb[j].y) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::aabb_vs_aabb_3d(
    const PackedVector3Array& mins_a, const PackedVector3Array& maxs_a,
    const PackedVector3Array& mins_b, const PackedVector3Array& maxs_b) {

    PackedInt32Array result;
    int32_t count_a = std::min(mins_a.size(), maxs_a.size());
    int32_t count_b = std::min(mins_b.size(), maxs_b.size());

    const Vector3* mina = mins_a.ptr();
    const Vector3* maxa = maxs_a.ptr();
    const Vector3* minb = mins_b.ptr();
    const Vector3* maxb = maxs_b.ptr();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            if (maxa[i].x >= minb[j].x && mina[i].x <= maxb[j].x &&
                maxa[i].y >= minb[j].y && mina[i].y <= maxb[j].y &&
                maxa[i].z >= minb[j].z && mina[i].z <= maxb[j].z) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::spheres_vs_spheres(
    const PackedVector3Array& centers_a, const PackedFloat32Array& radii_a,
    const PackedVector3Array& centers_b, const PackedFloat32Array& radii_b) {

    PackedInt32Array result;
    int32_t count_a = std::min(centers_a.size(), radii_a.size());
    int32_t count_b = std::min(centers_b.size(), radii_b.size());

    const Vector3* ca = centers_a.ptr();
    const float* ra = radii_a.ptr();
    const Vector3* cb = centers_b.ptr();
    const float* rb = radii_b.ptr();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            float dx = ca[i].x - cb[j].x;
            float dy = ca[i].y - cb[j].y;
            float dz = ca[i].z - cb[j].z;
            float dist_sq = dx * dx + dy * dy + dz * dz;
            float combined_r = ra[i] + rb[j];
            if (dist_sq <= combined_r * combined_r) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::spheres_vs_spheres_uniform(
    const PackedVector3Array& centers_a, float radius_a,
    const PackedVector3Array& centers_b, float radius_b) {

    PackedInt32Array result;
    int32_t count_a = centers_a.size();
    int32_t count_b = centers_b.size();

    const Vector3* ca = centers_a.ptr();
    const Vector3* cb = centers_b.ptr();

    float combined_r = radius_a + radius_b;
    float combined_r_sq = combined_r * combined_r;

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            float dx = ca[i].x - cb[j].x;
            float dy = ca[i].y - cb[j].y;
            float dz = ca[i].z - cb[j].z;
            if (dx * dx + dy * dy + dz * dz <= combined_r_sq) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

// ========== SELF-COLLISION ==========

PackedInt32Array CollisionOps::circles_self_collision(
    const PackedVector2Array& centers, const PackedFloat32Array& radii) {

    PackedInt32Array result;
    int32_t count = std::min(centers.size(), radii.size());

    const Vector2* c = centers.ptr();
    const float* r = radii.ptr();

    for (int32_t i = 0; i < count; i++) {
        for (int32_t j = i + 1; j < count; j++) {
            float dx = c[i].x - c[j].x;
            float dy = c[i].y - c[j].y;
            float dist_sq = dx * dx + dy * dy;
            float combined_r = r[i] + r[j];
            if (dist_sq <= combined_r * combined_r) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::circles_self_collision_uniform(
    const PackedVector2Array& centers, float radius) {

    PackedInt32Array result;
    int32_t count = centers.size();

    const Vector2* c = centers.ptr();

    float diameter = 2.0f * radius;
    float diameter_sq = diameter * diameter;

    for (int32_t i = 0; i < count; i++) {
        for (int32_t j = i + 1; j < count; j++) {
            float dx = c[i].x - c[j].x;
            float dy = c[i].y - c[j].y;
            if (dx * dx + dy * dy <= diameter_sq) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::spheres_self_collision(
    const PackedVector3Array& centers, const PackedFloat32Array& radii) {

    PackedInt32Array result;
    int32_t count = std::min(centers.size(), radii.size());

    const Vector3* c = centers.ptr();
    const float* r = radii.ptr();

    for (int32_t i = 0; i < count; i++) {
        for (int32_t j = i + 1; j < count; j++) {
            float dx = c[i].x - c[j].x;
            float dy = c[i].y - c[j].y;
            float dz = c[i].z - c[j].z;
            float dist_sq = dx * dx + dy * dy + dz * dz;
            float combined_r = r[i] + r[j];
            if (dist_sq <= combined_r * combined_r) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::spheres_self_collision_uniform(
    const PackedVector3Array& centers, float radius) {

    PackedInt32Array result;
    int32_t count = centers.size();

    const Vector3* c = centers.ptr();

    float diameter = 2.0f * radius;
    float diameter_sq = diameter * diameter;

    for (int32_t i = 0; i < count; i++) {
        for (int32_t j = i + 1; j < count; j++) {
            float dx = c[i].x - c[j].x;
            float dy = c[i].y - c[j].y;
            float dz = c[i].z - c[j].z;
            if (dx * dx + dy * dy + dz * dz <= diameter_sq) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

// ========== RAY INTERSECTION ==========

// Helper: Ray-circle intersection, returns distance or INF
static float ray_circle_intersection(
    float ox, float oy, float dx, float dy,
    float cx, float cy, float r) {

    // Vector from ray origin to circle center
    float ocx = cx - ox;
    float ocy = cy - oy;

    // Project onto ray direction
    float proj = ocx * dx + ocy * dy;

    // Closest point on ray to circle center
    float closest_x = proj * dx;
    float closest_y = proj * dy;

    // Distance from closest point to circle center
    float dist_x = ocx - closest_x;
    float dist_y = ocy - closest_y;
    float dist_sq = dist_x * dist_x + dist_y * dist_y;

    float r_sq = r * r;
    if (dist_sq > r_sq) {
        return std::numeric_limits<float>::infinity();
    }

    // Distance from closest point to intersection
    float half_chord = std::sqrt(r_sq - dist_sq);

    // Two potential intersections
    float t1 = proj - half_chord;
    float t2 = proj + half_chord;

    // Return closest positive intersection
    if (t1 >= 0.0f) {
        return t1;
    } else if (t2 >= 0.0f) {
        return t2;
    }

    return std::numeric_limits<float>::infinity();
}

PackedFloat32Array CollisionOps::ray_vs_circles(
    const Vector2& origin, const Vector2& direction,
    const PackedVector2Array& centers, const PackedFloat32Array& radii) {

    int32_t count = std::min(centers.size(), radii.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* c = centers.ptr();
    const float* r = radii.ptr();
    float* dst = result.ptrw();

    // Normalize direction
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float dx = len > 0.0f ? direction.x / len : 0.0f;
    float dy = len > 0.0f ? direction.y / len : 0.0f;

    for (int32_t i = 0; i < count; i++) {
        dst[i] = ray_circle_intersection(origin.x, origin.y, dx, dy, c[i].x, c[i].y, r[i]);
    }

    return result;
}

PackedFloat32Array CollisionOps::ray_vs_circles_uniform(
    const Vector2& origin, const Vector2& direction,
    const PackedVector2Array& centers, float radius) {

    int32_t count = centers.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* c = centers.ptr();
    float* dst = result.ptrw();

    // Normalize direction
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float dx = len > 0.0f ? direction.x / len : 0.0f;
    float dy = len > 0.0f ? direction.y / len : 0.0f;

    for (int32_t i = 0; i < count; i++) {
        dst[i] = ray_circle_intersection(origin.x, origin.y, dx, dy, c[i].x, c[i].y, radius);
    }

    return result;
}

PackedFloat32Array CollisionOps::ray_vs_aabbs_2d(
    const Vector2& origin, const Vector2& direction,
    const PackedVector2Array& mins, const PackedVector2Array& maxs) {

    int32_t count = std::min(mins.size(), maxs.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* pmin = mins.ptr();
    const Vector2* pmax = maxs.ptr();
    float* dst = result.ptrw();

    // Normalize direction
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float dx = len > 0.0f ? direction.x / len : 0.0f;
    float dy = len > 0.0f ? direction.y / len : 0.0f;

    float inv_dx = (dx != 0.0f) ? 1.0f / dx : std::numeric_limits<float>::infinity();
    float inv_dy = (dy != 0.0f) ? 1.0f / dy : std::numeric_limits<float>::infinity();

    for (int32_t i = 0; i < count; i++) {
        float t1x = (pmin[i].x - origin.x) * inv_dx;
        float t2x = (pmax[i].x - origin.x) * inv_dx;
        float t1y = (pmin[i].y - origin.y) * inv_dy;
        float t2y = (pmax[i].y - origin.y) * inv_dy;

        float tmin = std::fmax(std::fmin(t1x, t2x), std::fmin(t1y, t2y));
        float tmax = std::fmin(std::fmax(t1x, t2x), std::fmax(t1y, t2y));

        if (tmax >= 0.0f && tmin <= tmax) {
            dst[i] = tmin >= 0.0f ? tmin : tmax;
        } else {
            dst[i] = std::numeric_limits<float>::infinity();
        }
    }

    return result;
}

// Helper: Ray-sphere intersection
static float ray_sphere_intersection(
    float ox, float oy, float oz, float dx, float dy, float dz,
    float cx, float cy, float cz, float r) {

    float ocx = cx - ox;
    float ocy = cy - oy;
    float ocz = cz - oz;

    float proj = ocx * dx + ocy * dy + ocz * dz;

    float closest_x = proj * dx;
    float closest_y = proj * dy;
    float closest_z = proj * dz;

    float dist_x = ocx - closest_x;
    float dist_y = ocy - closest_y;
    float dist_z = ocz - closest_z;
    float dist_sq = dist_x * dist_x + dist_y * dist_y + dist_z * dist_z;

    float r_sq = r * r;
    if (dist_sq > r_sq) {
        return std::numeric_limits<float>::infinity();
    }

    float half_chord = std::sqrt(r_sq - dist_sq);

    float t1 = proj - half_chord;
    float t2 = proj + half_chord;

    if (t1 >= 0.0f) {
        return t1;
    } else if (t2 >= 0.0f) {
        return t2;
    }

    return std::numeric_limits<float>::infinity();
}

PackedFloat32Array CollisionOps::ray_vs_spheres(
    const Vector3& origin, const Vector3& direction,
    const PackedVector3Array& centers, const PackedFloat32Array& radii) {

    int32_t count = std::min(centers.size(), radii.size());
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* c = centers.ptr();
    const float* r = radii.ptr();
    float* dst = result.ptrw();

    // Normalize direction
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    float dx = len > 0.0f ? direction.x / len : 0.0f;
    float dy = len > 0.0f ? direction.y / len : 0.0f;
    float dz = len > 0.0f ? direction.z / len : 0.0f;

    for (int32_t i = 0; i < count; i++) {
        dst[i] = ray_sphere_intersection(origin.x, origin.y, origin.z, dx, dy, dz,
                                         c[i].x, c[i].y, c[i].z, r[i]);
    }

    return result;
}

int CollisionOps::ray_first_circle(
    const Vector2& origin, const Vector2& direction, float max_distance,
    const PackedVector2Array& centers, const PackedFloat32Array& radii) {

    int32_t count = std::min(centers.size(), radii.size());
    const Vector2* c = centers.ptr();
    const float* r = radii.ptr();

    // Normalize direction
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    float dx = len > 0.0f ? direction.x / len : 0.0f;
    float dy = len > 0.0f ? direction.y / len : 0.0f;

    int best_index = -1;
    float best_dist = max_distance;

    for (int32_t i = 0; i < count; i++) {
        float dist = ray_circle_intersection(origin.x, origin.y, dx, dy, c[i].x, c[i].y, r[i]);
        if (dist < best_dist) {
            best_dist = dist;
            best_index = i;
        }
    }

    return best_index;
}

int CollisionOps::ray_first_sphere(
    const Vector3& origin, const Vector3& direction, float max_distance,
    const PackedVector3Array& centers, const PackedFloat32Array& radii) {

    int32_t count = std::min(centers.size(), radii.size());
    const Vector3* c = centers.ptr();
    const float* r = radii.ptr();

    // Normalize direction
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    float dx = len > 0.0f ? direction.x / len : 0.0f;
    float dy = len > 0.0f ? direction.y / len : 0.0f;
    float dz = len > 0.0f ? direction.z / len : 0.0f;

    int best_index = -1;
    float best_dist = max_distance;

    for (int32_t i = 0; i < count; i++) {
        float dist = ray_sphere_intersection(origin.x, origin.y, origin.z, dx, dy, dz,
                                             c[i].x, c[i].y, c[i].z, r[i]);
        if (dist < best_dist) {
            best_dist = dist;
            best_index = i;
        }
    }

    return best_index;
}

// ========== SEGMENT INTERSECTION ==========

// Helper: Check if two segments intersect and optionally get intersection point
static bool segments_intersect_impl(
    float ax1, float ay1, float ax2, float ay2,
    float bx1, float by1, float bx2, float by2,
    float* out_x = nullptr, float* out_y = nullptr) {

    float dax = ax2 - ax1;
    float day = ay2 - ay1;
    float dbx = bx2 - bx1;
    float dby = by2 - by1;

    float cross = dax * dby - day * dbx;

    // Parallel or collinear
    if (std::fabs(cross) < 1e-10f) {
        return false;
    }

    float dx = bx1 - ax1;
    float dy = by1 - ay1;

    float t = (dx * dby - dy * dbx) / cross;
    float u = (dx * day - dy * dax) / cross;

    if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
        if (out_x && out_y) {
            *out_x = ax1 + t * dax;
            *out_y = ay1 + t * day;
        }
        return true;
    }

    return false;
}

PackedInt32Array CollisionOps::segments_intersect(
    const PackedVector2Array& starts_a, const PackedVector2Array& ends_a,
    const PackedVector2Array& starts_b, const PackedVector2Array& ends_b) {

    PackedInt32Array result;
    int32_t count_a = std::min(starts_a.size(), ends_a.size());
    int32_t count_b = std::min(starts_b.size(), ends_b.size());

    const Vector2* sa = starts_a.ptr();
    const Vector2* ea = ends_a.ptr();
    const Vector2* sb = starts_b.ptr();
    const Vector2* eb = ends_b.ptr();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            if (segments_intersect_impl(sa[i].x, sa[i].y, ea[i].x, ea[i].y,
                                        sb[j].x, sb[j].y, eb[j].x, eb[j].y)) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

PackedVector2Array CollisionOps::segment_intersection_points(
    const PackedVector2Array& starts_a, const PackedVector2Array& ends_a,
    const PackedVector2Array& starts_b, const PackedVector2Array& ends_b) {

    PackedVector2Array result;
    int32_t count_a = std::min(starts_a.size(), ends_a.size());
    int32_t count_b = std::min(starts_b.size(), ends_b.size());

    const Vector2* sa = starts_a.ptr();
    const Vector2* ea = ends_a.ptr();
    const Vector2* sb = starts_b.ptr();
    const Vector2* eb = ends_b.ptr();

    for (int32_t i = 0; i < count_a; i++) {
        for (int32_t j = 0; j < count_b; j++) {
            float ix, iy;
            if (segments_intersect_impl(sa[i].x, sa[i].y, ea[i].x, ea[i].y,
                                        sb[j].x, sb[j].y, eb[j].x, eb[j].y,
                                        &ix, &iy)) {
                result.push_back(Vector2(ix, iy));
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::segments_self_intersect(
    const PackedVector2Array& starts, const PackedVector2Array& ends) {

    PackedInt32Array result;
    int32_t count = std::min(starts.size(), ends.size());

    const Vector2* s = starts.ptr();
    const Vector2* e = ends.ptr();

    for (int32_t i = 0; i < count; i++) {
        for (int32_t j = i + 1; j < count; j++) {
            if (segments_intersect_impl(s[i].x, s[i].y, e[i].x, e[i].y,
                                        s[j].x, s[j].y, e[j].x, e[j].y)) {
                result.push_back(i);
                result.push_back(j);
            }
        }
    }

    return result;
}

// ========== CLOSEST POINT QUERIES ==========

PackedVector2Array CollisionOps::closest_points_on_segments(
    const PackedVector2Array& points,
    const PackedVector2Array& seg_starts, const PackedVector2Array& seg_ends) {

    int32_t point_count = points.size();
    int32_t seg_count = std::min(seg_starts.size(), seg_ends.size());

    PackedVector2Array result;
    result.resize(point_count * seg_count);

    const Vector2* p = points.ptr();
    const Vector2* ss = seg_starts.ptr();
    const Vector2* se = seg_ends.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < point_count; i++) {
        for (int32_t j = 0; j < seg_count; j++) {
            // Vector from segment start to end
            float dx = se[j].x - ss[j].x;
            float dy = se[j].y - ss[j].y;
            float len_sq = dx * dx + dy * dy;

            if (len_sq < 1e-10f) {
                // Degenerate segment (point)
                dst[i * seg_count + j] = ss[j];
            } else {
                // Project point onto segment line
                float t = ((p[i].x - ss[j].x) * dx + (p[i].y - ss[j].y) * dy) / len_sq;
                t = std::fmax(0.0f, std::fmin(1.0f, t));
                dst[i * seg_count + j] = Vector2(ss[j].x + t * dx, ss[j].y + t * dy);
            }
        }
    }

    return result;
}

PackedInt32Array CollisionOps::closest_circle_indices(
    const PackedVector2Array& points,
    const PackedVector2Array& centers, const PackedFloat32Array& radii) {

    int32_t point_count = points.size();
    int32_t circle_count = std::min(centers.size(), radii.size());

    PackedInt32Array result;
    result.resize(point_count);

    const Vector2* p = points.ptr();
    const Vector2* c = centers.ptr();
    const float* r = radii.ptr();
    int32_t* dst = result.ptrw();

    for (int32_t i = 0; i < point_count; i++) {
        int32_t best_idx = 0;
        float best_dist = std::numeric_limits<float>::infinity();

        for (int32_t j = 0; j < circle_count; j++) {
            float dx = p[i].x - c[j].x;
            float dy = p[i].y - c[j].y;
            float dist = std::sqrt(dx * dx + dy * dy) - r[j];
            if (dist < best_dist) {
                best_dist = dist;
                best_idx = j;
            }
        }

        dst[i] = best_idx;
    }

    return result;
}

PackedInt32Array CollisionOps::closest_circle_indices_uniform(
    const PackedVector2Array& points,
    const PackedVector2Array& centers, float radius) {

    int32_t point_count = points.size();
    int32_t circle_count = centers.size();

    PackedInt32Array result;
    result.resize(point_count);

    const Vector2* p = points.ptr();
    const Vector2* c = centers.ptr();
    int32_t* dst = result.ptrw();

    for (int32_t i = 0; i < point_count; i++) {
        int32_t best_idx = 0;
        float best_dist_sq = std::numeric_limits<float>::infinity();

        for (int32_t j = 0; j < circle_count; j++) {
            float dx = p[i].x - c[j].x;
            float dy = p[i].y - c[j].y;
            float dist_sq = dx * dx + dy * dy;
            if (dist_sq < best_dist_sq) {
                best_dist_sq = dist_sq;
                best_idx = j;
            }
        }

        dst[i] = best_idx;
    }

    return result;
}

}

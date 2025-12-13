/**
 * RandomOps Implementation
 *
 * Uses xoshiro256** for high-quality, fast random number generation.
 * This PRNG has excellent statistical properties and is very fast.
 */

#include "random_ops.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <chrono>
#include <algorithm>
#include <vector>

namespace godot {

// Rotate left helper
static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

// SplitMix64 for seeding (generates initial state from single seed)
static uint64_t splitmix64(uint64_t& state) {
    uint64_t z = (state += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
}

void RandomOps::_bind_methods() {
    // Seeding
    ClassDB::bind_method(D_METHOD("seed", "seed_value"), &RandomOps::seed);
    ClassDB::bind_method(D_METHOD("seed_from_time"), &RandomOps::seed_from_time);

    // Float generation
    ClassDB::bind_method(D_METHOD("randf_array", "count"), &RandomOps::randf_array);
    ClassDB::bind_method(D_METHOD("randf_range_array", "count", "min_val", "max_val"), &RandomOps::randf_range_array);

    // Integer generation
    ClassDB::bind_method(D_METHOD("randi_array", "count"), &RandomOps::randi_array);
    ClassDB::bind_method(D_METHOD("randi_range_array", "count", "min_val", "max_val"), &RandomOps::randi_range_array);

    // Points in shapes
    ClassDB::bind_method(D_METHOD("rand_points_in_rect", "count", "rect"), &RandomOps::rand_points_in_rect);
    ClassDB::bind_method(D_METHOD("rand_points_in_box", "count", "box"), &RandomOps::rand_points_in_box);
    ClassDB::bind_method(D_METHOD("rand_points_in_circle", "count", "center", "radius"), &RandomOps::rand_points_in_circle);
    ClassDB::bind_method(D_METHOD("rand_points_in_sphere", "count", "center", "radius"), &RandomOps::rand_points_in_sphere);
    ClassDB::bind_method(D_METHOD("rand_points_on_circle", "count", "center", "radius"), &RandomOps::rand_points_on_circle);
    ClassDB::bind_method(D_METHOD("rand_points_on_sphere", "count", "center", "radius"), &RandomOps::rand_points_on_sphere);

    // Shuffling
    ClassDB::bind_method(D_METHOD("shuffle_indices", "count"), &RandomOps::shuffle_indices);
    ClassDB::bind_method(D_METHOD("shuffle_floats", "values"), &RandomOps::shuffle_floats);
    ClassDB::bind_method(D_METHOD("shuffle_ints", "values"), &RandomOps::shuffle_ints);

    // Weighted selection
    ClassDB::bind_method(D_METHOD("weighted_choice", "weights", "count"), &RandomOps::weighted_choice);
    ClassDB::bind_method(D_METHOD("weighted_choice_one", "weights"), &RandomOps::weighted_choice_one);

    // Poisson disk sampling
    ClassDB::bind_method(D_METHOD("poisson_disk_2d", "bounds", "min_distance", "max_attempts"), &RandomOps::poisson_disk_2d, DEFVAL(30));
    ClassDB::bind_method(D_METHOD("poisson_disk_3d", "bounds", "min_distance", "max_attempts"), &RandomOps::poisson_disk_3d, DEFVAL(30));

    // Random directions
    ClassDB::bind_method(D_METHOD("rand_directions_2d", "count"), &RandomOps::rand_directions_2d);
    ClassDB::bind_method(D_METHOD("rand_directions_3d", "count"), &RandomOps::rand_directions_3d);

    // Gaussian distribution
    ClassDB::bind_method(D_METHOD("randn_array", "count"), &RandomOps::randn_array);
    ClassDB::bind_method(D_METHOD("randn_array_params", "count", "mean", "std_dev"), &RandomOps::randn_array_params);
}

RandomOps::RandomOps() {
    // Initialize with time-based seed by default
    seed_from_time();
}

RandomOps::~RandomOps() {
}

// ========== INTERNAL RNG (xoshiro256**) ==========

uint64_t RandomOps::next() {
    const uint64_t result = rotl(state[1] * 5, 7) * 9;

    const uint64_t t = state[1] << 17;

    state[2] ^= state[0];
    state[3] ^= state[1];
    state[1] ^= state[2];
    state[0] ^= state[3];

    state[2] ^= t;

    state[3] = rotl(state[3], 45);

    return result;
}

double RandomOps::next_double() {
    // Generate double in [0, 1) with 53 bits of precision
    return (next() >> 11) * (1.0 / 9007199254740992.0);
}

float RandomOps::next_float() {
    // Generate float in [0, 1) with 24 bits of precision
    return (next() >> 40) * (1.0f / 16777216.0f);
}

// ========== SEEDING ==========

void RandomOps::seed(int64_t seed_value) {
    uint64_t s = static_cast<uint64_t>(seed_value);
    state[0] = splitmix64(s);
    state[1] = splitmix64(s);
    state[2] = splitmix64(s);
    state[3] = splitmix64(s);
}

void RandomOps::seed_from_time() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    seed(static_cast<int64_t>(ns));
}

// ========== BULK FLOAT GENERATION ==========

PackedFloat32Array RandomOps::randf_array(int count) {
    if (count <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(count);
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = next_float();
    }

    return result;
}

PackedFloat32Array RandomOps::randf_range_array(int count, float min_val, float max_val) {
    if (count <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(count);
    float* dst = result.ptrw();
    float range = max_val - min_val;

    for (int i = 0; i < count; i++) {
        dst[i] = min_val + next_float() * range;
    }

    return result;
}

// ========== BULK INTEGER GENERATION ==========

PackedInt32Array RandomOps::randi_array(int count) {
    if (count <= 0) return PackedInt32Array();

    PackedInt32Array result;
    result.resize(count);
    int32_t* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = static_cast<int32_t>(next());
    }

    return result;
}

PackedInt32Array RandomOps::randi_range_array(int count, int min_val, int max_val) {
    if (count <= 0 || min_val > max_val) return PackedInt32Array();

    PackedInt32Array result;
    result.resize(count);
    int32_t* dst = result.ptrw();

    uint64_t range = static_cast<uint64_t>(max_val - min_val) + 1;

    for (int i = 0; i < count; i++) {
        // Unbiased bounded random integer using rejection sampling
        uint64_t x;
        uint64_t threshold = (-range) % range; // = 2^64 mod range
        do {
            x = next();
        } while (x < threshold);
        dst[i] = min_val + static_cast<int32_t>(x % range);
    }

    return result;
}

// ========== RANDOM POINTS IN SHAPES ==========

PackedVector2Array RandomOps::rand_points_in_rect(int count, const Rect2& rect) {
    if (count <= 0) return PackedVector2Array();

    PackedVector2Array result;
    result.resize(count);
    Vector2* dst = result.ptrw();

    float x0 = rect.position.x;
    float y0 = rect.position.y;
    float w = rect.size.x;
    float h = rect.size.y;

    for (int i = 0; i < count; i++) {
        dst[i] = Vector2(x0 + next_float() * w, y0 + next_float() * h);
    }

    return result;
}

PackedVector3Array RandomOps::rand_points_in_box(int count, const AABB& box) {
    if (count <= 0) return PackedVector3Array();

    PackedVector3Array result;
    result.resize(count);
    Vector3* dst = result.ptrw();

    float x0 = box.position.x;
    float y0 = box.position.y;
    float z0 = box.position.z;
    float sx = box.size.x;
    float sy = box.size.y;
    float sz = box.size.z;

    for (int i = 0; i < count; i++) {
        dst[i] = Vector3(
            x0 + next_float() * sx,
            y0 + next_float() * sy,
            z0 + next_float() * sz
        );
    }

    return result;
}

PackedVector2Array RandomOps::rand_points_in_circle(int count, const Vector2& center, float radius) {
    if (count <= 0 || radius <= 0.0f) return PackedVector2Array();

    PackedVector2Array result;
    result.resize(count);
    Vector2* dst = result.ptrw();

    // Use rejection sampling for uniform distribution
    // Alternative: sqrt(r) * direction but rejection is often faster
    float r2 = radius * radius;

    for (int i = 0; i < count; i++) {
        float x, y;
        do {
            x = (next_float() * 2.0f - 1.0f) * radius;
            y = (next_float() * 2.0f - 1.0f) * radius;
        } while (x * x + y * y > r2);
        dst[i] = Vector2(center.x + x, center.y + y);
    }

    return result;
}

PackedVector3Array RandomOps::rand_points_in_sphere(int count, const Vector3& center, float radius) {
    if (count <= 0 || radius <= 0.0f) return PackedVector3Array();

    PackedVector3Array result;
    result.resize(count);
    Vector3* dst = result.ptrw();

    float r2 = radius * radius;

    for (int i = 0; i < count; i++) {
        float x, y, z;
        do {
            x = (next_float() * 2.0f - 1.0f) * radius;
            y = (next_float() * 2.0f - 1.0f) * radius;
            z = (next_float() * 2.0f - 1.0f) * radius;
        } while (x * x + y * y + z * z > r2);
        dst[i] = Vector3(center.x + x, center.y + y, center.z + z);
    }

    return result;
}

PackedVector2Array RandomOps::rand_points_on_circle(int count, const Vector2& center, float radius) {
    if (count <= 0) return PackedVector2Array();

    PackedVector2Array result;
    result.resize(count);
    Vector2* dst = result.ptrw();

    const float TWO_PI = 6.283185307179586f;

    for (int i = 0; i < count; i++) {
        float angle = next_float() * TWO_PI;
        dst[i] = Vector2(
            center.x + std::cos(angle) * radius,
            center.y + std::sin(angle) * radius
        );
    }

    return result;
}

PackedVector3Array RandomOps::rand_points_on_sphere(int count, const Vector3& center, float radius) {
    if (count <= 0) return PackedVector3Array();

    PackedVector3Array result;
    result.resize(count);
    Vector3* dst = result.ptrw();

    const float TWO_PI = 6.283185307179586f;

    // Use spherical coordinates with uniform distribution
    for (int i = 0; i < count; i++) {
        float u = next_float();
        float v = next_float();

        float theta = TWO_PI * u;
        float phi = std::acos(2.0f * v - 1.0f);

        float sin_phi = std::sin(phi);
        dst[i] = Vector3(
            center.x + radius * sin_phi * std::cos(theta),
            center.y + radius * sin_phi * std::sin(theta),
            center.z + radius * std::cos(phi)
        );
    }

    return result;
}

// ========== SHUFFLING ==========

PackedInt32Array RandomOps::shuffle_indices(int count) {
    if (count <= 0) return PackedInt32Array();

    PackedInt32Array result;
    result.resize(count);
    int32_t* dst = result.ptrw();

    // Initialize with 0..count-1
    for (int i = 0; i < count; i++) {
        dst[i] = i;
    }

    // Fisher-Yates shuffle
    for (int i = count - 1; i > 0; i--) {
        int j = next() % (i + 1);
        int32_t tmp = dst[i];
        dst[i] = dst[j];
        dst[j] = tmp;
    }

    return result;
}

PackedFloat32Array RandomOps::shuffle_floats(const PackedFloat32Array& values) {
    int count = values.size();
    if (count <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(count);

    const float* src = values.ptr();
    float* dst = result.ptrw();

    // Copy to result
    for (int i = 0; i < count; i++) {
        dst[i] = src[i];
    }

    // Fisher-Yates shuffle
    for (int i = count - 1; i > 0; i--) {
        int j = next() % (i + 1);
        float tmp = dst[i];
        dst[i] = dst[j];
        dst[j] = tmp;
    }

    return result;
}

PackedInt32Array RandomOps::shuffle_ints(const PackedInt32Array& values) {
    int count = values.size();
    if (count <= 0) return PackedInt32Array();

    PackedInt32Array result;
    result.resize(count);

    const int32_t* src = values.ptr();
    int32_t* dst = result.ptrw();

    // Copy to result
    for (int i = 0; i < count; i++) {
        dst[i] = src[i];
    }

    // Fisher-Yates shuffle
    for (int i = count - 1; i > 0; i--) {
        int j = next() % (i + 1);
        int32_t tmp = dst[i];
        dst[i] = dst[j];
        dst[j] = tmp;
    }

    return result;
}

// ========== WEIGHTED SELECTION ==========

int RandomOps::weighted_choice_one(const PackedFloat32Array& weights) {
    int count = weights.size();
    if (count <= 0) return -1;

    const float* w = weights.ptr();

    // Calculate sum
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        if (w[i] > 0.0f) sum += w[i];
    }

    if (sum <= 0.0) return 0; // Fallback to first element

    // Pick random value and find corresponding index
    double target = next_double() * sum;
    double cumulative = 0.0;

    for (int i = 0; i < count; i++) {
        if (w[i] > 0.0f) {
            cumulative += w[i];
            if (target < cumulative) {
                return i;
            }
        }
    }

    return count - 1; // Fallback
}

PackedInt32Array RandomOps::weighted_choice(const PackedFloat32Array& weights, int count) {
    if (count <= 0) return PackedInt32Array();

    int n = weights.size();
    if (n <= 0) return PackedInt32Array();

    PackedInt32Array result;
    result.resize(count);
    int32_t* dst = result.ptrw();

    const float* w = weights.ptr();

    // Build cumulative distribution
    std::vector<double> cumulative(n);
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        if (w[i] > 0.0f) sum += w[i];
        cumulative[i] = sum;
    }

    if (sum <= 0.0) {
        // All zero weights, return all zeros
        for (int i = 0; i < count; i++) {
            dst[i] = 0;
        }
        return result;
    }

    // Sample with replacement
    for (int i = 0; i < count; i++) {
        double target = next_double() * sum;

        // Binary search for index
        int lo = 0, hi = n - 1;
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (cumulative[mid] <= target) {
                lo = mid + 1;
            } else {
                hi = mid;
            }
        }
        dst[i] = lo;
    }

    return result;
}

// ========== POISSON DISK SAMPLING ==========

PackedVector2Array RandomOps::poisson_disk_2d(const Rect2& bounds, float min_distance, int max_attempts) {
    if (min_distance <= 0.0f) return PackedVector2Array();

    // Cell size for spatial acceleration
    float cell_size = min_distance / std::sqrt(2.0f);
    int grid_width = static_cast<int>(std::ceil(bounds.size.x / cell_size));
    int grid_height = static_cast<int>(std::ceil(bounds.size.y / cell_size));

    if (grid_width <= 0 || grid_height <= 0) return PackedVector2Array();

    // Grid stores index into points array, -1 = empty
    std::vector<int> grid(grid_width * grid_height, -1);
    std::vector<Vector2> points;
    std::vector<int> active_list;

    float min_dist_sq = min_distance * min_distance;

    // Helper to get grid cell
    auto get_cell = [&](const Vector2& p) -> std::pair<int, int> {
        int cx = static_cast<int>((p.x - bounds.position.x) / cell_size);
        int cy = static_cast<int>((p.y - bounds.position.y) / cell_size);
        return {std::clamp(cx, 0, grid_width - 1), std::clamp(cy, 0, grid_height - 1)};
    };

    // Check if point is valid
    auto is_valid = [&](const Vector2& p) -> bool {
        // Check bounds
        if (p.x < bounds.position.x || p.x >= bounds.position.x + bounds.size.x ||
            p.y < bounds.position.y || p.y >= bounds.position.y + bounds.size.y) {
            return false;
        }

        auto [cx, cy] = get_cell(p);

        // Check 5x5 neighborhood
        for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                int nx = cx + dx;
                int ny = cy + dy;
                if (nx >= 0 && nx < grid_width && ny >= 0 && ny < grid_height) {
                    int idx = grid[ny * grid_width + nx];
                    if (idx >= 0) {
                        float dist_x = p.x - points[idx].x;
                        float dist_y = p.y - points[idx].y;
                        if (dist_x * dist_x + dist_y * dist_y < min_dist_sq) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    };

    // Start with first point
    Vector2 first(
        bounds.position.x + next_float() * bounds.size.x,
        bounds.position.y + next_float() * bounds.size.y
    );
    points.push_back(first);
    active_list.push_back(0);
    auto [fx, fy] = get_cell(first);
    grid[fy * grid_width + fx] = 0;

    const float TWO_PI = 6.283185307179586f;

    // Main loop
    while (!active_list.empty()) {
        // Pick random active point
        int active_idx = next() % active_list.size();
        Vector2 center = points[active_list[active_idx]];
        bool found = false;

        for (int attempt = 0; attempt < max_attempts; attempt++) {
            // Generate point in annulus [min_distance, 2*min_distance]
            float angle = next_float() * TWO_PI;
            float r = min_distance + next_float() * min_distance;
            Vector2 candidate(
                center.x + std::cos(angle) * r,
                center.y + std::sin(angle) * r
            );

            if (is_valid(candidate)) {
                int new_idx = static_cast<int>(points.size());
                points.push_back(candidate);
                active_list.push_back(new_idx);
                auto [ccx, ccy] = get_cell(candidate);
                grid[ccy * grid_width + ccx] = new_idx;
                found = true;
                break;
            }
        }

        if (!found) {
            // Remove from active list
            active_list[active_idx] = active_list.back();
            active_list.pop_back();
        }
    }

    // Convert to PackedVector2Array
    PackedVector2Array result;
    result.resize(static_cast<int>(points.size()));
    Vector2* dst = result.ptrw();
    for (size_t i = 0; i < points.size(); i++) {
        dst[i] = points[i];
    }

    return result;
}

PackedVector3Array RandomOps::poisson_disk_3d(const AABB& bounds, float min_distance, int max_attempts) {
    if (min_distance <= 0.0f) return PackedVector3Array();

    // Cell size for spatial acceleration
    float cell_size = min_distance / std::sqrt(3.0f);
    int grid_x = static_cast<int>(std::ceil(bounds.size.x / cell_size));
    int grid_y = static_cast<int>(std::ceil(bounds.size.y / cell_size));
    int grid_z = static_cast<int>(std::ceil(bounds.size.z / cell_size));

    if (grid_x <= 0 || grid_y <= 0 || grid_z <= 0) return PackedVector3Array();

    // Grid stores index into points array, -1 = empty
    std::vector<int> grid(grid_x * grid_y * grid_z, -1);
    std::vector<Vector3> points;
    std::vector<int> active_list;

    float min_dist_sq = min_distance * min_distance;

    auto grid_index = [&](int x, int y, int z) -> int {
        return z * grid_x * grid_y + y * grid_x + x;
    };

    auto get_cell = [&](const Vector3& p) -> std::tuple<int, int, int> {
        int cx = static_cast<int>((p.x - bounds.position.x) / cell_size);
        int cy = static_cast<int>((p.y - bounds.position.y) / cell_size);
        int cz = static_cast<int>((p.z - bounds.position.z) / cell_size);
        return {std::clamp(cx, 0, grid_x - 1),
                std::clamp(cy, 0, grid_y - 1),
                std::clamp(cz, 0, grid_z - 1)};
    };

    auto is_valid = [&](const Vector3& p) -> bool {
        if (p.x < bounds.position.x || p.x >= bounds.position.x + bounds.size.x ||
            p.y < bounds.position.y || p.y >= bounds.position.y + bounds.size.y ||
            p.z < bounds.position.z || p.z >= bounds.position.z + bounds.size.z) {
            return false;
        }

        auto [cx, cy, cz] = get_cell(p);

        for (int dz = -2; dz <= 2; dz++) {
            for (int dy = -2; dy <= 2; dy++) {
                for (int dx = -2; dx <= 2; dx++) {
                    int nx = cx + dx;
                    int ny = cy + dy;
                    int nz = cz + dz;
                    if (nx >= 0 && nx < grid_x &&
                        ny >= 0 && ny < grid_y &&
                        nz >= 0 && nz < grid_z) {
                        int idx = grid[grid_index(nx, ny, nz)];
                        if (idx >= 0) {
                            float dx2 = p.x - points[idx].x;
                            float dy2 = p.y - points[idx].y;
                            float dz2 = p.z - points[idx].z;
                            if (dx2 * dx2 + dy2 * dy2 + dz2 * dz2 < min_dist_sq) {
                                return false;
                            }
                        }
                    }
                }
            }
        }
        return true;
    };

    // First point
    Vector3 first(
        bounds.position.x + next_float() * bounds.size.x,
        bounds.position.y + next_float() * bounds.size.y,
        bounds.position.z + next_float() * bounds.size.z
    );
    points.push_back(first);
    active_list.push_back(0);
    auto [fx, fy, fz] = get_cell(first);
    grid[grid_index(fx, fy, fz)] = 0;

    const float TWO_PI = 6.283185307179586f;

    while (!active_list.empty()) {
        int active_idx = next() % active_list.size();
        Vector3 center = points[active_list[active_idx]];
        bool found = false;

        for (int attempt = 0; attempt < max_attempts; attempt++) {
            // Random point in spherical shell
            float u = next_float();
            float v = next_float();
            float theta = TWO_PI * u;
            float phi = std::acos(2.0f * v - 1.0f);
            float r = min_distance + next_float() * min_distance;

            float sin_phi = std::sin(phi);
            Vector3 candidate(
                center.x + r * sin_phi * std::cos(theta),
                center.y + r * sin_phi * std::sin(theta),
                center.z + r * std::cos(phi)
            );

            if (is_valid(candidate)) {
                int new_idx = static_cast<int>(points.size());
                points.push_back(candidate);
                active_list.push_back(new_idx);
                auto [ccx, ccy, ccz] = get_cell(candidate);
                grid[grid_index(ccx, ccy, ccz)] = new_idx;
                found = true;
                break;
            }
        }

        if (!found) {
            active_list[active_idx] = active_list.back();
            active_list.pop_back();
        }
    }

    PackedVector3Array result;
    result.resize(static_cast<int>(points.size()));
    Vector3* dst = result.ptrw();
    for (size_t i = 0; i < points.size(); i++) {
        dst[i] = points[i];
    }

    return result;
}

// ========== RANDOM DIRECTIONS ==========

PackedVector2Array RandomOps::rand_directions_2d(int count) {
    if (count <= 0) return PackedVector2Array();

    PackedVector2Array result;
    result.resize(count);
    Vector2* dst = result.ptrw();

    const float TWO_PI = 6.283185307179586f;

    for (int i = 0; i < count; i++) {
        float angle = next_float() * TWO_PI;
        dst[i] = Vector2(std::cos(angle), std::sin(angle));
    }

    return result;
}

PackedVector3Array RandomOps::rand_directions_3d(int count) {
    if (count <= 0) return PackedVector3Array();

    PackedVector3Array result;
    result.resize(count);
    Vector3* dst = result.ptrw();

    const float TWO_PI = 6.283185307179586f;

    for (int i = 0; i < count; i++) {
        float u = next_float();
        float v = next_float();
        float theta = TWO_PI * u;
        float phi = std::acos(2.0f * v - 1.0f);
        float sin_phi = std::sin(phi);
        dst[i] = Vector3(
            sin_phi * std::cos(theta),
            sin_phi * std::sin(theta),
            std::cos(phi)
        );
    }

    return result;
}

// ========== GAUSSIAN/NORMAL DISTRIBUTION ==========

PackedFloat32Array RandomOps::randn_array(int count) {
    if (count <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(count);
    float* dst = result.ptrw();

    const float TWO_PI = 6.283185307179586f;

    // Box-Muller transform generates pairs
    int pairs = count / 2;
    for (int i = 0; i < pairs; i++) {
        float u1 = next_float();
        float u2 = next_float();

        // Avoid log(0)
        if (u1 < 1e-10f) u1 = 1e-10f;

        float r = std::sqrt(-2.0f * std::log(u1));
        float theta = TWO_PI * u2;

        dst[i * 2] = r * std::cos(theta);
        dst[i * 2 + 1] = r * std::sin(theta);
    }

    // Handle odd count
    if (count % 2 == 1) {
        float u1 = next_float();
        float u2 = next_float();
        if (u1 < 1e-10f) u1 = 1e-10f;
        float r = std::sqrt(-2.0f * std::log(u1));
        float theta = TWO_PI * u2;
        dst[count - 1] = r * std::cos(theta);
    }

    return result;
}

PackedFloat32Array RandomOps::randn_array_params(int count, float mean, float std_dev) {
    PackedFloat32Array result = randn_array(count);
    if (result.size() == 0) return result;

    float* dst = result.ptrw();
    for (int i = 0; i < count; i++) {
        dst[i] = dst[i] * std_dev + mean;
    }

    return result;
}

}

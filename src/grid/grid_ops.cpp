/**
 * GridOps Implementation
 */

#include "grid_ops.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <queue>
#include <vector>
#include <unordered_set>

namespace godot {

void GridOps::_bind_methods() {
    // Coordinate conversion
    ClassDB::bind_static_method("GridOps", D_METHOD("to_index", "x", "y", "width"), &GridOps::to_index);
    ClassDB::bind_static_method("GridOps", D_METHOD("to_coords", "index", "width"), &GridOps::to_coords);
    ClassDB::bind_static_method("GridOps", D_METHOD("to_indices", "coords", "width"), &GridOps::to_indices);
    ClassDB::bind_static_method("GridOps", D_METHOD("to_coords_batch", "indices", "width"), &GridOps::to_coords_batch);
    ClassDB::bind_static_method("GridOps", D_METHOD("in_bounds", "x", "y", "width", "height"), &GridOps::in_bounds);
    ClassDB::bind_static_method("GridOps", D_METHOD("index_in_bounds", "index", "width", "height"), &GridOps::index_in_bounds);

    // Neighbors
    ClassDB::bind_static_method("GridOps", D_METHOD("neighbors_4", "index", "width", "height"), &GridOps::neighbors_4);
    ClassDB::bind_static_method("GridOps", D_METHOD("neighbors_8", "index", "width", "height"), &GridOps::neighbors_8);
    ClassDB::bind_static_method("GridOps", D_METHOD("neighbors_4_batch", "indices", "width", "height"), &GridOps::neighbors_4_batch);
    ClassDB::bind_static_method("GridOps", D_METHOD("neighbors_8_batch", "indices", "width", "height"), &GridOps::neighbors_8_batch);
    ClassDB::bind_static_method("GridOps", D_METHOD("neighbors_custom", "index", "width", "height", "offsets"), &GridOps::neighbors_custom);

    // Flood fill
    ClassDB::bind_static_method("GridOps", D_METHOD("flood_fill", "grid", "width", "start_index", "target_value"), &GridOps::flood_fill);
    ClassDB::bind_static_method("GridOps", D_METHOD("flood_fill_walkable", "grid", "width", "start_index", "walkable_value"), &GridOps::flood_fill_walkable);
    ClassDB::bind_static_method("GridOps", D_METHOD("flood_fill_limited", "grid", "width", "start_index", "target_value", "max_distance"), &GridOps::flood_fill_limited);

    // Line of sight
    ClassDB::bind_static_method("GridOps", D_METHOD("line_indices", "from", "to", "width"), &GridOps::line_indices);
    ClassDB::bind_static_method("GridOps", D_METHOD("line_clear", "grid", "width", "from", "to", "blocking_value"), &GridOps::line_clear);
    ClassDB::bind_static_method("GridOps", D_METHOD("raycast_grid", "grid", "width", "from", "direction", "max_distance", "blocking_value"), &GridOps::raycast_grid);
    ClassDB::bind_static_method("GridOps", D_METHOD("line_first_blocking", "grid", "width", "from", "to", "blocking_value"), &GridOps::line_first_blocking);

    // Field of view
    ClassDB::bind_static_method("GridOps", D_METHOD("fov_shadowcast", "grid", "width", "height", "origin", "radius", "blocking_value"), &GridOps::fov_shadowcast);
    ClassDB::bind_static_method("GridOps", D_METHOD("fov_raycast", "grid", "width", "height", "origin", "radius", "blocking_value", "ray_count"), &GridOps::fov_raycast);

    // Distance transforms
    ClassDB::bind_static_method("GridOps", D_METHOD("manhattan_distance_field", "grid", "width", "height", "target_value"), &GridOps::manhattan_distance_field);
    ClassDB::bind_static_method("GridOps", D_METHOD("chebyshev_distance_field", "grid", "width", "height", "target_value"), &GridOps::chebyshev_distance_field);
    ClassDB::bind_static_method("GridOps", D_METHOD("euclidean_distance_field", "grid", "width", "height", "target_value"), &GridOps::euclidean_distance_field);

    // Connected components
    ClassDB::bind_static_method("GridOps", D_METHOD("label_connected_components", "grid", "width", "height", "target_value"), &GridOps::label_connected_components);
    ClassDB::bind_static_method("GridOps", D_METHOD("count_connected_components", "grid", "width", "height", "target_value"), &GridOps::count_connected_components);
    ClassDB::bind_static_method("GridOps", D_METHOD("component_sizes", "grid", "width", "height", "target_value"), &GridOps::component_sizes);

    // Utilities
    ClassDB::bind_static_method("GridOps", D_METHOD("find_value", "grid", "value"), &GridOps::find_value);
    ClassDB::bind_static_method("GridOps", D_METHOD("find_not_value", "grid", "value"), &GridOps::find_not_value);
    ClassDB::bind_static_method("GridOps", D_METHOD("count_value", "grid", "value"), &GridOps::count_value);
    ClassDB::bind_static_method("GridOps", D_METHOD("get_region", "grid", "grid_width", "x", "y", "region_width", "region_height"), &GridOps::get_region);
    ClassDB::bind_static_method("GridOps", D_METHOD("set_region", "grid", "grid_width", "x", "y", "region_width", "region_height", "values"), &GridOps::set_region);
}

// ========== COORDINATE CONVERSION ==========

int GridOps::to_index(int x, int y, int width) {
    return y * width + x;
}

Vector2i GridOps::to_coords(int index, int width) {
    return Vector2i(index % width, index / width);
}

PackedInt32Array GridOps::to_indices(const PackedVector2Array& coords, int width) {
    int32_t count = coords.size();
    PackedInt32Array result;
    result.resize(count);

    const Vector2* src = coords.ptr();
    int32_t* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = static_cast<int>(src[i].y) * width + static_cast<int>(src[i].x);
    }

    return result;
}

PackedVector2Array GridOps::to_coords_batch(const PackedInt32Array& indices, int width) {
    int32_t count = indices.size();
    PackedVector2Array result;
    result.resize(count);

    const int32_t* src = indices.ptr();
    Vector2* dst = result.ptrw();

    for (int32_t i = 0; i < count; i++) {
        dst[i] = Vector2(static_cast<float>(src[i] % width), static_cast<float>(src[i] / width));
    }

    return result;
}

bool GridOps::in_bounds(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool GridOps::index_in_bounds(int index, int width, int height) {
    return index >= 0 && index < width * height;
}

// ========== NEIGHBOR QUERIES ==========

PackedInt32Array GridOps::neighbors_4(int index, int width, int height) {
    PackedInt32Array result;
    int x = index % width;
    int y = index / width;

    // Cardinal directions: N, E, S, W
    if (y > 0) result.append((y - 1) * width + x);           // North
    if (x < width - 1) result.append(y * width + (x + 1));   // East
    if (y < height - 1) result.append((y + 1) * width + x);  // South
    if (x > 0) result.append(y * width + (x - 1));           // West

    return result;
}

PackedInt32Array GridOps::neighbors_8(int index, int width, int height) {
    PackedInt32Array result;
    int x = index % width;
    int y = index / width;

    // All 8 directions
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                result.append(ny * width + nx);
            }
        }
    }

    return result;
}

PackedInt32Array GridOps::neighbors_4_batch(const PackedInt32Array& indices, int width, int height) {
    PackedInt32Array result;
    const int32_t* src = indices.ptr();
    int32_t count = indices.size();

    // Pre-allocate with reasonable estimate (4 neighbors per cell max)
    result.resize(count * 4);
    int32_t* dst = result.ptrw();
    int32_t out_count = 0;

    static const int dx[] = {0, 1, 0, -1};
    static const int dy[] = {-1, 0, 1, 0};

    for (int32_t i = 0; i < count; i++) {
        int x = src[i] % width;
        int y = src[i] / width;

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                dst[out_count++] = ny * width + nx;
            }
        }
    }

    result.resize(out_count);
    return result;
}

PackedInt32Array GridOps::neighbors_8_batch(const PackedInt32Array& indices, int width, int height) {
    PackedInt32Array result;
    const int32_t* src = indices.ptr();
    int32_t count = indices.size();

    result.resize(count * 8);
    int32_t* dst = result.ptrw();
    int32_t out_count = 0;

    for (int32_t i = 0; i < count; i++) {
        int x = src[i] % width;
        int y = src[i] / width;

        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    dst[out_count++] = ny * width + nx;
                }
            }
        }
    }

    result.resize(out_count);
    return result;
}

PackedInt32Array GridOps::neighbors_custom(int index, int width, int height, const PackedVector2Array& offsets) {
    PackedInt32Array result;
    int x = index % width;
    int y = index / width;

    const Vector2* off = offsets.ptr();
    int32_t offset_count = offsets.size();

    for (int32_t i = 0; i < offset_count; i++) {
        int nx = x + static_cast<int>(off[i].x);
        int ny = y + static_cast<int>(off[i].y);
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            result.append(ny * width + nx);
        }
    }

    return result;
}

// ========== FLOOD FILL ==========

PackedInt32Array GridOps::flood_fill(const PackedInt32Array& grid, int width, int start_index, int target_value) {
    PackedInt32Array result;
    int32_t grid_size = grid.size();

    if (start_index < 0 || start_index >= grid_size) return result;
    if (grid[start_index] != target_value) return result;

    int height = grid_size / width;

    std::vector<bool> visited(grid_size, false);
    std::queue<int> queue;

    queue.push(start_index);
    visited[start_index] = true;

    const int32_t* grid_ptr = grid.ptr();

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        result.append(current);

        int x = current % width;
        int y = current / width;

        // Check 4 neighbors
        static const int dx[] = {0, 1, 0, -1};
        static const int dy[] = {-1, 0, 1, 0};

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int ni = ny * width + nx;
                if (!visited[ni] && grid_ptr[ni] == target_value) {
                    visited[ni] = true;
                    queue.push(ni);
                }
            }
        }
    }

    return result;
}

PackedInt32Array GridOps::flood_fill_walkable(const PackedInt32Array& grid, int width, int start_index, int walkable_value) {
    return flood_fill(grid, width, start_index, walkable_value);
}

PackedInt32Array GridOps::flood_fill_limited(const PackedInt32Array& grid, int width, int start_index, int target_value, int max_distance) {
    PackedInt32Array result;
    int32_t grid_size = grid.size();

    if (start_index < 0 || start_index >= grid_size) return result;
    if (grid[start_index] != target_value) return result;

    int height = grid_size / width;

    std::vector<int> distances(grid_size, -1);
    std::queue<int> queue;

    queue.push(start_index);
    distances[start_index] = 0;

    const int32_t* grid_ptr = grid.ptr();

    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        result.append(current);

        int current_dist = distances[current];
        if (current_dist >= max_distance) continue;

        int x = current % width;
        int y = current / width;

        static const int dx[] = {0, 1, 0, -1};
        static const int dy[] = {-1, 0, 1, 0};

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int ni = ny * width + nx;
                if (distances[ni] == -1 && grid_ptr[ni] == target_value) {
                    distances[ni] = current_dist + 1;
                    queue.push(ni);
                }
            }
        }
    }

    return result;
}

// ========== LINE OF SIGHT (BRESENHAM) ==========

PackedInt32Array GridOps::line_indices(const Vector2i& from, const Vector2i& to, int width) {
    PackedInt32Array result;

    int x0 = from.x, y0 = from.y;
    int x1 = to.x, y1 = to.y;

    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true) {
        result.append(y0 * width + x0);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }

    return result;
}

bool GridOps::line_clear(const PackedInt32Array& grid, int width, const Vector2i& from, const Vector2i& to, int blocking_value) {
    int x0 = from.x, y0 = from.y;
    int x1 = to.x, y1 = to.y;

    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    const int32_t* grid_ptr = grid.ptr();
    int32_t grid_size = grid.size();

    while (true) {
        int idx = y0 * width + x0;
        if (idx >= 0 && idx < grid_size) {
            if (grid_ptr[idx] == blocking_value) {
                // Allow start and end points to be blocking
                if ((x0 != from.x || y0 != from.y) && (x0 != to.x || y0 != to.y)) {
                    return false;
                }
            }
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }

    return true;
}

PackedInt32Array GridOps::raycast_grid(const PackedInt32Array& grid, int width, const Vector2i& from, const Vector2i& direction, int max_distance, int blocking_value) {
    PackedInt32Array result;

    if (direction.x == 0 && direction.y == 0) return result;

    const int32_t* grid_ptr = grid.ptr();
    int32_t grid_size = grid.size();

    int x = from.x;
    int y = from.y;
    int dx = direction.x > 0 ? 1 : (direction.x < 0 ? -1 : 0);
    int dy = direction.y > 0 ? 1 : (direction.y < 0 ? -1 : 0);

    for (int dist = 0; dist <= max_distance; dist++) {
        int idx = y * width + x;
        if (idx < 0 || idx >= grid_size) break;

        result.append(idx);

        if (grid_ptr[idx] == blocking_value && dist > 0) break;

        x += dx;
        y += dy;
    }

    return result;
}

int GridOps::line_first_blocking(const PackedInt32Array& grid, int width, const Vector2i& from, const Vector2i& to, int blocking_value) {
    int x0 = from.x, y0 = from.y;
    int x1 = to.x, y1 = to.y;

    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    const int32_t* grid_ptr = grid.ptr();
    int32_t grid_size = grid.size();
    bool first = true;

    while (true) {
        int idx = y0 * width + x0;
        if (idx >= 0 && idx < grid_size) {
            if (!first && grid_ptr[idx] == blocking_value) {
                return idx;
            }
        }
        first = false;

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }

    return -1;
}

// ========== FIELD OF VIEW (SHADOWCASTING) ==========

void GridOps::cast_light(const PackedInt32Array& grid, int width, int height,
                        PackedInt32Array& visible, const Vector2i& origin, int radius,
                        int blocking_value, int row, float start_slope, float end_slope,
                        int xx, int xy, int yx, int yy) {
    if (start_slope < end_slope) return;

    const int32_t* grid_ptr = grid.ptr();
    float next_start_slope = start_slope;
    int radius_sq = radius * radius;

    for (int i = row; i <= radius; i++) {
        bool blocked = false;
        for (int dx = -i, dy = -i; dx <= 0; dx++) {
            // Translate from octant coordinates to actual coordinates
            int ax = origin.x + dx * xx + dy * xy;
            int ay = origin.y + dx * yx + dy * yy;

            float l_slope = (dx - 0.5f) / (dy + 0.5f);
            float r_slope = (dx + 0.5f) / (dy - 0.5f);

            if (start_slope < r_slope) continue;
            if (end_slope > l_slope) break;

            // Check if in bounds and within radius
            int dist_sq = dx * dx + dy * dy;
            if (dist_sq <= radius_sq && ax >= 0 && ax < width && ay >= 0 && ay < height) {
                int idx = ay * width + ax;
                visible.append(idx);
            }

            if (blocked) {
                if (ax < 0 || ax >= width || ay < 0 || ay >= height ||
                    grid_ptr[ay * width + ax] == blocking_value) {
                    next_start_slope = r_slope;
                    continue;
                } else {
                    blocked = false;
                    start_slope = next_start_slope;
                }
            } else {
                if (ax >= 0 && ax < width && ay >= 0 && ay < height &&
                    grid_ptr[ay * width + ax] == blocking_value && i < radius) {
                    blocked = true;
                    cast_light(grid, width, height, visible, origin, radius, blocking_value,
                              i + 1, start_slope, l_slope, xx, xy, yx, yy);
                    next_start_slope = r_slope;
                }
            }
        }
        if (blocked) break;
    }
}

PackedInt32Array GridOps::fov_shadowcast(const PackedInt32Array& grid, int width, int height,
                                         const Vector2i& origin, int radius, int blocking_value) {
    PackedInt32Array visible;

    // Add origin
    if (origin.x >= 0 && origin.x < width && origin.y >= 0 && origin.y < height) {
        visible.append(origin.y * width + origin.x);
    }

    // Cast light in all 8 octants
    // Multipliers for octant transformations
    static const int mult[4][8] = {
        {1, 0, 0, -1, -1, 0, 0, 1},
        {0, 1, -1, 0, 0, -1, 1, 0},
        {0, 1, 1, 0, 0, -1, -1, 0},
        {1, 0, 0, 1, -1, 0, 0, -1}
    };

    for (int oct = 0; oct < 8; oct++) {
        cast_light(grid, width, height, visible, origin, radius, blocking_value,
                  1, 1.0f, 0.0f, mult[0][oct], mult[1][oct], mult[2][oct], mult[3][oct]);
    }

    return visible;
}

PackedInt32Array GridOps::fov_raycast(const PackedInt32Array& grid, int width, int height,
                                      const Vector2i& origin, int radius, int blocking_value, int ray_count) {
    std::unordered_set<int> visible_set;
    const int32_t* grid_ptr = grid.ptr();

    // Add origin
    if (origin.x >= 0 && origin.x < width && origin.y >= 0 && origin.y < height) {
        visible_set.insert(origin.y * width + origin.x);
    }

    float angle_step = 6.28318530718f / ray_count;

    for (int r = 0; r < ray_count; r++) {
        float angle = r * angle_step;
        float dx = std::cos(angle);
        float dy = std::sin(angle);

        for (int dist = 1; dist <= radius; dist++) {
            int x = origin.x + static_cast<int>(std::round(dx * dist));
            int y = origin.y + static_cast<int>(std::round(dy * dist));

            if (x < 0 || x >= width || y < 0 || y >= height) break;

            int idx = y * width + x;
            visible_set.insert(idx);

            if (grid_ptr[idx] == blocking_value) break;
        }
    }

    PackedInt32Array result;
    for (int idx : visible_set) {
        result.append(idx);
    }
    return result;
}

// ========== DISTANCE TRANSFORMS ==========

PackedInt32Array GridOps::manhattan_distance_field(const PackedInt32Array& grid, int width, int height, int target_value) {
    int32_t size = width * height;
    PackedInt32Array result;
    result.resize(size);

    const int32_t* grid_ptr = grid.ptr();
    int32_t* dst = result.ptrw();

    const int INF = width + height + 1;

    // Initialize: 0 for target cells, INF for others
    std::queue<int> queue;
    for (int32_t i = 0; i < size; i++) {
        if (grid_ptr[i] == target_value) {
            dst[i] = 0;
            queue.push(i);
        } else {
            dst[i] = INF;
        }
    }

    // BFS
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();

        int x = current % width;
        int y = current / width;
        int current_dist = dst[current];

        static const int dx[] = {0, 1, 0, -1};
        static const int dy[] = {-1, 0, 1, 0};

        for (int d = 0; d < 4; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                int ni = ny * width + nx;
                if (dst[ni] > current_dist + 1) {
                    dst[ni] = current_dist + 1;
                    queue.push(ni);
                }
            }
        }
    }

    return result;
}

PackedInt32Array GridOps::chebyshev_distance_field(const PackedInt32Array& grid, int width, int height, int target_value) {
    int32_t size = width * height;
    PackedInt32Array result;
    result.resize(size);

    const int32_t* grid_ptr = grid.ptr();
    int32_t* dst = result.ptrw();

    const int INF = width + height + 1;

    std::queue<int> queue;
    for (int32_t i = 0; i < size; i++) {
        if (grid_ptr[i] == target_value) {
            dst[i] = 0;
            queue.push(i);
        } else {
            dst[i] = INF;
        }
    }

    // BFS with 8 directions
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();

        int x = current % width;
        int y = current / width;
        int current_dist = dst[current];

        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int ni = ny * width + nx;
                    if (dst[ni] > current_dist + 1) {
                        dst[ni] = current_dist + 1;
                        queue.push(ni);
                    }
                }
            }
        }
    }

    return result;
}

PackedFloat32Array GridOps::euclidean_distance_field(const PackedInt32Array& grid, int width, int height, int target_value) {
    // Uses 8SSEDT approximation (fast approximation of Euclidean distance)
    int32_t size = width * height;
    PackedFloat32Array result;
    result.resize(size);

    const int32_t* grid_ptr = grid.ptr();
    float* dst = result.ptrw();

    const float INF = static_cast<float>(width + height + 1);
    const float SQRT2 = 1.41421356237f;

    // Initialize
    for (int32_t i = 0; i < size; i++) {
        dst[i] = (grid_ptr[i] == target_value) ? 0.0f : INF;
    }

    // Forward pass
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            if (x > 0) dst[i] = std::fmin(dst[i], dst[i - 1] + 1.0f);
            if (y > 0) dst[i] = std::fmin(dst[i], dst[(y - 1) * width + x] + 1.0f);
            if (x > 0 && y > 0) dst[i] = std::fmin(dst[i], dst[(y - 1) * width + (x - 1)] + SQRT2);
            if (x < width - 1 && y > 0) dst[i] = std::fmin(dst[i], dst[(y - 1) * width + (x + 1)] + SQRT2);
        }
    }

    // Backward pass
    for (int y = height - 1; y >= 0; y--) {
        for (int x = width - 1; x >= 0; x--) {
            int i = y * width + x;
            if (x < width - 1) dst[i] = std::fmin(dst[i], dst[i + 1] + 1.0f);
            if (y < height - 1) dst[i] = std::fmin(dst[i], dst[(y + 1) * width + x] + 1.0f);
            if (x < width - 1 && y < height - 1) dst[i] = std::fmin(dst[i], dst[(y + 1) * width + (x + 1)] + SQRT2);
            if (x > 0 && y < height - 1) dst[i] = std::fmin(dst[i], dst[(y + 1) * width + (x - 1)] + SQRT2);
        }
    }

    return result;
}

// ========== CONNECTED COMPONENTS ==========

PackedInt32Array GridOps::label_connected_components(const PackedInt32Array& grid, int width, int height, int target_value) {
    int32_t size = width * height;
    PackedInt32Array labels;
    labels.resize(size);

    const int32_t* grid_ptr = grid.ptr();
    int32_t* label_ptr = labels.ptrw();

    // Initialize all labels to 0 (unlabeled)
    for (int32_t i = 0; i < size; i++) {
        label_ptr[i] = 0;
    }

    int current_label = 0;

    for (int32_t i = 0; i < size; i++) {
        if (grid_ptr[i] == target_value && label_ptr[i] == 0) {
            current_label++;

            // BFS to label component
            std::queue<int> queue;
            queue.push(i);
            label_ptr[i] = current_label;

            while (!queue.empty()) {
                int current = queue.front();
                queue.pop();

                int x = current % width;
                int y = current / width;

                static const int dx[] = {0, 1, 0, -1};
                static const int dy[] = {-1, 0, 1, 0};

                for (int d = 0; d < 4; d++) {
                    int nx = x + dx[d];
                    int ny = y + dy[d];
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        int ni = ny * width + nx;
                        if (grid_ptr[ni] == target_value && label_ptr[ni] == 0) {
                            label_ptr[ni] = current_label;
                            queue.push(ni);
                        }
                    }
                }
            }
        }
    }

    return labels;
}

int GridOps::count_connected_components(const PackedInt32Array& grid, int width, int height, int target_value) {
    int32_t size = width * height;
    std::vector<bool> visited(size, false);

    const int32_t* grid_ptr = grid.ptr();
    int count = 0;

    for (int32_t i = 0; i < size; i++) {
        if (grid_ptr[i] == target_value && !visited[i]) {
            count++;

            std::queue<int> queue;
            queue.push(i);
            visited[i] = true;

            while (!queue.empty()) {
                int current = queue.front();
                queue.pop();

                int x = current % width;
                int y = current / width;

                static const int dx[] = {0, 1, 0, -1};
                static const int dy[] = {-1, 0, 1, 0};

                for (int d = 0; d < 4; d++) {
                    int nx = x + dx[d];
                    int ny = y + dy[d];
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                        int ni = ny * width + nx;
                        if (grid_ptr[ni] == target_value && !visited[ni]) {
                            visited[ni] = true;
                            queue.push(ni);
                        }
                    }
                }
            }
        }
    }

    return count;
}

PackedInt32Array GridOps::component_sizes(const PackedInt32Array& grid, int width, int height, int target_value) {
    PackedInt32Array labels = label_connected_components(grid, width, height, target_value);

    // Find max label
    int32_t max_label = 0;
    const int32_t* label_ptr = labels.ptr();
    int32_t size = labels.size();

    for (int32_t i = 0; i < size; i++) {
        if (label_ptr[i] > max_label) max_label = label_ptr[i];
    }

    if (max_label == 0) return PackedInt32Array();

    // Count sizes
    std::vector<int> sizes(max_label + 1, 0);
    for (int32_t i = 0; i < size; i++) {
        if (label_ptr[i] > 0) {
            sizes[label_ptr[i]]++;
        }
    }

    // Convert to result (skip index 0)
    PackedInt32Array result;
    result.resize(max_label);
    int32_t* dst = result.ptrw();
    for (int i = 1; i <= max_label; i++) {
        dst[i - 1] = sizes[i];
    }

    return result;
}

// ========== GRID UTILITIES ==========

PackedInt32Array GridOps::find_value(const PackedInt32Array& grid, int value) {
    PackedInt32Array result;
    const int32_t* src = grid.ptr();
    int32_t size = grid.size();

    for (int32_t i = 0; i < size; i++) {
        if (src[i] == value) {
            result.append(i);
        }
    }

    return result;
}

PackedInt32Array GridOps::find_not_value(const PackedInt32Array& grid, int value) {
    PackedInt32Array result;
    const int32_t* src = grid.ptr();
    int32_t size = grid.size();

    for (int32_t i = 0; i < size; i++) {
        if (src[i] != value) {
            result.append(i);
        }
    }

    return result;
}

int GridOps::count_value(const PackedInt32Array& grid, int value) {
    const int32_t* src = grid.ptr();
    int32_t size = grid.size();
    int count = 0;

    for (int32_t i = 0; i < size; i++) {
        if (src[i] == value) count++;
    }

    return count;
}

PackedInt32Array GridOps::get_region(const PackedInt32Array& grid, int grid_width, int x, int y, int region_width, int region_height) {
    PackedInt32Array result;
    result.resize(region_width * region_height);

    const int32_t* src = grid.ptr();
    int32_t* dst = result.ptrw();
    int32_t grid_size = grid.size();

    for (int ry = 0; ry < region_height; ry++) {
        for (int rx = 0; rx < region_width; rx++) {
            int gx = x + rx;
            int gy = y + ry;
            int src_idx = gy * grid_width + gx;
            int dst_idx = ry * region_width + rx;

            if (src_idx >= 0 && src_idx < grid_size) {
                dst[dst_idx] = src[src_idx];
            } else {
                dst[dst_idx] = 0;
            }
        }
    }

    return result;
}

PackedInt32Array GridOps::set_region(const PackedInt32Array& grid, int grid_width, int x, int y, int region_width, int region_height, const PackedInt32Array& values) {
    PackedInt32Array result;
    result.resize(grid.size());
    const int32_t* src_grid = grid.ptr();
    int32_t* dst = result.ptrw();
    for (int i = 0; i < grid.size(); i++) {
        dst[i] = src_grid[i];
    }
    const int32_t* src = values.ptr();
    int32_t grid_size = result.size();
    int32_t values_size = values.size();

    for (int ry = 0; ry < region_height; ry++) {
        for (int rx = 0; rx < region_width; rx++) {
            int gx = x + rx;
            int gy = y + ry;
            int dst_idx = gy * grid_width + gx;
            int src_idx = ry * region_width + rx;

            if (dst_idx >= 0 && dst_idx < grid_size && src_idx < values_size) {
                dst[dst_idx] = src[src_idx];
            }
        }
    }

    return result;
}

}

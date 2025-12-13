/**
 * PathfindingOps Implementation
 */

#include "pathfinding_ops.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <queue>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <limits>

namespace godot {

// Priority queue node for A* and Dijkstra
struct PathNode {
    int index;
    float f_cost;  // For A*: g + h, for Dijkstra: g only

    bool operator>(const PathNode& other) const {
        return f_cost > other.f_cost;
    }
};

// Constants
static const float INF = std::numeric_limits<float>::infinity();
static const float SQRT2 = 1.41421356237f;

void PathfindingOps::_bind_methods() {
    // A*
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("astar_grid", "costs", "width", "height", "start", "goal", "allow_diagonal"), &PathfindingOps::astar_grid);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("astar_grid_weighted", "costs", "width", "height", "start", "goal", "allow_diagonal", "heuristic_weight"), &PathfindingOps::astar_grid_weighted);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("astar_uniform", "walkable", "width", "height", "start", "goal", "allow_diagonal"), &PathfindingOps::astar_uniform);

    // Dijkstra
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("dijkstra_grid", "costs", "width", "height", "start", "goals"), &PathfindingOps::dijkstra_grid);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("dijkstra_map", "costs", "width", "height", "goals"), &PathfindingOps::dijkstra_map);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("dijkstra_map_single", "costs", "width", "height", "goal"), &PathfindingOps::dijkstra_map_single);

    // Flow fields
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("flow_field", "costs", "width", "height", "goal"), &PathfindingOps::flow_field);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("flow_field_multi", "costs", "width", "height", "goals"), &PathfindingOps::flow_field_multi);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("flow_field_from_dijkstra", "dijkstra_map", "width", "height"), &PathfindingOps::flow_field_from_dijkstra);

    // JPS
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("jps_grid", "walkable", "width", "height", "start", "goal"), &PathfindingOps::jps_grid);

    // Path utilities
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("smooth_path", "path", "iterations"), &PathfindingOps::smooth_path);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("funnel_smooth", "path", "grid", "width", "blocking_value"), &PathfindingOps::funnel_smooth);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("path_to_vectors", "path", "width", "cell_size"), &PathfindingOps::path_to_vectors);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("simplify_path", "path", "width"), &PathfindingOps::simplify_path);

    // Batch
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("astar_batch", "costs", "width", "height", "starts", "goals", "allow_diagonal"), &PathfindingOps::astar_batch);

    // Reachability
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("reachable_cells", "costs", "width", "height", "start", "max_cost"), &PathfindingOps::reachable_cells);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("is_reachable", "costs", "width", "height", "start", "goal"), &PathfindingOps::is_reachable);
    ClassDB::bind_static_method("PathfindingOps", D_METHOD("path_cost", "costs", "width", "height", "start", "goal", "allow_diagonal"), &PathfindingOps::path_cost);
}

// Helper: heuristic for A*
static inline float heuristic(int x1, int y1, int x2, int y2, bool diagonal) {
    if (diagonal) {
        // Octile distance
        int dx = std::abs(x2 - x1);
        int dy = std::abs(y2 - y1);
        return static_cast<float>(std::max(dx, dy)) + (SQRT2 - 1.0f) * static_cast<float>(std::min(dx, dy));
    } else {
        // Manhattan distance
        return static_cast<float>(std::abs(x2 - x1) + std::abs(y2 - y1));
    }
}

// Helper: reconstruct path from came_from array
static PackedInt32Array reconstruct_path(const std::vector<int>& came_from, int start_idx, int goal_idx) {
    PackedInt32Array path;
    int current = goal_idx;

    while (current != start_idx && current != -1) {
        path.append(current);
        current = came_from[current];
    }

    if (current == start_idx) {
        path.append(start_idx);
    }

    // Reverse path
    int n = path.size();
    int32_t* ptr = path.ptrw();
    for (int i = 0; i < n / 2; i++) {
        int temp = ptr[i];
        ptr[i] = ptr[n - 1 - i];
        ptr[n - 1 - i] = temp;
    }

    return path;
}

// ========== A* PATHFINDING ==========

PackedInt32Array PathfindingOps::astar_grid(const PackedFloat32Array& costs, int width, int height,
                                            const Vector2i& start, const Vector2i& goal,
                                            bool allow_diagonal) {
    return astar_grid_weighted(costs, width, height, start, goal, allow_diagonal, 1.0f);
}

PackedInt32Array PathfindingOps::astar_grid_weighted(const PackedFloat32Array& costs, int width, int height,
                                                     const Vector2i& start, const Vector2i& goal,
                                                     bool allow_diagonal, float heuristic_weight) {
    PackedInt32Array empty_result;
    int size = width * height;

    if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) return empty_result;
    if (goal.x < 0 || goal.x >= width || goal.y < 0 || goal.y >= height) return empty_result;

    int start_idx = start.y * width + start.x;
    int goal_idx = goal.y * width + goal.x;

    const float* cost_ptr = costs.ptr();

    // Check if start or goal is blocked
    if (cost_ptr[start_idx] <= 0 || cost_ptr[goal_idx] <= 0) return empty_result;

    std::vector<float> g_cost(size, INF);
    std::vector<int> came_from(size, -1);
    std::vector<bool> closed(size, false);

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> open;

    g_cost[start_idx] = 0;
    float h = heuristic(start.x, start.y, goal.x, goal.y, allow_diagonal) * heuristic_weight;
    open.push({start_idx, h});

    // Direction arrays
    static const int dx4[] = {0, 1, 0, -1};
    static const int dy4[] = {-1, 0, 1, 0};
    static const int dx8[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static const int dy8[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    static const float cost8[] = {1, SQRT2, 1, SQRT2, 1, SQRT2, 1, SQRT2};

    const int* dx = allow_diagonal ? dx8 : dx4;
    const int* dy = allow_diagonal ? dy8 : dy4;
    int dir_count = allow_diagonal ? 8 : 4;

    while (!open.empty()) {
        PathNode current = open.top();
        open.pop();

        if (current.index == goal_idx) {
            return reconstruct_path(came_from, start_idx, goal_idx);
        }

        if (closed[current.index]) continue;
        closed[current.index] = true;

        int cx = current.index % width;
        int cy = current.index / width;

        for (int d = 0; d < dir_count; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int ni = ny * width + nx;
            if (closed[ni]) continue;

            float cell_cost = cost_ptr[ni];
            if (cell_cost <= 0) continue;  // Blocked

            float move_cost = allow_diagonal ? cost8[d] : 1.0f;
            float new_g = g_cost[current.index] + move_cost * cell_cost;

            if (new_g < g_cost[ni]) {
                g_cost[ni] = new_g;
                came_from[ni] = current.index;
                float h = heuristic(nx, ny, goal.x, goal.y, allow_diagonal) * heuristic_weight;
                open.push({ni, new_g + h});
            }
        }
    }

    return empty_result;  // No path found
}

PackedInt32Array PathfindingOps::astar_uniform(const PackedInt32Array& walkable, int width, int height,
                                               const Vector2i& start, const Vector2i& goal,
                                               bool allow_diagonal) {
    // Convert walkable to costs (1.0 for walkable, 0.0 for blocked)
    PackedFloat32Array costs;
    costs.resize(walkable.size());
    const int32_t* walk_ptr = walkable.ptr();
    float* cost_ptr = costs.ptrw();

    for (int i = 0; i < walkable.size(); i++) {
        cost_ptr[i] = walk_ptr[i] != 0 ? 1.0f : 0.0f;
    }

    return astar_grid(costs, width, height, start, goal, allow_diagonal);
}

// ========== DIJKSTRA ==========

PackedInt32Array PathfindingOps::dijkstra_grid(const PackedFloat32Array& costs, int width, int height,
                                               const Vector2i& start, const PackedVector2Array& goals) {
    PackedInt32Array empty_result;
    int size = width * height;

    if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) return empty_result;

    int start_idx = start.y * width + start.x;
    const float* cost_ptr = costs.ptr();

    if (cost_ptr[start_idx] <= 0) return empty_result;

    // Build goal set
    std::unordered_set<int> goal_set;
    const Vector2* goal_ptr = goals.ptr();
    for (int i = 0; i < goals.size(); i++) {
        int gx = static_cast<int>(goal_ptr[i].x);
        int gy = static_cast<int>(goal_ptr[i].y);
        if (gx >= 0 && gx < width && gy >= 0 && gy < height) {
            goal_set.insert(gy * width + gx);
        }
    }

    if (goal_set.empty()) return empty_result;

    std::vector<float> g_cost(size, INF);
    std::vector<int> came_from(size, -1);

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> open;

    g_cost[start_idx] = 0;
    open.push({start_idx, 0});

    static const int dx[] = {0, 1, 0, -1};
    static const int dy[] = {-1, 0, 1, 0};

    while (!open.empty()) {
        PathNode current = open.top();
        open.pop();

        if (goal_set.count(current.index)) {
            return reconstruct_path(came_from, start_idx, current.index);
        }

        if (current.f_cost > g_cost[current.index]) continue;

        int cx = current.index % width;
        int cy = current.index / width;

        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int ni = ny * width + nx;
            float cell_cost = cost_ptr[ni];
            if (cell_cost <= 0) continue;

            float new_g = g_cost[current.index] + cell_cost;

            if (new_g < g_cost[ni]) {
                g_cost[ni] = new_g;
                came_from[ni] = current.index;
                open.push({ni, new_g});
            }
        }
    }

    return empty_result;
}

PackedFloat32Array PathfindingOps::dijkstra_map(const PackedFloat32Array& costs, int width, int height,
                                                const PackedVector2Array& goals) {
    int size = width * height;
    PackedFloat32Array result;
    result.resize(size);

    float* dist_ptr = result.ptrw();
    const float* cost_ptr = costs.ptr();

    for (int i = 0; i < size; i++) {
        dist_ptr[i] = INF;
    }

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> open;

    // Initialize goals
    const Vector2* goal_ptr = goals.ptr();
    for (int i = 0; i < goals.size(); i++) {
        int gx = static_cast<int>(goal_ptr[i].x);
        int gy = static_cast<int>(goal_ptr[i].y);
        if (gx >= 0 && gx < width && gy >= 0 && gy < height) {
            int gi = gy * width + gx;
            if (cost_ptr[gi] > 0) {
                dist_ptr[gi] = 0;
                open.push({gi, 0});
            }
        }
    }

    static const int dx[] = {0, 1, 0, -1};
    static const int dy[] = {-1, 0, 1, 0};

    while (!open.empty()) {
        PathNode current = open.top();
        open.pop();

        if (current.f_cost > dist_ptr[current.index]) continue;

        int cx = current.index % width;
        int cy = current.index / width;

        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int ni = ny * width + nx;
            float cell_cost = cost_ptr[ni];
            if (cell_cost <= 0) continue;

            float new_dist = dist_ptr[current.index] + cell_cost;

            if (new_dist < dist_ptr[ni]) {
                dist_ptr[ni] = new_dist;
                open.push({ni, new_dist});
            }
        }
    }

    return result;
}

PackedFloat32Array PathfindingOps::dijkstra_map_single(const PackedFloat32Array& costs, int width, int height,
                                                       const Vector2i& goal) {
    PackedVector2Array goals;
    goals.append(Vector2(goal.x, goal.y));
    return dijkstra_map(costs, width, height, goals);
}

// ========== FLOW FIELDS ==========

PackedVector2Array PathfindingOps::flow_field(const PackedFloat32Array& costs, int width, int height,
                                              const Vector2i& goal) {
    PackedFloat32Array dmap = dijkstra_map_single(costs, width, height, goal);
    return flow_field_from_dijkstra(dmap, width, height);
}

PackedVector2Array PathfindingOps::flow_field_multi(const PackedFloat32Array& costs, int width, int height,
                                                    const PackedVector2Array& goals) {
    PackedFloat32Array dmap = dijkstra_map(costs, width, height, goals);
    return flow_field_from_dijkstra(dmap, width, height);
}

PackedVector2Array PathfindingOps::flow_field_from_dijkstra(const PackedFloat32Array& dijkstra_map,
                                                            int width, int height) {
    int size = width * height;
    PackedVector2Array result;
    result.resize(size);

    const float* dist_ptr = dijkstra_map.ptr();
    Vector2* flow_ptr = result.ptrw();

    static const int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static const int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

    for (int i = 0; i < size; i++) {
        if (dist_ptr[i] >= INF) {
            flow_ptr[i] = Vector2(0, 0);
            continue;
        }

        int x = i % width;
        int y = i / width;

        float best_dist = dist_ptr[i];
        int best_dx = 0;
        int best_dy = 0;

        for (int d = 0; d < 8; d++) {
            int nx = x + dx[d];
            int ny = y + dy[d];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int ni = ny * width + nx;
            if (dist_ptr[ni] < best_dist) {
                best_dist = dist_ptr[ni];
                best_dx = dx[d];
                best_dy = dy[d];
            }
        }

        // Normalize
        if (best_dx != 0 || best_dy != 0) {
            float len = std::sqrt(static_cast<float>(best_dx * best_dx + best_dy * best_dy));
            flow_ptr[i] = Vector2(best_dx / len, best_dy / len);
        } else {
            flow_ptr[i] = Vector2(0, 0);
        }
    }

    return result;
}

// ========== JUMP POINT SEARCH ==========

// JPS helper functions
namespace jps_internal {
    static inline bool is_walkable(const int32_t* grid, int width, int height, int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return false;
        return grid[y * width + x] != 0;
    }

    static int jump(const int32_t* grid, int width, int height, int x, int y, int dx, int dy, int goal_x, int goal_y) {
        int nx = x + dx;
        int ny = y + dy;

        if (!is_walkable(grid, width, height, nx, ny)) return -1;
        if (nx == goal_x && ny == goal_y) return ny * width + nx;

        // Diagonal movement
        if (dx != 0 && dy != 0) {
            // Check for forced neighbors
            if ((!is_walkable(grid, width, height, x, ny) && is_walkable(grid, width, height, nx, ny + dy)) ||
                (!is_walkable(grid, width, height, nx, y) && is_walkable(grid, width, height, nx + dx, ny))) {
                return ny * width + nx;
            }

            // Recursive horizontal/vertical jumps
            if (jump(grid, width, height, nx, ny, dx, 0, goal_x, goal_y) != -1 ||
                jump(grid, width, height, nx, ny, 0, dy, goal_x, goal_y) != -1) {
                return ny * width + nx;
            }
        } else {
            // Horizontal movement
            if (dx != 0) {
                if ((!is_walkable(grid, width, height, nx, y - 1) && is_walkable(grid, width, height, nx + dx, y - 1)) ||
                    (!is_walkable(grid, width, height, nx, y + 1) && is_walkable(grid, width, height, nx + dx, y + 1))) {
                    return ny * width + nx;
                }
            }
            // Vertical movement
            else {
                if ((!is_walkable(grid, width, height, x - 1, ny) && is_walkable(grid, width, height, x - 1, ny + dy)) ||
                    (!is_walkable(grid, width, height, x + 1, ny) && is_walkable(grid, width, height, x + 1, ny + dy))) {
                    return ny * width + nx;
                }
            }
        }

        return jump(grid, width, height, nx, ny, dx, dy, goal_x, goal_y);
    }
}

PackedInt32Array PathfindingOps::jps_grid(const PackedInt32Array& walkable, int width, int height,
                                          const Vector2i& start, const Vector2i& goal) {
    PackedInt32Array empty_result;
    int size = width * height;

    if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) return empty_result;
    if (goal.x < 0 || goal.x >= width || goal.y < 0 || goal.y >= height) return empty_result;

    int start_idx = start.y * width + start.x;
    int goal_idx = goal.y * width + goal.x;

    const int32_t* walk_ptr = walkable.ptr();

    if (walk_ptr[start_idx] == 0 || walk_ptr[goal_idx] == 0) return empty_result;

    std::vector<float> g_cost(size, INF);
    std::vector<int> came_from(size, -1);
    std::vector<bool> closed(size, false);

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> open;

    g_cost[start_idx] = 0;
    open.push({start_idx, heuristic(start.x, start.y, goal.x, goal.y, true)});

    // All 8 directions for initial expansion
    static const int dx8[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static const int dy8[] = {-1, -1, 0, 1, 1, 1, 0, -1};

    while (!open.empty()) {
        PathNode current = open.top();
        open.pop();

        if (current.index == goal_idx) {
            return reconstruct_path(came_from, start_idx, goal_idx);
        }

        if (closed[current.index]) continue;
        closed[current.index] = true;

        int cx = current.index % width;
        int cy = current.index / width;

        // Get parent direction
        int parent = came_from[current.index];
        int pdx = 0, pdy = 0;
        if (parent != -1) {
            int px = parent % width;
            int py = parent / width;
            pdx = (cx - px);
            pdy = (cy - py);
            if (pdx != 0) pdx = pdx / std::abs(pdx);
            if (pdy != 0) pdy = pdy / std::abs(pdy);
        }

        // Determine directions to search
        std::vector<std::pair<int, int>> directions;
        if (parent == -1) {
            // Start node: all 8 directions
            for (int d = 0; d < 8; d++) {
                directions.push_back({dx8[d], dy8[d]});
            }
        } else if (pdx != 0 && pdy != 0) {
            // Diagonal movement: natural neighbors + forced
            directions.push_back({pdx, pdy});  // Continue diagonal
            directions.push_back({pdx, 0});    // Horizontal
            directions.push_back({0, pdy});    // Vertical
            // Forced neighbors
            if (!jps_internal::is_walkable(walk_ptr, width, height, cx - pdx, cy)) {
                directions.push_back({-pdx, pdy});
            }
            if (!jps_internal::is_walkable(walk_ptr, width, height, cx, cy - pdy)) {
                directions.push_back({pdx, -pdy});
            }
        } else if (pdx != 0) {
            // Horizontal movement
            directions.push_back({pdx, 0});
            if (!jps_internal::is_walkable(walk_ptr, width, height, cx, cy - 1)) {
                directions.push_back({pdx, -1});
            }
            if (!jps_internal::is_walkable(walk_ptr, width, height, cx, cy + 1)) {
                directions.push_back({pdx, 1});
            }
        } else if (pdy != 0) {
            // Vertical movement
            directions.push_back({0, pdy});
            if (!jps_internal::is_walkable(walk_ptr, width, height, cx - 1, cy)) {
                directions.push_back({-1, pdy});
            }
            if (!jps_internal::is_walkable(walk_ptr, width, height, cx + 1, cy)) {
                directions.push_back({1, pdy});
            }
        }

        for (auto& dir : directions) {
            int jump_idx = jps_internal::jump(walk_ptr, width, height, cx, cy, dir.first, dir.second, goal.x, goal.y);
            if (jump_idx == -1) continue;

            int jx = jump_idx % width;
            int jy = jump_idx / width;

            if (closed[jump_idx]) continue;

            float dist = std::sqrt(static_cast<float>((jx - cx) * (jx - cx) + (jy - cy) * (jy - cy)));
            float new_g = g_cost[current.index] + dist;

            if (new_g < g_cost[jump_idx]) {
                g_cost[jump_idx] = new_g;
                came_from[jump_idx] = current.index;
                float h = heuristic(jx, jy, goal.x, goal.y, true);
                open.push({jump_idx, new_g + h});
            }
        }
    }

    return empty_result;
}

// ========== PATH UTILITIES ==========

PackedVector2Array PathfindingOps::smooth_path(const PackedVector2Array& path, int iterations) {
    if (path.size() < 3) return path;

    PackedVector2Array result = path;

    for (int iter = 0; iter < iterations; iter++) {
        PackedVector2Array smoothed;
        smoothed.resize(result.size());

        const Vector2* src = result.ptr();
        Vector2* dst = smoothed.ptrw();
        int n = result.size();

        dst[0] = src[0];
        dst[n - 1] = src[n - 1];

        for (int i = 1; i < n - 1; i++) {
            dst[i] = Vector2(
                (src[i - 1].x + src[i].x + src[i + 1].x) / 3.0f,
                (src[i - 1].y + src[i].y + src[i + 1].y) / 3.0f
            );
        }

        result = smoothed;
    }

    return result;
}

PackedVector2Array PathfindingOps::funnel_smooth(const PackedVector2Array& path, const PackedInt32Array& grid,
                                                 int width, int blocking_value) {
    if (path.size() < 3) return path;

    PackedVector2Array result;
    const Vector2* src = path.ptr();
    const int32_t* grid_ptr = grid.ptr();
    int n = path.size();
    int grid_size = grid.size();

    result.append(src[0]);
    int current = 0;

    while (current < n - 1) {
        int furthest = current + 1;

        // Find furthest visible point
        for (int i = current + 2; i < n; i++) {
            // Check line of sight
            int x0 = static_cast<int>(src[current].x);
            int y0 = static_cast<int>(src[current].y);
            int x1 = static_cast<int>(src[i].x);
            int y1 = static_cast<int>(src[i].y);

            int dx = std::abs(x1 - x0);
            int dy = -std::abs(y1 - y0);
            int sx = x0 < x1 ? 1 : -1;
            int sy = y0 < y1 ? 1 : -1;
            int err = dx + dy;

            bool clear = true;
            int px = x0, py = y0;

            while (true) {
                int idx = py * width + px;
                if (idx >= 0 && idx < grid_size) {
                    if (grid_ptr[idx] == blocking_value) {
                        clear = false;
                        break;
                    }
                }

                if (px == x1 && py == y1) break;

                int e2 = 2 * err;
                if (e2 >= dy) { err += dy; px += sx; }
                if (e2 <= dx) { err += dx; py += sy; }
            }

            if (clear) furthest = i;
        }

        result.append(src[furthest]);
        current = furthest;
    }

    return result;
}

PackedVector2Array PathfindingOps::path_to_vectors(const PackedInt32Array& path, int width, float cell_size) {
    int n = path.size();
    PackedVector2Array result;
    result.resize(n);

    const int32_t* src = path.ptr();
    Vector2* dst = result.ptrw();
    float half_cell = cell_size * 0.5f;

    for (int i = 0; i < n; i++) {
        int x = src[i] % width;
        int y = src[i] / width;
        dst[i] = Vector2(x * cell_size + half_cell, y * cell_size + half_cell);
    }

    return result;
}

PackedInt32Array PathfindingOps::simplify_path(const PackedInt32Array& path, int width) {
    if (path.size() < 3) return path;

    PackedInt32Array result;
    const int32_t* src = path.ptr();
    int n = path.size();

    result.append(src[0]);

    for (int i = 1; i < n - 1; i++) {
        int prev_x = src[i - 1] % width;
        int prev_y = src[i - 1] / width;
        int curr_x = src[i] % width;
        int curr_y = src[i] / width;
        int next_x = src[i + 1] % width;
        int next_y = src[i + 1] / width;

        int dx1 = curr_x - prev_x;
        int dy1 = curr_y - prev_y;
        int dx2 = next_x - curr_x;
        int dy2 = next_y - curr_y;

        // If direction changes, keep this point
        if (dx1 != dx2 || dy1 != dy2) {
            result.append(src[i]);
        }
    }

    result.append(src[n - 1]);
    return result;
}

// ========== BATCH PATHFINDING ==========

Array PathfindingOps::astar_batch(const PackedFloat32Array& costs, int width, int height,
                                  const PackedVector2Array& starts, const PackedVector2Array& goals,
                                  bool allow_diagonal) {
    Array result;
    int count = std::min(starts.size(), goals.size());

    const Vector2* start_ptr = starts.ptr();
    const Vector2* goal_ptr = goals.ptr();

    for (int i = 0; i < count; i++) {
        Vector2i start(static_cast<int>(start_ptr[i].x), static_cast<int>(start_ptr[i].y));
        Vector2i goal(static_cast<int>(goal_ptr[i].x), static_cast<int>(goal_ptr[i].y));
        result.append(astar_grid(costs, width, height, start, goal, allow_diagonal));
    }

    return result;
}

// ========== REACHABILITY ==========

PackedInt32Array PathfindingOps::reachable_cells(const PackedFloat32Array& costs, int width, int height,
                                                 const Vector2i& start, float max_cost) {
    PackedInt32Array result;
    int size = width * height;

    if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) return result;

    int start_idx = start.y * width + start.x;
    const float* cost_ptr = costs.ptr();

    if (cost_ptr[start_idx] <= 0) return result;

    std::vector<float> g_cost(size, INF);
    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> open;

    g_cost[start_idx] = 0;
    open.push({start_idx, 0});

    static const int dx[] = {0, 1, 0, -1};
    static const int dy[] = {-1, 0, 1, 0};

    while (!open.empty()) {
        PathNode current = open.top();
        open.pop();

        if (current.f_cost > g_cost[current.index]) continue;

        result.append(current.index);

        int cx = current.index % width;
        int cy = current.index / width;

        for (int d = 0; d < 4; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int ni = ny * width + nx;
            float cell_cost = cost_ptr[ni];
            if (cell_cost <= 0) continue;

            float new_g = g_cost[current.index] + cell_cost;
            if (new_g > max_cost) continue;

            if (new_g < g_cost[ni]) {
                g_cost[ni] = new_g;
                open.push({ni, new_g});
            }
        }
    }

    return result;
}

bool PathfindingOps::is_reachable(const PackedFloat32Array& costs, int width, int height,
                                  const Vector2i& start, const Vector2i& goal) {
    PackedInt32Array path = astar_grid(costs, width, height, start, goal, true);
    return path.size() > 0;
}

float PathfindingOps::path_cost(const PackedFloat32Array& costs, int width, int height,
                                const Vector2i& start, const Vector2i& goal, bool allow_diagonal) {
    int size = width * height;

    if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) return INF;
    if (goal.x < 0 || goal.x >= width || goal.y < 0 || goal.y >= height) return INF;

    int start_idx = start.y * width + start.x;
    int goal_idx = goal.y * width + goal.x;

    const float* cost_ptr = costs.ptr();

    if (cost_ptr[start_idx] <= 0 || cost_ptr[goal_idx] <= 0) return INF;

    std::vector<float> g_cost(size, INF);
    std::vector<bool> closed(size, false);

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> open;

    g_cost[start_idx] = 0;
    open.push({start_idx, heuristic(start.x, start.y, goal.x, goal.y, allow_diagonal)});

    static const int dx4[] = {0, 1, 0, -1};
    static const int dy4[] = {-1, 0, 1, 0};
    static const int dx8[] = {0, 1, 1, 1, 0, -1, -1, -1};
    static const int dy8[] = {-1, -1, 0, 1, 1, 1, 0, -1};
    static const float cost8[] = {1, SQRT2, 1, SQRT2, 1, SQRT2, 1, SQRT2};

    const int* dx = allow_diagonal ? dx8 : dx4;
    const int* dy = allow_diagonal ? dy8 : dy4;
    int dir_count = allow_diagonal ? 8 : 4;

    while (!open.empty()) {
        PathNode current = open.top();
        open.pop();

        if (current.index == goal_idx) {
            return g_cost[goal_idx];
        }

        if (closed[current.index]) continue;
        closed[current.index] = true;

        int cx = current.index % width;
        int cy = current.index / width;

        for (int d = 0; d < dir_count; d++) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];

            if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

            int ni = ny * width + nx;
            if (closed[ni]) continue;

            float cell_cost = cost_ptr[ni];
            if (cell_cost <= 0) continue;

            float move_cost = allow_diagonal ? cost8[d] : 1.0f;
            float new_g = g_cost[current.index] + move_cost * cell_cost;

            if (new_g < g_cost[ni]) {
                g_cost[ni] = new_g;
                float h = heuristic(nx, ny, goal.x, goal.y, allow_diagonal);
                open.push({ni, new_g + h});
            }
        }
    }

    return INF;
}

}

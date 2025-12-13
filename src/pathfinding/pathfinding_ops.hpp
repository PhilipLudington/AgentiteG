/**
 * PathfindingOps - High-performance pathfinding algorithms
 *
 * Provides optimized pathfinding for grid-based games.
 * All grids are assumed to be in row-major order.
 * Cost arrays use 0 or negative values for impassable cells.
 *
 * Usage:
 *   # A* pathfinding
 *   var path = PathfindingOps.astar_grid(costs, width, height, start, goal, true)
 *
 *   # Dijkstra map for AI
 *   var distances = PathfindingOps.dijkstra_map(costs, width, height, goals)
 *
 *   # Flow field for many units
 *   var flow = PathfindingOps.flow_field(costs, width, height, goal)
 */

#ifndef AGENTITE_PATHFINDING_OPS_HPP
#define AGENTITE_PATHFINDING_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/array.hpp>

namespace godot {

class PathfindingOps : public RefCounted {
    GDCLASS(PathfindingOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // ========== A* PATHFINDING ==========
    // A* on grid with costs. Returns path as indices from start to goal.
    // Costs array: positive = traversable (movement cost), 0 or negative = blocked
    static PackedInt32Array astar_grid(const PackedFloat32Array& costs, int width, int height,
                                       const Vector2i& start, const Vector2i& goal,
                                       bool allow_diagonal);

    // A* with weighted heuristic (weight > 1 = faster but less optimal)
    static PackedInt32Array astar_grid_weighted(const PackedFloat32Array& costs, int width, int height,
                                                const Vector2i& start, const Vector2i& goal,
                                                bool allow_diagonal, float heuristic_weight);

    // A* with uniform cost (all walkable cells cost 1)
    // walkable array: non-zero = walkable, 0 = blocked
    static PackedInt32Array astar_uniform(const PackedInt32Array& walkable, int width, int height,
                                          const Vector2i& start, const Vector2i& goal,
                                          bool allow_diagonal);

    // ========== DIJKSTRA ==========
    // Dijkstra from start to nearest goal. Returns path to nearest goal.
    static PackedInt32Array dijkstra_grid(const PackedFloat32Array& costs, int width, int height,
                                          const Vector2i& start, const PackedVector2Array& goals);

    // Dijkstra map: distance from every cell to nearest goal
    // Returns distance array (same size as costs). INF for unreachable cells.
    static PackedFloat32Array dijkstra_map(const PackedFloat32Array& costs, int width, int height,
                                           const PackedVector2Array& goals);

    // Dijkstra map from single goal
    static PackedFloat32Array dijkstra_map_single(const PackedFloat32Array& costs, int width, int height,
                                                  const Vector2i& goal);

    // ========== FLOW FIELDS ==========
    // Flow field: normalized direction at each cell toward goal
    static PackedVector2Array flow_field(const PackedFloat32Array& costs, int width, int height,
                                         const Vector2i& goal);

    // Flow field toward multiple goals
    static PackedVector2Array flow_field_multi(const PackedFloat32Array& costs, int width, int height,
                                               const PackedVector2Array& goals);

    // Flow field from pre-computed Dijkstra map
    static PackedVector2Array flow_field_from_dijkstra(const PackedFloat32Array& dijkstra_map,
                                                       int width, int height);

    // ========== JUMP POINT SEARCH ==========
    // JPS: faster A* for uniform-cost grids
    // walkable array: non-zero = walkable, 0 = blocked
    static PackedInt32Array jps_grid(const PackedInt32Array& walkable, int width, int height,
                                     const Vector2i& start, const Vector2i& goal);

    // ========== PATH UTILITIES ==========
    // Smooth path using string-pulling
    static PackedVector2Array smooth_path(const PackedVector2Array& path, int iterations);

    // Smooth path with line-of-sight checks (funnel algorithm)
    static PackedVector2Array funnel_smooth(const PackedVector2Array& path, const PackedInt32Array& grid,
                                            int width, int blocking_value);

    // Convert index path to Vector2 path (cell centers)
    static PackedVector2Array path_to_vectors(const PackedInt32Array& path, int width, float cell_size);

    // Simplify path by removing collinear points
    static PackedInt32Array simplify_path(const PackedInt32Array& path, int width);

    // ========== BATCH PATHFINDING ==========
    // Find paths for multiple start/goal pairs
    // Returns Array of PackedInt32Array paths
    static Array astar_batch(const PackedFloat32Array& costs, int width, int height,
                             const PackedVector2Array& starts, const PackedVector2Array& goals,
                             bool allow_diagonal);

    // ========== REACHABILITY ==========
    // Get all reachable cells from start within max_cost
    static PackedInt32Array reachable_cells(const PackedFloat32Array& costs, int width, int height,
                                            const Vector2i& start, float max_cost);

    // Check if goal is reachable from start
    static bool is_reachable(const PackedFloat32Array& costs, int width, int height,
                             const Vector2i& start, const Vector2i& goal);

    // Get cost to travel from start to goal (returns INF if unreachable)
    static float path_cost(const PackedFloat32Array& costs, int width, int height,
                           const Vector2i& start, const Vector2i& goal, bool allow_diagonal);
};

}

#endif // AGENTITE_PATHFINDING_OPS_HPP

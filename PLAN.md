# AgentiteG Implementation Plan

A phased roadmap for expanding AgentiteG's primitive library. Each phase builds on previous work and follows the core principles:

- **Zero game-specific logic** - Pure algorithms only
- **Arrays in, arrays out** - Work with PackedArrays, return indices
- **5-10x minimum speedup** - Every feature must be benchmarked
- **Claude-friendly API** - Simple signatures, sensible defaults

---

## Current State (Complete)

| Primitive | Status | Description |
|-----------|--------|-------------|
| SpatialHash2D | Done | O(1) spatial queries in 2D |
| SpatialHash3D | Done | O(1) spatial queries in 3D |
| ArrayOps | Done | Filter, sort, reduce, select operations |
| MathOps | Done | Batch vector and matrix operations |
| BatchOps | Done | Steering, flocking, game loop operations |

---

## Phase 1: Core Extensions (Complete)

**Goal:** Extend existing patterns to 3D and complete planned modules.

### 1.1 SpatialHash3D (Done)
Extend SpatialHash2D patterns to 3D space.

```cpp
class SpatialHash3D : public RefCounted {
    // Core
    void build(PackedVector3Array positions);
    void insert(Vector3 position);
    void update(int index, Vector3 new_position);
    void clear();

    // Queries
    PackedInt32Array query_radius(Vector3 origin, float radius);
    PackedInt32Array query_box(AABB box);
    PackedInt32Array query_nearest(Vector3 origin, int k);
    int query_nearest_one(Vector3 origin);
    bool has_any_in_radius(Vector3 origin, float radius);
    int count_in_radius(Vector3 origin, float radius);

    // Batch
    Array query_radius_batch(PackedVector3Array origins, PackedFloat32Array radii);
    Array query_radius_batch_uniform(PackedVector3Array origins, float radius);
};
```

**Files:** `src/spatial/spatial_hash_3d.hpp`, `src/spatial/spatial_hash_3d.cpp`

### 1.2 MathOps (Done)
SIMD-accelerated math operations.

```cpp
class MathOps {
    // Vector operations (batch)
    static PackedVector2Array normalize_batch_2d(PackedVector2Array vectors);
    static PackedVector3Array normalize_batch_3d(PackedVector3Array vectors);
    static PackedFloat32Array dot_batch_2d(PackedVector2Array a, PackedVector2Array b);
    static PackedFloat32Array dot_batch_3d(PackedVector3Array a, PackedVector3Array b);
    static PackedVector3Array cross_batch(PackedVector3Array a, PackedVector3Array b);
    static PackedFloat32Array length_batch_2d(PackedVector2Array vectors);
    static PackedFloat32Array length_batch_3d(PackedVector3Array vectors);
    static PackedFloat32Array length_squared_batch_2d(PackedVector2Array vectors);
    static PackedFloat32Array length_squared_batch_3d(PackedVector3Array vectors);

    // Distance matrices
    static PackedFloat32Array distance_matrix_2d(PackedVector2Array a, PackedVector2Array b);
    static PackedFloat32Array distance_matrix_3d(PackedVector3Array a, PackedVector3Array b);

    // Lerp/Slerp batch
    static PackedVector2Array lerp_batch_2d(PackedVector2Array a, PackedVector2Array b, float t);
    static PackedVector3Array lerp_batch_3d(PackedVector3Array a, PackedVector3Array b, float t);
    static PackedVector2Array lerp_batch_2d_weights(PackedVector2Array a, PackedVector2Array b, PackedFloat32Array t);

    // Matrix operations
    static PackedVector2Array transform_2d_batch(Transform2D xform, PackedVector2Array points);
    static PackedVector3Array transform_3d_batch(Transform3D xform, PackedVector3Array points);
};
```

**Files:** `src/math/math_ops.hpp`, `src/math/math_ops.cpp`

### 1.3 BatchOps (Done)
Batch operations for common game patterns.

```cpp
class BatchOps {
    // Transform batching
    static PackedVector2Array apply_velocities_2d(PackedVector2Array positions,
                                                   PackedVector2Array velocities,
                                                   float delta);
    static PackedVector3Array apply_velocities_3d(PackedVector3Array positions,
                                                   PackedVector3Array velocities,
                                                   float delta);

    // Steering behaviors (returns accelerations)
    static PackedVector2Array seek_batch(PackedVector2Array positions,
                                          PackedVector2Array targets,
                                          float max_speed);
    static PackedVector2Array flee_batch(PackedVector2Array positions,
                                          PackedVector2Array threats,
                                          float max_speed);
    static PackedVector2Array arrive_batch(PackedVector2Array positions,
                                            PackedVector2Array targets,
                                            float max_speed,
                                            float slowing_radius);

    // Separation (requires spatial hash)
    static PackedVector2Array separation_2d(PackedVector2Array positions,
                                             float radius,
                                             float strength);
};
```

**Files:** `src/batch/batch_ops.hpp`, `src/batch/batch_ops.cpp`

---

## Phase 2: Randomness & Noise (Complete)

**Goal:** Fast procedural generation primitives.

### 2.1 RandomOps (Done)
High-performance random number generation.

```cpp
class RandomOps : public RefCounted {
    // Seeding
    void seed(int64_t seed);
    void seed_from_time();

    // Bulk generation
    PackedFloat32Array randf_array(int count);  // 0.0 to 1.0
    PackedFloat32Array randf_range_array(int count, float min, float max);
    PackedInt32Array randi_array(int count);
    PackedInt32Array randi_range_array(int count, int min, int max);
    PackedVector2Array rand_points_in_rect(int count, Rect2 rect);
    PackedVector3Array rand_points_in_box(int count, AABB box);
    PackedVector2Array rand_points_in_circle(int count, Vector2 center, float radius);
    PackedVector3Array rand_points_in_sphere(int count, Vector3 center, float radius);

    // Shuffling
    PackedInt32Array shuffle_indices(int count);  // Returns shuffled 0..count-1
    PackedFloat32Array shuffle_floats(PackedFloat32Array values);
    PackedInt32Array shuffle_ints(PackedInt32Array values);

    // Weighted selection
    PackedInt32Array weighted_choice(PackedFloat32Array weights, int count);
    int weighted_choice_one(PackedFloat32Array weights);

    // Poisson disk sampling
    PackedVector2Array poisson_disk_2d(Rect2 bounds, float min_distance, int max_attempts);
    PackedVector3Array poisson_disk_3d(AABB bounds, float min_distance, int max_attempts);
};
```

**Files:** `src/random/random_ops.hpp`, `src/random/random_ops.cpp`

### 2.2 NoiseOps (Done)
Procedural noise generation.

```cpp
class NoiseOps : public RefCounted {
    // Configuration
    void set_seed(int seed);
    void set_octaves(int octaves);        // Default: 4
    void set_persistence(float p);         // Default: 0.5
    void set_lacunarity(float l);          // Default: 2.0
    void set_frequency(float f);           // Default: 1.0

    // Single values
    float perlin_2d(Vector2 pos);
    float perlin_3d(Vector3 pos);
    float simplex_2d(Vector2 pos);
    float simplex_3d(Vector3 pos);
    float worley_2d(Vector2 pos);          // Cellular/Voronoi
    float worley_3d(Vector3 pos);

    // Batch evaluation
    PackedFloat32Array perlin_2d_batch(PackedVector2Array positions);
    PackedFloat32Array perlin_3d_batch(PackedVector3Array positions);
    PackedFloat32Array simplex_2d_batch(PackedVector2Array positions);
    PackedFloat32Array simplex_3d_batch(PackedVector3Array positions);

    // Grid evaluation (returns row-major array)
    PackedFloat32Array perlin_2d_grid(Vector2 origin, Vector2 cell_size, int width, int height);
    PackedFloat32Array simplex_2d_grid(Vector2 origin, Vector2 cell_size, int width, int height);

    // Fractal variants (auto-apply octaves)
    PackedFloat32Array fbm_2d_batch(PackedVector2Array positions);       // Fractal Brownian Motion
    PackedFloat32Array ridged_2d_batch(PackedVector2Array positions);    // Ridged multifractal
    PackedFloat32Array turbulence_2d_batch(PackedVector2Array positions);
};
```

**Files:** `src/noise/noise_ops.hpp`, `src/noise/noise_ops.cpp`

---

## Phase 3: Grid & Pathfinding (Complete)

**Goal:** Fast operations for tile-based and navigation systems.

### 3.1 GridOps
Operations on 2D grids (stored as flat arrays).

```cpp
class GridOps {
    // Coordinate conversion
    static int to_index(int x, int y, int width);
    static Vector2i to_coords(int index, int width);
    static PackedInt32Array to_indices(PackedVector2Array coords, int width);
    static PackedVector2Array to_coords_batch(PackedInt32Array indices, int width);

    // Neighbor queries
    static PackedInt32Array neighbors_4(int index, int width, int height);  // Cardinal
    static PackedInt32Array neighbors_8(int index, int width, int height);  // Including diagonals
    static PackedInt32Array neighbors_4_batch(PackedInt32Array indices, int width, int height);

    // Flood fill
    static PackedInt32Array flood_fill(PackedInt32Array grid, int width, int start_index,
                                        int target_value);
    static PackedInt32Array flood_fill_walkable(PackedInt32Array grid, int width,
                                                 int start_index, int walkable_value);

    // Line of sight (Bresenham)
    static PackedInt32Array line_indices(Vector2i from, Vector2i to, int width);
    static bool line_clear(PackedInt32Array grid, int width, Vector2i from, Vector2i to,
                           int blocking_value);
    static PackedInt32Array raycast_grid(PackedInt32Array grid, int width, Vector2i from,
                                          Vector2i direction, int max_distance, int blocking_value);

    // Field of view
    static PackedInt32Array fov_shadowcast(PackedInt32Array grid, int width, int height,
                                            Vector2i origin, int radius, int blocking_value);

    // Distance transforms
    static PackedInt32Array manhattan_distance_field(PackedInt32Array grid, int width,
                                                      int height, int target_value);
    static PackedFloat32Array euclidean_distance_field(PackedInt32Array grid, int width,
                                                        int height, int target_value);

    // Connected components
    static PackedInt32Array label_connected_components(PackedInt32Array grid, int width,
                                                        int height, int target_value);
    static int count_connected_components(PackedInt32Array grid, int width, int height,
                                           int target_value);
};
```

**Files:** `src/grid/grid_ops.hpp`, `src/grid/grid_ops.cpp`

### 3.2 PathfindingOps
Pathfinding algorithms.

```cpp
class PathfindingOps {
    // A* on grid
    static PackedInt32Array astar_grid(PackedFloat32Array costs, int width, int height,
                                        Vector2i start, Vector2i goal, bool allow_diagonal);

    // A* with custom heuristic weight
    static PackedInt32Array astar_grid_weighted(PackedFloat32Array costs, int width, int height,
                                                 Vector2i start, Vector2i goal,
                                                 bool allow_diagonal, float heuristic_weight);

    // Dijkstra (multiple goals, returns path to nearest)
    static PackedInt32Array dijkstra_grid(PackedFloat32Array costs, int width, int height,
                                           Vector2i start, PackedVector2Array goals);

    // Dijkstra map (distance from all cells to goals)
    static PackedFloat32Array dijkstra_map(PackedFloat32Array costs, int width, int height,
                                            PackedVector2Array goals);

    // Flow field (direction at each cell toward goal)
    static PackedVector2Array flow_field(PackedFloat32Array costs, int width, int height,
                                          Vector2i goal);
    static PackedVector2Array flow_field_multi(PackedFloat32Array costs, int width, int height,
                                                PackedVector2Array goals);

    // Jump Point Search (faster A* for uniform grids)
    static PackedInt32Array jps_grid(PackedInt32Array walkable, int width, int height,
                                      Vector2i start, Vector2i goal);

    // Path smoothing
    static PackedVector2Array smooth_path(PackedVector2Array path, int iterations);
    static PackedVector2Array funnel_smooth(PackedVector2Array path, PackedInt32Array grid,
                                             int width, int blocking_value);

    // Batch pathfinding
    static Array astar_batch(PackedFloat32Array costs, int width, int height,
                              PackedVector2Array starts, PackedVector2Array goals,
                              bool allow_diagonal);
};
```

**Files:** `src/pathfinding/pathfinding_ops.hpp`, `src/pathfinding/pathfinding_ops.cpp`

---

## Phase 4: Geometry & Collision (Complete)

**Goal:** Computational geometry and batch collision detection.

### 4.1 CollisionOps
Batch collision detection primitives.

```cpp
class CollisionOps {
    // Point tests (batch)
    static PackedInt32Array points_in_rect(PackedVector2Array points, Rect2 rect);
    static PackedInt32Array points_in_circle(PackedVector2Array points, Vector2 center, float radius);
    static PackedInt32Array points_in_polygon(PackedVector2Array points, PackedVector2Array polygon);
    static PackedInt32Array points_in_aabb(PackedVector3Array points, AABB box);
    static PackedInt32Array points_in_sphere(PackedVector3Array points, Vector3 center, float radius);

    // Shape vs shape (returns collision pairs as flat array [i0,j0, i1,j1, ...])
    static PackedInt32Array circles_vs_circles(PackedVector2Array centers_a, PackedFloat32Array radii_a,
                                                PackedVector2Array centers_b, PackedFloat32Array radii_b);
    static PackedInt32Array circles_vs_circles_uniform(PackedVector2Array centers_a, float radius_a,
                                                        PackedVector2Array centers_b, float radius_b);
    static PackedInt32Array aabb_vs_aabb(PackedVector2Array mins_a, PackedVector2Array maxs_a,
                                          PackedVector2Array mins_b, PackedVector2Array maxs_b);
    static PackedInt32Array spheres_vs_spheres(PackedVector3Array centers_a, PackedFloat32Array radii_a,
                                                PackedVector3Array centers_b, PackedFloat32Array radii_b);

    // Self-collision (within same array)
    static PackedInt32Array circles_self_collision(PackedVector2Array centers, PackedFloat32Array radii);
    static PackedInt32Array circles_self_collision_uniform(PackedVector2Array centers, float radius);

    // Ray intersection
    static PackedFloat32Array ray_vs_circles(Vector2 origin, Vector2 direction,
                                              PackedVector2Array centers, PackedFloat32Array radii);
    static PackedFloat32Array ray_vs_aabbs(Vector2 origin, Vector2 direction,
                                            PackedVector2Array mins, PackedVector2Array maxs);
    static int ray_first_circle(Vector2 origin, Vector2 direction, float max_distance,
                                 PackedVector2Array centers, PackedFloat32Array radii);

    // Segment intersection
    static PackedInt32Array segments_intersect(PackedVector2Array starts_a, PackedVector2Array ends_a,
                                                PackedVector2Array starts_b, PackedVector2Array ends_b);
    static PackedVector2Array segment_intersection_points(PackedVector2Array starts_a, PackedVector2Array ends_a,
                                                           PackedVector2Array starts_b, PackedVector2Array ends_b);
};
```

**Files:** `src/collision/collision_ops.hpp`, `src/collision/collision_ops.cpp`

### 4.2 GeometryOps
Computational geometry algorithms.

```cpp
class GeometryOps {
    // Convex hull
    static PackedVector2Array convex_hull(PackedVector2Array points);
    static PackedInt32Array convex_hull_indices(PackedVector2Array points);

    // Triangulation
    static PackedInt32Array triangulate(PackedVector2Array polygon);  // Ear clipping
    static PackedInt32Array delaunay(PackedVector2Array points);      // Returns triangle indices

    // Voronoi
    static Array voronoi_cells(PackedVector2Array points, Rect2 bounds);  // Array of PackedVector2Array

    // Polygon operations
    static float polygon_area(PackedVector2Array polygon);
    static Vector2 polygon_centroid(PackedVector2Array polygon);
    static bool polygon_is_convex(PackedVector2Array polygon);
    static bool polygon_is_clockwise(PackedVector2Array polygon);
    static PackedVector2Array polygon_reverse(PackedVector2Array polygon);

    // Simplification
    static PackedVector2Array simplify_rdp(PackedVector2Array polyline, float epsilon);  // Ramer-Douglas-Peucker
    static PackedVector2Array simplify_visvalingam(PackedVector2Array polyline, int target_count);

    // Offset/Buffer
    static PackedVector2Array offset_polygon(PackedVector2Array polygon, float distance);

    // Minkowski operations
    static PackedVector2Array minkowski_sum(PackedVector2Array a, PackedVector2Array b);

    // Closest point queries
    static PackedVector2Array closest_points_on_segments(PackedVector2Array points,
                                                          PackedVector2Array seg_starts,
                                                          PackedVector2Array seg_ends);
    static PackedFloat32Array distances_to_polygon(PackedVector2Array points,
                                                    PackedVector2Array polygon);
};
```

**Files:** `src/geometry/geometry_ops.hpp`, `src/geometry/geometry_ops.cpp`

---

## Phase 5: Advanced Spatial Structures (Complete)

**Goal:** Alternative spatial data structures for specific use cases.

### 5.1 KDTree2D
K-D Tree for nearest neighbor queries.

```cpp
class KDTree2D : public RefCounted {
    // Build
    void build(PackedVector2Array points);
    void clear();
    int size();

    // Queries
    int query_nearest_one(Vector2 point);
    PackedInt32Array query_nearest(Vector2 point, int k);
    PackedInt32Array query_radius(Vector2 point, float radius);

    // Batch queries
    PackedInt32Array query_nearest_one_batch(PackedVector2Array points);
    Array query_nearest_batch(PackedVector2Array points, int k);

    // Note: KDTree is static (no insert/update) but faster for nearest-neighbor
};
```

**Files:** `src/spatial/kd_tree_2d.hpp`, `src/spatial/kd_tree_2d.cpp`

### 5.2 KDTree3D
3D variant.

```cpp
class KDTree3D : public RefCounted {
    void build(PackedVector3Array points);
    void clear();
    int size();

    int query_nearest_one(Vector3 point);
    PackedInt32Array query_nearest(Vector3 point, int k);
    PackedInt32Array query_radius(Vector3 point, float radius);

    PackedInt32Array query_nearest_one_batch(PackedVector3Array points);
    Array query_nearest_batch(PackedVector3Array points, int k);
};
```

**Files:** `src/spatial/kd_tree_3d.hpp`, `src/spatial/kd_tree_3d.cpp`

### 5.3 QuadTree
Adaptive spatial subdivision for 2D.

```cpp
class QuadTree : public RefCounted {
    // Configuration
    void set_bounds(Rect2 bounds);
    void set_max_depth(int depth);        // Default: 8
    void set_max_items_per_node(int n);   // Default: 8

    // Build
    void build(PackedVector2Array positions);
    void insert(Vector2 position);
    void clear();

    // Queries
    PackedInt32Array query_rect(Rect2 rect);
    PackedInt32Array query_radius(Vector2 center, float radius);

    // Debug
    Array get_node_bounds();  // Returns Array of Rect2 for visualization
};
```

**Files:** `src/spatial/quad_tree.hpp`, `src/spatial/quad_tree.cpp`

### 5.4 Octree
Adaptive spatial subdivision for 3D.

```cpp
class Octree : public RefCounted {
    void set_bounds(AABB bounds);
    void set_max_depth(int depth);
    void set_max_items_per_node(int n);

    void build(PackedVector3Array positions);
    void insert(Vector3 position);
    void clear();

    PackedInt32Array query_box(AABB box);
    PackedInt32Array query_radius(Vector3 center, float radius);

    Array get_node_bounds();
};
```

**Files:** `src/spatial/octree.hpp`, `src/spatial/octree.cpp`

---

## Phase 6: Interpolation & Statistics (Complete)

**Goal:** Numerical operations for animation, curves, and data analysis.

### 6.1 InterpolationOps
Curve and spline evaluation.

```cpp
class InterpolationOps {
    // Easing functions (batch)
    static PackedFloat32Array ease_in_quad(PackedFloat32Array t);
    static PackedFloat32Array ease_out_quad(PackedFloat32Array t);
    static PackedFloat32Array ease_in_out_quad(PackedFloat32Array t);
    static PackedFloat32Array ease_in_cubic(PackedFloat32Array t);
    static PackedFloat32Array ease_out_cubic(PackedFloat32Array t);
    static PackedFloat32Array ease_in_out_cubic(PackedFloat32Array t);
    static PackedFloat32Array ease_in_expo(PackedFloat32Array t);
    static PackedFloat32Array ease_out_expo(PackedFloat32Array t);
    static PackedFloat32Array ease_in_out_expo(PackedFloat32Array t);
    static PackedFloat32Array ease_in_elastic(PackedFloat32Array t);
    static PackedFloat32Array ease_out_elastic(PackedFloat32Array t);
    static PackedFloat32Array ease_in_bounce(PackedFloat32Array t);
    static PackedFloat32Array ease_out_bounce(PackedFloat32Array t);

    // Bezier curves
    static PackedVector2Array bezier_quadratic(Vector2 p0, Vector2 p1, Vector2 p2,
                                                PackedFloat32Array t);
    static PackedVector2Array bezier_cubic(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3,
                                            PackedFloat32Array t);
    static PackedVector3Array bezier_cubic_3d(Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3,
                                               PackedFloat32Array t);

    // Catmull-Rom splines
    static PackedVector2Array catmull_rom(PackedVector2Array control_points, int samples_per_segment);
    static PackedVector3Array catmull_rom_3d(PackedVector3Array control_points, int samples_per_segment);

    // B-splines
    static PackedVector2Array bspline(PackedVector2Array control_points, int degree, int samples);

    // Sample curve at multiple t values
    static PackedVector2Array sample_curve2d(Curve2D* curve, PackedFloat32Array t_values);
    static PackedVector3Array sample_curve3d(Curve3D* curve, PackedFloat32Array t_values);

    // Remap
    static PackedFloat32Array remap(PackedFloat32Array values, float in_min, float in_max,
                                     float out_min, float out_max);
    static PackedFloat32Array clamp_array(PackedFloat32Array values, float min_val, float max_val);
    static PackedFloat32Array smoothstep_array(PackedFloat32Array t);
};
```

**Files:** `src/interpolation/interpolation_ops.hpp`, `src/interpolation/interpolation_ops.cpp`

### 6.2 StatOps
Statistical operations.

```cpp
class StatOps {
    // Descriptive statistics
    static float mean(PackedFloat32Array values);
    static float median(PackedFloat32Array values);
    static float mode(PackedFloat32Array values, float bin_size);
    static float variance(PackedFloat32Array values);
    static float std_dev(PackedFloat32Array values);
    static float percentile(PackedFloat32Array values, float p);  // p in 0-100
    static PackedFloat32Array percentiles(PackedFloat32Array values, PackedFloat32Array ps);

    // Range statistics
    static Vector2 min_max(PackedFloat32Array values);  // Returns (min, max)
    static float range(PackedFloat32Array values);      // max - min
    static float iqr(PackedFloat32Array values);        // Interquartile range

    // Histograms
    static PackedInt32Array histogram(PackedFloat32Array values, int bin_count);
    static PackedInt32Array histogram_range(PackedFloat32Array values, float min_val,
                                             float max_val, int bin_count);
    static PackedFloat32Array bin_edges(float min_val, float max_val, int bin_count);

    // Correlation
    static float correlation(PackedFloat32Array x, PackedFloat32Array y);
    static float covariance(PackedFloat32Array x, PackedFloat32Array y);

    // Normalization
    static PackedFloat32Array normalize_min_max(PackedFloat32Array values);
    static PackedFloat32Array normalize_z_score(PackedFloat32Array values);

    // Running statistics
    static PackedFloat32Array running_mean(PackedFloat32Array values, int window);
    static PackedFloat32Array running_max(PackedFloat32Array values, int window);
    static PackedFloat32Array running_min(PackedFloat32Array values, int window);
    static PackedFloat32Array exponential_moving_average(PackedFloat32Array values, float alpha);

    // Outlier detection
    static PackedInt32Array outliers_iqr(PackedFloat32Array values, float k);  // k * IQR
    static PackedInt32Array outliers_zscore(PackedFloat32Array values, float threshold);
};
```

**Files:** `src/stats/stat_ops.hpp`, `src/stats/stat_ops.cpp`

---

## Implementation Guidelines

### For Each New Primitive

1. **Create header and implementation files** in appropriate `src/` subdirectory
2. **Add to `register_types.cpp`** for Godot binding
3. **Write GDScript tests** in `project/tests/`
4. **Write benchmarks** in `project/benchmarks/`
5. **Update CLAUDE.md** with usage examples
6. **Update README.md** with new features

### Performance Requirements

| Operation Type | Minimum Speedup |
|----------------|-----------------|
| Simple (single loop) | 5x |
| Medium (nested loops) | 10x |
| Complex (algorithms) | 20x |
| Batch operations | 10x per item |

### Testing Checklist

- [ ] Basic functionality tests
- [ ] Edge cases (empty arrays, single item, etc.)
- [ ] Large dataset tests (10k+ items)
- [ ] Benchmark vs GDScript equivalent
- [ ] Memory leak checks

### Code Style

- Use `PackedXxxArray` for all array inputs/outputs
- Return indices rather than copies when possible
- Provide both single and batch variants
- Use sensible defaults for optional parameters
- Document expected complexity in comments

---

## Summary

| Phase | Primitives | Focus Area |
|-------|------------|------------|
| 1 | SpatialHash3D, MathOps, BatchOps | Core extensions |
| 2 | RandomOps, NoiseOps | Procedural generation |
| 3 | GridOps, PathfindingOps | Tile-based & navigation |
| 4 | CollisionOps, GeometryOps | Physics & geometry |
| 5 | KDTree2D/3D, QuadTree, Octree | Advanced spatial |
| 6 | InterpolationOps, StatOps | Animation & analysis |

**Total new primitives:** 14
**Total new source files:** ~28 (header + impl each)

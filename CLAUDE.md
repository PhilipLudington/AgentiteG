# Claude Code Guide for AgentiteG

This file helps Claude Code (and other AI assistants) understand and use AgentiteG effectively.

## What is AgentiteG?

AgentiteG is a native C++ GDExtension for Godot that provides high-performance operations for common game development hot paths. Use it when GDScript loops become a bottleneck.

## When to Use AgentiteG

**Use AgentiteG when:**
- Iterating over 100+ entities per frame
- Performing spatial queries (find nearby, in radius, in rect)
- Sorting or filtering large arrays
- Per-frame distance calculations
- Batch vector operations (steering, physics)

**Don't use AgentiteG for:**
- Small arrays (<50 elements)
- One-time operations (level load, etc.)
- Logic that requires game-specific knowledge

## Available Classes

| Class | Purpose | API Docs |
|-------|---------|----------|
| `SpatialHash2D` | O(1) spatial queries in 2D | [docs/api/SpatialHash2D.md](docs/api/SpatialHash2D.md) |
| `SpatialHash3D` | O(1) spatial queries in 3D | [docs/api/SpatialHash3D.md](docs/api/SpatialHash3D.md) |
| `KDTree2D` | Fast nearest neighbor queries in 2D | [docs/api/KDTree2D.md](docs/api/KDTree2D.md) |
| `KDTree3D` | Fast nearest neighbor queries in 3D | [docs/api/KDTree3D.md](docs/api/KDTree3D.md) |
| `QuadTree` | Adaptive spatial subdivision for 2D | [docs/api/QuadTree.md](docs/api/QuadTree.md) |
| `Octree` | Adaptive spatial subdivision for 3D | [docs/api/Octree.md](docs/api/Octree.md) |
| `ArrayOps` | Filter, sort, reduce arrays | [docs/api/ArrayOps.md](docs/api/ArrayOps.md) |
| `MathOps` | Batch vector math operations | [docs/api/MathOps.md](docs/api/MathOps.md) |
| `BatchOps` | Steering, flocking, game loops | [docs/api/BatchOps.md](docs/api/BatchOps.md) |
| `RandomOps` | Bulk random generation | [docs/api/RandomOps.md](docs/api/RandomOps.md) |
| `NoiseOps` | Procedural noise (Perlin, etc.) | [docs/api/NoiseOps.md](docs/api/NoiseOps.md) |
| `GridOps` | 2D grid utilities (flood fill, FOV, etc.) | [docs/api/GridOps.md](docs/api/GridOps.md) |
| `PathfindingOps` | A*, Dijkstra, flow fields | [docs/api/PathfindingOps.md](docs/api/PathfindingOps.md) |
| `CollisionOps` | Batch collision detection | [docs/api/CollisionOps.md](docs/api/CollisionOps.md) |
| `GeometryOps` | Computational geometry algorithms | [docs/api/GeometryOps.md](docs/api/GeometryOps.md) |
| `InterpolationOps` | Easing, bezier, splines | [docs/api/InterpolationOps.md](docs/api/InterpolationOps.md) |
| `StatOps` | Statistical operations | [docs/api/StatOps.md](docs/api/StatOps.md) |

## Quick Examples

### Spatial Queries
```gdscript
var spatial = SpatialHash2D.new()
spatial.cell_size = 64.0
spatial.build(enemy_positions)

var nearby = spatial.query_radius(player.position, detection_range)
var closest = spatial.query_nearest_one(player.position)
var has_enemy = spatial.has_any_in_radius(player.position, alert_range)
```

### Array Filtering
```gdscript
var low_health = ArrayOps.filter_lt_float(health_values, 20.0)
var on_screen = ArrayOps.filter_in_rect(positions, viewport_rect)
var by_distance = ArrayOps.argsort_by_distance(positions, origin)
var weakest = ArrayOps.argmin_floats(health_values)
```

### Batch Math
```gdscript
var directions = MathOps.direction_batch_2d(positions, targets)
var velocities = MathOps.scale_batch_2d(directions, speed)
positions = MathOps.add_batch_2d(positions, MathOps.scale_batch_2d(velocities, delta))
velocities = MathOps.clamp_length_batch_2d(velocities, max_speed)
```

### Steering & Flocking
```gdscript
# Seek targets
var desired = BatchOps.seek_batch(positions, targets, max_speed)

# Arrive (slows down near target)
var desired = BatchOps.arrive_batch(positions, targets, max_speed, slowing_radius)

# Boids flocking
var forces = BatchOps.flock_2d(positions, velocities, sep_r, coh_r, align_r, sep_s, coh_s, align_s)

# Update with forces
velocities = BatchOps.apply_accelerations_2d(velocities, forces, delta)
velocities = BatchOps.limit_velocity_2d(velocities, max_speed)
positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
```

### Random Generation
```gdscript
var rng = RandomOps.new()
rng.seed(12345)  # Deterministic seed

# Bulk random values
var spawn_heights = rng.randf_range_array(100, 0.0, 10.0)
var loot_rolls = rng.randi_range_array(100, 1, 100)

# Random positions in shapes
var spawn_points = rng.rand_points_in_circle(50, spawn_center, spawn_radius)
var item_positions = rng.rand_points_in_rect(20, play_area)

# Weighted loot tables
var loot_weights = PackedFloat32Array([0.5, 0.3, 0.15, 0.05])  # Common to legendary
var drops = rng.weighted_choice(loot_weights, num_drops)

# Blue noise distribution (evenly spaced random)
var tree_positions = rng.poisson_disk_2d(forest_bounds, min_spacing, 30)

# Random directions for particle effects
var directions = rng.rand_directions_2d(particle_count)
```

### Procedural Noise
```gdscript
var noise = NoiseOps.new()
noise.set_seed(world_seed)
noise.set_octaves(4)
noise.set_frequency(0.02)

# Single value sampling
var terrain_height = noise.fbm_2d(world_position)

# Heightmap generation
var heightmap = noise.fbm_2d_grid(Vector2.ZERO, Vector2(1, 1), 256, 256)

# Different noise types for variety
var base_terrain = noise.fbm_2d(pos)        # Smooth hills
var mountains = noise.ridged_2d(pos)         # Sharp ridges
var caves = noise.worley_2d(pos)             # Cellular/caves
var detail = noise.turbulence_2d(pos)        # Clouds/smoke

# Domain warping for organic shapes
var warped_coords = noise.warp_2d(sample_positions, warp_strength)
```

### Grid Operations
```gdscript
# Coordinate conversion
var index = GridOps.to_index(x, y, width)
var pos = GridOps.to_coords(index, width)

# Neighbor queries
var neighbors = GridOps.neighbors_4(cell_index, width, height)
var all_neighbors = GridOps.neighbors_8(cell_index, width, height)

# Flood fill to find connected regions
var connected = GridOps.flood_fill(grid, width, start_cell, target_value)

# Line of sight checks
var can_see = GridOps.line_clear(grid, width, from_pos, to_pos, wall_value)
var line_cells = GridOps.line_indices(from_pos, to_pos, width)

# Field of view for roguelikes
var visible_cells = GridOps.fov_shadowcast(grid, width, height, origin, radius, wall_value)

# Distance fields for AI
var distances = GridOps.manhattan_distance_field(grid, width, height, goal_value)

# Connected component analysis
var room_count = GridOps.count_connected_components(grid, width, height, floor_value)
var labels = GridOps.label_connected_components(grid, width, height, floor_value)
```

### Pathfinding
```gdscript
# A* pathfinding with variable terrain costs
# costs array: positive = traversable (higher = slower), 0 or negative = blocked
var path = PathfindingOps.astar_grid(costs, width, height, start, goal, true)

# A* for uniform-cost grids (faster)
var path = PathfindingOps.astar_uniform(walkable, width, height, start, goal, true)

# Jump Point Search (much faster for uniform grids)
var path = PathfindingOps.jps_grid(walkable, width, height, start, goal)

# Dijkstra map for AI decision making
var player_distances = PathfindingOps.dijkstra_map_single(costs, width, height, player_pos)
# Now any enemy can look up: player_distances[enemy_cell] to get distance to player

# Flow field for steering many units to same goal
var flow = PathfindingOps.flow_field(costs, width, height, rally_point)
# Each unit: velocity = flow[my_cell] * speed

# Find all reachable cells within movement budget
var move_range = PathfindingOps.reachable_cells(costs, width, height, unit_pos, move_points)

# Convert index path to world positions
var world_path = PathfindingOps.path_to_vectors(path, width, cell_size)
```

### Collision Detection
```gdscript
# Point containment tests
var inside_rect = CollisionOps.points_in_rect(positions, viewport_rect)
var inside_circle = CollisionOps.points_in_circle(positions, center, radius)
var inside_polygon = CollisionOps.points_in_polygon(positions, polygon_verts)

# Circle vs circle collision (returns pairs [i0, j0, i1, j1, ...])
var pairs = CollisionOps.circles_vs_circles_uniform(
    bullet_positions, bullet_radius,
    enemy_positions, enemy_radius
)
for i in range(0, pairs.size(), 2):
    handle_hit(bullets[pairs[i]], enemies[pairs[i + 1]])

# Self-collision detection
var overlapping = CollisionOps.circles_self_collision_uniform(positions, radius)

# Ray casting
var hit_idx = CollisionOps.ray_first_circle(origin, direction, max_dist, centers, radii)
var distances = CollisionOps.ray_vs_circles(origin, direction, centers, radii)

# Segment intersection
var intersecting = CollisionOps.segments_intersect(starts_a, ends_a, starts_b, ends_b)
var intersection_points = CollisionOps.segment_intersection_points(starts_a, ends_a, starts_b, ends_b)
```

### Computational Geometry
```gdscript
# Convex hull
var hull = GeometryOps.convex_hull(scattered_points)
var hull_indices = GeometryOps.convex_hull_indices(scattered_points)

# Triangulation
var triangles = GeometryOps.triangulate(polygon)  # [i0, i1, i2, i3, i4, i5, ...]
var delaunay_tris = GeometryOps.delaunay(points)

# Voronoi diagrams
var cells = GeometryOps.voronoi_cells(seed_points, bounds)  # Array of PackedVector2Array

# Polygon properties
var area = GeometryOps.polygon_area(polygon)
var centroid = GeometryOps.polygon_centroid(polygon)
var is_convex = GeometryOps.polygon_is_convex(polygon)
var is_clockwise = GeometryOps.polygon_is_clockwise(polygon)

# Polyline simplification
var simplified = GeometryOps.simplify_rdp(polyline, epsilon)  # Ramer-Douglas-Peucker
var simplified = GeometryOps.simplify_visvalingam(polyline, target_count)

# Polygon offset (expand/shrink)
var expanded = GeometryOps.offset_polygon(polygon, 10.0)
var shrunk = GeometryOps.offset_polygon(polygon, -5.0)

# Distance queries
var dist = GeometryOps.point_to_polygon_distance(point, polygon)
var closest = GeometryOps.closest_point_on_polygon(point, polygon)
```

### K-D Trees (Nearest Neighbor)
```gdscript
# KDTree2D - optimized for nearest neighbor queries (static data)
var kdtree = KDTree2D.new()
kdtree.build(all_positions)

# Find single nearest
var nearest_idx = kdtree.query_nearest_one(player.position)

# Find k nearest neighbors
var closest_5 = kdtree.query_nearest(player.position, 5)

# Also supports radius queries
var in_range = kdtree.query_radius(player.position, detection_range)

# Batch queries for many points at once
var all_nearest = kdtree.query_nearest_one_batch(enemy_positions)
```

### QuadTree / Octree (Adaptive Spatial)
```gdscript
# QuadTree - adapts to point density, good for non-uniform distributions
var quadtree = QuadTree.new()
quadtree.set_bounds(Rect2(0, 0, world_width, world_height))
quadtree.max_depth = 8
quadtree.max_items_per_node = 8

quadtree.build(entity_positions)

# Query by rectangle or radius
var in_area = quadtree.query_rect(selection_rect)
var nearby = quadtree.query_radius(click_pos, selection_radius)

# Get node bounds for debug visualization
var all_bounds = quadtree.get_node_bounds()

# Octree works the same way in 3D
var octree = Octree.new()
octree.set_bounds(AABB(Vector3.ZERO, world_size))
octree.build(positions_3d)
var in_box = octree.query_box(frustum_aabb)
```

### Interpolation & Animation
```gdscript
# Generate evenly spaced t values
var t_values = InterpolationOps.linspace(0.0, 1.0, 100)

# Apply easing functions (all take/return arrays)
var eased = InterpolationOps.ease_out_elastic(t_values)
var smooth = InterpolationOps.smoothstep(t_values)

# Many easing types available:
# ease_in/out/in_out_quad, cubic, quart, quint
# ease_in/out/in_out_sine, expo, circ, back, elastic, bounce

# Bezier curves for smooth paths
var path_points = InterpolationOps.bezier_cubic_2d(
    start_pos, control1, control2, end_pos, t_values
)

# Catmull-Rom splines (smooth curve through all control points)
var control_points = PackedVector2Array([p0, p1, p2, p3, p4])
var spline = InterpolationOps.catmull_rom_2d(control_points, 10)  # 10 samples per segment

# Utility functions
var remapped = InterpolationOps.remap(values, 0, 100, -1, 1)
var clamped = InterpolationOps.clamp_array(values, 0.0, 1.0)
```

### Statistics & Analytics
```gdscript
var scores = PackedFloat32Array([...])

# Descriptive statistics
var avg = StatOps.mean(scores)
var mid = StatOps.median(scores)
var spread = StatOps.std_dev(scores)
var mm = StatOps.min_max(scores)  # Returns Vector2(min, max)

# Percentiles and quartiles
var p95 = StatOps.percentile(scores, 95.0)
var q1 = StatOps.q1(scores)  # 25th percentile
var q3 = StatOps.q3(scores)  # 75th percentile

# Normalization
var normalized = StatOps.normalize_min_max(scores)  # Scale to [0, 1]
var z_scores = StatOps.normalize_z_score(scores)    # Mean=0, std=1

# Running statistics (great for smoothing data)
var smoothed = StatOps.running_mean(scores, 10)  # 10-sample window
var ema = StatOps.exponential_moving_average(scores, 0.3)
var cumulative = StatOps.cumsum(scores)

# Outlier detection
var outliers = StatOps.outliers_zscore(scores, 2.0)  # Z > 2.0
var iqr_outliers = StatOps.outliers_iqr(scores, 1.5)  # IQR method

# Histograms
var bins = StatOps.histogram(scores, 10)  # 10 bins
var edges = StatOps.bin_edges(0.0, 100.0, 10)

# Correlation between two datasets
var r = StatOps.correlation(x_values, y_values)
```

## Core Patterns

### Index-Based Design
AgentiteG returns indices, not objects. Use indices to look up your game objects:
```gdscript
var nearby_indices = spatial.query_radius(pos, range)
for i in nearby_indices:
    var enemy = enemies[i]  # Look up actual object
    enemy.take_damage(10)
```

### Chain Operations
```gdscript
# Find weakest enemy in range
var in_range = spatial.query_radius(attacker.position, attack_range)
var in_range_health = ArrayOps.select_floats(all_health, in_range)
var weakest_local = ArrayOps.argmin_floats(in_range_health)
var weakest_global = in_range[weakest_local]
```

### Use Fast Checks
```gdscript
# Good: no allocation
if spatial.has_any_in_radius(pos, range):
    do_something()

# Less efficient: allocates array
if spatial.query_radius(pos, range).size() > 0:
    do_something()
```

## Performance Expectations

| Operation | ~Items | GDScript | AgentiteG | Speedup |
|-----------|--------|----------|-----------|---------|
| Radius query | 10,000 | ~20ms | ~0.15ms | 130x |
| Filter floats | 10,000 | ~1.5ms | ~0.08ms | 18x |
| Sort floats | 10,000 | ~5ms | ~0.3ms | 15x |
| Batch normalize | 10,000 | ~3ms | ~0.1ms | 30x |

## Troubleshooting

### "Identifier X not declared in the current scope"

This error has three possible causes. **Check in this order:**

1. **Extension not loading** (most common)
   ```bash
   godot --headless --import 2>&1 | grep -i "error\|can't open"
   ```
   If you see "Can't open dynamic library", check that library files exist at paths in `.gdextension`

2. **Class not registered** - Check `register_types.cpp` includes the class

3. **Code bug** - Class not defined properly

### Common Fix: .gdextension paths

Godot tries architecture-specific paths first. If they don't exist, loading fails:
```ini
# BAD - fails if arm64.dylib doesn't exist
macos.debug.arm64 = "res://addons/.../libagentiteg.macos.template_debug.arm64.dylib"

# GOOD - universal works for all
macos.debug = "res://addons/.../libagentiteg.macos.template_debug.universal.dylib"
```

### Build succeeds but classes not found

1. Clear cache: `rm -rf project/.godot`
2. Re-import: `godot --headless --import`
3. Test: `godot --headless --script tests/your_test.gd`

## Tips

1. **Tune cell_size** for spatial hashes: 1-2x your typical query radius

2. **Rebuild spatial hash strategically**: Once per frame if many move, or `update()` for individuals

3. **Prefer squared distances** when comparing: `distance_squared_batch` avoids sqrt

4. **Use count/has methods** when you don't need the actual indices

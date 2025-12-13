# AgentiteG API Reference

High-performance GDExtension primitives for Godot game development.

## Classes

### Spatial Data Structures

| Class | Description | Best For |
|-------|-------------|----------|
| [SpatialHash2D](SpatialHash2D.md) | O(1) spatial queries for 2D | Dynamic entities, radius queries |
| [SpatialHash3D](SpatialHash3D.md) | O(1) spatial queries for 3D | Dynamic 3D entities |
| [KDTree2D](KDTree2D.md) | K-d tree for 2D positions | Nearest neighbor, static data |
| [KDTree3D](KDTree3D.md) | K-d tree for 3D positions | 3D targeting, space games |
| [QuadTree](QuadTree.md) | Adaptive 2D subdivision | Clustered data, RTS games |
| [Octree](Octree.md) | Adaptive 3D subdivision | Asteroid fields, debris clouds |

### Array Operations

| Class | Description | Best For |
|-------|-------------|----------|
| [ArrayOps](ArrayOps.md) | Filter, sort, reduce arrays | Finding entities, sorting by distance |

### Math Operations

| Class | Description | Best For |
|-------|-------------|----------|
| [MathOps](MathOps.md) | Batch vector math | Steering calculations, physics |
| [BatchOps](BatchOps.md) | Steering and flocking | Boids, RTS movement, swarm AI |

### Procedural Generation

| Class | Description | Best For |
|-------|-------------|----------|
| [RandomOps](RandomOps.md) | Bulk random generation | Spawning, loot tables, Poisson disk |
| [NoiseOps](NoiseOps.md) | Procedural noise | Terrain, caves, biomes |

### Grid & Pathfinding

| Class | Description | Best For |
|-------|-------------|----------|
| [GridOps](GridOps.md) | 2D grid utilities | FOV, flood fill, distance fields |
| [PathfindingOps](PathfindingOps.md) | Pathfinding algorithms | A*, JPS, Dijkstra, flow fields |

### Physics & Geometry

| Class | Description | Best For |
|-------|-------------|----------|
| [CollisionOps](CollisionOps.md) | Batch collision detection | Bullet hell, many projectiles |
| [GeometryOps](GeometryOps.md) | Computational geometry | Convex hull, triangulation, Voronoi |

### Animation & Analytics

| Class | Description | Best For |
|-------|-------------|----------|
| [InterpolationOps](InterpolationOps.md) | Easing, bezier, splines | UI animations, camera paths |
| [StatOps](StatOps.md) | Statistical operations | Analytics, leaderboards, cheat detection |

## Choosing the Right Spatial Structure

| Need | Use | Why |
|------|-----|-----|
| Find entities in radius | `SpatialHash2D` | O(1) queries, handles dynamic updates |
| Find exact nearest neighbor | `KDTree2D` | Guaranteed closest, O(log n) |
| Clustered data (cities, bases) | `QuadTree` | Adapts to density |
| Frequent position updates | `SpatialHash2D` | Fast individual updates |
| Visualize spatial partitioning | `QuadTree`/`Octree` | get_node_bounds() for debug |

## Design Principles

1. **Zero game-specific logic** - Pure algorithms only
2. **Arrays in, arrays out** - Work with PackedArrays, return indices
3. **5-10x minimum speedup** - Every feature is benchmarked
4. **Simple API** - Easy signatures, sensible defaults

## Quick Start

```gdscript
# Spatial queries
var spatial = SpatialHash2D.new()
spatial.build(positions)
var nearby = spatial.query_radius(origin, radius)
var nearest = spatial.query_nearest_one(origin)

# Array filtering
var low_health = ArrayOps.filter_lt_float(health, 20.0)
var sorted_by_dist = ArrayOps.argsort_by_distance(positions, origin)

# Batch math
var directions = MathOps.direction_batch_2d(positions, targets)

# Pathfinding
var path = PathfindingOps.astar_grid(costs, width, height, start, goal, true)

# Procedural generation
var noise = NoiseOps.new()
noise.set_seed(12345)
var heightmap = noise.fbm_2d_grid(origin, cell_size, width, height)

# Easing
var t = InterpolationOps.linspace(0.0, 1.0, 60)
var eased = InterpolationOps.ease_out_quad(t)

# Statistics
var avg = StatOps.mean(scores)
var outliers = StatOps.outliers_zscore(scores, 2.0)
```

## Static vs Instance Classes

**Static classes** (call methods directly):
- ArrayOps, MathOps, BatchOps
- GridOps, PathfindingOps
- CollisionOps, GeometryOps
- InterpolationOps, StatOps

**Instance classes** (create with `.new()`):
- SpatialHash2D, SpatialHash3D
- KDTree2D, KDTree3D
- QuadTree, Octree
- RandomOps, NoiseOps

See [CLAUDE.md](../../CLAUDE.md) for usage patterns and troubleshooting.

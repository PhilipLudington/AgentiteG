# AgentiteG Examples

Comprehensive feature showcase examples for all 17 AgentiteG classes. Each example includes:

- **WHEN TO USE** guidance - Know exactly when each class is the right choice
- **USE CASE comments** - Real game development scenarios for every method
- **Game scenarios** - Complete workflows showing how to combine operations
- **Performance notes** - Understand the computational complexity

## Running Examples

All examples are self-contained GDScript files that can be run headlessly:

```bash
# From the project directory
godot --headless -s examples/example_spatial_hash_2d.gd
```

Or run all examples:

```bash
for f in examples/example_*.gd; do
    echo "=== Running $f ==="
    godot --headless -s "$f"
done
```

## Examples by Category

### Spatial Data Structures

| File | Class | Best For |
|------|-------|----------|
| `example_spatial_hash_2d.gd` | `SpatialHash2D` | Uniform distributions, dynamic updates, radius queries |
| `example_spatial_hash_3d.gd` | `SpatialHash3D` | 3D games with uniform entity distribution |
| `example_kdtree_2d.gd` | `KDTree2D` | Exact nearest neighbor, static point clouds |
| `example_kdtree_3d.gd` | `KDTree3D` | Space games, 3D targeting systems |
| `example_quadtree.gd` | `QuadTree` | Clustered 2D data, debug visualization |
| `example_octree.gd` | `Octree` | Clustered 3D data (asteroid fields, debris) |

### Array & Math Operations

| File | Class | Best For |
|------|-------|----------|
| `example_array_ops.gd` | `ArrayOps` | Filtering enemies, sorting by distance, finding min/max |
| `example_math_ops.gd` | `MathOps` | Batch vector math, steering calculations |
| `example_batch_ops.gd` | `BatchOps` | Boids flocking, RTS unit movement, swarm AI |

### Procedural Generation

| File | Class | Best For |
|------|-------|----------|
| `example_random_ops.gd` | `RandomOps` | Spawning, loot tables, Poisson disk placement |
| `example_noise_ops.gd` | `NoiseOps` | Terrain heightmaps, caves, biome transitions |

### Grid & Pathfinding

| File | Class | Best For |
|------|-------|----------|
| `example_grid_ops.gd` | `GridOps` | Roguelike FOV, flood fill, dungeon analysis |
| `example_pathfinding_ops.gd` | `PathfindingOps` | A*, JPS, Dijkstra maps, flow fields |

### Physics & Geometry

| File | Class | Best For |
|------|-------|----------|
| `example_collision_ops.gd` | `CollisionOps` | Bullet hell, thousands of projectiles |
| `example_geometry_ops.gd` | `GeometryOps` | Procedural islands, territories, mesh generation |

### Animation & Statistics

| File | Class | Best For |
|------|-------|----------|
| `example_interpolation_ops.gd` | `InterpolationOps` | UI animations, camera paths, patrol routes |
| `example_stat_ops.gd` | `StatOps` | Player analytics, leaderboards, cheat detection |

## Example Structure

Each example follows this pattern:

1. **Header Documentation** - WHEN TO USE, PERFECT FOR, key concepts
2. **Basic Usage** - Creating instances and configuration
3. **Method Demonstrations** - USE CASE comments for every method
4. **Game Scenario** - Complete workflow showing practical application

## Choosing the Right Spatial Structure

| Need | Use | Why |
|------|-----|-----|
| Find entities in radius | `SpatialHash2D` | O(1) queries, handles dynamic updates |
| Find nearest neighbor | `KDTree2D` | Exact results, O(log n) queries |
| Clustered data (cities, forests) | `QuadTree` | Adapts to density, efficient for clusters |
| Visualize spatial partitioning | `QuadTree`/`Octree` | get_node_bounds() for debug drawing |

## Quick Start

For the most common use cases, start with these examples:

- **Entity detection**: `example_spatial_hash_2d.gd` - radius queries, nearest neighbor
- **Pathfinding**: `example_pathfinding_ops.gd` - A*, flow fields, Dijkstra maps
- **Steering/AI**: `example_batch_ops.gd` - boids, seek, flee, arrive behaviors
- **Procedural terrain**: `example_noise_ops.gd` - FBM, ridged, Worley noise
- **Roguelike systems**: `example_grid_ops.gd` - FOV, flood fill, distance fields
- **Bullet hell**: `example_collision_ops.gd` - batch circle vs circle collision

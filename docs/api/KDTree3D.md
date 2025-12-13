# KDTree3D

Fast nearest neighbor queries for 3D positions using a k-d tree.

## When to Use

- 3D targeting systems (space games, flight simulators)
- Finding nearest resources in 3D (asteroids, pickups)
- Static 3D point clouds (star maps, waypoint networks)
- When you need exact nearest neighbors in 3D space

## Comparison with SpatialHash3D

| Feature | KDTree3D | SpatialHash3D |
|---------|----------|---------------|
| Nearest neighbor | Excellent (exact) | Good (approximate) |
| Radius queries | Good | Excellent |
| Dynamic updates | Poor (rebuild needed) | Excellent |
| Build time | O(n log n) | O(n) |

**Rule of thumb**: Use KDTree3D for static 3D data with nearest-neighbor queries. Use SpatialHash3D for dynamic data.

## Methods

### Building

#### `build(positions: PackedVector3Array) -> void`
Constructs a balanced k-d tree from 3D positions.

```gdscript
var kdtree = KDTree3D.new()
var asteroid_positions = PackedVector3Array([...])
kdtree.build(asteroid_positions)
```

#### `size() -> int`
Returns the number of points in the tree.

### Nearest Neighbor Queries

#### `query_nearest_one(origin: Vector3) -> int`
Find the single nearest point. Returns -1 if empty.

```gdscript
# Find nearest asteroid to mine
var closest = kdtree.query_nearest_one(ship.position)
if closest >= 0:
    ship.target_asteroid(asteroids[closest])
```

#### `query_nearest(origin: Vector3, k: int) -> PackedInt32Array`
Find the k nearest points, sorted by distance.

```gdscript
# Lock onto 3 nearest targets
var targets = kdtree.query_nearest(fighter.position, 3)
for idx in targets:
    fighter.add_target_lock(enemies[idx])
```

#### `query_nearest_one_batch(origins: PackedVector3Array) -> PackedInt32Array`
Find nearest point for multiple query points at once.

```gdscript
# Fleet targeting - each ship finds nearest enemy
var ship_positions = PackedVector3Array([...])
var targets = kdtree.query_nearest_one_batch(ship_positions)
```

### Radius Queries

#### `query_radius(origin: Vector3, radius: float) -> PackedInt32Array`
Find all points within radius.

```gdscript
# Sensor sweep
var detected = kdtree.query_radius(station.position, sensor_range)
for idx in detected:
    radar.add_contact(objects[idx])
```

## Performance Tips

1. **Build infrequently**: K-d trees are optimized for queries, not updates.

2. **3D space is vast**: In 3D, brute-force nearest neighbor is O(n) distance calculations. KDTree3D prunes entire octants, making it much faster for large datasets.

3. **Batch queries**: Use `query_nearest_one_batch()` when multiple entities need targeting.

## Example: Space Mining Game

```gdscript
var kdtree: KDTree3D
var asteroid_positions: PackedVector3Array
var asteroids: Array[Asteroid]

func _ready():
    kdtree = KDTree3D.new()
    rebuild_asteroid_tree()

func rebuild_asteroid_tree():
    asteroid_positions.clear()
    for asteroid in asteroids:
        asteroid_positions.append(asteroid.position)
    kdtree.build(asteroid_positions)

func on_asteroid_destroyed(asteroid):
    asteroids.erase(asteroid)
    rebuild_asteroid_tree()

func get_mining_target(ship_position: Vector3) -> Asteroid:
    var idx = kdtree.query_nearest_one(ship_position)
    return asteroids[idx] if idx >= 0 else null

func get_asteroids_in_range(position: Vector3, range: float) -> Array[Asteroid]:
    var result: Array[Asteroid] = []
    var indices = kdtree.query_radius(position, range)
    for idx in indices:
        result.append(asteroids[idx])
    return result
```

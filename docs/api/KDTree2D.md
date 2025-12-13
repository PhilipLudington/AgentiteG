# KDTree2D

Fast nearest neighbor queries for 2D positions using a k-d tree.

## When to Use

- Finding exact nearest neighbors (homing missiles, auto-aim)
- Static or rarely-changing point sets (waypoints, POIs, spawn points)
- When you need guaranteed closest results, not approximations
- K-nearest neighbor queries (top N threats, closest allies)

## Comparison with SpatialHash2D

| Feature | KDTree2D | SpatialHash2D |
|---------|----------|---------------|
| Nearest neighbor | Excellent (exact) | Good (approximate) |
| Radius queries | Good | Excellent |
| Dynamic updates | Poor (rebuild needed) | Excellent |
| Build time | O(n log n) | O(n) |
| Query time | O(log n) | O(1) |

**Rule of thumb**: Use KDTree2D for static data with nearest-neighbor queries. Use SpatialHash2D for dynamic data or radius-only queries.

## Methods

### Building

#### `build(positions: PackedVector2Array) -> void`
Constructs a balanced k-d tree from positions. This is O(n log n).

```gdscript
var kdtree = KDTree2D.new()
var waypoints = PackedVector2Array([...])
kdtree.build(waypoints)  # Do this once, not every frame
```

#### `size() -> int`
Returns the number of points in the tree.

### Nearest Neighbor Queries

#### `query_nearest_one(origin: Vector2) -> int`
Find the single nearest point. Returns -1 if empty.

```gdscript
# Homing missile finds its target
var target_idx = kdtree.query_nearest_one(missile.position)
if target_idx >= 0:
    missile.home_on(enemies[target_idx])
```

#### `query_nearest(origin: Vector2, k: int) -> PackedInt32Array`
Find the k nearest points, sorted by distance (closest first).

```gdscript
# Multi-target lock-on
var targets = kdtree.query_nearest(player.position, 3)
for idx in targets:
    lock_on_indicator.add(enemies[idx])
```

#### `query_nearest_one_batch(origins: PackedVector2Array) -> PackedInt32Array`
Find nearest point for multiple query points at once.

```gdscript
# Each seeker finds its target
var seeker_positions = PackedVector2Array([...])
var targets = kdtree.query_nearest_one_batch(seeker_positions)
for i in range(seekers.size()):
    seekers[i].target = enemies[targets[i]]
```

### Radius Queries

#### `query_radius(origin: Vector2, radius: float) -> PackedInt32Array`
Find all points within radius. Results are not sorted.

```gdscript
# Area effect damage
var in_blast = kdtree.query_radius(explosion.position, blast_radius)
for idx in in_blast:
    enemies[idx].take_damage(damage)
```

## Performance Tips

1. **Build infrequently**: The tree is optimized for queries, not updates. Rebuild when data changes significantly, not every frame.

2. **Use for static data**: KDTree2D excels when points don't move (map POIs, spawn points, waypoints). For frequently-moving entities, use SpatialHash2D.

3. **Batch queries**: When multiple entities need nearest neighbors, use `query_nearest_one_batch()` instead of looping.

4. **Rebuild strategically**:
   - Entity destroyed → Rebuild
   - New entities spawned → Rebuild
   - Entities moved → Only rebuild if positions changed significantly

## Example

```gdscript
var kdtree: KDTree2D
var enemy_positions: PackedVector2Array
var enemies: Array[Enemy]

func _ready():
    kdtree = KDTree2D.new()

func on_enemies_changed():
    # Rebuild when enemy list changes
    enemy_positions.clear()
    for enemy in enemies:
        enemy_positions.append(enemy.position)
    kdtree.build(enemy_positions)

func _physics_process(delta):
    # Missiles home on nearest enemy
    for missile in missiles:
        var target_idx = kdtree.query_nearest_one(missile.position)
        if target_idx >= 0:
            var direction = (enemies[target_idx].position - missile.position).normalized()
            missile.velocity = missile.velocity.lerp(direction * missile.speed, 0.1)
```

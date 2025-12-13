# SpatialHash2D

Fast O(1) spatial lookups for 2D positions using a hash grid.

## When to Use

- Finding nearby entities (enemies, allies, pickups)
- Collision detection broad phase
- Range-based ability targeting
- Proximity alerts

## Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `cell_size` | float | 64.0 | Size of each grid cell. Set to 1-2x your typical query radius. |

## Methods

### Building & Updating

#### `build(positions: PackedVector2Array) -> void`
Clears existing data and rebuilds the hash from scratch.

```gdscript
var positions = PackedVector2Array([enemy1.position, enemy2.position, ...])
spatial.build(positions)
```

#### `clear() -> void`
Removes all items from the hash.

#### `insert(position: Vector2) -> int`
Adds a single item. Returns the assigned index.

```gdscript
var idx = spatial.insert(new_enemy.position)
```

#### `update(index: int, new_position: Vector2) -> void`
Updates an existing item's position.

```gdscript
spatial.update(enemy_idx, enemy.position)
```

### Queries

All query methods return `PackedInt32Array` containing indices into the original positions array.

#### `query_radius(origin: Vector2, radius: float) -> PackedInt32Array`
Find all items within radius of origin.

```gdscript
var nearby = spatial.query_radius(player.position, detection_range)
for i in nearby:
    print("Enemy at index ", i, " is nearby")
```

#### `query_rect(rect: Rect2) -> PackedInt32Array`
Find all items within a rectangle.

```gdscript
var on_screen = spatial.query_rect(camera.get_viewport_rect())
```

#### `query_nearest(origin: Vector2, k: int) -> PackedInt32Array`
Find the k nearest items to origin, sorted by distance.

```gdscript
var closest_3 = spatial.query_nearest(player.position, 3)
```

#### `query_nearest_one(origin: Vector2) -> int`
Find the single nearest item. Returns -1 if empty.

```gdscript
var target = spatial.query_nearest_one(turret.position)
if target >= 0:
    turret.aim_at(enemies[target])
```

### Fast Checks (No Allocation)

#### `has_any_in_radius(origin: Vector2, radius: float) -> bool`
Check if any item exists within radius. Faster than `query_radius().size() > 0`.

```gdscript
if spatial.has_any_in_radius(player.position, alert_range):
    trigger_alarm()
```

#### `count_in_radius(origin: Vector2, radius: float) -> int`
Count items within radius without allocating result array.

```gdscript
var threat_level = spatial.count_in_radius(base.position, defense_range)
```

### Batch Queries

#### `query_radius_batch(origins: PackedVector2Array, radii: PackedFloat32Array) -> Array`
Multiple radius queries with different radii. Returns Array of PackedInt32Array.

```gdscript
var all_nearby = spatial.query_radius_batch(unit_positions, unit_ranges)
for i in range(units.size()):
    units[i].nearby_enemies = all_nearby[i]
```

#### `query_radius_batch_uniform(origins: PackedVector2Array, radius: float) -> Array`
Multiple radius queries with same radius.

```gdscript
var all_nearby = spatial.query_radius_batch_uniform(unit_positions, attack_range)
```

## Performance Tips

1. **Tune cell_size**: Set to 1-2x your typical query radius
   - Too small = many cells to check
   - Too large = many items per cell to iterate

2. **Rebuild strategically**: Call `build()` once per frame if many entities move, or use `update()` for individual changes

3. **Use fast checks**: Prefer `has_any_in_radius()` and `count_in_radius()` over `query_radius()` when you don't need the actual indices

## Example

```gdscript
var spatial: SpatialHash2D
var enemy_positions: PackedVector2Array
var enemies: Array[Enemy]

func _ready():
    spatial = SpatialHash2D.new()
    spatial.cell_size = 100.0  # 2x typical detection range of 50

func _physics_process(delta):
    # Rebuild when enemies move
    spatial.build(enemy_positions)

    # Each unit checks for nearby enemies
    for unit in friendly_units:
        if spatial.has_any_in_radius(unit.position, unit.detection_range):
            var nearby = spatial.query_radius(unit.position, unit.detection_range)
            for i in nearby:
                unit.on_enemy_detected(enemies[i])
```

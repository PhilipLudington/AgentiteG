# SpatialHash3D

Fast O(1) spatial lookups for 3D positions using a hash grid.

## When to Use

- Finding nearby entities in 3D space
- Space combat targeting
- 3D collision broad phase
- Proximity detection in 3D environments

## Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `cell_size` | float | 64.0 | Size of each grid cell. Set to 1-2x your typical query radius. |

## Methods

### Building & Updating

#### `build(positions: PackedVector3Array) -> void`
Clears existing data and rebuilds the hash from scratch.

```gdscript
var positions = PackedVector3Array([ship1.position, ship2.position, ...])
spatial.build(positions)
```

#### `clear() -> void`
Removes all items from the hash.

#### `insert(position: Vector3) -> int`
Adds a single item. Returns the assigned index.

```gdscript
var idx = spatial.insert(new_asteroid.position)
```

#### `update(index: int, new_position: Vector3) -> void`
Updates an existing item's position.

```gdscript
spatial.update(ship_idx, ship.global_position)
```

### Queries

All query methods return `PackedInt32Array` containing indices into the original positions array.

#### `query_radius(origin: Vector3, radius: float) -> PackedInt32Array`
Find all items within radius of origin (sphere query).

```gdscript
var nearby = spatial.query_radius(player_ship.position, sensor_range)
for i in nearby:
    print("Contact detected: ", ships[i].name)
```

#### `query_box(box: AABB) -> PackedInt32Array`
Find all items within an axis-aligned bounding box.

```gdscript
var in_sector = spatial.query_box(AABB(sector_origin, sector_size))
```

#### `query_nearest(origin: Vector3, k: int) -> PackedInt32Array`
Find the k nearest items to origin, sorted by distance.

```gdscript
var closest_targets = spatial.query_nearest(missile.position, 5)
```

#### `query_nearest_one(origin: Vector3) -> int`
Find the single nearest item. Returns -1 if empty.

```gdscript
var nearest = spatial.query_nearest_one(ship.position)
if nearest >= 0:
    ship.set_target(enemies[nearest])
```

### Fast Checks (No Allocation)

#### `has_any_in_radius(origin: Vector3, radius: float) -> bool`
Check if any item exists within radius. Faster than `query_radius().size() > 0`.

```gdscript
if spatial.has_any_in_radius(station.position, defense_perimeter):
    activate_turrets()
```

#### `count_in_radius(origin: Vector3, radius: float) -> int`
Count items within radius without allocating result array.

```gdscript
var enemy_count = spatial.count_in_radius(fleet_center, formation_radius)
```

### Batch Queries

#### `query_radius_batch(origins: PackedVector3Array, radii: PackedFloat32Array) -> Array`
Multiple radius queries with different radii. Returns Array of PackedInt32Array.

```gdscript
var all_contacts = spatial.query_radius_batch(ship_positions, sensor_ranges)
```

#### `query_radius_batch_uniform(origins: PackedVector3Array, radius: float) -> Array`
Multiple radius queries with same radius.

```gdscript
var all_nearby = spatial.query_radius_batch_uniform(turret_positions, weapons_range)
```

## Performance Tips

1. **Tune cell_size**: Set to 1-2x your typical query radius
   - For space games with large distances, use larger cells (100-500)
   - For indoor/room-scale, use smaller cells (10-50)

2. **Use AABB queries for rectangular regions**: `query_box()` is more efficient than `query_radius()` for box-shaped areas

3. **Rebuild vs Update**: Use `build()` when many items move, `update()` for individual changes

## Example

```gdscript
var spatial: SpatialHash3D
var ship_positions: PackedVector3Array
var ships: Array[Spaceship]

func _ready():
    spatial = SpatialHash3D.new()
    spatial.cell_size = 200.0  # Large for space scale

func _physics_process(delta):
    spatial.build(ship_positions)

    for ship in player_fleet:
        # Quick check first
        if not spatial.has_any_in_radius(ship.position, ship.weapons_range):
            continue

        # Find actual targets
        var targets = spatial.query_radius(ship.position, ship.weapons_range)
        var closest = spatial.query_nearest_one(ship.position)
        ship.set_target(ships[closest])
```

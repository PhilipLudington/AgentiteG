# Octree

Adaptive spatial subdivision for 3D positions. The 3D equivalent of QuadTree.

## When to Use

- Space games with clustered objects (asteroid fields, debris clouds)
- 3D games with non-uniform entity distributions
- Large 3D worlds with localized activity
- When you need to visualize/debug 3D spatial partitioning
- Broad-phase collision detection in 3D

## Comparison with Other 3D Structures

| Feature | Octree | SpatialHash3D | KDTree3D |
|---------|--------|---------------|----------|
| Clustered data | Excellent | Poor | Good |
| Uniform data | Fair | Excellent | Good |
| Nearest neighbor | Fair | Good | Excellent |
| Debug visualization | Excellent | Poor | Poor |

**Rule of thumb**: Use Octree for clustered 3D data (asteroid fields, debris). Use SpatialHash3D for uniform distributions.

## Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `max_depth` | int | 8 | Maximum subdivision depth |
| `max_items_per_node` | int | 8 | Items before node subdivides |

## Methods

### Configuration

#### `set_bounds(bounds: AABB) -> void`
Set the world bounds. **Must be called before build()**.

```gdscript
var octree = Octree.new()
octree.set_bounds(AABB(Vector3.ZERO, Vector3(1000, 1000, 1000)))
```

### Building

#### `build(positions: PackedVector3Array) -> void`
Build the octree from positions.

```gdscript
var asteroid_positions = PackedVector3Array([...])
octree.build(asteroid_positions)
```

#### `size() -> int`
Returns the number of items in the tree.

### Queries

#### `query_box(box: AABB) -> PackedInt32Array`
Find all items within an axis-aligned bounding box.

```gdscript
# Sector scan
var scan_volume = AABB(Vector3(-100, -100, -100), Vector3(200, 200, 200))
var in_sector = octree.query_box(scan_volume)
for idx in in_sector:
    radar.add_contact(objects[idx])
```

#### `query_radius(center: Vector3, radius: float) -> PackedInt32Array`
Find all items within a sphere.

```gdscript
# Sensor range check
var detected = octree.query_radius(station.position, sensor_range)
for idx in detected:
    if objects[idx].is_hostile:
        trigger_alert()
```

### Debug Visualization

#### `get_node_bounds() -> Array[AABB]`
Get all node bounding boxes for debug drawing.

```gdscript
# Draw octree structure
for aabb in octree.get_node_bounds():
    DebugDraw.draw_box(aabb, Color.GREEN)
```

## Example: Space Station Defense

```gdscript
var octree: Octree
var object_positions: PackedVector3Array
var space_objects: Array[SpaceObject]

func _ready():
    octree = Octree.new()
    octree.set_bounds(AABB(Vector3(-500, -500, -500), Vector3(1000, 1000, 1000)))

func rebuild_octree():
    object_positions.clear()
    for obj in space_objects:
        object_positions.append(obj.position)
    octree.build(object_positions)

func get_objects_in_turret_range(turret: Turret) -> Array[SpaceObject]:
    var result: Array[SpaceObject] = []
    var indices = octree.query_radius(turret.position, turret.range)
    for idx in indices:
        result.append(space_objects[idx])
    return result

func scan_sector(sector_bounds: AABB) -> Array[SpaceObject]:
    var result: Array[SpaceObject] = []
    var indices = octree.query_box(sector_bounds)
    for idx in indices:
        result.append(space_objects[idx])
    return result

func find_threats_in_range(position: Vector3, range: float) -> Array[SpaceObject]:
    var threats: Array[SpaceObject] = []
    for idx in octree.query_radius(position, range):
        if space_objects[idx].is_hostile:
            threats.append(space_objects[idx])
    return threats
```

## Configuration Tips

- **max_depth**: 8-10 is usually sufficient. Higher values use more memory.
- **max_items_per_node**: 8-16 is typical.
- **Bounds**: Set to encompass your entire play area with some margin.

## Performance Notes

- Dense regions create more octree nodes (more subdivisions)
- Empty space costs almost nothing (no nodes created)
- Use `query_box` for rectangular volumes (frustum culling approximation)
- Use `query_radius` for spherical volumes (sensors, weapons)

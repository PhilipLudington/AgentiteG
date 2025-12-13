# GridOps

High-performance operations for 2D grids stored as flat arrays in row-major order.

## Overview

GridOps provides fast algorithms for grid-based game systems including:
- Coordinate conversion between 2D and flat indices
- Neighbor queries (4-connected and 8-connected)
- Flood fill algorithms
- Line of sight (Bresenham's algorithm)
- Field of view (shadowcasting)
- Distance transforms
- Connected component labeling

All grid operations assume row-major order: `index = y * width + x`

## Quick Reference

| Method | Description |
|--------|-------------|
| `to_index` | Convert (x,y) to flat index |
| `to_coords` | Convert flat index to (x,y) |
| `neighbors_4` | Get 4-connected neighbors |
| `neighbors_8` | Get 8-connected neighbors |
| `flood_fill` | Get all connected cells with target value |
| `line_indices` | Get cells along a line |
| `line_clear` | Check if line is unobstructed |
| `fov_shadowcast` | Calculate visible cells |
| `manhattan_distance_field` | Distance from all cells to targets |
| `label_connected_components` | Label connected regions |

## Coordinate Conversion

### to_index

```gdscript
static func to_index(x: int, y: int, width: int) -> int
```

Convert 2D coordinates to a flat array index.

```gdscript
var index = GridOps.to_index(5, 10, 32)  # x=5, y=10, width=32
# index = 325
```

### to_coords

```gdscript
static func to_coords(index: int, width: int) -> Vector2i
```

Convert a flat array index to 2D coordinates.

```gdscript
var pos = GridOps.to_coords(325, 32)
# pos = Vector2i(5, 10)
```

### to_indices

```gdscript
static func to_indices(coords: PackedVector2Array, width: int) -> PackedInt32Array
```

Batch convert coordinates to indices.

```gdscript
var positions = PackedVector2Array([Vector2(0, 0), Vector2(5, 3), Vector2(10, 7)])
var indices = GridOps.to_indices(positions, 32)
```

### to_coords_batch

```gdscript
static func to_coords_batch(indices: PackedInt32Array, width: int) -> PackedVector2Array
```

Batch convert indices to coordinates.

### in_bounds / index_in_bounds

```gdscript
static func in_bounds(x: int, y: int, width: int, height: int) -> bool
static func index_in_bounds(index: int, width: int, height: int) -> bool
```

Check if coordinates or index are within grid bounds.

## Neighbor Queries

### neighbors_4

```gdscript
static func neighbors_4(index: int, width: int, height: int) -> PackedInt32Array
```

Get indices of 4-connected neighbors (N, E, S, W).

```gdscript
var grid_width = 10
var grid_height = 10
var center = GridOps.to_index(5, 5, grid_width)
var neighbors = GridOps.neighbors_4(center, grid_width, grid_height)
# Returns up to 4 valid neighbor indices
```

### neighbors_8

```gdscript
static func neighbors_8(index: int, width: int, height: int) -> PackedInt32Array
```

Get indices of 8-connected neighbors (includes diagonals).

### neighbors_4_batch / neighbors_8_batch

```gdscript
static func neighbors_4_batch(indices: PackedInt32Array, width: int, height: int) -> PackedInt32Array
static func neighbors_8_batch(indices: PackedInt32Array, width: int, height: int) -> PackedInt32Array
```

Get neighbors for multiple cells at once. Returns flat array of all neighbors.

### neighbors_custom

```gdscript
static func neighbors_custom(index: int, width: int, height: int, offsets: PackedVector2Array) -> PackedInt32Array
```

Get neighbors using custom offset patterns.

```gdscript
# Knight moves in chess
var knight_offsets = PackedVector2Array([
    Vector2(1, 2), Vector2(2, 1), Vector2(2, -1), Vector2(1, -2),
    Vector2(-1, -2), Vector2(-2, -1), Vector2(-2, 1), Vector2(-1, 2)
])
var moves = GridOps.neighbors_custom(pos, width, height, knight_offsets)
```

## Flood Fill

### flood_fill

```gdscript
static func flood_fill(grid: PackedInt32Array, width: int, start_index: int, target_value: int) -> PackedInt32Array
```

Get all cells connected to start that have the target value.

```gdscript
# Find all connected floor tiles
var floor_tiles = GridOps.flood_fill(tilemap, width, player_pos, FLOOR_TILE)
```

### flood_fill_walkable

```gdscript
static func flood_fill_walkable(grid: PackedInt32Array, width: int, start_index: int, walkable_value: int) -> PackedInt32Array
```

Same as flood_fill, named for clarity when finding walkable regions.

### flood_fill_limited

```gdscript
static func flood_fill_limited(grid: PackedInt32Array, width: int, start_index: int, target_value: int, max_distance: int) -> PackedInt32Array
```

Flood fill with maximum Manhattan distance limit.

```gdscript
# Find floor tiles within 5 steps
var nearby_floor = GridOps.flood_fill_limited(tilemap, width, start, FLOOR, 5)
```

## Line of Sight

### line_indices

```gdscript
static func line_indices(from: Vector2i, to: Vector2i, width: int) -> PackedInt32Array
```

Get all grid cells along a line using Bresenham's algorithm.

```gdscript
var line = GridOps.line_indices(Vector2i(0, 0), Vector2i(10, 5), grid_width)
```

### line_clear

```gdscript
static func line_clear(grid: PackedInt32Array, width: int, from: Vector2i, to: Vector2i, blocking_value: int) -> bool
```

Check if a line between two points is unobstructed.

```gdscript
var can_see = GridOps.line_clear(tilemap, width, player_pos, enemy_pos, WALL)
```

### raycast_grid

```gdscript
static func raycast_grid(grid: PackedInt32Array, width: int, from: Vector2i, direction: Vector2i, max_distance: int, blocking_value: int) -> PackedInt32Array
```

Cast a ray from a point in a direction, returning cells until blocked.

```gdscript
var ray = GridOps.raycast_grid(tilemap, width, player_pos, Vector2i(1, 0), 20, WALL)
```

### line_first_blocking

```gdscript
static func line_first_blocking(grid: PackedInt32Array, width: int, from: Vector2i, to: Vector2i, blocking_value: int) -> int
```

Get the index of the first blocking cell along a line, or -1 if none.

## Field of View

### fov_shadowcast

```gdscript
static func fov_shadowcast(grid: PackedInt32Array, width: int, height: int, origin: Vector2i, radius: int, blocking_value: int) -> PackedInt32Array
```

Calculate visible cells using recursive shadowcasting. Fast and accurate.

```gdscript
var visible = GridOps.fov_shadowcast(tilemap, width, height, player_pos, 10, WALL)
for i in visible:
    # Mark cell as visible
    fog_of_war[i] = REVEALED
```

### fov_raycast

```gdscript
static func fov_raycast(grid: PackedInt32Array, width: int, height: int, origin: Vector2i, radius: int, blocking_value: int, ray_count: int) -> PackedInt32Array
```

Calculate visible cells using ray tracing. Simpler but slower than shadowcast.

```gdscript
var visible = GridOps.fov_raycast(tilemap, width, height, player_pos, 10, WALL, 360)
```

## Distance Transforms

### manhattan_distance_field

```gdscript
static func manhattan_distance_field(grid: PackedInt32Array, width: int, height: int, target_value: int) -> PackedInt32Array
```

Calculate Manhattan distance from every cell to the nearest target cell.

```gdscript
# Distance to nearest exit
var dist_to_exit = GridOps.manhattan_distance_field(tilemap, width, height, EXIT_TILE)
```

### chebyshev_distance_field

```gdscript
static func chebyshev_distance_field(grid: PackedInt32Array, width: int, height: int, target_value: int) -> PackedInt32Array
```

Calculate Chebyshev (chessboard) distance to nearest target.

### euclidean_distance_field

```gdscript
static func euclidean_distance_field(grid: PackedInt32Array, width: int, height: int, target_value: int) -> PackedFloat32Array
```

Calculate approximate Euclidean distance to nearest target.

## Connected Components

### label_connected_components

```gdscript
static func label_connected_components(grid: PackedInt32Array, width: int, height: int, target_value: int) -> PackedInt32Array
```

Label all connected components. Each component gets a unique label (1, 2, 3, ...).

```gdscript
var labels = GridOps.label_connected_components(tilemap, width, height, FLOOR)
# labels[i] = component number for cell i (0 = not a floor tile)
```

### count_connected_components

```gdscript
static func count_connected_components(grid: PackedInt32Array, width: int, height: int, target_value: int) -> int
```

Count the number of connected components.

```gdscript
var room_count = GridOps.count_connected_components(tilemap, width, height, FLOOR)
```

### component_sizes

```gdscript
static func component_sizes(grid: PackedInt32Array, width: int, height: int, target_value: int) -> PackedInt32Array
```

Get the size (cell count) of each connected component.

```gdscript
var sizes = GridOps.component_sizes(tilemap, width, height, FLOOR)
# sizes[0] = size of component 1, sizes[1] = size of component 2, etc.
```

## Grid Utilities

### find_value / find_not_value

```gdscript
static func find_value(grid: PackedInt32Array, value: int) -> PackedInt32Array
static func find_not_value(grid: PackedInt32Array, value: int) -> PackedInt32Array
```

Find all cells with or without a specific value.

```gdscript
var walls = GridOps.find_value(tilemap, WALL)
var walkable = GridOps.find_not_value(tilemap, WALL)
```

### count_value

```gdscript
static func count_value(grid: PackedInt32Array, value: int) -> int
```

Count cells with a specific value.

### get_region / set_region

```gdscript
static func get_region(grid: PackedInt32Array, grid_width: int, x: int, y: int, region_width: int, region_height: int) -> PackedInt32Array
static func set_region(grid: PackedInt32Array, grid_width: int, x: int, y: int, region_width: int, region_height: int, values: PackedInt32Array) -> PackedInt32Array
```

Get or set a rectangular region within the grid.

## Examples

### Roguelike FOV System

```gdscript
var tilemap: PackedInt32Array
var width: int = 64
var height: int = 64
var visibility: PackedInt32Array  # 0=unseen, 1=seen, 2=visible
var player_pos: Vector2i

func update_fov():
    # Clear previous visibility
    for i in range(visibility.size()):
        if visibility[i] == 2:
            visibility[i] = 1  # Seen but not currently visible

    # Calculate new FOV
    var visible = GridOps.fov_shadowcast(tilemap, width, height, player_pos, 8, WALL)
    for i in visible:
        visibility[i] = 2  # Currently visible
```

### Room Detection

```gdscript
func find_rooms(tilemap: PackedInt32Array, width: int, height: int) -> Array:
    var labels = GridOps.label_connected_components(tilemap, width, height, FLOOR)
    var sizes = GridOps.component_sizes(tilemap, width, height, FLOOR)

    var rooms = []
    for component in range(1, sizes.size() + 1):
        var room_cells = []
        for i in range(labels.size()):
            if labels[i] == component:
                room_cells.append(i)
        rooms.append(room_cells)
    return rooms
```

### Heatmap for AI

```gdscript
# Create a danger heatmap around enemies
func create_danger_map(enemy_positions: PackedVector2Array) -> PackedFloat32Array:
    var danger = PackedFloat32Array()
    danger.resize(width * height)
    danger.fill(0.0)

    for enemy_pos in enemy_positions:
        var idx = GridOps.to_index(int(enemy_pos.x), int(enemy_pos.y), width)
        var threat_cells = GridOps.flood_fill_limited(tilemap, width, idx, FLOOR, 5)

        for cell in threat_cells:
            var dist = GridOps.to_coords(cell, width).distance_to(enemy_pos)
            danger[cell] += 1.0 / (1.0 + dist)  # Inverse distance weight

    return danger
```

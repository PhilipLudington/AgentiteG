# PathfindingOps

High-performance pathfinding algorithms for grid-based games.

## Overview

PathfindingOps provides optimized implementations of common pathfinding algorithms:
- A* pathfinding with customizable heuristics
- Dijkstra's algorithm for multi-goal pathfinding
- Dijkstra maps for AI decision-making
- Flow fields for steering many units
- Jump Point Search (JPS) for uniform-cost grids
- Path utilities (smoothing, simplification)

All grids use row-major order and cost arrays where positive values are traversable.

## Cost Arrays

Many methods accept a `costs` array:
- **Positive values**: Movement cost (higher = harder to traverse)
- **Zero or negative**: Blocked/impassable

For uniform grids, use `walkable` arrays (non-zero = walkable).

## Quick Reference

| Method | Description |
|--------|-------------|
| `astar_grid` | A* pathfinding with variable costs |
| `astar_uniform` | A* for uniform-cost grids |
| `dijkstra_grid` | Path to nearest of multiple goals |
| `dijkstra_map` | Distance field from goals |
| `flow_field` | Direction vectors toward goal |
| `jps_grid` | Fast JPS for uniform grids |
| `smooth_path` | Smooth a path |
| `path_to_vectors` | Convert indices to world positions |

## A* Pathfinding

### astar_grid

```gdscript
static func astar_grid(costs: PackedFloat32Array, width: int, height: int, start: Vector2i, goal: Vector2i, allow_diagonal: bool) -> PackedInt32Array
```

Find shortest path using A* with variable terrain costs.

```gdscript
# costs array: 1.0 = normal, 2.0 = rough terrain, 0.0 = wall
var path = PathfindingOps.astar_grid(costs, width, height, start, goal, true)
if path.size() > 0:
    for idx in path:
        var pos = GridOps.to_coords(idx, width)
        # Move through path...
```

**Parameters:**
- `costs`: Movement cost per cell (positive = passable, <= 0 = blocked)
- `width`, `height`: Grid dimensions
- `start`, `goal`: Start and goal positions
- `allow_diagonal`: Allow 8-directional movement

**Returns:** Path as array of indices from start to goal, or empty if no path.

### astar_grid_weighted

```gdscript
static func astar_grid_weighted(costs: PackedFloat32Array, width: int, height: int, start: Vector2i, goal: Vector2i, allow_diagonal: bool, heuristic_weight: float) -> PackedInt32Array
```

A* with weighted heuristic for faster (but potentially suboptimal) paths.

```gdscript
# heuristic_weight > 1 = faster but less optimal
var path = PathfindingOps.astar_grid_weighted(costs, width, height, start, goal, true, 1.5)
```

### astar_uniform

```gdscript
static func astar_uniform(walkable: PackedInt32Array, width: int, height: int, start: Vector2i, goal: Vector2i, allow_diagonal: bool) -> PackedInt32Array
```

A* for uniform-cost grids (all walkable cells cost 1).

```gdscript
# walkable array: 0 = blocked, non-zero = walkable
var path = PathfindingOps.astar_uniform(walkable, width, height, start, goal, true)
```

## Dijkstra's Algorithm

### dijkstra_grid

```gdscript
static func dijkstra_grid(costs: PackedFloat32Array, width: int, height: int, start: Vector2i, goals: PackedVector2Array) -> PackedInt32Array
```

Find path from start to the nearest of multiple goals.

```gdscript
var exits = PackedVector2Array([Vector2(10, 5), Vector2(50, 30), Vector2(25, 60)])
var path = PathfindingOps.dijkstra_grid(costs, width, height, player_pos, exits)
# Returns path to whichever exit is closest
```

### dijkstra_map

```gdscript
static func dijkstra_map(costs: PackedFloat32Array, width: int, height: int, goals: PackedVector2Array) -> PackedFloat32Array
```

Create a distance map from every cell to the nearest goal. Essential for AI.

```gdscript
var player_positions = PackedVector2Array([player.position])
var dijkstra = PathfindingOps.dijkstra_map(costs, width, height, player_positions)

# Now any enemy can find distance to player:
var enemy_idx = GridOps.to_index(int(enemy.x), int(enemy.y), width)
var distance_to_player = dijkstra[enemy_idx]
```

**Returns:** Array of distances. `INF` for unreachable cells.

### dijkstra_map_single

```gdscript
static func dijkstra_map_single(costs: PackedFloat32Array, width: int, height: int, goal: Vector2i) -> PackedFloat32Array
```

Convenience method for single-goal Dijkstra map.

## Flow Fields

Flow fields provide a direction vector at each cell pointing toward the goal. Perfect for steering many units efficiently.

### flow_field

```gdscript
static func flow_field(costs: PackedFloat32Array, width: int, height: int, goal: Vector2i) -> PackedVector2Array
```

Create flow field toward a single goal.

```gdscript
var flow = PathfindingOps.flow_field(costs, width, height, goal)

# Each unit looks up its direction:
func _process(delta):
    var idx = GridOps.to_index(int(position.x / cell_size), int(position.y / cell_size), width)
    var direction = flow[idx]
    velocity = direction * speed
```

### flow_field_multi

```gdscript
static func flow_field_multi(costs: PackedFloat32Array, width: int, height: int, goals: PackedVector2Array) -> PackedVector2Array
```

Create flow field toward multiple goals (flows to nearest).

```gdscript
var resource_locations = PackedVector2Array([...])
var flow = PathfindingOps.flow_field_multi(costs, width, height, resource_locations)
# Units will flow to nearest resource
```

### flow_field_from_dijkstra

```gdscript
static func flow_field_from_dijkstra(dijkstra_map: PackedFloat32Array, width: int, height: int) -> PackedVector2Array
```

Convert a pre-computed Dijkstra map to a flow field. Useful when you need both.

```gdscript
var dijkstra = PathfindingOps.dijkstra_map_single(costs, width, height, goal)
var flow = PathfindingOps.flow_field_from_dijkstra(dijkstra, width, height)
```

## Jump Point Search

JPS is much faster than A* for uniform-cost grids (where all walkable cells have the same cost).

### jps_grid

```gdscript
static func jps_grid(walkable: PackedInt32Array, width: int, height: int, start: Vector2i, goal: Vector2i) -> PackedInt32Array
```

Find path using Jump Point Search. Only works with 8-directional movement.

```gdscript
var path = PathfindingOps.jps_grid(walkable, width, height, start, goal)
```

**Note:** JPS paths may "jump" over intermediate cells. Use `simplify_path` if you need all cells.

## Path Utilities

### smooth_path

```gdscript
static func smooth_path(path: PackedVector2Array, iterations: int) -> PackedVector2Array
```

Smooth a path by averaging point positions.

```gdscript
var world_path = PathfindingOps.path_to_vectors(path, width, cell_size)
var smooth = PathfindingOps.smooth_path(world_path, 3)
```

### funnel_smooth

```gdscript
static func funnel_smooth(path: PackedVector2Array, grid: PackedInt32Array, width: int, blocking_value: int) -> PackedVector2Array
```

Smooth path using string-pulling with line-of-sight checks.

```gdscript
var smooth = PathfindingOps.funnel_smooth(world_path, tilemap, width, WALL)
# Result has fewer waypoints, following straight lines where possible
```

### path_to_vectors

```gdscript
static func path_to_vectors(path: PackedInt32Array, width: int, cell_size: float) -> PackedVector2Array
```

Convert index path to world-space positions (cell centers).

```gdscript
var world_path = PathfindingOps.path_to_vectors(path, width, 32.0)
```

### simplify_path

```gdscript
static func simplify_path(path: PackedInt32Array, width: int) -> PackedInt32Array
```

Remove collinear points from path (keep only direction changes).

```gdscript
var simplified = PathfindingOps.simplify_path(path, width)
```

## Batch Pathfinding

### astar_batch

```gdscript
static func astar_batch(costs: PackedFloat32Array, width: int, height: int, starts: PackedVector2Array, goals: PackedVector2Array, allow_diagonal: bool) -> Array
```

Find paths for multiple start/goal pairs.

```gdscript
var starts = PackedVector2Array([unit1.pos, unit2.pos, unit3.pos])
var goals = PackedVector2Array([target1, target2, target3])
var paths = PathfindingOps.astar_batch(costs, width, height, starts, goals, true)
# paths[0] = path for unit1, paths[1] = path for unit2, etc.
```

## Reachability

### reachable_cells

```gdscript
static func reachable_cells(costs: PackedFloat32Array, width: int, height: int, start: Vector2i, max_cost: float) -> PackedInt32Array
```

Get all cells reachable from start within a movement budget.

```gdscript
# Show movement range in tactics game
var range = PathfindingOps.reachable_cells(costs, width, height, unit_pos, unit.move_points)
for idx in range:
    highlight_cell(idx)
```

### is_reachable

```gdscript
static func is_reachable(costs: PackedFloat32Array, width: int, height: int, start: Vector2i, goal: Vector2i) -> bool
```

Check if goal is reachable from start.

```gdscript
if PathfindingOps.is_reachable(costs, width, height, start, goal):
    # Goal is accessible
```

### path_cost

```gdscript
static func path_cost(costs: PackedFloat32Array, width: int, height: int, start: Vector2i, goal: Vector2i, allow_diagonal: bool) -> float
```

Get the total cost to travel from start to goal. Returns `INF` if unreachable.

```gdscript
var cost = PathfindingOps.path_cost(costs, width, height, start, goal, true)
if cost < unit.stamina:
    # Unit can make the journey
```

## Examples

### Basic Unit Movement

```gdscript
var grid_width = 64
var grid_height = 64
var cell_size = 32.0
var costs: PackedFloat32Array

func move_unit_to(unit: Node2D, target: Vector2):
    var start = Vector2i(int(unit.position.x / cell_size), int(unit.position.y / cell_size))
    var goal = Vector2i(int(target.x / cell_size), int(target.y / cell_size))

    var path = PathfindingOps.astar_grid(costs, grid_width, grid_height, start, goal, true)
    if path.size() > 0:
        var world_path = PathfindingOps.path_to_vectors(path, grid_width, cell_size)
        unit.follow_path(world_path)
```

### RTS Flow Field Movement

```gdscript
var flow_field: PackedVector2Array
var rally_point: Vector2i

func update_rally_point(new_point: Vector2i):
    rally_point = new_point
    flow_field = PathfindingOps.flow_field(costs, width, height, rally_point)

func _process(delta):
    for unit in units:
        var idx = GridOps.to_index(int(unit.x / cell_size), int(unit.y / cell_size), width)
        if flow_field[idx].length() > 0:
            unit.velocity = flow_field[idx] * unit.speed
```

### Tactics Game Movement Range

```gdscript
func show_movement_range(unit: Unit):
    var reachable = PathfindingOps.reachable_cells(costs, width, height, unit.grid_pos, unit.movement)

    for idx in reachable:
        var pos = GridOps.to_coords(idx, width)
        var cost = PathfindingOps.path_cost(costs, width, height, unit.grid_pos, pos, false)
        if cost <= unit.movement:
            highlight_cell(idx, Color.BLUE)
```

### AI Decision Making with Dijkstra Maps

```gdscript
var player_dijkstra: PackedFloat32Array
var exit_dijkstra: PackedFloat32Array

func update_dijkstra_maps():
    player_dijkstra = PathfindingOps.dijkstra_map_single(costs, width, height, player.grid_pos)
    exit_dijkstra = PathfindingOps.dijkstra_map(costs, width, height, exit_positions)

func ai_decide(enemy: Enemy):
    var idx = GridOps.to_index(enemy.x, enemy.y, width)
    var dist_to_player = player_dijkstra[idx]
    var dist_to_exit = exit_dijkstra[idx]

    if enemy.health < 20 and dist_to_exit < dist_to_player:
        # Flee to exit
        return "flee"
    elif dist_to_player < enemy.attack_range:
        return "attack"
    else:
        return "chase"
```

## Performance Tips

1. **Reuse Dijkstra maps**: Calculate once per frame, use for all enemies
2. **Use flow fields** for many units with the same destination
3. **Use JPS** for uniform-cost grids (10-30x faster than A*)
4. **Use `path_cost`** before `astar_grid` if you just need cost
5. **Simplify paths** to reduce memory and smoothing time
6. **Cache flow fields** when goals don't change frequently

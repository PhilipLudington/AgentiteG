# QuadTree

Adaptive spatial subdivision for 2D positions. Automatically subdivides dense regions.

## When to Use

- Clustered entity distributions (cities, bases, unit groups)
- RTS games with drag-selection
- Large sparse worlds with localized activity
- When you want to visualize/debug spatial partitioning
- Broad-phase collision detection

## Comparison with Other Structures

| Feature | QuadTree | SpatialHash2D | KDTree2D |
|---------|----------|---------------|----------|
| Clustered data | Excellent | Poor | Good |
| Uniform data | Fair | Excellent | Good |
| Nearest neighbor | Fair | Good | Excellent |
| Debug visualization | Excellent | Poor | Poor |
| Dynamic updates | Fair | Excellent | Poor |

**Rule of thumb**: Use QuadTree when entities cluster (RTS bases, city areas). Use SpatialHash2D for uniform distributions.

## Properties

| Property | Type | Default | Description |
|----------|------|---------|-------------|
| `max_depth` | int | 8 | Maximum subdivision depth |
| `max_items_per_node` | int | 8 | Items before node subdivides |

## Methods

### Configuration

#### `set_bounds(bounds: Rect2) -> void`
Set the world bounds. **Must be called before build()**.

```gdscript
var quadtree = QuadTree.new()
quadtree.set_bounds(Rect2(0, 0, 1920, 1080))
```

### Building

#### `build(positions: PackedVector2Array) -> void`
Build the quadtree from positions.

```gdscript
var unit_positions = PackedVector2Array([...])
quadtree.build(unit_positions)
```

#### `size() -> int`
Returns the number of items in the tree.

### Queries

#### `query_rect(rect: Rect2) -> PackedInt32Array`
Find all items within a rectangle. Perfect for drag-selection.

```gdscript
# Drag-select units
var selection_box = Rect2(start_drag, end_drag - start_drag)
var selected = quadtree.query_rect(selection_box)
for idx in selected:
    units[idx].select()
```

#### `query_radius(center: Vector2, radius: float) -> PackedInt32Array`
Find all items within a circle.

```gdscript
# Ability area of effect
var affected = quadtree.query_radius(spell.position, spell.radius)
for idx in affected:
    units[idx].apply_effect(spell.effect)
```

### Debug Visualization

#### `get_node_bounds() -> Array[Rect2]`
Get all node rectangles for debug drawing.

```gdscript
func _draw():
    for rect in quadtree.get_node_bounds():
        draw_rect(rect, Color.GREEN, false)
```

## Configuration Tips

- **max_depth**: Higher = more precision but more memory. 8-10 is usually good.
- **max_items_per_node**: Lower = more subdivision. 4-16 is typical.
  - Lower values: More granular tree, better for very clustered data
  - Higher values: Flatter tree, faster build time

## Example: RTS Unit Selection

```gdscript
var quadtree: QuadTree
var unit_positions: PackedVector2Array
var units: Array[Unit]

func _ready():
    quadtree = QuadTree.new()
    quadtree.set_bounds(Rect2(0, 0, map_width, map_height))
    quadtree.max_items_per_node = 8

func _physics_process(delta):
    # Rebuild when units move
    unit_positions.clear()
    for unit in units:
        unit_positions.append(unit.position)
    quadtree.build(unit_positions)

func on_drag_select(start: Vector2, end: Vector2):
    var rect = Rect2(start, end - start).abs()
    var selected_indices = quadtree.query_rect(rect)

    for unit in units:
        unit.deselect()
    for idx in selected_indices:
        units[idx].select()

func get_units_in_range(center: Vector2, range: float) -> Array[Unit]:
    var result: Array[Unit] = []
    for idx in quadtree.query_radius(center, range):
        result.append(units[idx])
    return result

func _draw():
    # Debug: show quadtree structure
    if show_debug:
        for rect in quadtree.get_node_bounds():
            draw_rect(rect, Color(0, 1, 0, 0.3), false)
```

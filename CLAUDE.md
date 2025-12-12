# Claude Code Guide for AgentiteG

This file helps Claude Code (and other AI assistants) understand and use AgentiteG effectively.

## What is AgentiteG?

AgentiteG is a native C++ GDExtension for Godot that provides high-performance operations for common game development hot paths. Use it when GDScript loops become a bottleneck.

## When to Use AgentiteG

**Use AgentiteG when:**
- Iterating over 100+ entities per frame
- Performing spatial queries (find nearby, in radius, in rect)
- Sorting or filtering large arrays
- Per-frame distance calculations
- Batch operations on many objects

**Don't use AgentiteG for:**
- Small arrays (<50 elements)
- One-time operations (level load, etc.)
- Logic that requires game-specific knowledge

## Quick Reference

### SpatialHash2D

Fast O(1) spatial lookups for 2D positions.

```gdscript
# Create and configure
var spatial = SpatialHash2D.new()
spatial.cell_size = 64.0  # Tune based on query radius

# Build from positions (call when entities move significantly)
spatial.build(entity_positions)  # PackedVector2Array

# Queries - all return PackedInt32Array of indices
var nearby = spatial.query_radius(origin, radius)
var in_area = spatial.query_rect(Rect2(x, y, w, h))
var closest = spatial.query_nearest(origin, k)  # k nearest
var closest_one = spatial.query_nearest_one(origin)  # single nearest

# Fast checks (no allocation)
var has_enemy = spatial.has_any_in_radius(pos, detection_range)
var enemy_count = spatial.count_in_radius(pos, detection_range)

# Batch queries
var all_nearby = spatial.query_radius_batch_uniform(unit_positions, attack_range)
```

**Cell size tuning:** Set `cell_size` to roughly 1-2x your typical query radius for best performance.

### ArrayOps

Static methods for fast array operations. All return new arrays (non-mutating).

```gdscript
# FILTER - Returns indices where condition is true

# Float comparisons
var high_hp = ArrayOps.filter_gt_float(health_values, 50.0)
var low_hp = ArrayOps.filter_lt_float(health_values, 20.0)
var in_range = ArrayOps.filter_range_float(distances, 10.0, 100.0)

# Int comparisons
var team_a = ArrayOps.filter_eq_int(team_ids, 1)
var high_level = ArrayOps.filter_gte_int(levels, 10)

# Spatial filters
var on_screen = ArrayOps.filter_in_rect(positions, screen_rect)
var in_blast = ArrayOps.filter_in_radius(positions, explosion_pos, blast_radius)


# SORT - Returns sorted array or indices

var sorted = ArrayOps.sort_floats(values)
var sorted_desc = ArrayOps.sort_floats(values, true)  # descending

# Argsort: get indices that would sort the array
var order = ArrayOps.argsort_floats(priorities)  # [2, 0, 1] means index 2 is lowest

# Sort by distance
var closest_first = ArrayOps.argsort_by_distance(positions, player_pos)
var farthest_first = ArrayOps.argsort_by_distance(positions, player_pos, true)


# REDUCE - Aggregation operations

var total = ArrayOps.sum_floats(damage_values)
var total_force = ArrayOps.sum_vector2(force_vectors)

var lowest = ArrayOps.min_float(prices)
var highest = ArrayOps.max_float(scores)

var weakest_idx = ArrayOps.argmin_floats(health_values)
var strongest_idx = ArrayOps.argmax_floats(health_values)

var avg = ArrayOps.mean_floats(values)
var centroid = ArrayOps.mean_vector2(positions)


# SELECT - Pick elements by indices

var selected = ArrayOps.select_floats(all_values, indices)
var selected_pos = ArrayOps.select_vector2(all_positions, indices)


# REORDER - Apply sort order to array

var sorted_names = ArrayOps.reorder_ints(entity_ids, sort_order)


# FIND - First match

var first_high = ArrayOps.find_first_gt_float(values, threshold)  # -1 if none
var first_match = ArrayOps.find_first_eq_int(ids, target_id)


# COUNT - Fast counting

var num_high = ArrayOps.count_gt_float(values, threshold)
var num_enemies = ArrayOps.count_in_radius(positions, player_pos, detection_range)
```

## Common Patterns

### Enemy Detection System
```gdscript
var spatial: SpatialHash2D
var enemy_positions: PackedVector2Array
var enemies: Array[Enemy]

func _ready():
    spatial = SpatialHash2D.new()
    spatial.cell_size = 100.0  # ~2x typical detection range

func _physics_process(delta):
    # Rebuild when enemies move (or use update() for individual changes)
    spatial.build(enemy_positions)

    # Each unit checks for nearby enemies
    for unit in friendly_units:
        var nearby_indices = spatial.query_radius(unit.position, unit.detection_range)
        for i in nearby_indices:
            unit.on_enemy_detected(enemies[i])
```

### Target Prioritization
```gdscript
func find_best_target(attacker_pos: Vector2, targets: PackedVector2Array, healths: PackedFloat32Array) -> int:
    # Get targets in range, sorted by distance
    var in_range = ArrayOps.filter_in_radius(targets, attacker_pos, attack_range)
    if in_range.is_empty():
        return -1

    # Get health values of in-range targets
    var in_range_health = ArrayOps.select_floats(healths, in_range)

    # Target lowest health
    var lowest_health_local = ArrayOps.argmin_floats(in_range_health)
    return in_range[lowest_health_local]
```

### Projectile Culling
```gdscript
func cull_offscreen_projectiles():
    var alive_indices = ArrayOps.filter_in_rect(projectile_positions, visible_rect)

    # Remove dead projectiles (iterate in reverse to preserve indices)
    for i in range(projectile_positions.size() - 1, -1, -1):
        if i not in alive_indices:
            remove_projectile(i)
```

### Batch Distance Sorting
```gdscript
func get_units_by_distance(from: Vector2) -> Array[Unit]:
    var order = ArrayOps.argsort_by_distance(unit_positions, from)
    var sorted_units: Array[Unit] = []
    for i in order:
        sorted_units.append(units[i])
    return sorted_units
```

## Performance Expectations

| Operation | ~Items | GDScript | AgentiteG | Speedup |
|-----------|--------|----------|-----------|---------|
| Radius query | 1,000 | ~2ms | ~0.1ms | 20x |
| Radius query | 10,000 | ~20ms | ~0.15ms | 130x |
| Filter floats | 10,000 | ~1.5ms | ~0.08ms | 18x |
| Sort floats | 10,000 | ~5ms | ~0.3ms | 15x |
| Argsort | 10,000 | ~15ms | ~0.5ms | 30x |
| Sum | 10,000 | ~0.5ms | ~0.02ms | 25x |

## Tips

1. **Return indices, not objects.** AgentiteG returns `PackedInt32Array` of indices. Use these to look up your actual game objects.

2. **Rebuild spatial hash strategically.** Call `build()` once per frame if many entities move, or use `update()` for individual changes.

3. **Chain operations efficiently:**
   ```gdscript
   # Good: filter then select
   var nearby = spatial.query_radius(pos, range)
   var nearby_health = ArrayOps.select_floats(all_health, nearby)
   var weakest_local = ArrayOps.argmin_floats(nearby_health)
   var weakest = nearby[weakest_local]
   ```

4. **Use count/has for boolean checks:**
   ```gdscript
   # Good: no allocation
   if spatial.has_any_in_radius(pos, alert_range):
       trigger_alert()

   # Less efficient: allocates array
   if spatial.query_radius(pos, alert_range).size() > 0:
       trigger_alert()
   ```

5. **Tune cell_size for your use case.** Too small = many cells to check. Too large = many items per cell. Start with 1-2x your typical query radius.

# ArrayOps

Static methods for fast array filtering, sorting, and reduction operations.

## When to Use

- Filtering large arrays by condition
- Sorting entities by distance, priority, etc.
- Aggregating values (sum, min, max, mean)
- Selecting subsets of data by indices

## Methods

All methods are static - call directly on the class: `ArrayOps.method_name(...)`

### Filter Operations

All filter methods return `PackedInt32Array` of indices where the condition is true.

#### Float Comparisons

```gdscript
# Greater than
var high_hp = ArrayOps.filter_gt_float(health_values, 50.0)

# Greater than or equal
var alive = ArrayOps.filter_gte_float(health_values, 0.0)

# Less than
var low_hp = ArrayOps.filter_lt_float(health_values, 20.0)

# Less than or equal
var depleted = ArrayOps.filter_lte_float(energy_values, 0.0)

# Equal (with epsilon tolerance, default 0.0001)
var exact = ArrayOps.filter_eq_float(values, 1.0)
var exact = ArrayOps.filter_eq_float(values, 1.0, 0.01)  # custom epsilon

# Range (inclusive)
var mid_range = ArrayOps.filter_range_float(values, 25.0, 75.0)
```

#### Int Comparisons

```gdscript
var team_a = ArrayOps.filter_eq_int(team_ids, 1)
var high_level = ArrayOps.filter_gte_int(levels, 10)
var low_tier = ArrayOps.filter_lt_int(tiers, 3)
var in_range = ArrayOps.filter_range_int(scores, 100, 200)
```

#### Spatial Filters

```gdscript
# Points inside rectangle
var on_screen = ArrayOps.filter_in_rect(positions, viewport_rect)

# Points inside circle
var in_blast = ArrayOps.filter_in_radius(positions, explosion_pos, blast_radius)
```

### Sort Operations

#### Sort (Returns Sorted Copy)

```gdscript
var sorted = ArrayOps.sort_floats(values)           # ascending
var sorted = ArrayOps.sort_floats(values, true)     # descending

var sorted = ArrayOps.sort_ints(values)
var sorted = ArrayOps.sort_ints(values, true)
```

#### Argsort (Returns Indices That Would Sort)

```gdscript
# Get sort order without modifying original
var order = ArrayOps.argsort_floats(priorities)
# order[0] is index of smallest, order[1] is index of second smallest, etc.

var order = ArrayOps.argsort_floats(priorities, true)  # descending

var order = ArrayOps.argsort_ints(values)
```

#### Sort by Distance

```gdscript
# Indices sorted by distance from origin
var closest_first = ArrayOps.argsort_by_distance(positions, player_pos)
var farthest_first = ArrayOps.argsort_by_distance(positions, player_pos, true)

# 3D version
var closest_first = ArrayOps.argsort_by_distance_3d(positions_3d, origin_3d)
```

#### Reorder (Apply Sort Order)

```gdscript
var order = ArrayOps.argsort_floats(priorities)

# Reorder other arrays to match
var sorted_floats = ArrayOps.reorder_floats(values, order)
var sorted_ints = ArrayOps.reorder_ints(ids, order)
var sorted_pos = ArrayOps.reorder_vector2(positions, order)
var sorted_pos3d = ArrayOps.reorder_vector3(positions_3d, order)
```

### Reduce Operations

#### Sum

```gdscript
var total = ArrayOps.sum_floats(damage_values)        # float
var total = ArrayOps.sum_ints(counts)                 # int64
var total = ArrayOps.sum_vector2(force_vectors)       # Vector2
var total = ArrayOps.sum_vector3(velocities)          # Vector3
```

#### Min / Max

```gdscript
var lowest = ArrayOps.min_float(prices)
var highest = ArrayOps.max_float(scores)
var lowest = ArrayOps.min_int(levels)
var highest = ArrayOps.max_int(levels)
```

#### Argmin / Argmax (Index of Min/Max)

```gdscript
var weakest_idx = ArrayOps.argmin_floats(health_values)
var strongest_idx = ArrayOps.argmax_floats(health_values)
var lowest_idx = ArrayOps.argmin_ints(priorities)
var highest_idx = ArrayOps.argmax_ints(priorities)
```

#### Mean

```gdscript
var avg = ArrayOps.mean_floats(values)
var centroid = ArrayOps.mean_vector2(positions)
var center = ArrayOps.mean_vector3(positions_3d)
```

### Select Operations

Pick elements by indices (like numpy indexing).

```gdscript
var indices = PackedInt32Array([0, 3, 7])

var selected = ArrayOps.select_floats(all_values, indices)
var selected = ArrayOps.select_ints(all_ids, indices)
var selected = ArrayOps.select_vector2(all_positions, indices)
var selected = ArrayOps.select_vector3(all_positions_3d, indices)
```

### Find Operations

#### Find First Match

Returns index of first match, or -1 if not found.

```gdscript
var first_high = ArrayOps.find_first_gt_float(values, threshold)
var first_match = ArrayOps.find_first_eq_int(ids, target_id)
```

### Count Operations

```gdscript
var num_high = ArrayOps.count_gt_float(values, threshold)
var num_matches = ArrayOps.count_eq_int(ids, target_id)
var num_nearby = ArrayOps.count_in_radius(positions, origin, radius)
```

## Common Patterns

### Target Prioritization

```gdscript
func find_best_target(attacker_pos: Vector2, targets: PackedVector2Array,
                      healths: PackedFloat32Array) -> int:
    # Get targets in range
    var in_range = ArrayOps.filter_in_radius(targets, attacker_pos, attack_range)
    if in_range.is_empty():
        return -1

    # Get health of in-range targets
    var in_range_health = ArrayOps.select_floats(healths, in_range)

    # Find lowest health among in-range
    var local_idx = ArrayOps.argmin_floats(in_range_health)
    return in_range[local_idx]  # Convert back to global index
```

### Sorted Entity List

```gdscript
func get_units_by_priority(positions: PackedVector2Array,
                           priorities: PackedFloat32Array) -> PackedInt32Array:
    # Sort by priority (highest first)
    return ArrayOps.argsort_floats(priorities, true)
```

### Efficient Filtering Chain

```gdscript
# Filter by multiple conditions
var alive = ArrayOps.filter_gt_float(health, 0.0)
var alive_positions = ArrayOps.select_vector2(positions, alive)
var alive_in_range = ArrayOps.filter_in_radius(alive_positions, origin, range)

# Map back to original indices
var result = ArrayOps.select_ints(alive, alive_in_range)
```

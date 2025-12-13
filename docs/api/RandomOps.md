# RandomOps

High-performance random number generation for bulk operations.

## Overview

`RandomOps` provides fast bulk random generation using xoshiro256** PRNG. Create an instance, seed it for determinism, then generate thousands of random values efficiently.

## Usage

```gdscript
var rng = RandomOps.new()
rng.seed(12345)  # Deterministic

var positions = rng.rand_points_in_circle(1000, center, radius)
```

## Methods

### Seeding

| Method | Description |
|--------|-------------|
| `seed(int64_t seed_value)` | Set deterministic seed |
| `seed_from_time()` | Seed from current time (non-deterministic) |

### Float Generation

| Method | Returns |
|--------|---------|
| `randf_array(count)` | `PackedFloat32Array` of `count` values in [0, 1) |
| `randf_range_array(count, min, max)` | `PackedFloat32Array` of `count` values in [min, max) |

### Integer Generation

| Method | Returns |
|--------|---------|
| `randi_array(count)` | `PackedInt32Array` of `count` random 32-bit integers |
| `randi_range_array(count, min, max)` | `PackedInt32Array` of `count` values in [min, max] (inclusive) |

### Points in Shapes

| Method | Returns |
|--------|---------|
| `rand_points_in_rect(count, rect)` | `PackedVector2Array` uniformly in Rect2 |
| `rand_points_in_box(count, box)` | `PackedVector3Array` uniformly in AABB |
| `rand_points_in_circle(count, center, radius)` | `PackedVector2Array` uniformly in circle |
| `rand_points_in_sphere(count, center, radius)` | `PackedVector3Array` uniformly in sphere |
| `rand_points_on_circle(count, center, radius)` | `PackedVector2Array` on circle perimeter |
| `rand_points_on_sphere(count, center, radius)` | `PackedVector3Array` on sphere surface |

### Shuffling

| Method | Returns |
|--------|---------|
| `shuffle_indices(count)` | `PackedInt32Array` of shuffled 0..count-1 |
| `shuffle_floats(values)` | Shuffled copy of float array |
| `shuffle_ints(values)` | Shuffled copy of int array |

### Weighted Selection

| Method | Returns |
|--------|---------|
| `weighted_choice(weights, count)` | `PackedInt32Array` of `count` indices selected by weight |
| `weighted_choice_one(weights)` | Single index selected by weight |

### Poisson Disk Sampling

| Method | Returns |
|--------|---------|
| `poisson_disk_2d(bounds, min_distance, max_attempts=30)` | Blue noise distributed points in Rect2 |
| `poisson_disk_3d(bounds, min_distance, max_attempts=30)` | Blue noise distributed points in AABB |

### Random Directions

| Method | Returns |
|--------|---------|
| `rand_directions_2d(count)` | `PackedVector2Array` of unit vectors |
| `rand_directions_3d(count)` | `PackedVector3Array` of unit vectors |

### Gaussian Distribution

| Method | Returns |
|--------|---------|
| `randn_array(count)` | Normal distribution (mean=0, std=1) |
| `randn_array_params(count, mean, std_dev)` | Normal distribution with custom parameters |

## Examples

### Loot System
```gdscript
var rng = RandomOps.new()
rng.seed(player_seed)

# Loot rarity weights: common=50%, uncommon=30%, rare=15%, legendary=5%
var weights = PackedFloat32Array([0.5, 0.3, 0.15, 0.05])
var drops = rng.weighted_choice(weights, chest_loot_count)

for rarity_index in drops:
    spawn_item(rarity_index)
```

### Spawn Points
```gdscript
# Trees with minimum spacing
var tree_positions = rng.poisson_disk_2d(forest_rect, 10.0)

# Enemies in spawn area
var enemy_spawns = rng.rand_points_in_circle(wave_size, spawn_center, spawn_radius)
```

### Particle Effects
```gdscript
# Random directions for explosion
var dirs = rng.rand_directions_2d(particle_count)
var speeds = rng.randf_range_array(particle_count, min_speed, max_speed)
```

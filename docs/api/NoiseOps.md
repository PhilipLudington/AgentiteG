# NoiseOps

High-performance procedural noise generation.

## Overview

`NoiseOps` provides Perlin, Simplex, and Worley (cellular) noise with fractal variants. Optimized for terrain generation, texture creation, and procedural content.

## Usage

```gdscript
var noise = NoiseOps.new()
noise.set_seed(world_seed)
noise.set_octaves(4)
noise.set_frequency(0.02)

var heightmap = noise.fbm_2d_grid(Vector2.ZERO, Vector2(1, 1), 256, 256)
```

## Configuration

| Property | Default | Description |
|----------|---------|-------------|
| `seed` | 0 | Random seed for determinism |
| `octaves` | 4 | Number of noise layers (1-16) |
| `persistence` | 0.5 | Amplitude multiplier per octave (0-1) |
| `lacunarity` | 2.0 | Frequency multiplier per octave |
| `frequency` | 1.0 | Base frequency scale |

## Noise Types

### Perlin Noise
Classic gradient noise, smooth and continuous. Range: approximately [-1, 1].

| Method | Returns |
|--------|---------|
| `perlin_2d(pos)` | Single float value |
| `perlin_3d(pos)` | Single float value |
| `perlin_2d_batch(positions)` | `PackedFloat32Array` |
| `perlin_3d_batch(positions)` | `PackedFloat32Array` |
| `perlin_2d_grid(origin, cell_size, width, height)` | `PackedFloat32Array` (row-major) |
| `perlin_3d_grid(origin, cell_size, width, height, depth)` | `PackedFloat32Array` |

### Simplex Noise
Faster than Perlin with fewer directional artifacts. Range: approximately [-1, 1].

| Method | Returns |
|--------|---------|
| `simplex_2d(pos)` | Single float value |
| `simplex_3d(pos)` | Single float value |
| `simplex_2d_batch(positions)` | `PackedFloat32Array` |
| `simplex_3d_batch(positions)` | `PackedFloat32Array` |
| `simplex_2d_grid(origin, cell_size, width, height)` | `PackedFloat32Array` |

### Worley (Cellular) Noise
Returns distance to nearest feature point. Good for caves, cells, stone textures. Range: [0, ~1.5].

| Method | Returns |
|--------|---------|
| `worley_2d(pos)` | Distance to nearest point |
| `worley_3d(pos)` | Distance to nearest point |
| `worley_2d_batch(positions)` | `PackedFloat32Array` |
| `worley_3d_batch(positions)` | `PackedFloat32Array` |
| `worley_2d_grid(origin, cell_size, width, height)` | `PackedFloat32Array` |

## Fractal Variants

### FBM (Fractal Brownian Motion)
Sum of multiple octaves. Natural-looking terrain and clouds.

| Method | Returns |
|--------|---------|
| `fbm_2d(pos)` | Single float value |
| `fbm_3d(pos)` | Single float value |
| `fbm_2d_batch(positions)` | `PackedFloat32Array` |
| `fbm_3d_batch(positions)` | `PackedFloat32Array` |
| `fbm_2d_grid(origin, cell_size, width, height)` | `PackedFloat32Array` |

### Ridged Multifractal
Sharp ridges, good for mountains and rivers.

| Method | Returns |
|--------|---------|
| `ridged_2d(pos)` | Single float value |
| `ridged_3d(pos)` | Single float value |
| `ridged_2d_batch(positions)` | `PackedFloat32Array` |
| `ridged_3d_batch(positions)` | `PackedFloat32Array` |
| `ridged_2d_grid(origin, cell_size, width, height)` | `PackedFloat32Array` |

### Turbulence
Sum of absolute values. Good for clouds, smoke, fire.

| Method | Returns |
|--------|---------|
| `turbulence_2d(pos)` | Single float value (always positive) |
| `turbulence_3d(pos)` | Single float value (always positive) |
| `turbulence_2d_batch(positions)` | `PackedFloat32Array` |
| `turbulence_3d_batch(positions)` | `PackedFloat32Array` |

## Domain Warping

Distort coordinates using noise for organic-looking results.

| Method | Returns |
|--------|---------|
| `warp_2d(positions, strength)` | `PackedVector2Array` of warped positions |
| `warp_3d(positions, strength)` | `PackedVector3Array` of warped positions |

## Examples

### Terrain Generation
```gdscript
var noise = NoiseOps.new()
noise.set_seed(world_seed)
noise.set_octaves(6)
noise.set_frequency(0.01)

# Generate heightmap
var heightmap = noise.fbm_2d_grid(chunk_origin, Vector2(1, 1), 64, 64)

# Apply to mesh vertices
for i in range(heightmap.size()):
    vertices[i].y = heightmap[i] * terrain_scale
```

### Cave System
```gdscript
noise.set_frequency(0.05)
var cave_values = noise.worley_3d_batch(sample_positions)

for i in range(cave_values.size()):
    if cave_values[i] > cave_threshold:
        # Solid rock
    else:
        # Air/cave
```

### Layered Terrain
```gdscript
# Base terrain
noise.set_frequency(0.01)
var base = noise.fbm_2d(pos)

# Mountain ridges
noise.set_frequency(0.02)
var mountains = noise.ridged_2d(pos)

# Detail
noise.set_frequency(0.1)
var detail = noise.perlin_2d(pos)

var final_height = base * 0.6 + mountains * 0.3 + detail * 0.1
```

### Organic Shapes with Domain Warping
```gdscript
# Create sample grid
var grid_points = PackedVector2Array()
for y in range(64):
    for x in range(64):
        grid_points.append(Vector2(x, y))

# Warp the grid for organic distortion
var warped = noise.warp_2d(grid_points, 5.0)

# Sample noise at warped positions
var values = noise.fbm_2d_batch(warped)
```

## Tips

1. **Frequency tuning**: Lower frequency = larger features, higher = more detail
2. **Octaves**: More octaves = more detail but slower. 4-6 is usually enough
3. **Persistence**: Lower = smoother, higher = more rough detail
4. **Grid vs Batch**: Use grid functions for heightmaps, batch for arbitrary points
5. **Combine noise types**: Layer FBM + ridged + Worley for complex terrain

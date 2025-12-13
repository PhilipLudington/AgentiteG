# AgentiteG API Reference

High-performance GDExtension primitives for Godot game development.

## Classes

### Spatial Data Structures

| Class | Description |
|-------|-------------|
| [SpatialHash2D](SpatialHash2D.md) | O(1) spatial queries for 2D positions |
| [SpatialHash3D](SpatialHash3D.md) | O(1) spatial queries for 3D positions |

### Array Operations

| Class | Description |
|-------|-------------|
| [ArrayOps](ArrayOps.md) | Filter, sort, and reduce operations on PackedArrays |

### Math Operations

| Class | Description |
|-------|-------------|
| [MathOps](MathOps.md) | Batch vector and matrix operations |

### Game Operations

| Class | Description |
|-------|-------------|
| [BatchOps](BatchOps.md) | Steering behaviors, flocking, and game loop operations |

## Design Principles

1. **Zero game-specific logic** - Pure algorithms only
2. **Arrays in, arrays out** - Work with PackedArrays, return indices
3. **5-10x minimum speedup** - Every feature is benchmarked
4. **Simple API** - Easy signatures, sensible defaults

## Quick Start

```gdscript
# Spatial queries
var spatial = SpatialHash2D.new()
spatial.build(positions)
var nearby = spatial.query_radius(origin, radius)

# Array filtering
var low_health = ArrayOps.filter_lt_float(health, 20.0)

# Batch math
var directions = MathOps.direction_batch_2d(positions, targets)
```

See [CLAUDE.md](../../CLAUDE.md) for usage patterns and troubleshooting.

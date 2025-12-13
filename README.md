# AgentiteG

**Native performance primitives for Godot, designed for AI-assisted development.**

AgentiteG provides high-performance, GDExtension-based operations for common game development hot paths. It's designed to be easy to use with AI coding assistants like Claude Code while delivering native C++ performance.

## Features

### Spatial Structures
- **SpatialHash2D / SpatialHash3D** - O(1) spatial queries using hash grids
- **KDTree2D / KDTree3D** - O(log n) nearest neighbor queries
- **QuadTree / Octree** - Adaptive spatial subdivision

### Array & Math Operations
- **ArrayOps** - Filter, sort, reduce, select on PackedArrays
- **MathOps** - Batch vector/matrix operations, distance matrices
- **BatchOps** - Steering behaviors, flocking, velocity updates

### Procedural Generation
- **RandomOps** - Bulk random generation, Poisson disk sampling, weighted choice
- **NoiseOps** - Perlin, simplex, Worley noise with FBM, ridged, turbulence variants

### Grid & Pathfinding
- **GridOps** - Coordinate conversion, flood fill, FOV shadowcasting, distance fields
- **PathfindingOps** - A*, Dijkstra maps, Jump Point Search, flow fields

### Collision & Geometry
- **CollisionOps** - Batch point-in-shape, circle/sphere collisions, ray casting
- **GeometryOps** - Convex hull, Delaunay triangulation, Voronoi, polygon operations

### Interpolation & Statistics
- **InterpolationOps** - 30+ easing functions, bezier curves, Catmull-Rom splines
- **StatOps** - Mean, median, std dev, percentiles, histograms, outlier detection

## Installation

### From Release

1. Download the latest release for your platform
2. Copy the `addons/agentiteg` folder to your project's `addons/` directory
3. Enable the plugin in Project Settings

### From Source

```bash
# Clone with submodules
git clone --recursive https://github.com/yourusername/AgentiteG.git
cd AgentiteG

# Build
scons platform=<windows|linux|macos> -j4

# Output will be in project/addons/agentiteg/bin/
```

## Quick Start

```gdscript
extends Node2D

var spatial: SpatialHash2D
var enemy_positions: PackedVector2Array

func _ready():
    # Create spatial hash with 64-unit cells
    spatial = SpatialHash2D.new()
    spatial.cell_size = 64.0

func _physics_process(delta):
    # Rebuild spatial hash (or update incrementally)
    spatial.build(enemy_positions)

    # Find all enemies within 200 units of player
    var nearby = spatial.query_radius(player.position, 200.0)

    # nearby is PackedInt32Array of indices into enemy_positions
    for i in nearby:
        var enemy = enemies[i]
        enemy.alert()
```

## Documentation

- [Getting Started](docs/guides/getting-started.md)
- [API Reference](docs/api/README.md)
- [Claude Code Guide](CLAUDE.md)

## Performance

| Operation | Items | GDScript | AgentiteG | Speedup |
|-----------|-------|----------|-----------|---------|
| Radius query | 10,000 | ~20ms | ~0.15ms | 130x |
| K-nearest neighbors | 10,000 | ~25ms | ~0.2ms | 125x |
| Sort floats | 10,000 | ~5ms | ~0.3ms | 15x |
| Filter values | 10,000 | ~1.5ms | ~0.08ms | 18x |
| Batch normalize | 10,000 | ~3ms | ~0.1ms | 30x |
| A* pathfinding | 100x100 | ~15ms | ~0.8ms | 18x |
| Noise grid | 256x256 | ~50ms | ~2ms | 25x |

## Philosophy

1. **Zero game-specific logic** - Pure algorithms, no architecture opinions
2. **Arrays in, arrays out** - Work with PackedArrays, return indices
3. **Claude-friendly API** - Simple signatures, clear documentation
4. **Opt-in complexity** - Simple defaults, advanced options available
5. **Benchmark everything** - Performance claims are tested

## License

MIT License - see [LICENSE](LICENSE)

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

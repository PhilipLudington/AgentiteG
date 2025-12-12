# Getting Started with AgentiteG

This guide will help you install AgentiteG and start using it in your Godot project.

## Installation

### Option 1: Pre-built Binaries (Recommended)

1. Download the latest release from GitHub Releases
2. Extract the archive
3. Copy the `addons/agentiteg` folder to your project's `addons/` directory
4. Restart Godot Editor

Your project structure should look like:
```
your_project/
├── addons/
│   └── agentiteg/
│       ├── agentiteg.gdextension
│       └── bin/
│           └── libagentiteg.*.so/dll/dylib
├── project.godot
└── ...
```

### Option 2: Build from Source

See [CONTRIBUTING.md](../../CONTRIBUTING.md) for build instructions.

## Verifying Installation

Create a test script to verify AgentiteG is working:

```gdscript
extends Node

func _ready():
    # Test SpatialHash2D
    var spatial = SpatialHash2D.new()
    print("SpatialHash2D created: ", spatial != null)

    # Test ArrayOps
    var values = PackedFloat32Array([3.0, 1.0, 2.0])
    var sorted = ArrayOps.sort_floats(values)
    print("ArrayOps working: ", sorted[0] == 1.0)

    print("AgentiteG installation successful!")
```

## Your First Spatial Query

Let's build a simple enemy detection system:

```gdscript
extends Node2D

var spatial: SpatialHash2D
var enemy_positions: PackedVector2Array
var player_position: Vector2

func _ready():
    # Create spatial hash with 64-unit cells
    spatial = SpatialHash2D.new()
    spatial.cell_size = 64.0

    # Simulate some enemy positions
    enemy_positions = PackedVector2Array()
    for i in 100:
        enemy_positions.append(Vector2(
            randf_range(0, 800),
            randf_range(0, 600)
        ))

    # Build the spatial hash
    spatial.build(enemy_positions)

func _process(delta):
    player_position = get_global_mouse_position()

    # Find enemies within 100 units
    var nearby = spatial.query_radius(player_position, 100.0)

    # nearby is PackedInt32Array of indices
    print("Found %d enemies nearby" % nearby.size())

func _draw():
    # Draw all enemies
    for pos in enemy_positions:
        draw_circle(pos, 5, Color.RED)

    # Draw detection radius
    draw_arc(player_position, 100, 0, TAU, 32, Color.GREEN, 2)
```

## Your First Array Operation

Filter and sort game data efficiently:

```gdscript
extends Node

func _ready():
    # Simulate unit data
    var unit_health = PackedFloat32Array()
    var unit_positions = PackedVector2Array()

    for i in 1000:
        unit_health.append(randf_range(0, 100))
        unit_positions.append(Vector2(randf() * 1000, randf() * 1000))

    var player_pos = Vector2(500, 500)
    var heal_range = 200.0

    # Find units in range that need healing (health < 50)
    var in_range = ArrayOps.filter_in_radius(unit_positions, player_pos, heal_range)
    var in_range_health = ArrayOps.select_floats(unit_health, in_range)
    var need_healing = ArrayOps.filter_lt_float(in_range_health, 50.0)

    # Get the actual indices of units needing healing
    var heal_targets = ArrayOps.select_ints(in_range, need_healing)

    print("Units in range: ", in_range.size())
    print("Units needing healing: ", heal_targets.size())

    # Find the most injured unit
    if heal_targets.size() > 0:
        var target_healths = ArrayOps.select_floats(unit_health, heal_targets)
        var most_injured_local = ArrayOps.argmin_floats(target_healths)
        var most_injured = heal_targets[most_injured_local]
        print("Most injured unit index: ", most_injured)
        print("Their health: ", unit_health[most_injured])
```

## Performance Comparison

Here's how AgentiteG compares to pure GDScript:

```gdscript
extends Node

const ITEM_COUNT = 10000
const ITERATIONS = 100

func _ready():
    var positions = PackedVector2Array()
    for i in ITEM_COUNT:
        positions.append(Vector2(randf() * 1000, randf() * 1000))

    var origin = Vector2(500, 500)
    var radius = 100.0

    # GDScript approach
    var start = Time.get_ticks_usec()
    for iter in ITERATIONS:
        var result = []
        var radius_sq = radius * radius
        for i in positions.size():
            if origin.distance_squared_to(positions[i]) <= radius_sq:
                result.append(i)
    var gdscript_time = (Time.get_ticks_usec() - start) / 1000.0

    # AgentiteG approach
    var spatial = SpatialHash2D.new()
    spatial.cell_size = 50.0
    spatial.build(positions)

    start = Time.get_ticks_usec()
    for iter in ITERATIONS:
        var result = spatial.query_radius(origin, radius)
    var agentite_time = (Time.get_ticks_usec() - start) / 1000.0

    print("GDScript: %.2f ms" % gdscript_time)
    print("AgentiteG: %.2f ms" % agentite_time)
    print("Speedup: %.1fx" % (gdscript_time / agentite_time))
```

## Next Steps

- Read [CLAUDE.md](../../CLAUDE.md) for the full API reference
- Check out the `project/tests/` directory for more usage examples
- Run the benchmarks to see performance on your hardware

# BatchOps

Static methods for batch game operations - velocity updates, steering behaviors, and flocking.

## When to Use

- Updating many entity positions/velocities per frame
- Implementing steering behaviors (seek, flee, arrive)
- Boids-style flocking (separation, cohesion, alignment)
- Limiting velocities/speeds for many entities
- Any per-entity game logic loop that becomes a bottleneck

## Methods

All methods are static - call directly on the class: `BatchOps.method_name(...)`

Most 2D methods have 3D equivalents (replace `_2d` with `_3d`).

### Velocity / Position Updates

Apply velocities or accelerations to update positions/velocities.

```gdscript
# Apply velocities: result[i] = positions[i] + velocities[i] * delta
var new_positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
var new_positions = BatchOps.apply_velocities_3d(positions, velocities, delta)

# Apply accelerations: result[i] = velocities[i] + accelerations[i] * delta
var new_velocities = BatchOps.apply_accelerations_2d(velocities, accelerations, delta)
var new_velocities = BatchOps.apply_accelerations_3d(velocities, accelerations, delta)
```

### Seek

Move toward targets at max speed. Returns desired velocity vectors.

`result[i] = (targets[i] - positions[i]).normalized() * max_speed`

```gdscript
var desired = BatchOps.seek_batch(positions, targets, max_speed)
var desired = BatchOps.seek_batch_3d(positions, targets, max_speed)
```

### Flee

Move away from threats at max speed. Returns desired velocity vectors.

`result[i] = (positions[i] - threats[i]).normalized() * max_speed`

```gdscript
var desired = BatchOps.flee_batch(positions, threats, max_speed)
var desired = BatchOps.flee_batch_3d(positions, threats, max_speed)
```

### Arrive

Move toward targets, slowing down within the slowing radius. Returns desired velocity vectors that ramp down as entities approach their targets.

```gdscript
var desired = BatchOps.arrive_batch(positions, targets, max_speed, slowing_radius)
var desired = BatchOps.arrive_batch_3d(positions, targets, max_speed, slowing_radius)
```

When distance to target:
- `>= slowing_radius`: full max_speed
- `< slowing_radius`: speed scales linearly with distance

### Separation

Push apart entities that are too close. Returns separation force for each entity.

O(n²) - for large counts, consider using SpatialHash2D for neighbor queries.

```gdscript
# Uniform radius
var forces = BatchOps.separation_2d(positions, radius, strength)
var forces = BatchOps.separation_3d(positions, radius, strength)

# Per-entity radius
var forces = BatchOps.separation_2d_radii(positions, radii, strength)
```

Force is inversely proportional to distance - closer entities push harder.

### Cohesion

Steer toward the center of nearby entities. Returns direction toward local center of mass.

```gdscript
var forces = BatchOps.cohesion_2d(positions, radius, strength)
var forces = BatchOps.cohesion_3d(positions, radius, strength)
```

### Alignment

Match velocity of nearby entities. Returns average velocity of neighbors.

```gdscript
var aligned = BatchOps.alignment_2d(positions, velocities, radius)
var aligned = BatchOps.alignment_3d(positions, velocities, radius)
```

### Combined Flocking (Boids)

All three flocking behaviors combined in one efficient call.

```gdscript
var forces = BatchOps.flock_2d(
    positions,           # Entity positions
    velocities,          # Entity velocities
    separation_radius,   # Radius for separation
    cohesion_radius,     # Radius for cohesion
    alignment_radius,    # Radius for alignment
    separation_strength, # Separation force multiplier
    cohesion_strength,   # Cohesion force multiplier
    alignment_strength   # Alignment force multiplier
)
```

### Wander

Semi-random movement with smoothness. Uses a circle-based wander algorithm.

```gdscript
var wander_dirs = BatchOps.wander_2d(
    forward_directions,  # Current forward direction of each entity
    wander_angles,       # Current wander angle for each entity (update externally)
    circle_distance,     # How far ahead the wander circle is
    circle_radius,       # Radius of the wander circle
    angle_change         # Max angle change per call (not used internally, for caller reference)
)
```

### Obstacle Avoidance

Simple raycast-style obstacle avoidance for circular obstacles.

```gdscript
var avoidance = BatchOps.avoid_circles_2d(
    positions,           # Entity positions
    velocities,          # Entity velocities (for lookahead direction)
    obstacle_centers,    # Center positions of obstacles
    obstacle_radii,      # Radius of each obstacle
    lookahead_distance,  # How far ahead to check
    avoidance_strength   # Force strength when avoiding
)
```

### Velocity Limiting

Clamp velocity magnitudes to prevent overspeeding.

```gdscript
# Clamp to maximum
var limited = BatchOps.limit_velocity_2d(velocities, max_speed)
var limited = BatchOps.limit_velocity_3d(velocities, max_speed)

# Clamp to range [min, max] - prevents stopping
var limited = BatchOps.limit_velocity_range_2d(velocities, min_speed, max_speed)
```

## Common Patterns

### Basic Steering Agent

```gdscript
var positions: PackedVector2Array
var velocities: PackedVector2Array
var targets: PackedVector2Array

func _physics_process(delta: float) -> void:
    # Get desired velocities toward targets
    var desired = BatchOps.seek_batch(positions, targets, max_speed)

    # Steering = desired - current
    var steering = MathOps.sub_batch_2d(desired, velocities)
    steering = MathOps.clamp_length_batch_2d(steering, max_force)

    # Apply steering as acceleration
    velocities = BatchOps.apply_accelerations_2d(velocities, steering, delta)
    velocities = BatchOps.limit_velocity_2d(velocities, max_speed)

    # Update positions
    positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
```

### Arrive at Destination

```gdscript
func _physics_process(delta: float) -> void:
    # Arrive slows down when close
    var desired = BatchOps.arrive_batch(positions, targets, max_speed, 100.0)

    # Apply steering
    var steering = MathOps.sub_batch_2d(desired, velocities)
    velocities = BatchOps.apply_accelerations_2d(velocities, steering, delta)
    positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
```

### Boids Flocking

```gdscript
var positions: PackedVector2Array
var velocities: PackedVector2Array

func _physics_process(delta: float) -> void:
    # Combined flocking forces
    var flock_forces = BatchOps.flock_2d(
        positions, velocities,
        30.0,  # separation radius
        100.0, # cohesion radius
        50.0,  # alignment radius
        2.0,   # separation strength
        1.0,   # cohesion strength
        1.0    # alignment strength
    )

    # Apply forces
    velocities = BatchOps.apply_accelerations_2d(velocities, flock_forces, delta)
    velocities = BatchOps.limit_velocity_2d(velocities, max_speed)
    positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
```

### Flee from Danger

```gdscript
func flee_from_player(player_pos: Vector2) -> void:
    # Create array of threats (same threat for all enemies)
    var threats = PackedVector2Array()
    threats.resize(positions.size())
    threats.fill(player_pos)

    # Flee behavior
    var flee_vel = BatchOps.flee_batch(positions, threats, max_speed)

    # Blend with current behavior
    velocities = MathOps.lerp_batch_2d(velocities, flee_vel, 0.3)
```

### Separation to Prevent Overlap

```gdscript
func _physics_process(delta: float) -> void:
    # Separation forces
    var sep = BatchOps.separation_2d(positions, entity_radius * 2, 5.0)

    # Add to existing velocities/accelerations
    velocities = MathOps.add_batch_2d(velocities, MathOps.scale_batch_2d(sep, delta))
    velocities = BatchOps.limit_velocity_2d(velocities, max_speed)

    positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
```

### Obstacle Avoidance

```gdscript
var obstacle_centers: PackedVector2Array
var obstacle_radii: PackedFloat32Array

func _physics_process(delta: float) -> void:
    # Check for obstacles ahead
    var avoidance = BatchOps.avoid_circles_2d(
        positions, velocities,
        obstacle_centers, obstacle_radii,
        100.0,  # lookahead
        3.0     # strength
    )

    # Blend avoidance with normal steering
    var combined = MathOps.add_batch_2d(desired_velocities, avoidance)
    velocities = MathOps.lerp_batch_2d(velocities, combined, 0.1)
```

## Performance Notes

- Separation, cohesion, and alignment are O(n²). For >500 entities, use SpatialHash2D to query neighbors instead
- `flock_2d` combines all three behaviors in one pass - more efficient than calling them separately
- Use `limit_velocity_2d` instead of per-entity clamp loops
- For simple physics, `apply_velocities_2d` is faster than computing movement manually

## See Also

- [MathOps](MathOps.md) - Lower-level vector math (normalize, lerp, transform)
- [SpatialHash2D](SpatialHash2D.md) - O(1) neighbor queries for large entity counts
- [ArrayOps](ArrayOps.md) - Filter, sort, and select subsets of arrays

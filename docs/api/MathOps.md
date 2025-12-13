# MathOps

Static methods for batch vector and math operations.

## When to Use

- Processing many vectors per frame (steering, physics)
- Batch distance/direction calculations
- Transforming many points at once
- Any loop over vectors that becomes a bottleneck

## Methods

All methods are static - call directly on the class: `MathOps.method_name(...)`

All 2D methods have 3D equivalents (replace `_2d` with `_3d`).

### Normalization

Safe normalization - zero vectors become zero vectors.

```gdscript
var normals = MathOps.normalize_batch_2d(vectors)      # PackedVector2Array
var normals = MathOps.normalize_batch_3d(vectors_3d)   # PackedVector3Array
```

### Dot Products

Pairwise: `result[i] = a[i].dot(b[i])`

```gdscript
var dots = MathOps.dot_batch_2d(vectors_a, vectors_b)   # PackedFloat32Array
var dots = MathOps.dot_batch_3d(vectors_a, vectors_b)
```

### Cross Products

```gdscript
# 3D cross product
var crosses = MathOps.cross_batch(vectors_a, vectors_b)  # PackedVector3Array

# 2D "cross" (z-component of 3D cross, useful for winding order)
var crosses = MathOps.cross_batch_2d(vectors_a, vectors_b)  # PackedFloat32Array
```

### Lengths

```gdscript
var lengths = MathOps.length_batch_2d(vectors)           # PackedFloat32Array
var lengths = MathOps.length_batch_3d(vectors_3d)

# Squared length (faster - no sqrt)
var lengths_sq = MathOps.length_squared_batch_2d(vectors)
var lengths_sq = MathOps.length_squared_batch_3d(vectors_3d)
```

### Distances

Pairwise: `result[i] = a[i].distance_to(b[i])`

```gdscript
var dists = MathOps.distance_batch_2d(positions_a, positions_b)
var dists = MathOps.distance_batch_3d(positions_a, positions_b)

# Squared distance (faster)
var dists_sq = MathOps.distance_squared_batch_2d(positions_a, positions_b)
var dists_sq = MathOps.distance_squared_batch_3d(positions_a, positions_b)
```

### Distance Matrix

All-pairs distances: `result[i * b.size() + j] = a[i].distance_to(b[j])`

Returns flat array in row-major order.

```gdscript
var matrix = MathOps.distance_matrix_2d(group_a, group_b)
var matrix = MathOps.distance_matrix_3d(group_a, group_b)

# Access distance from a[i] to b[j]:
var dist = matrix[i * group_b.size() + j]
```

### Interpolation

```gdscript
# Uniform t for all pairs
var lerped = MathOps.lerp_batch_2d(starts, ends, 0.5)
var lerped = MathOps.lerp_batch_3d(starts, ends, 0.5)

# Per-element t values
var lerped = MathOps.lerp_batch_2d_weights(starts, ends, t_values)
var lerped = MathOps.lerp_batch_3d_weights(starts, ends, t_values)
```

### Vector Arithmetic

```gdscript
# Addition: result[i] = a[i] + b[i]
var sums = MathOps.add_batch_2d(vectors_a, vectors_b)
var sums = MathOps.add_batch_3d(vectors_a, vectors_b)

# Subtraction: result[i] = a[i] - b[i]
var diffs = MathOps.sub_batch_2d(vectors_a, vectors_b)
var diffs = MathOps.sub_batch_3d(vectors_a, vectors_b)

# Uniform scalar multiplication
var scaled = MathOps.scale_batch_2d(vectors, 2.0)
var scaled = MathOps.scale_batch_3d(vectors, 2.0)

# Per-element scalar multiplication
var scaled = MathOps.scale_batch_2d_weights(vectors, scalars)
var scaled = MathOps.scale_batch_3d_weights(vectors, scalars)
```

### Directions

Normalized direction from each point to corresponding target.

`result[i] = (to[i] - from[i]).normalized()`

```gdscript
var dirs = MathOps.direction_batch_2d(positions, targets)
var dirs = MathOps.direction_batch_3d(positions, targets)
```

### Transform Operations

Apply Transform2D/Transform3D to all points.

```gdscript
# Forward transform
var world_pos = MathOps.transform_2d_batch(global_transform, local_points)
var world_pos = MathOps.transform_3d_batch(global_transform, local_points)

# Inverse transform
var local_pos = MathOps.transform_2d_batch_inverse(global_transform, world_points)
var local_pos = MathOps.transform_3d_batch_inverse(global_transform, world_points)
```

### Angle Operations

```gdscript
# Angle of each 2D vector (atan2)
var angles = MathOps.angle_batch_2d(vectors)  # PackedFloat32Array (radians)

# Angle between pairs of vectors
var between = MathOps.angle_between_batch_2d(vectors_a, vectors_b)
var between = MathOps.angle_between_batch_3d(vectors_a, vectors_b)

# Create unit vectors from angles
var unit_vecs = MathOps.from_angle_batch(angles)  # PackedVector2Array
```

### Projection & Reflection

```gdscript
# Project a onto b: result[i] = a[i].project(b[i])
var projected = MathOps.project_batch_2d(vectors, onto_vectors)
var projected = MathOps.project_batch_3d(vectors, onto_vectors)

# Reflect around normal: result[i] = vectors[i].reflect(normals[i])
var reflected = MathOps.reflect_batch_2d(velocities, surface_normals)
var reflected = MathOps.reflect_batch_3d(velocities, surface_normals)
```

### Length Clamping

```gdscript
# Clamp to maximum length
var clamped = MathOps.clamp_length_batch_2d(velocities, max_speed)
var clamped = MathOps.clamp_length_batch_3d(velocities, max_speed)

# Clamp to range [min, max]
var clamped = MathOps.clamp_length_range_batch_2d(velocities, min_speed, max_speed)
var clamped = MathOps.clamp_length_range_batch_3d(velocities, min_speed, max_speed)
```

## Common Patterns

### Steering Behavior

```gdscript
func update_steering(delta: float):
    # Desired velocity toward targets
    var desired_dirs = MathOps.direction_batch_2d(positions, targets)
    var desired_vel = MathOps.scale_batch_2d(desired_dirs, max_speed)

    # Steering = desired - current
    var steering = MathOps.sub_batch_2d(desired_vel, velocities)
    steering = MathOps.clamp_length_batch_2d(steering, max_force)

    # Apply steering
    var accel = MathOps.scale_batch_2d(steering, delta)
    velocities = MathOps.add_batch_2d(velocities, accel)
    velocities = MathOps.clamp_length_batch_2d(velocities, max_speed)

    # Move
    var movement = MathOps.scale_batch_2d(velocities, delta)
    positions = MathOps.add_batch_2d(positions, movement)
```

### Projectile Physics

```gdscript
func update_projectiles(delta: float):
    # Apply gravity
    var gravity_delta = MathOps.scale_batch_2d(
        PackedVector2Array([Vector2(0, 980)]),  # Would need to fill array
        delta
    )
    # ... or just loop if gravity is constant

    # Move projectiles
    var movement = MathOps.scale_batch_2d(velocities, delta)
    positions = MathOps.add_batch_2d(positions, movement)

    # Update sprite rotations
    var angles = MathOps.angle_batch_2d(velocities)
```

### Ball Bouncing (Reflection)

```gdscript
func handle_wall_collisions(wall_normals: PackedVector2Array, collision_indices: PackedInt32Array):
    # Get velocities of colliding balls
    var colliding_vel = ArrayOps.select_vector2(velocities, collision_indices)

    # Reflect them
    var reflected = MathOps.reflect_batch_2d(colliding_vel, wall_normals)

    # Update original array (need to loop for this part)
    for i in range(collision_indices.size()):
        velocities[collision_indices[i]] = reflected[i]
```

### Distance-Based Falloff

```gdscript
func apply_explosion_force(explosion_pos: Vector2, radius: float, force: float):
    # Get directions from explosion to all entities
    var to_entities = MathOps.sub_batch_2d(positions,
        PackedVector2Array([explosion_pos]))  # Would broadcast

    # Get distances
    var distances = MathOps.length_batch_2d(to_entities)

    # Normalize directions
    var dirs = MathOps.normalize_batch_2d(to_entities)

    # Calculate force falloff (1 - dist/radius), clamped
    # ... apply force based on distance
```

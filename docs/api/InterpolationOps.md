# InterpolationOps

Easing functions, bezier curves, and spline interpolation for smooth animations.

## When to Use

- UI animations (health bars, menus, transitions)
- Camera movements (smooth follow, dolly shots)
- Projectile arcs (thrown objects, ballistic paths)
- Patrol paths and AI movement
- Difficulty curves and progression systems

All methods are **static** - call directly on the class.

## Easing Functions

Easing makes animations feel natural. Linear motion feels robotic.

### Easing Types

| Type | Effect | Best For |
|------|--------|----------|
| `ease_in_*` | Slow start, fast end | Accelerating objects |
| `ease_out_*` | Fast start, slow end | Decelerating, UI popups |
| `ease_in_out_*` | Slow start and end | Smooth transitions |

### Available Easing Curves

All easing functions take `PackedFloat32Array` of t values (0-1) and return eased values.

```gdscript
# Polynomial
InterpolationOps.ease_in_quad(t)      # Gentle acceleration
InterpolationOps.ease_out_quad(t)     # Gentle deceleration
InterpolationOps.ease_in_out_quad(t)
InterpolationOps.ease_in_cubic(t)     # More pronounced
InterpolationOps.ease_out_cubic(t)
InterpolationOps.ease_in_out_cubic(t)
InterpolationOps.ease_in_quart(t)     # Even more pronounced
InterpolationOps.ease_out_quart(t)
InterpolationOps.ease_in_out_quart(t)
InterpolationOps.ease_in_quint(t)
InterpolationOps.ease_out_quint(t)
InterpolationOps.ease_in_out_quint(t)

# Special curves
InterpolationOps.ease_in_sine(t)      # Gentle, organic
InterpolationOps.ease_out_sine(t)
InterpolationOps.ease_in_out_sine(t)
InterpolationOps.ease_in_expo(t)      # Dramatic
InterpolationOps.ease_out_expo(t)
InterpolationOps.ease_in_out_expo(t)
InterpolationOps.ease_in_circ(t)      # Circular motion
InterpolationOps.ease_out_circ(t)
InterpolationOps.ease_in_out_circ(t)

# Overshoot and bounce
InterpolationOps.ease_in_back(t)      # Pulls back before going
InterpolationOps.ease_out_back(t)     # Overshoots then settles
InterpolationOps.ease_in_out_back(t)
InterpolationOps.ease_in_elastic(t)   # Springy
InterpolationOps.ease_out_elastic(t)
InterpolationOps.ease_in_out_elastic(t)
InterpolationOps.ease_in_bounce(t)    # Bouncy
InterpolationOps.ease_out_bounce(t)
InterpolationOps.ease_in_out_bounce(t)

# General purpose
InterpolationOps.smoothstep(t)        # Smooth S-curve
```

### Easing Example

```gdscript
# Animate health bar from 100 to 30 over 1 second
var t = InterpolationOps.linspace(0.0, 1.0, 60)  # 60 frames
var eased = InterpolationOps.ease_out_quad(t)
var health_values = InterpolationOps.remap(eased, 0.0, 1.0, 100.0, 30.0)
```

## Utility Functions

#### `linspace(start: float, end: float, count: int) -> PackedFloat32Array`
Generate evenly spaced values.

```gdscript
var t = InterpolationOps.linspace(0.0, 1.0, 10)  # [0.0, 0.111, 0.222, ...]
```

#### `remap(values: PackedFloat32Array, from_min: float, from_max: float, to_min: float, to_max: float) -> PackedFloat32Array`
Map values from one range to another.

```gdscript
var normalized = InterpolationOps.remap(scores, 0.0, 100.0, 0.0, 1.0)
```

#### `clamp_array(values: PackedFloat32Array, min: float, max: float) -> PackedFloat32Array`
Clamp all values to a range.

```gdscript
var clamped = InterpolationOps.clamp_array(values, 0.0, 1.0)
```

## Bezier Curves

Smooth curves controlled by anchor points. The curve passes through start and end, pulled toward control points.

#### `bezier_cubic_2d(p0: Vector2, p1: Vector2, p2: Vector2, p3: Vector2, t: PackedFloat32Array) -> PackedVector2Array`
Sample a cubic bezier curve.

```gdscript
var start = Vector2(0, 0)
var control1 = Vector2(30, 100)   # Pulls curve up
var control2 = Vector2(70, 100)
var end_point = Vector2(100, 0)

var t = InterpolationOps.linspace(0.0, 1.0, 20)
var path = InterpolationOps.bezier_cubic_2d(start, control1, control2, end_point, t)
```

#### `bezier_cubic_3d(p0: Vector3, p1: Vector3, p2: Vector3, p3: Vector3, t: PackedFloat32Array) -> PackedVector3Array`
3D version of cubic bezier.

## Catmull-Rom Splines

Smooth curves that pass through ALL control points. Perfect for waypoints.

#### `catmull_rom_2d(control_points: PackedVector2Array, samples_per_segment: int) -> PackedVector2Array`
Generate a smooth curve through waypoints.

```gdscript
var waypoints = PackedVector2Array([
    Vector2(0, 50),
    Vector2(100, 0),
    Vector2(200, 100),
    Vector2(300, 50)
])
var smooth_path = InterpolationOps.catmull_rom_2d(waypoints, 10)
# Returns ~30 points forming a smooth curve through all waypoints
```

#### `catmull_rom_3d(control_points: PackedVector3Array, samples_per_segment: int) -> PackedVector3Array`
3D version for camera paths, rail tracks, etc.

## Common Patterns

### UI Animation

```gdscript
func animate_popup():
    var t = InterpolationOps.linspace(0.0, 1.0, 30)
    var scale_values = InterpolationOps.ease_out_back(t)  # Overshoot effect
    # Apply scale_values[frame] each frame
```

### Camera Shake Recovery

```gdscript
func shake_camera():
    var t = InterpolationOps.linspace(0.0, 1.0, 20)
    var decay = InterpolationOps.ease_out_elastic(t)
    # Multiply shake intensity by (1 - decay[frame])
```

### Projectile Arc

```gdscript
func create_throw_arc(start: Vector2, end_point: Vector2) -> PackedVector2Array:
    var apex_height = 100  # How high the arc goes
    var mid_x = (start.x + end_point.x) / 2
    var t = InterpolationOps.linspace(0.0, 1.0, 30)
    return InterpolationOps.bezier_cubic_2d(
        start,
        Vector2(mid_x - 30, start.y - apex_height),
        Vector2(mid_x + 30, end_point.y - apex_height),
        end_point,
        t
    )
```

### Patrol Path

```gdscript
func create_patrol_path(waypoints: PackedVector2Array) -> PackedVector2Array:
    return InterpolationOps.catmull_rom_2d(waypoints, 10)
```

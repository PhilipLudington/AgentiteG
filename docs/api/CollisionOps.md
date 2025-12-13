# CollisionOps

Static methods for batch collision detection operations.

## When to Use

- Checking many point-in-shape tests per frame
- Circle/sphere collision detection between groups
- Self-collision detection within a group
- Ray casting against many shapes
- Segment intersection detection

## Methods

All methods are static - call directly on the class: `CollisionOps.method_name(...)`

### Point Containment Tests

Returns indices of points inside the shape.

```gdscript
# Points inside a rectangle
var inside = CollisionOps.points_in_rect(points, rect)  # PackedInt32Array

# Points inside a circle
var inside = CollisionOps.points_in_circle(points, center, radius)

# Points inside a polygon (convex or concave)
var inside = CollisionOps.points_in_polygon(points, polygon_vertices)

# Points inside an AABB (3D)
var inside = CollisionOps.points_in_aabb(points_3d, aabb)

# Points inside a sphere (3D)
var inside = CollisionOps.points_in_sphere(points_3d, center, radius)
```

### Shape vs Shape Collision

Returns collision pairs as flat array `[i0, j0, i1, j1, ...]` where `i` is from first set, `j` is from second set.

```gdscript
# Circle vs circle (variable radii)
var pairs = CollisionOps.circles_vs_circles(centers_a, radii_a, centers_b, radii_b)

# Circle vs circle (uniform radius per set - faster)
var pairs = CollisionOps.circles_vs_circles_uniform(centers_a, radius_a, centers_b, radius_b)

# 2D AABB vs AABB
var pairs = CollisionOps.aabb_vs_aabb_2d(mins_a, maxs_a, mins_b, maxs_b)

# 3D AABB vs AABB
var pairs = CollisionOps.aabb_vs_aabb_3d(mins_a, maxs_a, mins_b, maxs_b)

# Sphere vs sphere (variable radii)
var pairs = CollisionOps.spheres_vs_spheres(centers_a, radii_a, centers_b, radii_b)

# Sphere vs sphere (uniform radius per set)
var pairs = CollisionOps.spheres_vs_spheres_uniform(centers_a, radius_a, centers_b, radius_b)

# Process collision pairs
for i in range(0, pairs.size(), 2):
    var idx_a = pairs[i]
    var idx_b = pairs[i + 1]
    handle_collision(entities_a[idx_a], entities_b[idx_b])
```

### Self-Collision

Returns collision pairs within the same set. Each pair appears once (not duplicated as `[i,j]` and `[j,i]`).

```gdscript
# Circle self-collision (variable radii)
var pairs = CollisionOps.circles_self_collision(centers, radii)

# Circle self-collision (uniform radius - faster)
var pairs = CollisionOps.circles_self_collision_uniform(centers, radius)

# Sphere self-collision (variable radii)
var pairs = CollisionOps.spheres_self_collision(centers_3d, radii)

# Sphere self-collision (uniform radius)
var pairs = CollisionOps.spheres_self_collision_uniform(centers_3d, radius)

# Process self-collision pairs
for i in range(0, pairs.size(), 2):
    var idx_a = pairs[i]
    var idx_b = pairs[i + 1]
    resolve_overlap(entities[idx_a], entities[idx_b])
```

### Ray Intersection

```gdscript
# Ray vs circles: returns distance to each circle (INF if no hit)
var distances = CollisionOps.ray_vs_circles(origin, direction, centers, radii)

# Ray vs circles (uniform radius)
var distances = CollisionOps.ray_vs_circles_uniform(origin, direction, centers, radius)

# Ray vs 2D AABBs
var distances = CollisionOps.ray_vs_aabbs_2d(origin, direction, mins, maxs)

# Ray vs spheres (3D)
var distances = CollisionOps.ray_vs_spheres(origin_3d, direction_3d, centers_3d, radii)

# Find first circle hit by ray (returns index, or -1 if none)
var hit_idx = CollisionOps.ray_first_circle(origin, direction, max_distance, centers, radii)

# Find first sphere hit by ray (returns index, or -1 if none)
var hit_idx = CollisionOps.ray_first_sphere(origin_3d, direction_3d, max_distance, centers_3d, radii)
```

### Segment Intersection

```gdscript
# Check which segment pairs intersect
# Returns pairs [i0, j0, i1, j1, ...] where segments i from A and j from B intersect
var pairs = CollisionOps.segments_intersect(starts_a, ends_a, starts_b, ends_b)

# Get intersection points for intersecting segment pairs
var points = CollisionOps.segment_intersection_points(starts_a, ends_a, starts_b, ends_b)

# Segment self-intersection (within same set)
var pairs = CollisionOps.segments_self_intersect(starts, ends)
```

### Closest Point Queries

```gdscript
# Find closest point on each segment to each query point
# Returns array of size points.size() * segments.size() in row-major order
var closest = CollisionOps.closest_points_on_segments(points, seg_starts, seg_ends)

# Find index of closest circle to each point
var indices = CollisionOps.closest_circle_indices(points, centers, radii)

# Find index of closest circle (uniform radius - faster)
var indices = CollisionOps.closest_circle_indices_uniform(points, centers, radius)
```

## Common Patterns

### Bullet vs Enemy Collision

```gdscript
func check_bullet_hits():
    # Get collision pairs
    var pairs = CollisionOps.circles_vs_circles_uniform(
        bullet_positions, bullet_radius,
        enemy_positions, enemy_radius
    )

    # Process hits
    var bullets_to_remove = []
    for i in range(0, pairs.size(), 2):
        var bullet_idx = pairs[i]
        var enemy_idx = pairs[i + 1]

        enemies[enemy_idx].take_damage(bullet_damage)
        bullets_to_remove.append(bullet_idx)

    # Remove bullets (in reverse order)
    bullets_to_remove.sort()
    for i in range(bullets_to_remove.size() - 1, -1, -1):
        remove_bullet(bullets_to_remove[i])
```

### Physics Overlap Resolution

```gdscript
func resolve_overlaps():
    var pairs = CollisionOps.circles_self_collision_uniform(positions, entity_radius)

    for i in range(0, pairs.size(), 2):
        var a = pairs[i]
        var b = pairs[i + 1]

        # Calculate separation
        var dir = positions[b] - positions[a]
        var dist = dir.length()
        var overlap = entity_radius * 2.0 - dist

        if overlap > 0 and dist > 0:
            dir = dir.normalized()
            positions[a] -= dir * overlap * 0.5
            positions[b] += dir * overlap * 0.5
```

### Mouse Picking

```gdscript
func get_entity_under_mouse(mouse_pos: Vector2) -> int:
    # Check which entities contain the mouse position
    var under_mouse = CollisionOps.points_in_circle(
        PackedVector2Array([mouse_pos]),  # Single point query
        positions[0],  # Would need to loop or use closest
        radius
    )

    # Better: find closest circle to mouse
    var closest = CollisionOps.closest_circle_indices_uniform(
        PackedVector2Array([mouse_pos]),
        positions,
        radius
    )

    var idx = closest[0]
    var dist = positions[idx].distance_to(mouse_pos)
    if dist <= radius:
        return idx
    return -1
```

### Laser/Line of Fire

```gdscript
func fire_laser(start: Vector2, direction: Vector2, max_range: float) -> Dictionary:
    # Find first enemy hit
    var hit_idx = CollisionOps.ray_first_circle(
        start, direction, max_range,
        enemy_positions, enemy_radii
    )

    if hit_idx >= 0:
        # Get all distances to calculate hit point
        var distances = CollisionOps.ray_vs_circles(
            start, direction, enemy_positions, enemy_radii
        )
        var hit_point = start + direction.normalized() * distances[hit_idx]
        return {"hit": true, "enemy": hit_idx, "point": hit_point}

    return {"hit": false, "point": start + direction.normalized() * max_range}
```

### Viewport Culling

```gdscript
func get_visible_entities(viewport_rect: Rect2) -> PackedInt32Array:
    return CollisionOps.points_in_rect(entity_positions, viewport_rect)

func draw_visible_entities():
    var visible = get_visible_entities(get_viewport_rect())
    for i in visible:
        draw_entity(entities[i])
```

### Polygon Selection Tool

```gdscript
func select_in_lasso(lasso_points: PackedVector2Array):
    var selected = CollisionOps.points_in_polygon(entity_positions, lasso_points)
    for i in selected:
        entities[i].selected = true
```

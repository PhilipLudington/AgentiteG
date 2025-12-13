# GeometryOps

Static methods for computational geometry algorithms.

## When to Use

- Computing convex hulls for collision shapes
- Triangulating polygons for rendering
- Simplifying paths/polylines
- Analyzing polygon properties
- Computing Voronoi diagrams

## Methods

All methods are static - call directly on the class: `GeometryOps.method_name(...)`

### Convex Hull

```gdscript
# Returns points on the convex hull in counter-clockwise order
var hull = GeometryOps.convex_hull(points)  # PackedVector2Array

# Returns indices of points on the convex hull
var hull_indices = GeometryOps.convex_hull_indices(points)  # PackedInt32Array
```

### Triangulation

```gdscript
# Ear clipping triangulation of a simple polygon
# Returns indices [i0, i1, i2, i3, i4, i5, ...] forming triangles
var triangles = GeometryOps.triangulate(polygon)

# Draw triangles
for i in range(0, triangles.size(), 3):
    draw_triangle(polygon[triangles[i]], polygon[triangles[i+1]], polygon[triangles[i+2]])

# Delaunay triangulation of a point set
var triangles = GeometryOps.delaunay(points)
```

### Voronoi Diagrams

```gdscript
# Returns Array of PackedVector2Array, one polygon per input point
var cells = GeometryOps.voronoi_cells(points, bounding_rect)

# Draw each Voronoi cell
for i in range(cells.size()):
    draw_polygon(cells[i], colors[i])

# Returns edges as flat array [x1, y1, x2, y2, ...]
var edges = GeometryOps.voronoi_edges(points, bounding_rect)
```

### Polygon Properties

```gdscript
# Signed area (positive = CCW, negative = CW)
var signed_area = GeometryOps.polygon_signed_area(polygon)

# Absolute area
var area = GeometryOps.polygon_area(polygon)

# Centroid (center of mass)
var center = GeometryOps.polygon_centroid(polygon)  # Vector2

# Bounding box
var bounds = GeometryOps.polygon_bounds(polygon)  # Rect2

# Perimeter length
var perimeter = GeometryOps.polygon_perimeter(polygon)
```

### Polygon Tests

```gdscript
# Check if polygon is convex
var is_convex = GeometryOps.polygon_is_convex(polygon)  # bool

# Check if polygon is clockwise
var is_cw = GeometryOps.polygon_is_clockwise(polygon)

# Check if polygon is simple (no self-intersections)
var is_simple = GeometryOps.polygon_is_simple(polygon)

# Check if point is inside polygon
var inside = GeometryOps.point_in_polygon(point, polygon)
```

### Polygon Operations

```gdscript
# Reverse winding order
var reversed = GeometryOps.polygon_reverse(polygon)

# Ensure counter-clockwise winding
var ccw = GeometryOps.polygon_make_ccw(polygon)

# Ensure clockwise winding
var cw = GeometryOps.polygon_make_cw(polygon)
```

### Polyline Simplification

```gdscript
# Ramer-Douglas-Peucker simplification
# epsilon: maximum distance from original line allowed
var simplified = GeometryOps.simplify_rdp(polyline, epsilon)

# Returns indices of kept points
var kept_indices = GeometryOps.simplify_rdp_indices(polyline, epsilon)

# Visvalingam-Whyatt simplification (area-based)
# target_count: number of points to keep
var simplified = GeometryOps.simplify_visvalingam(polyline, target_count)
```

### Polygon Offset / Buffer

```gdscript
# Offset polygon by distance (positive = outward, negative = inward)
var expanded = GeometryOps.offset_polygon(polygon, 10.0)   # Grow by 10
var shrunk = GeometryOps.offset_polygon(polygon, -5.0)     # Shrink by 5
```

### Minkowski Operations

```gdscript
# Minkowski sum of two convex polygons
var sum = GeometryOps.minkowski_sum(polygon_a, polygon_b)

# Minkowski difference (useful for collision detection)
var diff = GeometryOps.minkowski_diff(polygon_a, polygon_b)
```

### Distance Queries

```gdscript
# Distance from point to line segment
var dist = GeometryOps.point_to_segment_distance(point, seg_start, seg_end)

# Distance from point to polygon boundary
var dist = GeometryOps.point_to_polygon_distance(point, polygon)

# Batch: distances from each point to polygon boundary
var distances = GeometryOps.distances_to_polygon(points, polygon)

# Closest point on polygon boundary to query point
var closest = GeometryOps.closest_point_on_polygon(point, polygon)  # Vector2
```

### Line / Segment Utilities

```gdscript
# Side of point relative to line (positive = left, negative = right, 0 = on line)
var side = GeometryOps.point_line_side(point, line_start, line_end)

# Project point onto line (returns parameter t: 0=start, 1=end)
var t = GeometryOps.project_point_to_line(point, line_start, line_end)

# Batch project points onto line
var t_values = GeometryOps.project_points_to_line(points, line_start, line_end)
```

## Common Patterns

### Create Collision Shape from Points

```gdscript
func create_collision_polygon(sprite_outline: PackedVector2Array) -> PackedVector2Array:
    # Simplify the outline
    var simplified = GeometryOps.simplify_rdp(sprite_outline, 2.0)

    # Get convex hull for physics
    return GeometryOps.convex_hull(simplified)
```

### Generate Terrain Mesh

```gdscript
func generate_terrain_mesh(boundary: PackedVector2Array):
    # Triangulate the boundary
    var triangles = GeometryOps.triangulate(boundary)

    # Create mesh from triangles
    var vertices = PackedVector3Array()
    for i in range(0, triangles.size(), 3):
        var a = boundary[triangles[i]]
        var b = boundary[triangles[i + 1]]
        var c = boundary[triangles[i + 2]]
        vertices.append(Vector3(a.x, 0, a.y))
        vertices.append(Vector3(b.x, 0, b.y))
        vertices.append(Vector3(c.x, 0, c.y))
    # ... create ArrayMesh
```

### Voronoi-Based Map Generation

```gdscript
func generate_map_regions(seed_points: PackedVector2Array, map_bounds: Rect2):
    var cells = GeometryOps.voronoi_cells(seed_points, map_bounds)

    for i in range(cells.size()):
        var cell = cells[i]
        var region = Region.new()
        region.boundary = cell
        region.area = GeometryOps.polygon_area(cell)
        region.center = GeometryOps.polygon_centroid(cell)
        regions.append(region)
```

### Path Smoothing for Navigation

```gdscript
func smooth_nav_path(raw_path: PackedVector2Array) -> PackedVector2Array:
    # Remove unnecessary waypoints while keeping path within epsilon of original
    return GeometryOps.simplify_rdp(raw_path, 5.0)
```

### Expand Selection Area

```gdscript
func expand_selection(selection_polygon: PackedVector2Array, margin: float) -> PackedVector2Array:
    return GeometryOps.offset_polygon(selection_polygon, margin)
```

### Check if Entity Fits

```gdscript
func can_entity_pass_through(entity_shape: PackedVector2Array, gap_shape: PackedVector2Array) -> bool:
    # Using Minkowski difference: if origin is inside diff, shapes overlap
    var diff = GeometryOps.minkowski_diff(gap_shape, entity_shape)
    return GeometryOps.point_in_polygon(Vector2.ZERO, diff)
```

### Drawing Traced Paths

```gdscript
var traced_path: PackedVector2Array = []

func _input(event):
    if event is InputEventMouseMotion and drawing:
        traced_path.append(event.position)

func finish_drawing():
    # Simplify to manageable number of points
    traced_path = GeometryOps.simplify_visvalingam(traced_path, 20)
```

### Level of Detail for Polygons

```gdscript
func get_polygon_lod(polygon: PackedVector2Array, camera_distance: float) -> PackedVector2Array:
    # More simplification at greater distances
    var epsilon = camera_distance * 0.1
    return GeometryOps.simplify_rdp(polygon, epsilon)
```

### Point Classification

```gdscript
func classify_points(points: PackedVector2Array, dividing_line: Array) -> Dictionary:
    var left = []
    var right = []
    var on_line = []

    for i in range(points.size()):
        var side = GeometryOps.point_line_side(points[i], dividing_line[0], dividing_line[1])
        if side > 0.001:
            left.append(i)
        elif side < -0.001:
            right.append(i)
        else:
            on_line.append(i)

    return {"left": left, "right": right, "on_line": on_line}
```

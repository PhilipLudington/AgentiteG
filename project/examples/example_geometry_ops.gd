extends SceneTree
## =============================================================================
## GeometryOps Example - Computational geometry algorithms
## =============================================================================
##
## WHEN TO USE:
## ------------
## GeometryOps provides computational geometry algorithms for shape analysis,
## mesh generation, and spatial queries. Use for procedural content and tools.
##
## PERFECT FOR:
## - Procedural level generation (triangulation, Voronoi)
## - Map editors (polygon manipulation, simplification)
## - Navigation mesh generation
## - Visibility/occlusion (convex hull)
## - Procedural islands, territories, regions
##
## KEY ALGORITHMS:
## - Convex Hull: Smallest convex polygon containing all points
## - Triangulation: Convert polygons to triangles for rendering
## - Delaunay: Well-shaped triangles, good for terrain meshes
## - Voronoi: Partition space into regions (territories, biomes)
## - Polygon Operations: Area, centroid, containment, offset
## - Simplification: Reduce points while preserving shape
##
## ALL METHODS ARE STATIC - call directly on GeometryOps class.
##
## Run with: godot --headless -s examples/example_geometry_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("GeometryOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	convex_hull()
	triangulation()
	voronoi()
	polygon_properties()
	polygon_tests()
	simplification()
	polygon_offset()
	distance_queries()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func convex_hull() -> void:
	print("--- Convex Hull ---")
	print("")
	print("The smallest convex polygon that contains all points.")
	print("Like stretching a rubber band around the points.")
	print("")

	# -------------------------------------------------------------------------
	# convex_hull - Get the outer boundary of a point set
	# USE WHEN: Bounding shapes, visibility tests, collision simplification
	# INPUT: Any set of points
	# OUTPUT: Points on the hull in order (interior points removed)
	# -------------------------------------------------------------------------
	var points = PackedVector2Array([
		Vector2(0, 0),
		Vector2(10, 5),   # Interior point
		Vector2(20, 0),
		Vector2(20, 20),
		Vector2(10, 15),  # Interior point
		Vector2(0, 20)
	])

	print("Convex hull USE CASES:")
	print("  - Collision shape simplification")
	print("  - Selection bounding box")
	print("  - Visibility/shadow casting")
	print("")

	print("Points: ", points)

	# Get hull points
	var hull = GeometryOps.convex_hull(points)
	print("\nconvex_hull:")
	print("  Hull points: ", hull)
	print("  (", hull.size(), " points, interior points removed)")

	# Get hull indices
	var hull_indices = GeometryOps.convex_hull_indices(points)
	print("\nconvex_hull_indices: ", hull_indices)
	print("")


func triangulation() -> void:
	print("--- Triangulation ---")
	print("")
	print("Convert polygons into triangles for rendering or physics.")
	print("Essential for procedural mesh generation.")
	print("")

	# -------------------------------------------------------------------------
	# triangulate - Convert polygon to triangles (ear clipping)
	# USE WHEN: Rendering polygons, creating collision shapes
	# OUTPUT: Flat array of indices [i0, i1, i2, i3, i4, i5, ...] (3 per triangle)
	# -------------------------------------------------------------------------
	var square = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 0), Vector2(10, 10), Vector2(0, 10)
	])

	print("Triangulation USE CASES:")
	print("  - Polygon rendering (Godot needs triangles)")
	print("  - Collision mesh generation")
	print("  - Procedural terrain chunks")
	print("")

	print("Square polygon: ", square)

	# Ear clipping triangulation
	var triangles = GeometryOps.triangulate(square)
	print("\ntriangulate (ear clipping):")
	print("  Triangle indices: ", triangles)
	print("  ", triangles.size() / 3, " triangles")

	# Print triangles
	for i in range(0, triangles.size(), 3):
		print("  Triangle: ", square[triangles[i]], ", ",
			  square[triangles[i+1]], ", ", square[triangles[i+2]])

	# Delaunay triangulation
	var points = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 0), Vector2(20, 0),
		Vector2(5, 10), Vector2(15, 10),
		Vector2(10, 20)
	])

	print("\nDelaunay triangulation:")
	var delaunay = GeometryOps.delaunay(points)
	print("  Points: ", points)
	print("  Triangle count: ", delaunay.size() / 3)
	print("")


func voronoi() -> void:
	print("--- Voronoi Diagrams ---")
	print("")
	print("Partition space so each region contains all points closest to one seed.")
	print("Perfect for territories, biomes, shattered glass effects.")
	print("")

	# -------------------------------------------------------------------------
	# voronoi_cells - Partition space into regions
	# USE WHEN: Territory/faction maps, biome generation, crystal/glass effects
	# INPUT: Seed points (region centers) and bounds
	# OUTPUT: Array of polygons (one per seed point)
	# -------------------------------------------------------------------------
	var seed_points = PackedVector2Array([
		Vector2(25, 25),
		Vector2(75, 25),
		Vector2(50, 75)
	])
	var bounds = Rect2(0, 0, 100, 100)

	print("Voronoi USE CASES:")
	print("  - Faction territories (place capitals, regions form naturally)")
	print("  - Biome boundaries")
	print("  - Procedural city districts")
	print("  - Shattered glass/ice effects")
	print("")

	print("Seed points: ", seed_points)
	print("Bounds: ", bounds)

	# Get Voronoi cells
	var cells = GeometryOps.voronoi_cells(seed_points, bounds)
	print("\nvoronoi_cells:")
	print("  ", cells.size(), " cells generated")
	for i in range(cells.size()):
		print("  Cell ", i, " vertices: ", cells[i].size())

	# Get Voronoi edges
	var edges = GeometryOps.voronoi_edges(seed_points, bounds)
	print("\nvoronoi_edges:")
	print("  ", edges.size() / 4, " edges")
	print("")


func polygon_properties() -> void:
	print("--- Polygon Properties ---")

	var square = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 0), Vector2(10, 10), Vector2(0, 10)
	])

	print("Square: ", square)

	# Area
	var area = GeometryOps.polygon_area(square)
	print("  polygon_area: ", area)

	var signed_area = GeometryOps.polygon_signed_area(square)
	print("  polygon_signed_area: ", signed_area, " (positive=CCW)")

	# Centroid
	var centroid = GeometryOps.polygon_centroid(square)
	print("  polygon_centroid: ", centroid)

	# Bounds
	var bounds = GeometryOps.polygon_bounds(square)
	print("  polygon_bounds: ", bounds)

	# Perimeter
	var perimeter = GeometryOps.polygon_perimeter(square)
	print("  polygon_perimeter: ", perimeter)
	print("")


func polygon_tests() -> void:
	print("--- Polygon Tests ---")

	var square = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 0), Vector2(10, 10), Vector2(0, 10)
	])

	# Convexity
	print("polygon_is_convex (square): ", GeometryOps.polygon_is_convex(square))

	# Winding order
	print("polygon_is_clockwise (square): ", GeometryOps.polygon_is_clockwise(square))

	# Point in polygon
	print("\npoint_in_polygon tests:")
	print("  (5, 5) inside: ", GeometryOps.point_in_polygon(Vector2(5, 5), square))
	print("  (15, 5) inside: ", GeometryOps.point_in_polygon(Vector2(15, 5), square))
	print("  (0, 0) inside: ", GeometryOps.point_in_polygon(Vector2(0, 0), square))

	# Winding order operations
	print("\nWinding order operations:")
	var reversed = GeometryOps.polygon_reverse(square)
	print("  Original clockwise: ", GeometryOps.polygon_is_clockwise(square))
	print("  Reversed clockwise: ", GeometryOps.polygon_is_clockwise(reversed))

	var ccw = GeometryOps.polygon_make_ccw(square)
	print("  polygon_make_ccw clockwise: ", GeometryOps.polygon_is_clockwise(ccw))
	print("")


func simplification() -> void:
	print("--- Polyline Simplification ---")

	# Noisy polyline
	var polyline = PackedVector2Array([
		Vector2(0, 0),
		Vector2(1, 0.1),
		Vector2(2, -0.1),
		Vector2(3, 0.05),
		Vector2(4, 0),
		Vector2(5, 5),
		Vector2(6, 4.9),
		Vector2(7, 5.1),
		Vector2(8, 5),
		Vector2(9, 5)
	])

	print("Original polyline: ", polyline.size(), " points")

	# Ramer-Douglas-Peucker
	var epsilon = 0.5
	var rdp = GeometryOps.simplify_rdp(polyline, epsilon)
	print("\nsimplify_rdp (epsilon=", epsilon, "):")
	print("  Simplified: ", rdp.size(), " points")
	print("  Points: ", rdp)

	# Get indices of kept points
	var rdp_indices = GeometryOps.simplify_rdp_indices(polyline, epsilon)
	print("  Kept indices: ", rdp_indices)

	# Visvalingam-Whyatt (target point count)
	var target = 4
	var vis = GeometryOps.simplify_visvalingam(polyline, target)
	print("\nsimplify_visvalingam (target=", target, " points):")
	print("  Points: ", vis)
	print("")


func polygon_offset() -> void:
	print("--- Polygon Offset (Buffer) ---")

	var square = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 0), Vector2(10, 10), Vector2(0, 10)
	])

	print("Original square area: ", GeometryOps.polygon_area(square))

	# Expand
	var expanded = GeometryOps.offset_polygon(square, 5.0)
	print("\noffset_polygon (expand by 5):")
	print("  New area: ", GeometryOps.polygon_area(expanded))
	print("  Points: ", expanded)

	# Shrink
	var shrunk = GeometryOps.offset_polygon(square, -2.0)
	print("\noffset_polygon (shrink by 2):")
	print("  New area: ", GeometryOps.polygon_area(shrunk))
	print("  Points: ", shrunk)
	print("")


func distance_queries() -> void:
	print("--- Distance Queries ---")

	var polygon = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 0), Vector2(10, 10), Vector2(0, 10)
	])

	# Point to polygon distance
	var outside_point = Vector2(15, 5)
	var dist = GeometryOps.point_to_polygon_distance(outside_point, polygon)
	print("point_to_polygon_distance (", outside_point, "): ", dist)

	# Closest point on polygon
	var closest = GeometryOps.closest_point_on_polygon(outside_point, polygon)
	print("closest_point_on_polygon: ", closest)

	# Point to segment distance
	var seg_start = Vector2(0, 0)
	var seg_end = Vector2(10, 0)
	var point = Vector2(5, 5)
	var seg_dist = GeometryOps.point_to_segment_distance(point, seg_start, seg_end)
	print("\npoint_to_segment_distance (", point, " to segment): ", seg_dist)

	# Batch distances
	var points = PackedVector2Array([
		Vector2(5, 5), Vector2(15, 5), Vector2(-5, -5)
	])
	var distances = GeometryOps.distances_to_polygon(points, polygon)
	print("\ndistances_to_polygon: ", distances)
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Procedural Island Generation ---")

	# Generate random points for island shape
	var seed_points = PackedVector2Array()
	for i in range(8):
		var angle = i * PI / 4
		var r = 50.0 + randf() * 20.0
		seed_points.append(Vector2(
			100 + cos(angle) * r,
			100 + sin(angle) * r
		))

	print("1. Generate island outline from random points:")
	print("   Seed points: ", seed_points.size())

	# Get convex hull for basic island shape
	var hull = GeometryOps.convex_hull(seed_points)
	print("   Convex hull: ", hull.size(), " vertices")

	# Calculate island stats
	var area = GeometryOps.polygon_area(hull)
	var centroid = GeometryOps.polygon_centroid(hull)
	print("   Island area: ", "%.0f" % area, " sq units")
	print("   Island center: ", centroid)

	# Create coastal buffer zone
	print("\n2. Create coastal zone (expand by 10):")
	var coastal = GeometryOps.offset_polygon(hull, 10.0)
	var coastal_area = GeometryOps.polygon_area(coastal)
	print("   Coastal zone area: ", "%.0f" % (coastal_area - area), " sq units")

	# Create interior (shrink by 15)
	print("\n3. Create mountain region (shrink by 15):")
	var mountains = GeometryOps.offset_polygon(hull, -15.0)
	if mountains.size() > 0:
		var mountain_area = GeometryOps.polygon_area(mountains)
		print("   Mountain region area: ", "%.0f" % mountain_area, " sq units")
	else:
		print("   Island too small for mountains")

	# Triangulate for rendering
	print("\n4. Triangulate for terrain mesh:")
	var triangles = GeometryOps.triangulate(hull)
	print("   Triangle count: ", triangles.size() / 3)

	# Generate territory regions using Voronoi
	print("\n5. Generate territory regions:")
	var region_seeds = PackedVector2Array()
	for i in range(4):
		region_seeds.append(Vector2(
			80 + randf() * 40,
			80 + randf() * 40
		))

	var bounds = GeometryOps.polygon_bounds(hull)
	var regions = GeometryOps.voronoi_cells(region_seeds, bounds)
	print("   ", regions.size(), " territory regions")

	for i in range(regions.size()):
		var region_area = GeometryOps.polygon_area(regions[i])
		print("   Region ", i, ": ", "%.0f" % region_area, " sq units")

	# Simplify coastline for minimap
	print("\n6. Simplify for minimap:")
	var simplified = GeometryOps.simplify_rdp(hull, 5.0)
	print("   Original: ", hull.size(), " points")
	print("   Simplified: ", simplified.size(), " points")

	# Check spawn point
	print("\n7. Validate spawn point:")
	var spawn = Vector2(100, 100)
	var on_island = GeometryOps.point_in_polygon(spawn, hull)
	print("   Spawn at ", spawn, " on island: ", on_island)

	if not on_island:
		var closest = GeometryOps.closest_point_on_polygon(spawn, hull)
		print("   Closest valid point: ", closest)

	print("")

extends SceneTree

func _init():
	print("Testing AgentiteG...")

	# Test if classes exist
	print("\n=== SpatialHash2D ===")
	var spatial = SpatialHash2D.new()
	print("SpatialHash2D created: ", spatial)
	print("Cell size: ", spatial.cell_size)

	print("\n=== SpatialHash3D ===")
	var spatial3d = SpatialHash3D.new()
	print("SpatialHash3D created: ", spatial3d)
	spatial3d.cell_size = 32.0
	var positions_3d = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(10, 10, 10),
		Vector3(100, 100, 100)
	])
	spatial3d.build(positions_3d)
	var nearby_3d = spatial3d.query_radius(Vector3(5, 5, 5), 20.0)
	print("3D nearby query result: ", nearby_3d)  # Should find indices 0 and 1

	print("\n=== ArrayOps ===")
	var values = PackedFloat32Array([3.0, 1.0, 2.0])
	var result = ArrayOps.filter_gt_float(values, 1.5)
	print("Filter result: ", result)

	print("\n=== MathOps ===")
	# Test normalize
	var vectors_2d = PackedVector2Array([Vector2(3, 4), Vector2(0, 5), Vector2(0, 0)])
	var normalized = MathOps.normalize_batch_2d(vectors_2d)
	print("Normalized 2D: ", normalized)  # Should be (0.6, 0.8), (0, 1), (0, 0)

	# Test dot product
	var a_vecs = PackedVector2Array([Vector2(1, 0), Vector2(1, 1)])
	var b_vecs = PackedVector2Array([Vector2(0, 1), Vector2(1, 1)])
	var dots = MathOps.dot_batch_2d(a_vecs, b_vecs)
	print("Dot products: ", dots)  # Should be 0, 2

	# Test lengths
	var lengths = MathOps.length_batch_2d(vectors_2d)
	print("Lengths: ", lengths)  # Should be 5, 5, 0

	# Test lerp
	var start = PackedVector2Array([Vector2(0, 0), Vector2(10, 10)])
	var end = PackedVector2Array([Vector2(10, 10), Vector2(20, 20)])
	var lerped = MathOps.lerp_batch_2d(start, end, 0.5)
	print("Lerped: ", lerped)  # Should be (5, 5), (15, 15)

	# Test distance
	var distances = MathOps.distance_batch_2d(start, end)
	print("Distances: ", distances)  # Should be ~14.14, ~14.14

	# Test direction
	var dirs = MathOps.direction_batch_2d(start, end)
	print("Directions: ", dirs)  # Normalized direction vectors

	# Test transform
	var xform = Transform2D(0.0, Vector2(100, 50))  # Translation only
	var points = PackedVector2Array([Vector2(0, 0), Vector2(10, 10)])
	var transformed = MathOps.transform_2d_batch(xform, points)
	print("Transformed: ", transformed)  # Should be (100, 50), (110, 60)

	# Test angles
	var angle_vecs = PackedVector2Array([Vector2(1, 0), Vector2(0, 1), Vector2(-1, 0)])
	var angles = MathOps.angle_batch_2d(angle_vecs)
	print("Angles (radians): ", angles)  # Should be 0, PI/2, PI

	# Test clamp length
	var big_vecs = PackedVector2Array([Vector2(100, 0), Vector2(5, 0)])
	var clamped = MathOps.clamp_length_batch_2d(big_vecs, 10.0)
	print("Clamped: ", clamped)  # Should be (10, 0), (5, 0)

	print("\n=== BatchOps ===")
	# Test apply velocities
	var positions = PackedVector2Array([Vector2(0, 0), Vector2(10, 10)])
	var velocities = PackedVector2Array([Vector2(100, 0), Vector2(0, 50)])
	var delta = 0.1
	var new_positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
	print("Apply velocities: ", new_positions)  # Should be (10, 0), (10, 15)

	# Test seek batch
	var seeker_positions = PackedVector2Array([Vector2(0, 0), Vector2(100, 0)])
	var targets = PackedVector2Array([Vector2(100, 0), Vector2(0, 0)])
	var desired = BatchOps.seek_batch(seeker_positions, targets, 10.0)
	print("Seek velocities: ", desired)  # Should be (10, 0), (-10, 0)

	# Test arrive batch
	var arrive_positions = PackedVector2Array([Vector2(0, 0), Vector2(90, 0)])
	var arrive_targets = PackedVector2Array([Vector2(100, 0), Vector2(100, 0)])
	var arrive_desired = BatchOps.arrive_batch(arrive_positions, arrive_targets, 10.0, 50.0)
	print("Arrive velocities: ", arrive_desired)  # Second should be slower (within slowing radius)

	# Test separation
	var crowd_positions = PackedVector2Array([Vector2(0, 0), Vector2(5, 0), Vector2(100, 0)])
	var sep_forces = BatchOps.separation_2d(crowd_positions, 20.0, 1.0)
	print("Separation forces: ", sep_forces)  # First two should push apart, third unaffected

	# Test limit velocity
	var fast_vels = PackedVector2Array([Vector2(100, 0), Vector2(5, 0)])
	var limited = BatchOps.limit_velocity_2d(fast_vels, 10.0)
	print("Limited velocities: ", limited)  # Should be (10, 0), (5, 0)

	# Test flock
	var flock_pos = PackedVector2Array([Vector2(0, 0), Vector2(10, 0), Vector2(5, 10)])
	var flock_vel = PackedVector2Array([Vector2(1, 0), Vector2(1, 0), Vector2(1, 0)])
	var flock_forces = BatchOps.flock_2d(flock_pos, flock_vel, 20.0, 30.0, 30.0, 1.0, 0.5, 0.3)
	print("Flock forces: ", flock_forces)

	print("\n=== RandomOps ===")
	var rng = RandomOps.new()
	rng.seed(12345)
	print("RandomOps created with seed 12345")

	# Test float generation
	var floats = rng.randf_array(5)
	print("Random floats: ", floats)
	assert(floats.size() == 5, "Should generate 5 floats")
	for f in floats:
		assert(f >= 0.0 and f < 1.0, "Float should be in [0, 1)")

	# Test range generation
	var range_floats = rng.randf_range_array(5, 10.0, 20.0)
	print("Range floats [10, 20): ", range_floats)
	for f in range_floats:
		assert(f >= 10.0 and f < 20.0, "Float should be in [10, 20)")

	# Test integer generation
	var ints = rng.randi_range_array(5, 1, 10)
	print("Range ints [1, 10]: ", ints)
	for i in ints:
		assert(i >= 1 and i <= 10, "Int should be in [1, 10]")

	# Test points in shapes
	var rect_points = rng.rand_points_in_rect(100, Rect2(0, 0, 100, 100))
	print("Points in rect (first 3): ", rect_points.slice(0, 3))
	assert(rect_points.size() == 100, "Should generate 100 points")

	var circle_points = rng.rand_points_in_circle(100, Vector2(50, 50), 25.0)
	print("Points in circle (first 3): ", circle_points.slice(0, 3))
	for p in circle_points:
		var dist = p.distance_to(Vector2(50, 50))
		assert(dist <= 25.0, "Point should be within circle radius")

	# Test shuffle
	var indices = rng.shuffle_indices(10)
	print("Shuffled indices: ", indices)
	assert(indices.size() == 10, "Should have 10 indices")

	# Test weighted choice
	var weights = PackedFloat32Array([1.0, 2.0, 3.0])  # 3 is most likely
	var choices = rng.weighted_choice(weights, 100)
	var counts = [0, 0, 0]
	for c in choices:
		counts[c] += 1
	print("Weighted choices (100 samples): ", counts)  # Should roughly be ~17, ~33, ~50

	# Test random directions
	var dirs_2d = rng.rand_directions_2d(5)
	print("Random 2D directions: ", dirs_2d)
	for d in dirs_2d:
		var len = d.length()
		assert(abs(len - 1.0) < 0.001, "Direction should be unit length")

	# Test Poisson disk
	var poisson = rng.poisson_disk_2d(Rect2(0, 0, 100, 100), 10.0, 30)
	print("Poisson disk 2D: ", poisson.size(), " points")
	# Verify minimum distance
	for i in range(poisson.size()):
		for j in range(i + 1, poisson.size()):
			var dist = poisson[i].distance_to(poisson[j])
			assert(dist >= 9.9, "Poisson points should be at least min_distance apart")

	# Test Gaussian distribution
	var gaussian = rng.randn_array(1000)
	var mean_sum = 0.0
	for g in gaussian:
		mean_sum += g
	var mean = mean_sum / gaussian.size()
	print("Gaussian mean (should be ~0): ", mean)
	assert(abs(mean) < 0.2, "Gaussian mean should be close to 0")

	print("\n=== NoiseOps ===")
	var noise = NoiseOps.new()
	noise.set_seed(12345)
	noise.set_octaves(4)
	noise.set_frequency(0.1)
	print("NoiseOps created with seed 12345, 4 octaves, freq 0.1")

	# Test Perlin noise
	var p_val = noise.perlin_2d(Vector2(10.5, 20.3))
	print("Perlin 2D at (10.5, 20.3): ", p_val)
	assert(p_val >= -1.0 and p_val <= 1.0, "Perlin noise should be in [-1, 1]")

	# Test determinism
	var p_val2 = noise.perlin_2d(Vector2(10.5, 20.3))
	assert(abs(p_val - p_val2) < 0.0001, "Same input should give same output")

	# Test batch
	var sample_points = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 10), Vector2(20, 20)
	])
	var batch_vals = noise.perlin_2d_batch(sample_points)
	print("Perlin batch: ", batch_vals)

	# Test grid
	var grid_vals = noise.perlin_2d_grid(Vector2(0, 0), Vector2(1, 1), 4, 4)
	print("Perlin grid 4x4: ", grid_vals.size(), " values")
	assert(grid_vals.size() == 16, "Grid should have 16 values")

	# Test Simplex noise
	var s_val = noise.simplex_2d(Vector2(10.5, 20.3))
	print("Simplex 2D at (10.5, 20.3): ", s_val)

	# Test Worley noise
	var w_val = noise.worley_2d(Vector2(10.5, 20.3))
	print("Worley 2D at (10.5, 20.3): ", w_val)
	assert(w_val >= 0.0, "Worley noise should be non-negative (distance)")

	# Test FBM
	var fbm_val = noise.fbm_2d(Vector2(10.5, 20.3))
	print("FBM 2D at (10.5, 20.3): ", fbm_val)

	# Test ridged
	var ridged_val = noise.ridged_2d(Vector2(10.5, 20.3))
	print("Ridged 2D at (10.5, 20.3): ", ridged_val)

	# Test turbulence
	var turb_val = noise.turbulence_2d(Vector2(10.5, 20.3))
	print("Turbulence 2D at (10.5, 20.3): ", turb_val)
	assert(turb_val >= 0.0, "Turbulence should be non-negative")

	# Test domain warping
	var warp_input = PackedVector2Array([Vector2(0, 0), Vector2(10, 10)])
	var warped = noise.warp_2d(warp_input, 5.0)
	print("Warped positions: ", warped)

	# Test 3D noise
	var p3d = noise.perlin_3d(Vector3(5.0, 5.0, 5.0))
	print("Perlin 3D: ", p3d)

	print("\n=== GridOps ===")
	# Test coordinate conversion
	var idx = GridOps.to_index(5, 3, 10)
	print("to_index(5, 3, 10): ", idx)  # Should be 35
	assert(idx == 35, "to_index should be 35")

	var coords = GridOps.to_coords(35, 10)
	print("to_coords(35, 10): ", coords)  # Should be (5, 3)
	assert(coords.x == 5 and coords.y == 3, "to_coords should be (5, 3)")

	# Test bounds checking
	assert(GridOps.in_bounds(5, 3, 10, 10) == true, "Should be in bounds")
	assert(GridOps.in_bounds(10, 3, 10, 10) == false, "Should be out of bounds")

	# Test neighbors
	var neighbors4 = GridOps.neighbors_4(35, 10, 10)
	print("neighbors_4 of (5, 3): ", neighbors4)  # Should have 4 neighbors
	assert(neighbors4.size() == 4, "Center cell should have 4 neighbors")

	var neighbors8 = GridOps.neighbors_8(35, 10, 10)
	print("neighbors_8 of (5, 3): ", neighbors8)  # Should have 8 neighbors
	assert(neighbors8.size() == 8, "Center cell should have 8 neighbors")

	# Test corner neighbors (fewer)
	var corner_neighbors = GridOps.neighbors_4(0, 10, 10)
	assert(corner_neighbors.size() == 2, "Corner cell should have 2 neighbors (4-connected)")

	# Test flood fill
	var test_grid = PackedInt32Array([
		1, 1, 0, 0, 0,
		1, 1, 0, 1, 1,
		0, 0, 0, 1, 1,
		0, 1, 1, 1, 0,
		0, 1, 1, 0, 0
	])
	var flood = GridOps.flood_fill(test_grid, 5, 0, 1)  # Start at (0,0), find 1s
	print("Flood fill from (0,0) found ", flood.size(), " cells")
	assert(flood.size() == 4, "Should find 4 connected cells")

	# Test line indices
	var line = GridOps.line_indices(Vector2i(0, 0), Vector2i(4, 2), 5)
	print("Line from (0,0) to (4,2): ", line)
	assert(line.size() >= 5, "Line should have at least 5 cells")

	# Test line clear
	var blocking_grid = PackedInt32Array([
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 1, 0, 0,  # Wall at (2, 2)
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
	])
	var clear1 = GridOps.line_clear(blocking_grid, 5, Vector2i(0, 0), Vector2i(4, 4), 1)
	print("Line (0,0) to (4,4) through wall: ", clear1)  # Should be false
	assert(clear1 == false, "Line through wall should not be clear")

	var clear2 = GridOps.line_clear(blocking_grid, 5, Vector2i(0, 0), Vector2i(0, 4), 1)
	print("Line (0,0) to (0,4) no wall: ", clear2)  # Should be true
	assert(clear2 == true, "Line with no wall should be clear")

	# Test connected components
	var comp_count = GridOps.count_connected_components(test_grid, 5, 5, 1)
	print("Connected components of 1s: ", comp_count)
	assert(comp_count == 2, "Should have 2 connected components of 1s")

	# Test distance field
	var target_grid = PackedInt32Array([
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 1, 0, 0,  # Target at center
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0
	])
	var dist_field = GridOps.manhattan_distance_field(target_grid, 5, 5, 1)
	print("Manhattan distance field (center row): ",
		dist_field[10], dist_field[11], dist_field[12], dist_field[13], dist_field[14])
	assert(dist_field[12] == 0, "Target cell distance should be 0")
	assert(dist_field[11] == 1, "Adjacent cell distance should be 1")

	# Test find value
	var found = GridOps.find_value(test_grid, 1)
	print("Found ", found.size(), " cells with value 1")

	print("\n=== PathfindingOps ===")
	# Create a simple cost grid
	var costs = PackedFloat32Array([
		1, 1, 1, 1, 1,
		1, 0, 0, 0, 1,  # Wall in middle
		1, 0, 0, 0, 1,
		1, 0, 0, 0, 1,
		1, 1, 1, 1, 1
	])

	# Test A* around wall
	var path = PathfindingOps.astar_grid(costs, 5, 5, Vector2i(0, 0), Vector2i(4, 4), true)
	print("A* path from (0,0) to (4,4): ", path.size(), " steps")
	assert(path.size() > 0, "Should find a path")
	assert(path[0] == 0, "Path should start at start position")
	assert(path[path.size() - 1] == 24, "Path should end at goal position")

	# Test unreachable
	var blocked_costs = PackedFloat32Array([
		1, 0, 1,
		1, 0, 1,
		1, 0, 1
	])
	var no_path = PathfindingOps.astar_grid(blocked_costs, 3, 3, Vector2i(0, 0), Vector2i(2, 0), false)
	print("Path through wall: ", no_path.size(), " (should be 0)")
	assert(no_path.size() == 0, "Should not find path through wall")

	# Test Dijkstra map
	var simple_costs = PackedFloat32Array([
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	])
	var dijkstra_map = PathfindingOps.dijkstra_map_single(simple_costs, 3, 3, Vector2i(1, 1))
	print("Dijkstra map from center: corner=", dijkstra_map[0], " center=", dijkstra_map[4])
	assert(dijkstra_map[4] == 0, "Center should be distance 0")
	assert(dijkstra_map[0] == 2, "Corner should be distance 2 (Manhattan)")

	# Test flow field
	var flow = PathfindingOps.flow_field(simple_costs, 3, 3, Vector2i(1, 1))
	print("Flow field from center: corner direction=", flow[0])
	assert(flow[4].length() < 0.01, "Flow at goal should be zero")
	assert(flow[0].length() > 0.9, "Flow away from goal should be normalized")

	# Test reachable cells
	var reachable = PathfindingOps.reachable_cells(simple_costs, 3, 3, Vector2i(0, 0), 2.0)
	print("Reachable from (0,0) with cost 2: ", reachable.size(), " cells")
	assert(reachable.size() > 0 and reachable.size() <= 6, "Should reach limited cells")

	# Test is_reachable
	assert(PathfindingOps.is_reachable(simple_costs, 3, 3, Vector2i(0, 0), Vector2i(2, 2)) == true, "Should be reachable")

	# Test path cost
	var cost = PathfindingOps.path_cost(simple_costs, 3, 3, Vector2i(0, 0), Vector2i(2, 2), true)
	print("Path cost (0,0) to (2,2) diagonal: ", cost)
	assert(cost > 0 and cost < 10, "Cost should be reasonable")

	# Test path utilities
	var world_path = PathfindingOps.path_to_vectors(path, 5, 32.0)
	print("World path first point: ", world_path[0])  # Should be (16, 16) - center of cell 0

	# Test JPS on uniform grid
	var walkable_grid = PackedInt32Array([
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1
	])
	var jps_path = PathfindingOps.jps_grid(walkable_grid, 5, 5, Vector2i(0, 0), Vector2i(4, 4))
	print("JPS path: ", jps_path.size(), " steps")
	assert(jps_path.size() > 0, "JPS should find a path")

	print("\n=== CollisionOps ===")
	# Test points in rect
	var test_points = PackedVector2Array([
		Vector2(5, 5), Vector2(15, 15), Vector2(50, 50), Vector2(150, 150)
	])
	var test_rect = Rect2(0, 0, 100, 100)
	var in_rect = CollisionOps.points_in_rect(test_points, test_rect)
	print("Points in rect: ", in_rect)  # Should be [0, 1, 2]
	assert(in_rect.size() == 3, "Should find 3 points in rect")

	# Test points in circle
	var in_circle = CollisionOps.points_in_circle(test_points, Vector2(10, 10), 20.0)
	print("Points in circle: ", in_circle)  # Should be [0, 1]
	assert(in_circle.size() == 2, "Should find 2 points in circle")

	# Test points in polygon
	var polygon = PackedVector2Array([Vector2(0, 0), Vector2(100, 0), Vector2(100, 100), Vector2(0, 100)])
	var in_poly = CollisionOps.points_in_polygon(test_points, polygon)
	print("Points in polygon: ", in_poly)  # Should be [0, 1, 2]
	assert(in_poly.size() == 3, "Should find 3 points in polygon")

	# Test circle collision
	var centers_a = PackedVector2Array([Vector2(0, 0), Vector2(100, 0)])
	var centers_b = PackedVector2Array([Vector2(5, 0), Vector2(200, 0)])
	var collision_pairs = CollisionOps.circles_vs_circles_uniform(centers_a, 10.0, centers_b, 10.0)
	print("Circle collision pairs: ", collision_pairs)  # Should find [0, 0]
	assert(collision_pairs.size() == 2, "Should find 1 collision pair")
	assert(collision_pairs[0] == 0 and collision_pairs[1] == 0, "First circles should collide")

	# Test self-collision
	var self_centers = PackedVector2Array([Vector2(0, 0), Vector2(5, 0), Vector2(100, 0)])
	var self_pairs = CollisionOps.circles_self_collision_uniform(self_centers, 10.0)
	print("Self-collision pairs: ", self_pairs)  # Should find [0, 1]
	assert(self_pairs.size() == 2, "Should find 1 self-collision pair")

	# Test ray first circle
	var ray_centers = PackedVector2Array([Vector2(50, 0), Vector2(100, 0)])
	var ray_radii = PackedFloat32Array([10.0, 10.0])
	var hit = CollisionOps.ray_first_circle(Vector2(0, 0), Vector2(1, 0), 1000.0, ray_centers, ray_radii)
	print("Ray hit first circle: ", hit)  # Should be 0
	assert(hit == 0, "Should hit first circle")

	# Test segment intersection
	var seg_starts_a = PackedVector2Array([Vector2(0, 0)])
	var seg_ends_a = PackedVector2Array([Vector2(10, 10)])
	var seg_starts_b = PackedVector2Array([Vector2(0, 10)])
	var seg_ends_b = PackedVector2Array([Vector2(10, 0)])
	var seg_pairs = CollisionOps.segments_intersect(seg_starts_a, seg_ends_a, seg_starts_b, seg_ends_b)
	print("Segment intersection pairs: ", seg_pairs)  # Should be [0, 0]
	assert(seg_pairs.size() == 2, "Should find 1 segment intersection")

	print("\n=== GeometryOps ===")
	# Test convex hull
	var hull_points = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 0), Vector2(5, 5), Vector2(10, 10), Vector2(0, 10)
	])
	var hull = GeometryOps.convex_hull(hull_points)
	print("Convex hull: ", hull.size(), " points")
	assert(hull.size() == 4, "Hull should have 4 points (interior point removed)")

	# Test polygon area
	var square = PackedVector2Array([Vector2(0, 0), Vector2(10, 0), Vector2(10, 10), Vector2(0, 10)])
	var area = GeometryOps.polygon_area(square)
	print("Square area: ", area)  # Should be 100
	assert(abs(area - 100.0) < 0.01, "Square area should be 100")

	# Test polygon centroid
	var centroid = GeometryOps.polygon_centroid(square)
	print("Square centroid: ", centroid)  # Should be (5, 5)
	assert(abs(centroid.x - 5.0) < 0.01 and abs(centroid.y - 5.0) < 0.01, "Centroid should be (5, 5)")

	# Test polygon is convex
	assert(GeometryOps.polygon_is_convex(square) == true, "Square should be convex")

	# Test triangulation
	var tris = GeometryOps.triangulate(square)
	print("Square triangulation: ", tris.size() / 3, " triangles")
	assert(tris.size() == 6, "Square should triangulate to 2 triangles (6 indices)")

	# Test simplification
	var polyline = PackedVector2Array([
		Vector2(0, 0), Vector2(1, 0.1), Vector2(2, 0), Vector2(3, 0.1), Vector2(4, 0),
		Vector2(5, 5), Vector2(6, 4.9), Vector2(7, 5), Vector2(8, 5.1), Vector2(9, 5)
	])
	var simplified = GeometryOps.simplify_rdp(polyline, 1.0)
	print("Simplified polyline: ", simplified.size(), " points (from ", polyline.size(), ")")
	assert(simplified.size() < polyline.size(), "Simplified should have fewer points")

	# Test point in polygon
	var inside = GeometryOps.point_in_polygon(Vector2(5, 5), square)
	var outside = GeometryOps.point_in_polygon(Vector2(15, 5), square)
	print("Point (5,5) in square: ", inside)
	print("Point (15,5) in square: ", outside)
	assert(inside == true, "(5,5) should be inside square")
	assert(outside == false, "(15,5) should be outside square")

	# Test polygon offset
	var expanded = GeometryOps.offset_polygon(square, 5.0)
	var expanded_area = GeometryOps.polygon_area(expanded)
	print("Expanded square area: ", expanded_area, " (original: 100)")
	assert(expanded_area > area, "Expanded polygon should have larger area")

	# Test distance to polygon
	var dist = GeometryOps.point_to_polygon_distance(Vector2(15, 5), square)
	print("Distance from (15, 5) to square: ", dist)
	assert(abs(dist - 5.0) < 0.01, "Distance should be 5")

	# Test closest point
	var closest = GeometryOps.closest_point_on_polygon(Vector2(15, 5), square)
	print("Closest point on square to (15, 5): ", closest)
	assert(abs(closest.x - 10.0) < 0.01 and abs(closest.y - 5.0) < 0.01, "Closest should be (10, 5)")

	# Test polygon bounds
	var bounds = GeometryOps.polygon_bounds(square)
	print("Square bounds: ", bounds)
	assert(bounds.position.x == 0 and bounds.position.y == 0, "Bounds position should be (0, 0)")
	assert(bounds.size.x == 10 and bounds.size.y == 10, "Bounds size should be (10, 10)")

	print("\nAll tests passed!")
	quit(0)

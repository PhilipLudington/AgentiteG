extends SceneTree
## Test Phase 5 (Advanced Spatial) and Phase 6 (Interpolation & Stats)

var tests_passed := 0
var tests_failed := 0
var current_test := ""

func _init() -> void:
	print("=" .repeat(60))
	print("AgentiteG Phase 5 & 6 Test Suite")
	print("=" .repeat(60))
	print("")

	run_kdtree2d_tests()
	run_kdtree3d_tests()
	run_quadtree_tests()
	run_octree_tests()
	run_interpolation_tests()
	run_stat_tests()

	print("")
	print("=" .repeat(60))
	print("Results: %d passed, %d failed" % [tests_passed, tests_failed])
	print("=" .repeat(60))

	if tests_failed > 0:
		quit(1)
	else:
		quit(0)


func run_kdtree2d_tests() -> void:
	print("--- KDTree2D Tests ---")

	# Test: KDTree2D creation and basic query
	current_test = "KDTree2D creation"
	var kdtree = KDTree2D.new()
	check(kdtree != null, "Should create KDTree2D")

	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(10, 0),
		Vector2(100, 0),
		Vector2(50, 50),
	])

	kdtree.build(positions)
	check(kdtree.size() == 4, "Should have 4 items")
	pass_test()

	# Test: query_nearest_one
	current_test = "KDTree2D query_nearest_one"
	var nearest = kdtree.query_nearest_one(Vector2(5, 0))
	check(nearest == 1, "Nearest to (5,0) should be index 1 (at 10,0)")
	pass_test()

	# Test: query_nearest
	current_test = "KDTree2D query_nearest"
	var k_nearest = kdtree.query_nearest(Vector2.ZERO, 2)
	check(k_nearest.size() == 2, "Should return 2 nearest")
	check(k_nearest[0] == 0, "Nearest should be index 0")
	check(k_nearest[1] == 1, "Second should be index 1")
	pass_test()

	# Test: query_radius
	current_test = "KDTree2D query_radius"
	var in_radius = kdtree.query_radius(Vector2.ZERO, 15.0)
	check(in_radius.size() == 2, "Should find 2 items in radius 15")
	pass_test()

	print("")


func run_kdtree3d_tests() -> void:
	print("--- KDTree3D Tests ---")

	current_test = "KDTree3D creation and query"
	var kdtree = KDTree3D.new()

	var positions = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(10, 0, 0),
		Vector3(100, 0, 0),
	])

	kdtree.build(positions)
	check(kdtree.size() == 3, "Should have 3 items")

	var nearest = kdtree.query_nearest_one(Vector3(5, 0, 0))
	check(nearest == 1 or nearest == 0, "Nearest should be close to origin")
	pass_test()

	print("")


func run_quadtree_tests() -> void:
	print("--- QuadTree Tests ---")

	current_test = "QuadTree creation and query"
	var quadtree = QuadTree.new()
	quadtree.set_bounds(Rect2(0, 0, 1000, 1000))

	var positions = PackedVector2Array([
		Vector2(100, 100),
		Vector2(200, 200),
		Vector2(800, 800),
	])

	quadtree.build(positions)
	check(quadtree.size() == 3, "Should have 3 items")
	pass_test()

	# Test: query_rect
	current_test = "QuadTree query_rect"
	var in_rect = quadtree.query_rect(Rect2(0, 0, 300, 300))
	check(in_rect.size() == 2, "Should find 2 items in rect")
	pass_test()

	# Test: query_radius
	current_test = "QuadTree query_radius"
	var in_radius = quadtree.query_radius(Vector2(100, 100), 150.0)
	check(in_radius.size() == 2, "Should find 2 items in radius")
	pass_test()

	# Test: get_node_bounds for visualization
	current_test = "QuadTree get_node_bounds"
	var bounds = quadtree.get_node_bounds()
	check(bounds.size() > 0, "Should have node bounds for visualization")
	pass_test()

	print("")


func run_octree_tests() -> void:
	print("--- Octree Tests ---")

	current_test = "Octree creation and query"
	var octree = Octree.new()
	octree.set_bounds(AABB(Vector3.ZERO, Vector3(1000, 1000, 1000)))

	var positions = PackedVector3Array([
		Vector3(100, 100, 100),
		Vector3(200, 200, 200),
		Vector3(800, 800, 800),
	])

	octree.build(positions)
	check(octree.size() == 3, "Should have 3 items")
	pass_test()

	# Test: query_box
	current_test = "Octree query_box"
	var in_box = octree.query_box(AABB(Vector3.ZERO, Vector3(300, 300, 300)))
	check(in_box.size() == 2, "Should find 2 items in box")
	pass_test()

	# Test: query_radius
	current_test = "Octree query_radius"
	var in_radius = octree.query_radius(Vector3(100, 100, 100), 200.0)
	check(in_radius.size() == 2, "Should find 2 items in radius")
	pass_test()

	print("")


func run_interpolation_tests() -> void:
	print("--- InterpolationOps Tests ---")

	# Test: easing functions
	current_test = "InterpolationOps ease_out_quad"
	var t = PackedFloat32Array([0.0, 0.5, 1.0])
	var eased = InterpolationOps.ease_out_quad(t)
	check(eased.size() == 3, "Should return 3 values")
	check(abs(eased[0]) < 0.001, "ease_out_quad(0) should be 0")
	check(abs(eased[2] - 1.0) < 0.001, "ease_out_quad(1) should be 1")
	pass_test()

	# Test: bezier curves
	current_test = "InterpolationOps bezier_cubic_2d"
	t = PackedFloat32Array([0.0, 0.5, 1.0])
	var curve = InterpolationOps.bezier_cubic_2d(
		Vector2(0, 0), Vector2(0, 100), Vector2(100, 100), Vector2(100, 0), t
	)
	check(curve.size() == 3, "Should return 3 points")
	check(curve[0].is_equal_approx(Vector2(0, 0)), "Start should be (0, 0)")
	check(curve[2].is_equal_approx(Vector2(100, 0)), "End should be (100, 0)")
	pass_test()

	# Test: linspace
	current_test = "InterpolationOps linspace"
	var space = InterpolationOps.linspace(0.0, 1.0, 5)
	check(space.size() == 5, "Should return 5 values")
	check(abs(space[0]) < 0.001, "First should be 0")
	check(abs(space[4] - 1.0) < 0.001, "Last should be 1")
	check(abs(space[2] - 0.5) < 0.001, "Middle should be 0.5")
	pass_test()

	# Test: smoothstep
	current_test = "InterpolationOps smoothstep"
	t = PackedFloat32Array([0.0, 0.5, 1.0])
	var smooth = InterpolationOps.smoothstep(t)
	check(abs(smooth[0]) < 0.001, "smoothstep(0) should be 0")
	check(abs(smooth[2] - 1.0) < 0.001, "smoothstep(1) should be 1")
	check(abs(smooth[1] - 0.5) < 0.001, "smoothstep(0.5) should be 0.5")
	pass_test()

	# Test: catmull_rom_2d
	current_test = "InterpolationOps catmull_rom_2d"
	var control_points = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 10), Vector2(20, 10), Vector2(30, 0)
	])
	var spline = InterpolationOps.catmull_rom_2d(control_points, 5)
	check(spline.size() > 0, "Should return spline points")
	pass_test()

	print("")


func run_stat_tests() -> void:
	print("--- StatOps Tests ---")

	var values = PackedFloat32Array([1.0, 2.0, 3.0, 4.0, 5.0])

	# Test: mean
	current_test = "StatOps mean"
	var avg = StatOps.mean(values)
	check(abs(avg - 3.0) < 0.001, "Mean should be 3.0")
	pass_test()

	# Test: median
	current_test = "StatOps median"
	var med = StatOps.median(values)
	check(abs(med - 3.0) < 0.001, "Median should be 3.0")
	pass_test()

	# Test: std_dev
	current_test = "StatOps std_dev"
	var std = StatOps.std_dev(values)
	check(std > 0, "Std dev should be positive")
	pass_test()

	# Test: min_max
	current_test = "StatOps min_max"
	var mm = StatOps.min_max(values)
	check(abs(mm.x - 1.0) < 0.001, "Min should be 1.0")
	check(abs(mm.y - 5.0) < 0.001, "Max should be 5.0")
	pass_test()

	# Test: sum
	current_test = "StatOps sum"
	var total = StatOps.sum(values)
	check(abs(total - 15.0) < 0.001, "Sum should be 15.0")
	pass_test()

	# Test: percentile
	current_test = "StatOps percentile"
	var p50 = StatOps.percentile(values, 50.0)
	check(abs(p50 - 3.0) < 0.001, "50th percentile should be 3.0")
	pass_test()

	# Test: normalize_min_max
	current_test = "StatOps normalize_min_max"
	var normalized = StatOps.normalize_min_max(values)
	check(normalized.size() == 5, "Should return 5 values")
	check(abs(normalized[0]) < 0.001, "First normalized should be 0")
	check(abs(normalized[4] - 1.0) < 0.001, "Last normalized should be 1")
	pass_test()

	# Test: histogram
	current_test = "StatOps histogram"
	var hist = StatOps.histogram(values, 5)
	check(hist.size() == 5, "Should have 5 bins")
	pass_test()

	# Test: cumsum
	current_test = "StatOps cumsum"
	var cumulative = StatOps.cumsum(values)
	check(cumulative.size() == 5, "Should return 5 values")
	check(abs(cumulative[0] - 1.0) < 0.001, "First cumsum should be 1")
	check(abs(cumulative[4] - 15.0) < 0.001, "Last cumsum should be 15")
	pass_test()

	# Test: exponential_moving_average
	current_test = "StatOps exponential_moving_average"
	var ema = StatOps.exponential_moving_average(values, 0.5)
	check(ema.size() == 5, "Should return 5 values")
	check(abs(ema[0] - 1.0) < 0.001, "First EMA should equal first value")
	pass_test()

	# Test: outliers_zscore - use larger dataset for reliable detection
	current_test = "StatOps outliers_zscore"
	var with_outlier = PackedFloat32Array([1.0, 1.1, 1.2, 0.9, 1.0, 1.1, 0.8, 1.0, 1.2, 100.0])
	var outliers = StatOps.outliers_zscore(with_outlier, 2.0)
	# 100.0 should be detected as an outlier since it's far from mean ~1.0
	check(outliers.size() > 0 and 9 in outliers, "Should detect index 9 (value 100) as outlier")
	pass_test()

	print("")


func check(condition: bool, message: String) -> void:
	if not condition:
		print("  ✗ %s - FAILED: %s" % [current_test, message])
		tests_failed += 1


func pass_test() -> void:
	print("  ✓ %s" % current_test)
	tests_passed += 1

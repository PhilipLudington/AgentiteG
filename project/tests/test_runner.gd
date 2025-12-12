extends SceneTree
## AgentiteG Test Runner
##
## Run with: godot --headless -s tests/test_runner.gd

var tests_passed := 0
var tests_failed := 0
var current_test := ""

func _init() -> void:
	print("=" .repeat(60))
	print("AgentiteG Test Suite")
	print("=" .repeat(60))
	print("")

	run_spatial_hash_tests()
	run_array_ops_tests()

	print("")
	print("=" .repeat(60))
	print("Results: %d passed, %d failed" % [tests_passed, tests_failed])
	print("=" .repeat(60))

	if tests_failed > 0:
		quit(1)
	else:
		quit(0)


func run_spatial_hash_tests() -> void:
	print("--- SpatialHash2D Tests ---")

	# Test: SpatialHash2D creation
	current_test = "SpatialHash2D creation"
	var spatial = SpatialHash2D.new()
	check(spatial != null, "Should create SpatialHash2D")
	check(spatial.cell_size == 64.0, "Default cell_size should be 64.0")
	pass_test()

	# Test: SpatialHash2D build and query
	current_test = "SpatialHash2D build and query"
	spatial = SpatialHash2D.new()
	spatial.cell_size = 50.0

	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(100, 0),
		Vector2(200, 0),
		Vector2(30, 30),  # ~42.4 units from origin
	])

	spatial.build(positions)
	check(spatial.get_count() == 4, "Should have 4 items")

	var nearby = spatial.query_radius(Vector2(0, 0), 60.0)
	check(nearby.size() == 2, "Should find 2 items within radius 60 of origin")
	check(0 in nearby, "Should include item at origin")
	check(3 in nearby, "Should include item at (30, 30)")
	pass_test()

	# Test: SpatialHash2D query_rect
	current_test = "SpatialHash2D query_rect"
	spatial = SpatialHash2D.new()

	positions = PackedVector2Array([
		Vector2(10, 10),
		Vector2(90, 90),
		Vector2(150, 150),
	])

	spatial.build(positions)

	var in_rect = spatial.query_rect(Rect2(0, 0, 100, 100))
	check(in_rect.size() == 2, "Should find 2 items in rect")
	pass_test()

	# Test: SpatialHash2D query_nearest
	current_test = "SpatialHash2D query_nearest"
	spatial = SpatialHash2D.new()

	positions = PackedVector2Array([
		Vector2(100, 0),
		Vector2(10, 0),
		Vector2(50, 0),
	])

	spatial.build(positions)

	var nearest = spatial.query_nearest(Vector2.ZERO, 2)
	check(nearest.size() == 2, "Should return 2 nearest")
	check(nearest[0] == 1, "Nearest should be index 1 (at 10,0)")
	check(nearest[1] == 2, "Second nearest should be index 2 (at 50,0)")
	pass_test()

	# Test: SpatialHash2D has_any_in_radius
	current_test = "SpatialHash2D has_any_in_radius"
	spatial = SpatialHash2D.new()

	positions = PackedVector2Array([
		Vector2(100, 100),
	])

	spatial.build(positions)

	check(spatial.has_any_in_radius(Vector2(100, 100), 10.0) == true, "Should find item at exact position")
	check(spatial.has_any_in_radius(Vector2.ZERO, 10.0) == false, "Should not find item far away")
	pass_test()

	# Test: SpatialHash2D count_in_radius
	current_test = "SpatialHash2D count_in_radius"
	spatial = SpatialHash2D.new()

	positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(5, 0),
		Vector2(100, 0),
	])

	spatial.build(positions)

	check(spatial.count_in_radius(Vector2.ZERO, 10.0) == 2, "Should count 2 items in radius")
	pass_test()

	print("")


func run_array_ops_tests() -> void:
	print("--- ArrayOps Tests ---")

	# Test: filter_gt_float
	current_test = "ArrayOps filter_gt_float"
	var values = PackedFloat32Array([1.0, 5.0, 3.0, 8.0, 2.0])
	var result = ArrayOps.filter_gt_float(values, 3.0)
	check(result.size() == 2, "Should find 2 values > 3.0")
	check(1 in result, "Should include index 1 (value 5.0)")
	check(3 in result, "Should include index 3 (value 8.0)")
	pass_test()

	# Test: filter_range_float
	current_test = "ArrayOps filter_range_float"
	values = PackedFloat32Array([1.0, 5.0, 3.0, 8.0, 2.0])
	result = ArrayOps.filter_range_float(values, 2.0, 5.0)
	check(result.size() == 3, "Should find 3 values in range [2.0, 5.0]")
	pass_test()

	# Test: filter_eq_int
	current_test = "ArrayOps filter_eq_int"
	var int_values = PackedInt32Array([1, 2, 3, 2, 4, 2])
	var int_result = ArrayOps.filter_eq_int(int_values, 2)
	check(int_result.size() == 3, "Should find 3 occurrences of 2")
	pass_test()

	# Test: filter_in_radius
	current_test = "ArrayOps filter_in_radius"
	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(5, 0),
		Vector2(100, 0),
	])
	result = ArrayOps.filter_in_radius(positions, Vector2.ZERO, 10.0)
	check(result.size() == 2, "Should find 2 positions in radius")
	pass_test()

	# Test: sort_floats
	current_test = "ArrayOps sort_floats"
	values = PackedFloat32Array([3.0, 1.0, 4.0, 1.0, 5.0])
	var sorted = ArrayOps.sort_floats(values)
	check(sorted[0] == 1.0, "First should be 1.0")
	check(sorted[4] == 5.0, "Last should be 5.0")

	var sorted_desc = ArrayOps.sort_floats(values, true)
	check(sorted_desc[0] == 5.0, "First descending should be 5.0")
	pass_test()

	# Test: argsort_floats
	current_test = "ArrayOps argsort_floats"
	values = PackedFloat32Array([3.0, 1.0, 4.0])
	var indices = ArrayOps.argsort_floats(values)
	check(indices[0] == 1, "First index should be 1 (value 1.0)")
	check(indices[1] == 0, "Second index should be 0 (value 3.0)")
	check(indices[2] == 2, "Third index should be 2 (value 4.0)")
	pass_test()

	# Test: argsort_by_distance
	current_test = "ArrayOps argsort_by_distance"
	positions = PackedVector2Array([
		Vector2(100, 0),
		Vector2(10, 0),
		Vector2(50, 0),
	])
	indices = ArrayOps.argsort_by_distance(positions, Vector2.ZERO)
	check(indices[0] == 1, "Nearest should be index 1")
	check(indices[1] == 2, "Second should be index 2")
	check(indices[2] == 0, "Farthest should be index 0")
	pass_test()

	# Test: sum_floats
	current_test = "ArrayOps sum_floats"
	values = PackedFloat32Array([1.0, 2.0, 3.0, 4.0])
	var total = ArrayOps.sum_floats(values)
	check(abs(total - 10.0) < 0.001, "Sum should be 10.0")
	pass_test()

	# Test: sum_vector2
	current_test = "ArrayOps sum_vector2"
	var vectors = PackedVector2Array([
		Vector2(1, 0),
		Vector2(0, 1),
		Vector2(2, 3),
	])
	var vec_total = ArrayOps.sum_vector2(vectors)
	check(vec_total.is_equal_approx(Vector2(3, 4)), "Sum should be (3, 4)")
	pass_test()

	# Test: min/max
	current_test = "ArrayOps min/max"
	values = PackedFloat32Array([3.0, 1.0, 4.0, 1.0, 5.0])
	check(ArrayOps.min_float(values) == 1.0, "Min should be 1.0")
	check(ArrayOps.max_float(values) == 5.0, "Max should be 5.0")
	pass_test()

	# Test: argmin/argmax
	current_test = "ArrayOps argmin/argmax"
	values = PackedFloat32Array([3.0, 1.0, 4.0, 5.0])
	check(ArrayOps.argmin_floats(values) == 1, "Argmin should be 1")
	check(ArrayOps.argmax_floats(values) == 3, "Argmax should be 3")
	pass_test()

	# Test: mean
	current_test = "ArrayOps mean"
	values = PackedFloat32Array([2.0, 4.0, 6.0, 8.0])
	var avg = ArrayOps.mean_floats(values)
	check(abs(avg - 5.0) < 0.001, "Mean should be 5.0")
	pass_test()

	# Test: select
	current_test = "ArrayOps select"
	values = PackedFloat32Array([10.0, 20.0, 30.0, 40.0, 50.0])
	indices = PackedInt32Array([0, 2, 4])
	var selected = ArrayOps.select_floats(values, indices)
	check(selected.size() == 3, "Should select 3 values")
	check(selected[0] == 10.0, "First selected should be 10.0")
	check(selected[1] == 30.0, "Second selected should be 30.0")
	check(selected[2] == 50.0, "Third selected should be 50.0")
	pass_test()

	# Test: reorder
	current_test = "ArrayOps reorder"
	values = PackedFloat32Array([10.0, 20.0, 30.0])
	indices = PackedInt32Array([2, 0, 1])
	var reordered = ArrayOps.reorder_floats(values, indices)
	check(reordered[0] == 30.0, "First should be 30.0")
	check(reordered[1] == 10.0, "Second should be 10.0")
	check(reordered[2] == 20.0, "Third should be 20.0")
	pass_test()

	print("")


func check(condition: bool, message: String) -> void:
	if not condition:
		print("  ✗ %s - FAILED: %s" % [current_test, message])
		tests_failed += 1


func pass_test() -> void:
	print("  ✓ %s" % current_test)
	tests_passed += 1

extends SceneTree
## AgentiteG Benchmark Suite
##
## Compares AgentiteG performance against pure GDScript implementations.
## Run with: godot --headless -s benchmarks/benchmark_runner.gd

const ITERATIONS := 100
const ITEM_COUNT := 10000

func _init() -> void:
	print("=" .repeat(60))
	print("AgentiteG Benchmark Suite")
	print("=" .repeat(60))
	print("Iterations: %d, Items: %d" % [ITERATIONS, ITEM_COUNT])
	print("")

	benchmark_spatial_queries_2d()
	benchmark_spatial_queries_3d()
	benchmark_array_filter()
	benchmark_array_sort()
	benchmark_array_reduce()

	print("")
	print("Benchmark complete!")
	quit(0)


func benchmark_spatial_queries_2d() -> void:
	print("--- SpatialHash2D Query Benchmarks ---")

	# Generate random positions
	var positions := PackedVector2Array()
	positions.resize(ITEM_COUNT)
	for i in ITEM_COUNT:
		positions[i] = Vector2(randf() * 1000, randf() * 1000)

	var origin := Vector2(500, 500)
	var radius := 100.0

	# GDScript implementation
	var gdscript_time := benchmark(func():
		var result := []
		var radius_sq := radius * radius
		for i in positions.size():
			if origin.distance_squared_to(positions[i]) <= radius_sq:
				result.append(i)
	)

	# AgentiteG SpatialHash2D
	var spatial := SpatialHash2D.new()
	spatial.cell_size = 50.0
	spatial.build(positions)

	var agentite_time := benchmark(func():
		var _result := spatial.query_radius(origin, radius)
	)

	print_comparison("2D Radius query", gdscript_time, agentite_time)

	# ArrayOps filter_in_radius (no spatial structure)
	var arrayops_time := benchmark(func():
		var _result := ArrayOps.filter_in_radius(positions, origin, radius)
	)

	print_comparison("2D Radius filter (no spatial)", gdscript_time, arrayops_time)
	print("")


func benchmark_spatial_queries_3d() -> void:
	print("--- SpatialHash3D Query Benchmarks ---")

	# Generate random 3D positions
	var positions := PackedVector3Array()
	positions.resize(ITEM_COUNT)
	for i in ITEM_COUNT:
		positions[i] = Vector3(randf() * 1000, randf() * 1000, randf() * 1000)

	var origin := Vector3(500, 500, 500)
	var radius := 100.0

	# GDScript implementation
	var gdscript_time := benchmark(func():
		var result := []
		var radius_sq := radius * radius
		for i in positions.size():
			if origin.distance_squared_to(positions[i]) <= radius_sq:
				result.append(i)
	)

	# AgentiteG SpatialHash3D
	var spatial := SpatialHash3D.new()
	spatial.cell_size = 50.0
	spatial.build(positions)

	var agentite_time := benchmark(func():
		var _result := spatial.query_radius(origin, radius)
	)

	print_comparison("3D Radius query", gdscript_time, agentite_time)

	# Box query
	var box := AABB(Vector3(400, 400, 400), Vector3(200, 200, 200))

	var gdscript_box_time := benchmark(func():
		var result := []
		for i in positions.size():
			if box.has_point(positions[i]):
				result.append(i)
	)

	var agentite_box_time := benchmark(func():
		var _result := spatial.query_box(box)
	)

	print_comparison("3D Box query", gdscript_box_time, agentite_box_time)
	print("")


func benchmark_array_filter() -> void:
	print("--- Array Filter Benchmarks ---")

	var values := PackedFloat32Array()
	values.resize(ITEM_COUNT)
	for i in ITEM_COUNT:
		values[i] = randf() * 100.0

	var threshold := 50.0

	# GDScript
	var gdscript_time := benchmark(func():
		var result := []
		for i in values.size():
			if values[i] > threshold:
				result.append(i)
	)

	# AgentiteG
	var agentite_time := benchmark(func():
		var _result := ArrayOps.filter_gt_float(values, threshold)
	)

	print_comparison("Filter > threshold", gdscript_time, agentite_time)
	print("")


func benchmark_array_sort() -> void:
	print("--- Array Sort Benchmarks ---")

	var values := PackedFloat32Array()
	values.resize(ITEM_COUNT)
	for i in ITEM_COUNT:
		values[i] = randf() * 100.0

	# GDScript (using Array.sort)
	var gdscript_time := benchmark(func():
		var arr := Array(values)
		arr.sort()
	)

	# AgentiteG
	var agentite_time := benchmark(func():
		var _result := ArrayOps.sort_floats(values)
	)

	print_comparison("Sort floats", gdscript_time, agentite_time)

	# Argsort comparison
	var gdscript_argsort_time := benchmark(func():
		var indices := range(values.size())
		indices.sort_custom(func(a, b): return values[a] < values[b])
	)

	var agentite_argsort_time := benchmark(func():
		var _result := ArrayOps.argsort_floats(values)
	)

	print_comparison("Argsort floats", gdscript_argsort_time, agentite_argsort_time)
	print("")


func benchmark_array_reduce() -> void:
	print("--- Array Reduce Benchmarks ---")

	var values := PackedFloat32Array()
	values.resize(ITEM_COUNT)
	for i in ITEM_COUNT:
		values[i] = randf() * 100.0

	# Sum - GDScript
	var gdscript_sum_time := benchmark(func():
		var total := 0.0
		for v in values:
			total += v
	)

	# Sum - AgentiteG
	var agentite_sum_time := benchmark(func():
		var _total := ArrayOps.sum_floats(values)
	)

	print_comparison("Sum floats", gdscript_sum_time, agentite_sum_time)

	# Argmin - GDScript
	var gdscript_argmin_time := benchmark(func():
		var min_idx := 0
		var min_val: float = values[0]
		for i in range(1, values.size()):
			if values[i] < min_val:
				min_val = values[i]
				min_idx = i
	)

	# Argmin - AgentiteG
	var agentite_argmin_time := benchmark(func():
		var _idx := ArrayOps.argmin_floats(values)
	)

	print_comparison("Argmin floats", gdscript_argmin_time, agentite_argmin_time)
	print("")


func benchmark(test_func: Callable) -> float:
	var start := Time.get_ticks_usec()
	for i in ITERATIONS:
		test_func.call()
	var end := Time.get_ticks_usec()
	return (end - start) / 1000.0 / ITERATIONS  # ms per iteration


func print_comparison(name: String, gdscript_ms: float, agentite_ms: float) -> void:
	var speedup := gdscript_ms / agentite_ms if agentite_ms > 0 else 0.0
	print("  %s:" % name)
	print("    GDScript:  %.3f ms" % gdscript_ms)
	print("    AgentiteG: %.3f ms (%.1fx faster)" % [agentite_ms, speedup])

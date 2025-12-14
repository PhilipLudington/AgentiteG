extends BenchmarkScenario
## Distance Scenario - MathOps batch distance vs GDScript loop

var reference_point: Vector2
var distances: PackedFloat32Array
var nearest_indices: PackedInt32Array
const K_NEAREST := 10


func get_name() -> String:
	return "Distance Calc"


func get_description() -> String:
	return "[b]Distance Calculations[/b]\n\n" + \
		"Calculates distance from reference point to all entities, finds K nearest.\n\n" + \
		"[color=gray]GDScript:[/color] Loop calling distance_to() + manual argmin\n\n" + \
		"[color=cyan]AgentiteG:[/color] MathOps.distance_batch_2d() + ArrayOps.argsort_floats()\n\n" + \
		"[i]Expected speedup: 15-40x[/i]"


func _setup_entities() -> void:
	super._setup_entities()

	reference_point = visualization_bounds.get_center()
	distances = PackedFloat32Array()
	distances.resize(entity_count)
	nearest_indices = PackedInt32Array()


func run_gdscript() -> void:
	# Calculate distances manually
	for i in positions.size():
		distances[i] = reference_point.distance_to(positions[i])

	# Find k nearest (simple selection)
	var indices := range(entity_count)
	indices.sort_custom(func(a, b): return distances[a] < distances[b])

	var result := PackedInt32Array()
	for i in min(K_NEAREST, indices.size()):
		result.append(indices[i])

	gdscript_result = result
	nearest_indices = result


func run_agentiteg() -> void:
	# Calculate all distances at once
	var ref_array := PackedVector2Array()
	ref_array.resize(entity_count)
	ref_array.fill(reference_point)

	distances = MathOps.distance_batch_2d(positions, ref_array)

	# Find k nearest
	var sorted_indices := ArrayOps.argsort_floats(distances)
	var result := PackedInt32Array()
	for i in min(K_NEAREST, sorted_indices.size()):
		result.append(sorted_indices[i])

	agentiteg_result = result
	nearest_indices = result


func update_visualization(delta: float) -> void:
	# Move reference point in a figure-8 pattern
	var time := Time.get_ticks_msec() / 1000.0
	var center := visualization_bounds.get_center()
	var range_x := visualization_bounds.size.x * 0.35
	var range_y := visualization_bounds.size.y * 0.35

	reference_point = center + Vector2(
		sin(time * 0.4) * range_x,
		sin(time * 0.8) * range_y
	)

	_update_colors()


func _update_colors() -> void:
	if distances.size() != entity_count:
		return

	# Color by distance (gradient)
	var max_dist := visualization_bounds.size.length() * 0.5

	for i in entity_count:
		var t := clampf(distances[i] / max_dist, 0.0, 1.0)
		# Near = bright cyan, Far = dark blue
		colors[i] = Color(0.2, 0.4 + 0.5 * (1.0 - t), 0.6 + 0.4 * (1.0 - t), 0.7)

	# Highlight k nearest
	for idx in nearest_indices:
		if idx < colors.size():
			colors[idx] = Color(1.0, 0.9, 0.2, 1.0)  # Yellow


func get_draw_data() -> Dictionary:
	var data := super.get_draw_data()

	# Draw reference point
	data["circles"] = [{
		"center": reference_point,
		"radius": 8.0,
		"color": Color(1.0, 0.3, 0.3, 1.0),
		"filled": true
	}]

	# Draw lines to k nearest
	var lines := PackedVector2Array()
	for idx in nearest_indices:
		if idx < positions.size():
			lines.append(reference_point)
			lines.append(positions[idx])

	data["lines"] = lines

	return data

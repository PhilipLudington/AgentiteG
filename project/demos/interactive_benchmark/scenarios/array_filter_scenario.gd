extends BenchmarkScenario
## Array Filter Scenario - ArrayOps filtering vs GDScript loop

var values: PackedFloat32Array
var threshold: float = 50.0
var filtered_indices: PackedInt32Array


func get_name() -> String:
	return "Array Filter"


func get_description() -> String:
	return "[b]Array Filter & Sort[/b]\n\n" + \
		"Filters array to find values above threshold, then finds indices sorted by value.\n\n" + \
		"[color=gray]GDScript:[/color] Loop with condition check + Array.sort_custom()\n\n" + \
		"[color=cyan]AgentiteG:[/color] ArrayOps.filter_gt_float() + argsort_floats()\n\n" + \
		"[i]Expected speedup: 10-30x[/i]"


func _setup_entities() -> void:
	super._setup_entities()

	# Generate random values for each entity (representing "health" or similar)
	values = PackedFloat32Array()
	values.resize(entity_count)

	for i in entity_count:
		values[i] = randf() * 100.0

	filtered_indices = PackedInt32Array()
	_update_colors()


func run_gdscript() -> void:
	# Filter values > threshold
	var filtered := []
	for i in values.size():
		if values[i] > threshold:
			filtered.append(i)

	# Sort by value (descending)
	filtered.sort_custom(func(a, b): return values[a] > values[b])

	gdscript_result = filtered
	filtered_indices = PackedInt32Array(filtered)


func run_agentiteg() -> void:
	# Filter and sort using AgentiteG
	var above_threshold := ArrayOps.filter_gt_float(values, threshold)

	# Get the values for filtered indices and argsort them
	var filtered_values := ArrayOps.select_floats(values, above_threshold)
	var sorted_local := ArrayOps.argsort_floats(filtered_values)

	# Map back to global indices (reversed for descending)
	var result := PackedInt32Array()
	result.resize(sorted_local.size())
	for i in sorted_local.size():
		result[i] = above_threshold[sorted_local[sorted_local.size() - 1 - i]]

	agentiteg_result = result
	filtered_indices = result


func update_visualization(delta: float) -> void:
	# Animate threshold
	var time := Time.get_ticks_msec() / 1000.0
	threshold = 30.0 + sin(time * 0.3) * 20.0  # Oscillate between 10 and 50

	_update_colors()


func _update_colors() -> void:
	# Color entities by value (gradient from red to green)
	for i in entity_count:
		var t := values[i] / 100.0
		if values[i] > threshold:
			colors[i] = Color(0.2, 0.8, 0.3, 0.9)  # Green for above threshold
		else:
			colors[i] = Color(0.8, 0.3, 0.2, 0.6)  # Red for below threshold


func get_draw_data() -> Dictionary:
	var data := super.get_draw_data()

	# Draw threshold line indicator on the side
	var line_x := visualization_bounds.position.x + 10
	var line_top := visualization_bounds.position.y + 20
	var line_bottom := visualization_bounds.position.y + visualization_bounds.size.y - 20
	var threshold_y := line_top + (1.0 - threshold / 100.0) * (line_bottom - line_top)

	var lines := PackedVector2Array()
	# Vertical scale line
	lines.append(Vector2(line_x, line_top))
	lines.append(Vector2(line_x, line_bottom))
	# Threshold marker
	lines.append(Vector2(line_x - 5, threshold_y))
	lines.append(Vector2(line_x + 20, threshold_y))

	data["lines"] = lines

	return data

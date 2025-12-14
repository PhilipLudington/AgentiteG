extends BenchmarkScenario
## Spatial Query Scenario - SpatialHash2D radius queries vs GDScript loop

var query_center: Vector2
var query_radius: float = 100.0
var highlighted_indices: PackedInt32Array
var spatial_hash: SpatialHash2D


func get_name() -> String:
	return "Spatial Query"


func get_description() -> String:
	return "[b]Spatial Query (Radius)[/b]\n\n" + \
		"Finds all entities within a radius of a query point.\n\n" + \
		"[color=gray]GDScript:[/color] Loop through all positions, check distance_squared_to()\n\n" + \
		"[color=cyan]AgentiteG:[/color] SpatialHash2D.query_radius() - O(1) cell lookup\n\n" + \
		"[i]Expected speedup: 50-200x at high entity counts[/i]"


func _setup_entities() -> void:
	super._setup_entities()

	# Set query center to middle of bounds
	query_center = visualization_bounds.get_center()
	query_radius = min(visualization_bounds.size.x, visualization_bounds.size.y) * 0.15

	# Build spatial hash
	spatial_hash = SpatialHash2D.new()
	spatial_hash.cell_size = query_radius * 1.5
	spatial_hash.build(positions)

	# Initialize colors
	for i in entity_count:
		colors[i] = Color(0.3, 0.5, 0.9, 0.8)  # Blue

	highlighted_indices = PackedInt32Array()


func run_gdscript() -> void:
	var result := PackedInt32Array()
	var radius_sq := query_radius * query_radius

	for i in positions.size():
		if query_center.distance_squared_to(positions[i]) <= radius_sq:
			result.append(i)

	gdscript_result = result
	highlighted_indices = result


func run_agentiteg() -> void:
	agentiteg_result = spatial_hash.query_radius(query_center, query_radius)
	highlighted_indices = agentiteg_result


func update_visualization(_delta: float) -> void:
	# Move query center in a circle
	var time: float = Time.get_ticks_msec() / 1000.0
	var center := visualization_bounds.get_center()
	var orbit_radius: float = minf(visualization_bounds.size.x, visualization_bounds.size.y) * 0.25
	query_center = center + Vector2(cos(time * 0.5), sin(time * 0.5)) * orbit_radius

	# Update colors based on highlighted
	for i in entity_count:
		colors[i] = Color(0.3, 0.5, 0.9, 0.8)  # Blue

	for idx in highlighted_indices:
		if idx < colors.size():
			colors[idx] = Color(0.3, 0.9, 0.4, 1.0)  # Green


func get_draw_data() -> Dictionary:
	var data := super.get_draw_data()

	# Add query circle
	data["circles"] = [{
		"center": query_center,
		"radius": query_radius,
		"color": Color(1.0, 0.8, 0.2, 0.5),
		"filled": false
	}]

	# Add spatial hash grid debug lines
	if spatial_hash:
		data["lines"] = spatial_hash.debug_grid_lines()

	return data

extends BenchmarkScenario
## Collision Scenario - CollisionOps vs O(n^2) GDScript loop

var entity_radius: float = 8.0
var collision_pairs: PackedInt32Array


func get_name() -> String:
	return "Collision Detection"


func get_description() -> String:
	return "[b]Collision Detection (Circle vs Circle)[/b]\n\n" + \
		"Finds all overlapping circle pairs (self-collision).\n\n" + \
		"[color=gray]GDScript:[/color] O(n^2) loop checking distance < sum of radii\n\n" + \
		"[color=cyan]AgentiteG:[/color] CollisionOps.circles_self_collision_uniform()\n\n" + \
		"[i]Expected speedup: 30-100x[/i]"


func _setup_entities() -> void:
	super._setup_entities()

	# Random velocities for movement
	for i in entity_count:
		var angle := randf() * TAU
		velocities[i] = Vector2(cos(angle), sin(angle)) * randf_range(30, 80)

	collision_pairs = PackedInt32Array()


func run_gdscript() -> void:
	# Manual O(n^2) collision check
	var pairs := PackedInt32Array()
	var combined_radius := entity_radius * 2.0
	var combined_radius_sq := combined_radius * combined_radius

	for i in range(entity_count - 1):
		for j in range(i + 1, entity_count):
			if positions[i].distance_squared_to(positions[j]) <= combined_radius_sq:
				pairs.append(i)
				pairs.append(j)

	gdscript_result = pairs
	collision_pairs = pairs


func run_agentiteg() -> void:
	collision_pairs = CollisionOps.circles_self_collision_uniform(positions, entity_radius)
	agentiteg_result = collision_pairs.duplicate()


func update_visualization(delta: float) -> void:
	# Move entities
	for i in entity_count:
		positions[i] += velocities[i] * delta

		# Bounce off walls
		if positions[i].x < visualization_bounds.position.x + entity_radius:
			positions[i].x = visualization_bounds.position.x + entity_radius
			velocities[i].x = abs(velocities[i].x)
		elif positions[i].x > visualization_bounds.end.x - entity_radius:
			positions[i].x = visualization_bounds.end.x - entity_radius
			velocities[i].x = -abs(velocities[i].x)

		if positions[i].y < visualization_bounds.position.y + entity_radius:
			positions[i].y = visualization_bounds.position.y + entity_radius
			velocities[i].y = abs(velocities[i].y)
		elif positions[i].y > visualization_bounds.end.y - entity_radius:
			positions[i].y = visualization_bounds.end.y - entity_radius
			velocities[i].y = -abs(velocities[i].y)

	_update_colors()


func _update_colors() -> void:
	# Reset all to cyan
	for i in entity_count:
		colors[i] = Color(0.3, 0.7, 0.9, 0.8)

	# Mark colliding pairs as red
	for idx in range(0, collision_pairs.size(), 2):
		if idx + 1 < collision_pairs.size():
			var i: int = collision_pairs[idx]
			var j: int = collision_pairs[idx + 1]
			if i < colors.size():
				colors[i] = Color(1.0, 0.3, 0.3, 1.0)
			if j < colors.size():
				colors[j] = Color(1.0, 0.3, 0.3, 1.0)


func get_draw_data() -> Dictionary:
	var data := super.get_draw_data()

	# Set radii for all entities
	var radii := PackedFloat32Array()
	radii.resize(entity_count)
	radii.fill(entity_radius)
	data["radii"] = radii

	# Draw lines between colliding pairs
	var lines := PackedVector2Array()
	for idx in range(0, collision_pairs.size(), 2):
		if idx + 1 < collision_pairs.size():
			var i: int = collision_pairs[idx]
			var j: int = collision_pairs[idx + 1]
			if i < positions.size() and j < positions.size():
				lines.append(positions[i])
				lines.append(positions[j])

	data["lines"] = lines

	return data

extends BenchmarkScenario
## Steering Scenario - BatchOps seek/arrive vs GDScript loop

var targets: PackedVector2Array
var max_speed: float = 200.0
var steering_forces: PackedVector2Array


func get_name() -> String:
	return "Steering"


func get_description() -> String:
	return "[b]Steering Behaviors (Seek/Arrive)[/b]\n\n" + \
		"Calculates steering forces to move entities toward targets.\n\n" + \
		"[color=gray]GDScript:[/color] Loop calculating desired velocity - current velocity\n\n" + \
		"[color=cyan]AgentiteG:[/color] BatchOps.seek_batch() / arrive_batch()\n\n" + \
		"[i]Expected speedup: 20-50x[/i]"


func _setup_entities() -> void:
	super._setup_entities()

	# Initialize velocities with random directions
	for i in entity_count:
		var angle := randf() * TAU
		velocities[i] = Vector2(cos(angle), sin(angle)) * randf_range(50, 150)

	# Create targets (one per entity, initially at random positions)
	targets = PackedVector2Array()
	targets.resize(entity_count)
	_randomize_targets()

	steering_forces = PackedVector2Array()
	steering_forces.resize(entity_count)


func _randomize_targets() -> void:
	for i in entity_count:
		targets[i] = Vector2(
			visualization_bounds.position.x + randf() * visualization_bounds.size.x,
			visualization_bounds.position.y + randf() * visualization_bounds.size.y
		)


func run_gdscript() -> void:
	# Manual seek steering calculation
	for i in positions.size():
		var desired := (targets[i] - positions[i]).normalized() * max_speed
		steering_forces[i] = desired - velocities[i]

	gdscript_result = steering_forces.duplicate()


func run_agentiteg() -> void:
	steering_forces = BatchOps.seek_batch(positions, targets, max_speed)
	agentiteg_result = steering_forces.duplicate()


func update_visualization(delta: float) -> void:
	# Apply steering and update positions
	for i in entity_count:
		# Apply steering force (limited)
		var steering := steering_forces[i]
		if steering.length() > 100.0:
			steering = steering.normalized() * 100.0

		velocities[i] += steering * delta
		if velocities[i].length() > max_speed:
			velocities[i] = velocities[i].normalized() * max_speed

		positions[i] += velocities[i] * delta

		# Wrap around bounds
		if positions[i].x < visualization_bounds.position.x:
			positions[i].x = visualization_bounds.end.x
		elif positions[i].x > visualization_bounds.end.x:
			positions[i].x = visualization_bounds.position.x

		if positions[i].y < visualization_bounds.position.y:
			positions[i].y = visualization_bounds.end.y
		elif positions[i].y > visualization_bounds.end.y:
			positions[i].y = visualization_bounds.position.y

		# Check if reached target
		if positions[i].distance_to(targets[i]) < 10.0:
			# Assign new random target
			targets[i] = Vector2(
				visualization_bounds.position.x + randf() * visualization_bounds.size.x,
				visualization_bounds.position.y + randf() * visualization_bounds.size.y
			)

	_update_colors()


func _update_colors() -> void:
	# Color by velocity magnitude
	for i in entity_count:
		var speed := velocities[i].length()
		var t := clampf(speed / max_speed, 0.0, 1.0)
		# Slow = blue, Fast = red
		colors[i] = Color(0.3 + 0.6 * t, 0.4, 0.9 - 0.6 * t, 0.8)


func get_draw_data() -> Dictionary:
	var data := super.get_draw_data()

	# Draw velocity vectors for a subset of entities
	var lines := PackedVector2Array()
	var step: int = maxi(1, entity_count / 200)  # Show ~200 velocity lines max

	for i in range(0, entity_count, step):
		lines.append(positions[i])
		lines.append(positions[i] + velocities[i] * 0.1)

	data["lines"] = lines

	return data

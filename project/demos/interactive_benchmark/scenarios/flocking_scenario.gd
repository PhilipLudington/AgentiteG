extends BenchmarkScenario
## Flocking Scenario - BatchOps.flock_2d vs O(n^2) GDScript loop

const MAX_ENTITIES := 2000  # Cap for O(n^2) GDScript

var max_speed: float = 200.0
# Larger radii for 1080p screen with reasonable entity density
var separation_radius: float = 40.0
var cohesion_radius: float = 150.0
var alignment_radius: float = 100.0
var separation_strength: float = 3.0
var cohesion_strength: float = 1.0
var alignment_strength: float = 1.5
var flock_forces: PackedVector2Array


func get_name() -> String:
	return "Flocking (Boids)"


func get_description() -> String:
	return "[b]Flocking (Boids Simulation)[/b]\n\n" + \
		"Classic boids with separation, cohesion, and alignment.\n\n" + \
		"[color=gray]GDScript:[/color] O(n^2) loop checking all pairs\n\n" + \
		"[color=cyan]AgentiteG:[/color] BatchOps.flock_2d() - optimized\n\n" + \
		"[color=yellow]Note: Capped at 2000 entities (GDScript O(n^2) too slow)[/color]\n\n" + \
		"[i]Expected speedup: 50-150x[/i]"


func initialize(count: int, bounds: Rect2) -> void:
	# Cap entity count for flocking
	var capped_count := mini(count, MAX_ENTITIES)
	super.initialize(capped_count, bounds)


func _setup_entities() -> void:
	super._setup_entities()

	# Initialize with random velocities
	for i in entity_count:
		var angle := randf() * TAU
		velocities[i] = Vector2(cos(angle), sin(angle)) * randf_range(80, 120)

	flock_forces = PackedVector2Array()
	flock_forces.resize(entity_count)


func run_gdscript() -> void:
	# Manual O(n^2) flocking calculation
	for i in entity_count:
		var separation := Vector2.ZERO
		var cohesion_center := Vector2.ZERO
		var alignment_vel := Vector2.ZERO
		var sep_count := 0
		var neighbor_count := 0

		for j in entity_count:
			if i == j:
				continue

			var diff := positions[i] - positions[j]
			var dist := diff.length()

			# Separation
			if dist < separation_radius and dist > 0.001:
				separation += diff.normalized() / dist
				sep_count += 1

			# Cohesion and Alignment
			if dist < cohesion_radius:
				cohesion_center += positions[j]
				neighbor_count += 1

			if dist < alignment_radius:
				alignment_vel += velocities[j]

		var force := Vector2.ZERO

		if sep_count > 0:
			force += separation.normalized() * separation_strength

		if neighbor_count > 0:
			var center := cohesion_center / neighbor_count
			force += (center - positions[i]).normalized() * cohesion_strength
			force += (alignment_vel / neighbor_count).normalized() * alignment_strength

		flock_forces[i] = force

	gdscript_result = flock_forces.duplicate()


func run_agentiteg() -> void:
	flock_forces = BatchOps.flock_2d(
		positions, velocities,
		separation_radius, cohesion_radius, alignment_radius,
		separation_strength, cohesion_strength, alignment_strength
	)
	agentiteg_result = flock_forces.duplicate()


func update_visualization(delta: float) -> void:
	# Use BatchOps for proper force application (matches AgentiteG pattern)
	if flock_forces.size() == entity_count:
		# Apply flocking forces as accelerations
		velocities = BatchOps.apply_accelerations_2d(velocities, flock_forces, delta)

	# Apply boundary steering and speed limits per-entity
	for i in entity_count:
		# Soft boundary steering
		var margin := 100.0
		var turn_strength := 200.0 * delta

		var pos := positions[i]
		if pos.x < visualization_bounds.position.x + margin:
			velocities[i].x += turn_strength
		elif pos.x > visualization_bounds.end.x - margin:
			velocities[i].x -= turn_strength

		if pos.y < visualization_bounds.position.y + margin:
			velocities[i].y += turn_strength
		elif pos.y > visualization_bounds.end.y - margin:
			velocities[i].y -= turn_strength

	# Use BatchOps to limit velocity
	velocities = BatchOps.limit_velocity_range_2d(velocities, 50.0, max_speed)

	# Apply velocities to update positions
	positions = BatchOps.apply_velocities_2d(positions, velocities, delta)

	_update_colors()


func _update_colors() -> void:
	# Color based on velocity direction (hue = angle)
	for i in entity_count:
		var angle := velocities[i].angle()
		var hue := (angle + PI) / TAU
		colors[i] = Color.from_hsv(hue, 0.7, 0.9, 0.9)


func get_draw_data() -> Dictionary:
	var data := super.get_draw_data()

	# Draw boids as triangles instead of circles
	var triangles: Array = []

	for i in entity_count:
		var pos := positions[i]
		var vel := velocities[i]
		var angle := vel.angle()
		var size := 6.0

		# Triangle pointing in velocity direction
		var front := pos + Vector2(cos(angle), sin(angle)) * size
		var back_left := pos + Vector2(cos(angle + 2.5), sin(angle + 2.5)) * size * 0.6
		var back_right := pos + Vector2(cos(angle - 2.5), sin(angle - 2.5)) * size * 0.6

		var points := PackedVector2Array([front, back_left, back_right])
		triangles.append({
			"points": points,
			"color": colors[i]
		})

	data["triangles"] = triangles
	# Don't draw circles for flocking (we use triangles)
	data["positions"] = PackedVector2Array()
	data["colors"] = PackedColorArray()

	return data

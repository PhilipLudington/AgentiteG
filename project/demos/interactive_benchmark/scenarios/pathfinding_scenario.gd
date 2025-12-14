extends BenchmarkScenario
## Pathfinding Scenario - PathfindingOps flow field vs GDScript Dijkstra

const GRID_SIZE := 40  # Fixed grid resolution
const OBSTACLE_DENSITY := 0.20  # Reduced to minimize unreachable areas

var grid_width: int
var grid_height: int
var cell_size: Vector2
var costs: PackedFloat32Array
var flow_field: PackedVector2Array
var goal_cell: Vector2i
var flow_arrows: PackedVector2Array


func get_name() -> String:
	return "Flow Field"


func get_description() -> String:
	return "[b]Flow Field Pathfinding[/b]\n\n" + \
		"Generates flow field for all cells toward goal. Entities follow arrows.\n\n" + \
		"[color=gray]GDScript:[/color] Manual Dijkstra-style distance propagation\n\n" + \
		"[color=cyan]AgentiteG:[/color] PathfindingOps.flow_field()\n\n" + \
		"[i]Grid: %dx%d = %d cells[/i]\n\n" % [GRID_SIZE, GRID_SIZE, GRID_SIZE * GRID_SIZE] + \
		"[i]Expected speedup: 10-30x[/i]"


func _setup_entities() -> void:
	super._setup_entities()

	# Calculate cell size based on visualization bounds
	cell_size = Vector2(
		visualization_bounds.size.x / GRID_SIZE,
		visualization_bounds.size.y / GRID_SIZE
	)
	grid_width = GRID_SIZE
	grid_height = GRID_SIZE

	# Generate obstacle grid
	costs = PackedFloat32Array()
	costs.resize(grid_width * grid_height)

	for i in costs.size():
		if randf() < OBSTACLE_DENSITY:
			costs[i] = 0.0  # Blocked
		else:
			costs[i] = 1.0  # Walkable

	# Set goal at center
	goal_cell = Vector2i(grid_width / 2, grid_height / 2)
	var goal_idx := goal_cell.y * grid_width + goal_cell.x
	costs[goal_idx] = 1.0  # Ensure goal is walkable

	# Clear area around goal
	for dy in range(-2, 3):
		for dx in range(-2, 3):
			var x := goal_cell.x + dx
			var y := goal_cell.y + dy
			if x >= 0 and x < grid_width and y >= 0 and y < grid_height:
				costs[y * grid_width + x] = 1.0

	flow_field = PackedVector2Array()
	flow_field.resize(grid_width * grid_height)
	flow_arrows = PackedVector2Array()

	# Place entities on walkable cells
	var walkable_positions: Array[Vector2] = []
	for y in grid_height:
		for x in grid_width:
			if costs[y * grid_width + x] > 0:
				walkable_positions.append(Vector2(
					visualization_bounds.position.x + (x + 0.5) * cell_size.x,
					visualization_bounds.position.y + (y + 0.5) * cell_size.y
				))

	positions.resize(mini(entity_count, walkable_positions.size()))
	walkable_positions.shuffle()
	for i in positions.size():
		positions[i] = walkable_positions[i]

	entity_count = positions.size()
	colors.resize(entity_count)
	velocities.resize(entity_count)

	for i in entity_count:
		colors[i] = Color(0.3, 0.8, 0.4, 0.9)
		velocities[i] = Vector2.ZERO


func run_gdscript() -> void:
	# Manual Dijkstra-style flow field generation
	var distances := PackedFloat32Array()
	distances.resize(grid_width * grid_height)
	distances.fill(INF)

	var goal_idx := goal_cell.y * grid_width + goal_cell.x
	distances[goal_idx] = 0.0

	# Simple flood fill (not optimized)
	var changed := true
	while changed:
		changed = false
		for y in grid_height:
			for x in grid_width:
				var idx := y * grid_width + x
				if costs[idx] <= 0:
					continue

				var current_dist := distances[idx]

				# Check neighbors
				for dir in [Vector2i(-1, 0), Vector2i(1, 0), Vector2i(0, -1), Vector2i(0, 1)]:
					var nx: int = x + dir.x
					var ny: int = y + dir.y
					if nx >= 0 and nx < grid_width and ny >= 0 and ny < grid_height:
						var nidx: int = ny * grid_width + nx
						if costs[nidx] > 0:
							var new_dist := distances[nidx] + 1.0
							if new_dist < current_dist:
								distances[idx] = new_dist
								changed = true
								current_dist = new_dist

	# Generate flow directions from distances
	for y in grid_height:
		for x in grid_width:
			var idx := y * grid_width + x
			if costs[idx] <= 0:
				flow_field[idx] = Vector2.ZERO
				continue

			var best_dir := Vector2.ZERO
			var best_dist := distances[idx]

			for dir in [Vector2i(-1, 0), Vector2i(1, 0), Vector2i(0, -1), Vector2i(0, 1)]:
				var nx: int = x + dir.x
				var ny: int = y + dir.y
				if nx >= 0 and nx < grid_width and ny >= 0 and ny < grid_height:
					var nidx: int = ny * grid_width + nx
					if distances[nidx] < best_dist:
						best_dist = distances[nidx]
						best_dir = Vector2(dir.x, dir.y)

			flow_field[idx] = best_dir

	gdscript_result = flow_field.duplicate()


func run_agentiteg() -> void:
	flow_field = PathfindingOps.flow_field(costs, grid_width, grid_height, goal_cell)
	agentiteg_result = flow_field.duplicate()

	# Generate debug arrows (use average cell size as float)
	var avg_cell_size: float = (cell_size.x + cell_size.y) / 2.0
	flow_arrows = PathfindingOps.debug_flow_arrows(
		flow_field, grid_width, grid_height, avg_cell_size, 0.4, 2
	)


func update_visualization(delta: float) -> void:
	# Move entities along flow field
	var speed := 100.0

	for i in entity_count:
		# Get current cell coordinates
		var local_pos := positions[i] - visualization_bounds.position
		var cell_x := int(local_pos.x / cell_size.x)
		var cell_y := int(local_pos.y / cell_size.y)

		cell_x = clampi(cell_x, 0, grid_width - 1)
		cell_y = clampi(cell_y, 0, grid_height - 1)

		var idx := cell_y * grid_width + cell_x
		var flow_dir := flow_field[idx] if idx < flow_field.size() else Vector2.ZERO

		if flow_dir.length_squared() > 0.01:
			var movement := flow_dir.normalized() * speed * delta
			var new_pos := positions[i] + movement

			# Check if new position would be in an obstacle - try sliding
			var new_local := new_pos - visualization_bounds.position
			var new_cell_x := int(new_local.x / cell_size.x)
			var new_cell_y := int(new_local.y / cell_size.y)
			new_cell_x = clampi(new_cell_x, 0, grid_width - 1)
			new_cell_y = clampi(new_cell_y, 0, grid_height - 1)
			var new_idx := new_cell_y * grid_width + new_cell_x

			if new_idx < costs.size() and costs[new_idx] > 0:
				# Clear path - move normally
				positions[i] = new_pos
			else:
				# Blocked - try sliding along X axis only
				var slide_x := positions[i] + Vector2(movement.x, 0)
				var slide_x_local := slide_x - visualization_bounds.position
				var slide_x_cell := int(slide_x_local.x / cell_size.x)
				slide_x_cell = clampi(slide_x_cell, 0, grid_width - 1)
				var slide_x_idx := cell_y * grid_width + slide_x_cell

				if slide_x_idx < costs.size() and costs[slide_x_idx] > 0:
					positions[i] = slide_x
				else:
					# Try sliding along Y axis only
					var slide_y := positions[i] + Vector2(0, movement.y)
					var slide_y_local := slide_y - visualization_bounds.position
					var slide_y_cell := int(slide_y_local.y / cell_size.y)
					slide_y_cell = clampi(slide_y_cell, 0, grid_height - 1)
					var slide_y_idx := slide_y_cell * grid_width + cell_x

					if slide_y_idx < costs.size() and costs[slide_y_idx] > 0:
						positions[i] = slide_y
					# else: completely blocked, stay in place

			# Check if reached goal
			var final_local := positions[i] - visualization_bounds.position
			var final_cell_x := int(final_local.x / cell_size.x)
			var final_cell_y := int(final_local.y / cell_size.y)
			if Vector2i(final_cell_x, final_cell_y) == goal_cell:
				_respawn_entity(i)
		else:
			# Entity is stuck (unreachable cell or at goal) - respawn it
			_respawn_entity(i)

	_update_colors()


func _respawn_entity(idx: int) -> void:
	# Find random REACHABLE cell far from goal (must have valid flow direction)
	for _attempt in 50:
		var x := randi() % grid_width
		var y := randi() % grid_height
		var cell_idx := y * grid_width + x

		# Check if cell is walkable AND has a valid flow direction (reachable)
		if costs[cell_idx] > 0 and cell_idx < flow_field.size():
			var flow_dir := flow_field[cell_idx]
			if flow_dir.length_squared() > 0.01:  # Has valid flow = reachable
				var dist_to_goal := Vector2i(x, y).distance_to(goal_cell)
				if dist_to_goal > 5:
					positions[idx] = Vector2(
						visualization_bounds.position.x + (x + 0.5) * cell_size.x,
						visualization_bounds.position.y + (y + 0.5) * cell_size.y
					)
					return


func _update_colors() -> void:
	for i in entity_count:
		colors[i] = Color(0.3, 0.9, 0.4, 0.9)


func get_draw_data() -> Dictionary:
	var data := super.get_draw_data()

	# Draw grid and obstacles
	var rects: Array[Rect2] = []

	for y in grid_height:
		for x in grid_width:
			var idx := y * grid_width + x
			if costs[idx] <= 0:
				# Obstacle
				var rect := Rect2(
					visualization_bounds.position.x + x * cell_size.x,
					visualization_bounds.position.y + y * cell_size.y,
					cell_size.x,
					cell_size.y
				)
				rects.append(rect)

	data["rects"] = rects

	# Draw flow arrows (offset by visualization bounds position)
	var offset_arrows := PackedVector2Array()
	offset_arrows.resize(flow_arrows.size())
	for i in flow_arrows.size():
		offset_arrows[i] = flow_arrows[i] + visualization_bounds.position

	data["lines"] = offset_arrows

	# Draw goal
	data["circles"] = [{
		"center": Vector2(
			visualization_bounds.position.x + (goal_cell.x + 0.5) * cell_size.x,
			visualization_bounds.position.y + (goal_cell.y + 0.5) * cell_size.y
		),
		"radius": cell_size.x * 0.8,
		"color": Color(1.0, 0.8, 0.2, 0.8),
		"filled": true
	}]

	return data

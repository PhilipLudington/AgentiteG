extends SceneTree
## =============================================================================
## PathfindingOps Example - High-performance pathfinding algorithms
## =============================================================================
##
## WHEN TO USE:
## ------------
## PathfindingOps provides optimized grid-based pathfinding for games.
## Use it for turn-based tactics, RTS, roguelikes, tower defense.
##
## ALGORITHMS PROVIDED:
## - A*: General pathfinding with variable terrain costs
## - A* Uniform: Faster A* when all walkable cells cost the same
## - JPS (Jump Point Search): 10-30x faster than A* for uniform grids
## - Dijkstra: Path to nearest of multiple goals, or distance maps
## - Flow Fields: Direction vectors for steering many units to one goal
##
## CHOOSING AN ALGORITHM:
## - Single unit, variable terrain (mud, roads) -> A* (astar_grid)
## - Single unit, uniform terrain -> JPS (jps_grid) - FASTEST
## - Many units to same goal -> Flow Field (flow_field)
## - AI decision making, "how far to safety?" -> Dijkstra Map
## - Tactics game movement range -> reachable_cells
##
## GRID FORMAT:
## - All grids use row-major order: index = y * width + x
## - Cost arrays: positive = traversable (higher = slower), 0/negative = blocked
## - Walkable arrays: non-zero = walkable, zero = blocked
##
## Run with: godot --headless -s examples/example_pathfinding_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("PathfindingOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	astar_pathfinding()
	dijkstra_algorithms()
	flow_fields()
	jump_point_search()
	path_utilities()
	reachability()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func astar_pathfinding() -> void:
	print("--- A* Pathfinding ---")
	print("")
	print("A* finds the shortest path considering terrain costs.")
	print("Use astar_grid for variable costs, astar_uniform for uniform.")
	print("")

	# Cost grid: 1.0 = normal, 2.0 = slow terrain, 0 = blocked
	var costs = PackedFloat32Array([
		1, 1, 1, 1, 1,
		1, 0, 0, 0, 1,  # Wall in middle
		1, 0, 1, 0, 1,  # Gap in wall
		1, 0, 0, 0, 1,
		1, 1, 1, 1, 1
	])
	var width = 5
	var height = 5

	print("Cost grid (1=floor, 0=wall):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += ("." if costs[y * width + x] > 0 else "#") + " "
		print(row)

	# -------------------------------------------------------------------------
	# astar_grid - A* with variable terrain costs
	# USE WHEN: Different terrain types have different movement costs
	# EXAMPLES: RPG with roads/grass/mud, RTS with terrain penalties
	# -------------------------------------------------------------------------
	print("\nastar_grid:")
	print("  USE CASE: Path through varying terrain costs")
	var start = Vector2i(0, 0)
	var goal = Vector2i(4, 4)
	var path = PathfindingOps.astar_grid(costs, width, height, start, goal, true)
	print("  Path from ", start, " to ", goal, ": ", path.size(), " steps")
	print("  Path indices: ", path)

	# Visualize path
	print("  Path visualization (S=start, G=goal, *=path):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			var idx = y * width + x
			if idx == path[0]:
				row += "S "
			elif idx == path[path.size() - 1]:
				row += "G "
			elif idx in path:
				row += "* "
			elif costs[idx] == 0:
				row += "# "
			else:
				row += ". "
		print(row)

	# -------------------------------------------------------------------------
	# astar_uniform - Faster A* for uniform-cost grids
	# USE WHEN: All walkable cells have the same movement cost
	# EXAMPLES: Simple dungeon crawler, maze games
	# -------------------------------------------------------------------------
	print("\nastar_uniform:")
	print("  USE CASE: Simpler pathfinding when all floor costs the same")
	var walkable = PackedInt32Array([
		1, 1, 1, 1, 1,
		1, 0, 0, 0, 1,
		1, 0, 1, 0, 1,
		1, 0, 0, 0, 1,
		1, 1, 1, 1, 1
	])
	var uniform_path = PathfindingOps.astar_uniform(walkable, width, height, start, goal, true)
	print("  Path: ", uniform_path)
	print("")


func dijkstra_algorithms() -> void:
	print("--- Dijkstra Algorithms ---")
	print("")
	print("Dijkstra finds optimal paths and creates distance maps.")
	print("Distance maps are powerful for AI: 'how far to nearest X?'")
	print("")

	var costs = PackedFloat32Array([
		1, 1, 1,
		1, 1, 1,
		1, 1, 1
	])
	var width = 3
	var height = 3

	# -------------------------------------------------------------------------
	# dijkstra_map_single - Distance from every cell to one goal
	# USE WHEN: AI needs to know distance to player/objective
	# PATTERN: Calculate once, query many times
	# -------------------------------------------------------------------------
	print("dijkstra_map_single:")
	print("  USE CASE: 'How far is each enemy from the player?'")
	var goal = Vector2i(1, 1)
	var dijkstra = PathfindingOps.dijkstra_map_single(costs, width, height, goal)

	print("  Distance from center:")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += "%.0f " % dijkstra[y * width + x]
		print(row)
	print("  Now any entity can look up: dijkstra[my_cell]")

	# -------------------------------------------------------------------------
	# dijkstra_map - Distance to nearest of MULTIPLE goals
	# USE WHEN: Multiple objectives, exits, resources
	# EXAMPLES: Find nearest exit, closest resource, nearest ally
	# -------------------------------------------------------------------------
	print("\ndijkstra_map (multiple goals):")
	print("  USE CASE: 'How far to nearest exit?' (multiple exits)")
	var exits = PackedVector2Array([Vector2(0, 0), Vector2(2, 2)])
	var multi_dijkstra = PathfindingOps.dijkstra_map(costs, width, height, exits)
	print("  Goals at (0,0) and (2,2):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += "%.0f " % multi_dijkstra[y * width + x]
		print(row)
	print("  Each cell shows distance to NEAREST goal")

	# -------------------------------------------------------------------------
	# dijkstra_grid - Path to nearest of multiple goals
	# USE WHEN: "Go to nearest X"
	# -------------------------------------------------------------------------
	print("\ndijkstra_grid (path to nearest goal):")
	print("  USE CASE: 'Path to nearest exit'")
	var player_pos = Vector2i(1, 0)
	var path = PathfindingOps.dijkstra_grid(costs, width, height, player_pos, exits)
	print("  From ", player_pos, " to nearest exit: ", path)
	print("")


func flow_fields() -> void:
	print("--- Flow Fields ---")
	print("")
	print("Flow fields provide a direction vector at each cell.")
	print("Entities simply follow the direction at their current cell.")
	print("PERFECT for many units moving to the same destination.")
	print("")

	var costs = PackedFloat32Array([
		1, 1, 1, 1, 1,
		1, 0, 0, 0, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1
	])
	var width = 5
	var height = 5

	print("Grid (wall in row 1):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += ("." if costs[y * width + x] > 0 else "#") + " "
		print(row)

	# -------------------------------------------------------------------------
	# flow_field - Direction vectors toward goal
	# USE WHEN: Many units need to reach the same destination
	# EXAMPLES: RTS rally points, tower defense paths, evacuation
	# -------------------------------------------------------------------------
	print("\nflow_field:")
	print("  USE CASE: RTS rally point, all units flow toward goal")
	var goal = Vector2i(4, 4)
	var flow = PathfindingOps.flow_field(costs, width, height, goal)

	print("  Direction vectors toward goal at ", goal, ":")
	for y in range(height):
		var row = "  "
		for x in range(width):
			var dir = flow[y * width + x]
			if costs[y * width + x] == 0:
				row += "# "
			elif dir.length() < 0.1:
				row += "G "  # Goal
			else:
				# Visualize with arrows
				var angle = dir.angle()
				if angle > -PI/8 and angle <= PI/8:
					row += "→ "
				elif angle > PI/8 and angle <= 3*PI/8:
					row += "↘ "
				elif angle > 3*PI/8 and angle <= 5*PI/8:
					row += "↓ "
				elif angle > 5*PI/8 and angle <= 7*PI/8:
					row += "↙ "
				elif angle < -PI/8 and angle >= -3*PI/8:
					row += "↗ "
				elif angle < -3*PI/8 and angle >= -5*PI/8:
					row += "↑ "
				elif angle < -5*PI/8 and angle >= -7*PI/8:
					row += "↖ "
				else:
					row += "← "
		print(row)

	print("\n  USAGE: velocity = flow[my_cell] * speed")
	print("  Each unit just follows the arrow at its cell!")
	print("")


func jump_point_search() -> void:
	print("--- Jump Point Search (JPS) ---")
	print("")
	print("JPS is 10-30x faster than A* for UNIFORM cost grids.")
	print("It 'jumps' over cells that don't need examination.")
	print("Only works with 8-directional movement.")
	print("")

	var walkable = PackedInt32Array([
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 0, 0, 0, 0, 1, 1,
		1, 1, 0, 1, 1, 0, 1, 1,
		1, 1, 0, 1, 1, 0, 1, 1,
		1, 1, 0, 0, 0, 0, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1
	])
	var width = 8
	var height = 8

	print("Grid with obstacle:")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += ("." if walkable[y * width + x] > 0 else "#") + " "
		print(row)

	# -------------------------------------------------------------------------
	# jps_grid - Jump Point Search
	# USE WHEN: Uniform cost grid and you need SPEED
	# WARNING: Only works with 8-directional, uniform costs
	# -------------------------------------------------------------------------
	print("\njps_grid:")
	print("  USE CASE: Fast pathfinding on uniform-cost grid")
	var start = Vector2i(0, 0)
	var goal = Vector2i(7, 7)
	var path = PathfindingOps.jps_grid(walkable, width, height, start, goal)
	print("  Path from ", start, " to ", goal, ": ", path.size(), " jump points")
	print("  Path: ", path)
	print("  NOTE: JPS may skip intermediate cells (jumps)")

	print("\n  PERFORMANCE COMPARISON:")
	print("  - A* uniform: Good, works everywhere")
	print("  - JPS: 10-30x faster, but only uniform cost + 8-dir")
	print("")


func path_utilities() -> void:
	print("--- Path Utilities ---")
	print("")
	print("Utilities for converting and smoothing paths.")
	print("")

	var width = 5
	var cell_size = 32.0

	# Sample path
	var path = PackedInt32Array([0, 1, 2, 7, 12, 17, 22, 23, 24])

	print("Path indices: ", path)

	# -------------------------------------------------------------------------
	# path_to_vectors - Convert cell indices to world positions
	# USE WHEN: You need world coordinates for rendering/movement
	# -------------------------------------------------------------------------
	print("\npath_to_vectors:")
	print("  USE CASE: Convert grid path to world coordinates")
	var world_path = PathfindingOps.path_to_vectors(path, width, cell_size)
	print("  Cell size: ", cell_size)
	print("  World positions: ", world_path.slice(0, 3), "...")
	print("  (Returns cell centers)")

	# -------------------------------------------------------------------------
	# simplify_path - Remove collinear points
	# USE WHEN: Reducing waypoints for smoother movement
	# -------------------------------------------------------------------------
	print("\nsimplify_path:")
	print("  USE CASE: Reduce waypoints, keep only direction changes")
	var simplified = PathfindingOps.simplify_path(path, width)
	print("  Original: ", path.size(), " points")
	print("  Simplified: ", simplified.size(), " points")
	print("  Kept: ", simplified)

	# -------------------------------------------------------------------------
	# smooth_path - Smooth path by averaging
	# USE WHEN: Making paths look more natural
	# -------------------------------------------------------------------------
	print("\nsmooth_path:")
	print("  USE CASE: Natural-looking movement curves")
	var smooth = PathfindingOps.smooth_path(world_path, 3)
	print("  Original points: ", world_path.size())
	print("  After 3 smoothing iterations: still ", smooth.size(), " points")
	print("  (Points shifted toward neighbors)")
	print("")


func reachability() -> void:
	print("--- Reachability ---")
	print("")
	print("Reachability functions answer 'can I get there?' questions.")
	print("Essential for tactics games with movement budgets.")
	print("")

	var costs = PackedFloat32Array([
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1
	])
	var width = 5
	var height = 5
	var start = Vector2i(2, 2)

	# -------------------------------------------------------------------------
	# reachable_cells - All cells within movement budget
	# USE WHEN: Tactics game showing movement range
	# EXAMPLES: Fire Emblem, XCOM, chess-like games
	# -------------------------------------------------------------------------
	print("reachable_cells:")
	print("  USE CASE: 'Where can this unit move?' (tactics game)")
	var max_cost = 2.0
	var reachable = PathfindingOps.reachable_cells(costs, width, height, start, max_cost)
	print("  From ", start, " with budget ", max_cost, ":")
	print("  Reachable cells: ", reachable.size())

	# Visualize
	for y in range(height):
		var row = "  "
		for x in range(width):
			var idx = y * width + x
			if x == start.x and y == start.y:
				row += "S "
			elif idx in reachable:
				row += "o "
			else:
				row += ". "
		print(row)

	# -------------------------------------------------------------------------
	# is_reachable - Can we reach the goal at all?
	# USE WHEN: Quick check before pathfinding
	# -------------------------------------------------------------------------
	print("\nis_reachable:")
	print("  USE CASE: 'Is this destination accessible?'")
	var dest = Vector2i(4, 4)
	var can_reach = PathfindingOps.is_reachable(costs, width, height, start, dest)
	print("  ", start, " to ", dest, ": ", can_reach)

	# -------------------------------------------------------------------------
	# path_cost - Total cost without computing full path
	# USE WHEN: Comparing destinations, checking movement budget
	# -------------------------------------------------------------------------
	print("\npath_cost:")
	print("  USE CASE: 'How much will this move cost?'")
	var cost = PathfindingOps.path_cost(costs, width, height, start, dest, true)
	print("  Cost from ", start, " to ", dest, ": ", cost)
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: RTS Unit Movement ---")
	print("")
	print("This shows a complete RTS pathfinding setup.")
	print("Variable terrain costs + flow field for multiple units.")
	print("")

	var width = 10
	var height = 8

	# Terrain with variable costs
	# 1 = grass, 2 = forest (slow), 0 = water (blocked)
	var terrain = PackedFloat32Array([
		1, 1, 1, 1, 0, 0, 1, 1, 1, 1,
		1, 1, 2, 2, 0, 0, 2, 2, 1, 1,
		1, 2, 2, 2, 0, 0, 2, 2, 2, 1,
		1, 2, 2, 1, 1, 1, 1, 2, 2, 1,
		1, 2, 2, 1, 1, 1, 1, 2, 2, 1,
		1, 2, 2, 2, 0, 0, 2, 2, 2, 1,
		1, 1, 2, 2, 0, 0, 2, 2, 1, 1,
		1, 1, 1, 1, 0, 0, 1, 1, 1, 1
	])

	print("Terrain (. = grass, # = forest, ~ = water):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			var c = terrain[y * width + x]
			if c == 0:
				row += "~ "
			elif c == 2:
				row += "# "
			else:
				row += ". "
		print(row)

	# Single unit pathfinding
	print("\n--- SINGLE UNIT: A* Pathfinding ---")
	var unit_pos = Vector2i(0, 0)
	var target = Vector2i(9, 7)
	print("Unit at ", unit_pos, " moving to ", target)

	var path = PathfindingOps.astar_grid(terrain, width, height, unit_pos, target, true)
	print("Path found: ", path.size(), " steps")

	# Show path
	print("Path visualization:")
	for y in range(height):
		var row = "  "
		for x in range(width):
			var idx = y * width + x
			if idx == path[0]:
				row += "S "
			elif idx == path[path.size() - 1]:
				row += "G "
			elif idx in path:
				row += "* "
			elif terrain[idx] == 0:
				row += "~ "
			elif terrain[idx] == 2:
				row += "# "
			else:
				row += ". "
		print(row)

	# Flow field for multiple units
	print("\n--- MULTIPLE UNITS: Flow Field ---")
	var rally_point = Vector2i(9, 4)
	print("Rally point set at ", rally_point)
	var flow = PathfindingOps.flow_field(terrain, width, height, rally_point)
	print("Flow field generated - all units can now follow it")

	# AI with Dijkstra map
	print("\n--- AI: Dijkstra Map for Decision Making ---")
	var player_pos = Vector2i(5, 3)
	var distances = PathfindingOps.dijkstra_map_single(terrain, width, height, player_pos)

	# Enemy positions and their decisions
	var enemy_positions = [Vector2i(0, 0), Vector2i(9, 7), Vector2i(2, 4)]
	print("Enemy distances to player at ", player_pos, ":")
	for epos in enemy_positions:
		var dist = distances[epos.y * width + epos.x]
		print("  Enemy at ", epos, ": ", dist, " away")

	print("\n  PATHFINDING TIPS:")
	print("  - Single unit + variable terrain: astar_grid")
	print("  - Single unit + uniform terrain: jps_grid (faster)")
	print("  - Many units to same goal: flow_field")
	print("  - AI 'how far to X?': dijkstra_map")
	print("  - Movement range preview: reachable_cells")
	print("")

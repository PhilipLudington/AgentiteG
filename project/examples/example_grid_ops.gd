extends SceneTree
## =============================================================================
## GridOps Example - 2D grid operations for tile-based games
## =============================================================================
##
## WHEN TO USE:
## ------------
## GridOps provides algorithms for 2D tile/grid-based games. Essential for
## roguelikes, strategy games, puzzle games, and dungeon crawlers.
##
## PERFECT FOR:
## - Roguelikes (FOV, pathfinding, dungeon analysis)
## - Strategy/tactics games (movement range, line of sight)
## - Puzzle games (flood fill, connected components)
## - Dungeon generation (room detection, connectivity)
## - Tower defense (path analysis, blocking detection)
##
## GRID REPRESENTATION:
## - Grids are stored as flat PackedInt32Array in ROW-MAJOR order
## - Index = y * width + x
## - Use to_index() and to_coords() to convert
##
## ALL METHODS ARE STATIC - call directly on GridOps class.
##
## KEY ALGORITHMS:
## - Flood fill: Find connected regions
## - Shadowcasting FOV: Roguelike visibility
## - Distance fields: AI navigation, influence maps
## - Connected components: Room detection, area analysis
## - Line algorithms: Bresenham for line of sight
##
## Run with: godot --headless -s examples/example_grid_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("GridOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	coordinate_conversion()
	neighbor_queries()
	flood_fill()
	line_of_sight()
	field_of_view()
	distance_fields()
	connected_components()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func coordinate_conversion() -> void:
	print("--- Coordinate Conversion ---")
	print("")
	print("Convert between 2D coordinates (x, y) and flat array indices.")
	print("ESSENTIAL: All grid operations use flat arrays for performance.")
	print("")

	var width = 10

	# -------------------------------------------------------------------------
	# to_index - Convert (x, y) to array index
	# USE WHEN: Accessing grid data, storing positions compactly
	# Formula: y * width + x
	# -------------------------------------------------------------------------
	var x = 5
	var y = 3
	var idx = GridOps.to_index(x, y, width)
	print("to_index(", x, ", ", y, ", width=", width, ") = ", idx)

	# Index to 2D
	var coords = GridOps.to_coords(idx, width)
	print("to_coords(", idx, ", width=", width, ") = ", coords)

	# Bounds checking
	print("\nin_bounds checking:")
	print("  in_bounds(5, 3, 10, 10) = ", GridOps.in_bounds(5, 3, 10, 10))
	print("  in_bounds(10, 3, 10, 10) = ", GridOps.in_bounds(10, 3, 10, 10), " (x out of bounds)")

	# Batch conversion
	var positions = PackedVector2Array([Vector2(0, 0), Vector2(5, 3), Vector2(9, 9)])
	var indices = GridOps.to_indices(positions, width)
	print("\nto_indices batch: ", positions, " -> ", indices)

	var back = GridOps.to_coords_batch(indices, width)
	print("to_coords_batch: ", indices, " -> ", back)
	print("")


func neighbor_queries() -> void:
	print("--- Neighbor Queries ---")
	print("")
	print("Get neighboring cells for movement, spreading effects, etc.")
	print("")

	var width = 5
	var height = 5

	# Visualize grid
	print("5x5 grid (indices):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += "%2d " % (y * width + x)
		print(row)

	# -------------------------------------------------------------------------
	# neighbors_4 - Cardinal neighbors (N, E, S, W)
	# USE WHEN: Orthogonal movement only (roguelikes, chess rooks)
	# Returns only valid neighbors (handles edges automatically)
	# -------------------------------------------------------------------------
	var center = 12  # Cell at (2, 2)
	print("\nneighbors_4 of cell ", center, " (position 2,2):")
	print("  USE FOR: Orthogonal movement, spread effects (4-way)")
	var n4 = GridOps.neighbors_4(center, width, height)
	print("  ", n4, " (up, right, down, left)")

	# -------------------------------------------------------------------------
	# neighbors_8 - All 8 neighbors (includes diagonals)
	# USE WHEN: Diagonal movement allowed (chess kings, many RPGs)
	# -------------------------------------------------------------------------
	print("\nneighbors_8 of cell ", center, ":")
	print("  USE FOR: 8-way movement, FOV calculations")
	var n8 = GridOps.neighbors_8(center, width, height)
	print("  ", n8)

	# Corner cell has fewer neighbors
	print("\nneighbors_4 of corner cell 0:")
	var corner = GridOps.neighbors_4(0, width, height)
	print("  ", corner, " (only 2 neighbors)")

	# Custom neighbor pattern (chess knight moves)
	print("\nCustom neighbors (knight moves) from cell 12:")
	var knight_offsets = PackedVector2Array([
		Vector2(1, 2), Vector2(2, 1), Vector2(2, -1), Vector2(1, -2),
		Vector2(-1, -2), Vector2(-2, -1), Vector2(-2, 1), Vector2(-1, 2)
	])
	var knight_moves = GridOps.neighbors_custom(center, width, height, knight_offsets)
	print("  ", knight_moves)
	print("")


func flood_fill() -> void:
	print("--- Flood Fill ---")
	print("")
	print("Flood fill finds all connected cells of the same type.")
	print("Like the paint bucket tool - spreads until hitting different values.")
	print("")

	# -------------------------------------------------------------------------
	# flood_fill - Find all connected cells with same value
	# USE WHEN: Room detection, area selection, spread effects
	# -------------------------------------------------------------------------
	var grid = PackedInt32Array([
		1, 1, 0, 0, 0,
		1, 1, 0, 1, 1,
		0, 0, 0, 1, 1,
		0, 1, 1, 1, 0,
		0, 1, 1, 0, 0
	])
	var width = 5

	print("Grid (1=floor, 0=wall):")
	print("  USE CASES: Room detection, paint bucket, infection spread")
	for y in range(5):
		var row = "  "
		for x in range(5):
			row += str(grid[y * width + x]) + " "
		print(row)

	# Flood fill from top-left corner
	print("\nflood_fill from cell 0 (top-left), value=1:")
	var filled = GridOps.flood_fill(grid, width, 0, 1)
	print("  Connected cells: ", filled)
	print("  (Only 4 cells - separated by wall)")

	# Flood fill from different region
	print("\nflood_fill from cell 8 (bottom region):")
	var filled2 = GridOps.flood_fill(grid, width, 8, 1)
	print("  Connected cells: ", filled2)

	# Limited flood fill (movement range)
	print("\nflood_fill_limited from cell 16 (max_distance=2):")
	var limited = GridOps.flood_fill_limited(grid, width, 16, 1, 2)
	print("  Cells within 2 steps: ", limited)
	print("")


func line_of_sight() -> void:
	print("--- Line of Sight ---")
	print("")
	print("Line algorithms for visibility, targeting, and laser effects.")
	print("")

	var width = 10
	var height = 10

	# Grid with a wall
	var grid = PackedInt32Array()
	grid.resize(100)
	grid.fill(0)
	grid[45] = 1  # Wall at (5, 4)
	grid[55] = 1  # Wall at (5, 5)

	# -------------------------------------------------------------------------
	# line_indices - Bresenham's line algorithm
	# USE WHEN: Projectile paths, laser beams, line selection
	# Returns all cells the line passes through
	# -------------------------------------------------------------------------
	print("line_indices (Bresenham's algorithm):")
	print("  USE FOR: Laser beams, bullet paths, line-select tools")
	var line = GridOps.line_indices(Vector2i(0, 0), Vector2i(9, 5), width)
	print("  (0,0) to (9,5): ", line)

	print("\nLine clear check:")
	var blocked_line = GridOps.line_clear(grid, width, Vector2i(0, 5), Vector2i(9, 5), 1)
	print("  (0,5) to (9,5) through wall: ", blocked_line, " (blocked)")

	var clear_line = GridOps.line_clear(grid, width, Vector2i(0, 0), Vector2i(9, 0), 1)
	print("  (0,0) to (9,0) no wall: ", clear_line, " (clear)")

	# First blocking cell
	print("\nFirst blocking cell:")
	var first_block = GridOps.line_first_blocking(grid, width, Vector2i(0, 5), Vector2i(9, 5), 1)
	print("  First wall along (0,5) to (9,5): index ", first_block)
	if first_block >= 0:
		print("  Position: ", GridOps.to_coords(first_block, width))
	print("")


func field_of_view() -> void:
	print("--- Field of View (Shadowcasting) ---")
	print("")
	print("Shadowcasting FOV - the gold standard for roguelike visibility.")
	print("Determines what the player can see based on walls blocking view.")
	print("")

	var width = 11
	var height = 11

	# -------------------------------------------------------------------------
	# fov_shadowcast - Recursive shadowcasting FOV
	# USE WHEN: Roguelike visibility, stealth games, fog of war
	# Returns indices of all visible cells
	# PERFORMANCE: Very fast, handles thousands of cells
	# -------------------------------------------------------------------------
	var grid = PackedInt32Array()
	grid.resize(width * height)
	grid.fill(0)

	# Add some walls/pillars
	grid[GridOps.to_index(3, 3, width)] = 1
	grid[GridOps.to_index(7, 3, width)] = 1
	grid[GridOps.to_index(3, 7, width)] = 1
	grid[GridOps.to_index(7, 7, width)] = 1

	var player_pos = Vector2i(5, 5)  # Center
	var view_radius = 4

	print("fov_shadowcast USE CASES:")
	print("  - Roguelike visibility (what can player see?)")
	print("  - Stealth games (guard vision cones)")
	print("  - Fog of war reveal")
	print("")

	print("FOV from player at ", player_pos, " radius ", view_radius, ":")
	var visible = GridOps.fov_shadowcast(grid, width, height, player_pos, view_radius, 1)
	print("  Visible cells: ", visible.size())

	# Visualize
	print("\n  FOV visualization (. = not visible, o = visible, # = wall):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			var idx = y * width + x
			if grid[idx] == 1:
				row += "# "
			elif idx in visible:
				row += "o "
			else:
				row += ". "
		print(row)
	print("")


func distance_fields() -> void:
	print("--- Distance Fields ---")
	print("")
	print("Distance fields store how far each cell is from target cells.")
	print("Essential for AI pathfinding, influence maps, and tactical analysis.")
	print("")

	var width = 5
	var height = 5

	# -------------------------------------------------------------------------
	# Distance fields - Precompute distances for fast AI decisions
	# USE WHEN: AI needs to know distance to goals, threats, or resources
	# PATTERNS:
	#   - Single goal: AI moves toward/away from player
	#   - Multiple goals: AI finds nearest of several objectives
	#   - Combined: Add influence from multiple sources
	# -------------------------------------------------------------------------
	var grid = PackedInt32Array()
	grid.resize(width * height)
	grid.fill(0)
	grid[12] = 1  # Target at (2, 2)

	print("Distance field USE CASES:")
	print("  - AI pathfinding (move toward lower values)")
	print("  - Influence maps (combine multiple fields)")
	print("  - Threat assessment (how close is danger?)")
	print("")

	print("Target grid (1 = target):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += str(grid[y * width + x]) + " "
		print(row)

	# Manhattan distance
	print("\nManhattan distance field:")
	var manhattan = GridOps.manhattan_distance_field(grid, width, height, 1)
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += str(manhattan[y * width + x]) + " "
		print(row)

	# Chebyshev distance (allows diagonals)
	print("\nChebyshev distance field:")
	var chebyshev = GridOps.chebyshev_distance_field(grid, width, height, 1)
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += str(chebyshev[y * width + x]) + " "
		print(row)
	print("")


func connected_components() -> void:
	print("--- Connected Components ---")
	print("")
	print("Find and analyze separate regions in a grid.")
	print("Perfect for dungeon analysis, validation, and procedural generation.")
	print("")

	# -------------------------------------------------------------------------
	# Connected component analysis
	# USE WHEN: Analyzing procedural dungeons, validating connectivity
	# ALGORITHMS:
	#   - count_connected_components: How many separate regions?
	#   - label_connected_components: Which region is each cell in?
	#   - component_sizes: How big is each region?
	# -------------------------------------------------------------------------
	var grid = PackedInt32Array([
		1, 1, 0, 1, 1,
		1, 0, 0, 1, 1,
		0, 0, 0, 0, 0,
		1, 1, 0, 0, 1,
		1, 1, 0, 0, 1
	])
	var width = 5
	var height = 5

	print("Connected components USE CASES:")
	print("  - Dungeon validation (is everything reachable?)")
	print("  - Room detection (how many rooms? how big?)")
	print("  - Island detection in procedural worlds")
	print("")

	print("Grid (1 = floor):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += str(grid[y * width + x]) + " "
		print(row)

	# Count components
	var count = GridOps.count_connected_components(grid, width, height, 1)
	print("\ncount_connected_components: ", count, " separate floor regions")

	# Label components
	var labels = GridOps.label_connected_components(grid, width, height, 1)
	print("\nlabel_connected_components:")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += str(labels[y * width + x]) + " "
		print(row)
	print("  (0 = not floor, 1/2/3... = component ID)")

	# Component sizes
	var sizes = GridOps.component_sizes(grid, width, height, 1)
	print("\ncomponent_sizes: ", sizes)
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Roguelike FOV System ---")

	var width = 15
	var height = 10

	# Create dungeon layout
	var dungeon = PackedInt32Array()
	dungeon.resize(width * height)
	dungeon.fill(0)  # 0 = floor

	# Add walls (1)
	for x in range(width):
		dungeon[x] = 1  # Top wall
		dungeon[(height-1) * width + x] = 1  # Bottom wall
	for y in range(height):
		dungeon[y * width] = 1  # Left wall
		dungeon[y * width + width - 1] = 1  # Right wall

	# Interior walls
	for y in range(2, 6):
		dungeon[y * width + 5] = 1
	for x in range(8, 12):
		dungeon[4 * width + x] = 1

	print("Dungeon (# = wall, . = floor):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			row += "#" if dungeon[y * width + x] == 1 else "."
		print(row)

	# Player position
	var player = Vector2i(3, 5)

	# Calculate FOV
	var visible = GridOps.fov_shadowcast(dungeon, width, height, player, 6, 1)

	print("\nPlayer at ", player, " with vision radius 6:")
	print("Visible area (@ = player, o = visible, . = fog):")
	for y in range(height):
		var row = "  "
		for x in range(width):
			var idx = y * width + x
			if x == player.x and y == player.y:
				row += "@"
			elif dungeon[idx] == 1:
				if idx in visible:
					row += "#"
				else:
					row += "."
			elif idx in visible:
				row += "o"
			else:
				row += "."
		print(row)

	# Find connected rooms
	var rooms = GridOps.count_connected_components(dungeon, width, height, 0)
	print("\nConnected floor regions: ", rooms)

	# Distance to exit
	var exit_grid = PackedInt32Array()
	exit_grid.resize(width * height)
	exit_grid.fill(0)
	exit_grid[(height-2) * width + (width-2)] = 1  # Exit at bottom-right

	var distances = GridOps.manhattan_distance_field(exit_grid, width, height, 1)
	print("Distance from player to exit: ", distances[player.y * width + player.x])

	print("")

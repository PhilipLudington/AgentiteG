extends SceneTree
## =============================================================================
## QuadTree Example - Adaptive spatial subdivision for 2D
## =============================================================================
##
## WHEN TO USE:
## ------------
## QuadTree adapts to point density by subdividing space where entities cluster.
## This makes it efficient for non-uniform distributions.
##
## PERFECT FOR:
## - RTS games with clustered unit groups
## - Games where entities cluster in specific areas (cities, bases)
## - Large sparse worlds with dense pockets of activity
## - When you need debug visualization of spatial structure
## - Broad-phase collision for non-uniform entity distributions
##
## COMPARISON WITH OTHER STRUCTURES:
## - SpatialHash2D: Better for uniform distributions, simpler
## - KDTree2D: Better for nearest-neighbor queries
## - QuadTree: Best for clustered/hierarchical data, visual debugging
##
## PERFORMANCE:
## - Build: O(n log n) - depends on distribution
## - Query: O(log n + k) - depends on tree depth
## - Memory: O(n) - but tree overhead varies with clustering
##
## KEY INSIGHT:
## QuadTree automatically creates more subdivisions where data is dense
## and fewer where data is sparse. Great for natural game distributions.
##
## Run with: godot --headless -s examples/example_quadtree.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("QuadTree Feature Showcase")
	print("=" .repeat(60))
	print("")

	basic_usage()
	query_methods()
	visualization_data()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func basic_usage() -> void:
	print("--- Basic Usage ---")
	print("")
	print("QuadTree recursively divides 2D space into 4 quadrants.")
	print("It only subdivides where entities actually exist, adapting")
	print("to the density of your data.")
	print("")

	var quadtree = QuadTree.new()

	# IMPORTANT: Set bounds BEFORE building
	# This defines the world area the quadtree covers
	quadtree.set_bounds(Rect2(0, 0, 1000, 1000))

	# Configuration options control tree behavior
	quadtree.max_depth = 8        # How deep the tree can go (prevents infinite subdivision)
	quadtree.max_items_per_node = 8  # Items before a node subdivides

	print("Bounds: ", Rect2(0, 0, 1000, 1000))
	print("Max depth: ", quadtree.max_depth)
	print("  TIP: Higher max_depth = more precise but more memory")
	print("Max items per node: ", quadtree.max_items_per_node)
	print("  TIP: Lower = more subdivision, higher = flatter tree")

	# Build from positions
	var positions = PackedVector2Array([
		Vector2(100, 100),
		Vector2(150, 120),
		Vector2(800, 800),
		Vector2(500, 500)
	])
	quadtree.build(positions)

	print("Built quadtree with ", quadtree.size(), " items")
	print("")


func query_methods() -> void:
	print("--- Query Methods ---")
	print("")
	print("QuadTree supports both rectangle and radius queries.")
	print("The tree structure means queries skip large empty regions.")
	print("")

	var quadtree = QuadTree.new()
	quadtree.set_bounds(Rect2(0, 0, 1000, 1000))

	# Create clustered distribution (realistic for games)
	# This is where QuadTree excels vs SpatialHash
	var positions = PackedVector2Array()

	# Cluster 1: Dense city at (100, 100)
	print("Creating clustered data (where QuadTree excels)...")
	print("  Cluster 1: 20 units around (100, 100)")
	for i in range(20):
		positions.append(Vector2(100 + randf() * 50, 100 + randf() * 50))

	# Cluster 2: Dense city at (800, 800)
	print("  Cluster 2: 15 units around (800, 800)")
	for i in range(15):
		positions.append(Vector2(800 + randf() * 50, 800 + randf() * 50))

	# Scattered units (patrol, scouts)
	print("  Scattered: 10 random units")
	for i in range(10):
		positions.append(Vector2(randf() * 1000, randf() * 1000))

	quadtree.build(positions)
	print("Built quadtree with ", positions.size(), " clustered items")

	# -------------------------------------------------------------------------
	# query_rect - Find entities in a rectangle
	# USE WHEN: Selection box, viewport culling, zone checks
	# -------------------------------------------------------------------------
	print("\nquery_rect - Find items in selection rectangle:")
	print("  USE CASE: Drag-select box, viewport culling, zone triggers")
	var selection = Rect2(50, 50, 150, 150)
	var in_rect = quadtree.query_rect(selection)
	print("  Rect ", selection, " contains ", in_rect.size(), " items")

	# -------------------------------------------------------------------------
	# query_radius - Find entities in a circle
	# USE WHEN: Circular abilities, detection, explosions
	# -------------------------------------------------------------------------
	print("\nquery_radius - Find items around a point:")
	print("  USE CASE: Ability radius, detection range, blast zone")
	var center = Vector2(100, 100)
	var radius = 100.0
	var in_radius = quadtree.query_radius(center, radius)
	print("  Circle at ", center, " radius ", radius, " contains ", in_radius.size(), " items")

	print("\n  QUADTREE ADVANTAGE:")
	print("  With clustered data, QuadTree skips empty quadrants quickly.")
	print("  The tree adapts: dense areas have deep subdivision,")
	print("  sparse areas have shallow/no subdivision.")
	print("")


func visualization_data() -> void:
	print("--- Visualization Data ---")
	print("")
	print("QuadTree provides node bounds for debug visualization.")
	print("This is great for understanding how your data is distributed")
	print("and tuning max_depth and max_items_per_node.")
	print("")

	var quadtree = QuadTree.new()
	quadtree.set_bounds(Rect2(0, 0, 400, 400))
	quadtree.max_items_per_node = 2  # Force more subdivision for demo

	# Create points that will cause subdivision
	var positions = PackedVector2Array([
		Vector2(50, 50),    # These 3 points will force subdivision
		Vector2(60, 60),    # in the top-left quadrant
		Vector2(70, 70),
		Vector2(300, 300),  # These 2 points in bottom-right
		Vector2(310, 310)
	])
	quadtree.build(positions)

	# -------------------------------------------------------------------------
	# get_node_bounds - Get all node rectangles for debug drawing
	# USE WHEN: Debugging spatial issues, visualizing tree structure
	# -------------------------------------------------------------------------
	print("get_node_bounds - Quadtree structure for debug drawing:")
	var bounds = quadtree.get_node_bounds()
	print("  Total nodes: ", bounds.size())

	print("\n  Node boundaries (draw these as rectangles):")
	for i in range(min(bounds.size(), 8)):
		print("    Node ", i, ": ", bounds[i])
	if bounds.size() > 8:
		print("    ... and ", bounds.size() - 8, " more nodes")

	print("\n  DEBUG DRAWING (in _draw):")
	print("    for rect in quadtree.get_node_bounds():")
	print("        draw_rect(rect, Color.GREEN, false)")
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: RTS Unit Selection ---")
	print("")
	print("RTS games are a perfect fit for QuadTree because:")
	print("  1. Units cluster (bases, armies)")
	print("  2. Drag-selection needs efficient rect queries")
	print("  3. Large maps with sparse distribution overall")
	print("")

	var quadtree = QuadTree.new()
	quadtree.set_bounds(Rect2(0, 0, 1920, 1080))

	# Units on screen with realistic clustering
	var units = [
		# Infantry squad at (100, 200)
		{"name": "Infantry-1", "pos": Vector2(100, 200), "type": "infantry"},
		{"name": "Infantry-2", "pos": Vector2(120, 210), "type": "infantry"},
		{"name": "Infantry-3", "pos": Vector2(110, 190), "type": "infantry"},
		# Tank platoon at (500, 300)
		{"name": "Tank-1", "pos": Vector2(500, 300), "type": "tank"},
		{"name": "Tank-2", "pos": Vector2(530, 320), "type": "tank"},
		# Air unit and scout scattered
		{"name": "Helicopter", "pos": Vector2(800, 150), "type": "air"},
		{"name": "Scout", "pos": Vector2(1500, 800), "type": "infantry"}
	]

	var unit_positions = PackedVector2Array()
	for unit in units:
		unit_positions.append(unit.pos)
	quadtree.build(unit_positions)

	print("Units on field:")
	for unit in units:
		print("  ", unit.name, " (", unit.type, ") at ", unit.pos)

	# Simulate drag-select box
	print("\n--- DRAG SELECTION ---")
	print("Player drag-selects area (80, 180) to (250, 250):")
	print("  This covers the infantry squad")
	var select_box = Rect2(80, 180, 170, 70)
	var selected = quadtree.query_rect(select_box)
	print("  Selected ", selected.size(), " units:")
	for idx in selected:
		print("    ", units[idx].name)

	# Click selection - find unit near click
	print("\n--- CLICK SELECTION ---")
	print("Player clicks at (510, 310):")
	print("  Looking for unit within 30 pixels of click")
	var click_pos = Vector2(510, 310)
	var click_radius = 30.0
	var near_click = quadtree.query_radius(click_pos, click_radius)
	if near_click.size() > 0:
		print("  Selected: ", units[near_click[0]].name)
	else:
		print("  No unit selected")

	# Ability targeting - circular AoE
	print("\n--- AOE ABILITY ---")
	print("Artillery strike at (115, 200) radius 50:")
	print("  This should hit the clustered infantry")
	var strike_pos = Vector2(115, 200)
	var strike_radius = 50.0
	var affected = quadtree.query_radius(strike_pos, strike_radius)
	print("  Affected units: ", affected.size())
	for idx in affected:
		print("    ", units[idx].name, " takes damage!")

	# Debug visualization
	print("\n--- DEBUG INFO ---")
	var nodes = quadtree.get_node_bounds()
	print("QuadTree structure: ", nodes.size(), " nodes")
	print("  Use get_node_bounds() to draw debug overlay")
	print("  Dense areas will have more subdivisions")

	print("\n  RTS TIPS:")
	print("  - Rebuild when many units move significantly")
	print("  - Use query_rect for selection boxes")
	print("  - Use query_radius for abilities and detection")
	print("  - max_items_per_node ~8-16 is usually good")
	print("")

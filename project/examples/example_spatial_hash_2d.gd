extends SceneTree
## =============================================================================
## SpatialHash2D Example - Fast O(1) spatial lookups for 2D positions
## =============================================================================
##
## WHEN TO USE:
## ------------
## SpatialHash2D excels when you need to repeatedly find "what's nearby" for
## many entities. It's the go-to choice for most 2D games.
##
## PERFECT FOR:
## - Enemy detection systems (which enemies are near the player?)
## - Collision broad-phase (which entities might be colliding?)
## - Range-based abilities (who's in my attack radius?)
## - Proximity triggers (did something enter this zone?)
## - Tower defense targeting (which enemies are in turret range?)
## - Any "find nearby" query you run every frame
##
## NOT IDEAL FOR:
## - Static data that never changes (use KDTree2D instead)
## - Non-uniform distributions where most entities cluster (use QuadTree)
## - When you need exact k-nearest neighbors (use KDTree2D)
## - Very small entity counts (<50) - just loop through them
##
## PERFORMANCE:
## - Build: O(n) - rebuilding is cheap
## - Query: O(1) average - extremely fast
## - Memory: O(n) - linear with entity count
##
## KEY INSIGHT:
## The cell_size parameter is crucial. Set it to 1-2x your typical query radius.
## Too small = checking many cells. Too large = many entities per cell.
##
## Run with: godot --headless -s examples/example_spatial_hash_2d.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("SpatialHash2D Feature Showcase")
	print("=" .repeat(60))
	print("")

	basic_usage()
	query_methods()
	batch_queries()
	fast_checks()
	incremental_updates()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func basic_usage() -> void:
	print("--- Basic Usage ---")
	print("")
	print("SpatialHash2D divides 2D space into a grid of cells.")
	print("Each entity is assigned to a cell based on its position.")
	print("Queries only check nearby cells, making lookups O(1) on average.")
	print("")

	# Create a spatial hash with default cell size (64)
	var spatial = SpatialHash2D.new()
	print("Default cell_size: ", spatial.cell_size)

	# Set cell size to 1-2x your typical query radius
	# If your detection range is 50 units, use cell_size = 50-100
	spatial.cell_size = 100.0
	print("Cell size set to: ", spatial.cell_size)
	print("  TIP: Set cell_size to 1-2x your typical query radius")

	# Build from an array of positions
	# This is the most common pattern: rebuild once per frame
	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(50, 50),
		Vector2(100, 0),
		Vector2(500, 500),
		Vector2(1000, 1000)
	])
	spatial.build(positions)
	print("Built spatial hash with ", positions.size(), " items")
	print("")


func query_methods() -> void:
	print("--- Query Methods ---")
	print("")
	print("All queries return indices into your original positions array.")
	print("This lets you look up your actual game objects by index.")
	print("")

	var spatial = SpatialHash2D.new()
	spatial.cell_size = 100.0

	# Setup test data - enemies at various positions
	var enemy_positions = PackedVector2Array([
		Vector2(100, 100),   # Index 0: Close enemy
		Vector2(150, 100),   # Index 1: Close enemy
		Vector2(500, 500),   # Index 2: Far enemy
		Vector2(120, 80),    # Index 3: Close enemy
		Vector2(1000, 1000)  # Index 4: Very far enemy
	])
	spatial.build(enemy_positions)

	var player_pos = Vector2(100, 100)

	# -------------------------------------------------------------------------
	# query_radius - Most common query type
	# USE WHEN: You need to find all entities within a circular area
	# EXAMPLES: Attack range, detection radius, explosion blast
	# -------------------------------------------------------------------------
	print("query_radius - Find all enemies within 100 units of player:")
	print("  USE CASE: Detection radius, attack range, blast radius")
	var nearby = spatial.query_radius(player_pos, 100.0)
	print("  Found ", nearby.size(), " enemies: indices ", nearby)
	for i in nearby:
		print("    Enemy ", i, " at ", enemy_positions[i])

	# -------------------------------------------------------------------------
	# query_rect - Rectangle-based query
	# USE WHEN: You need entities in a rectangular area
	# EXAMPLES: Screen culling, selection box, rectangular triggers
	# -------------------------------------------------------------------------
	print("\nquery_rect - Find all enemies in rectangle (0,0)-(200,200):")
	print("  USE CASE: Viewport culling, drag-selection, rectangular zones")
	var rect = Rect2(0, 0, 200, 200)
	var in_rect = spatial.query_rect(rect)
	print("  Found ", in_rect.size(), " enemies: indices ", in_rect)

	# -------------------------------------------------------------------------
	# query_nearest - K nearest neighbors
	# USE WHEN: You need the closest N entities, regardless of distance
	# EXAMPLES: Target prioritization, AI awareness, social connections
	# NOTE: For exact nearest, KDTree2D is more accurate
	# -------------------------------------------------------------------------
	print("\nquery_nearest - Find 3 nearest enemies to player:")
	print("  USE CASE: Target priority, awareness system, closest threats")
	var nearest_3 = spatial.query_nearest(player_pos, 3)
	print("  Nearest 3 indices: ", nearest_3)
	for i in nearest_3:
		var dist = enemy_positions[i].distance_to(player_pos)
		print("    Enemy ", i, " at distance ", "%.2f" % dist)

	# -------------------------------------------------------------------------
	# query_nearest_one - Single nearest neighbor
	# USE WHEN: You only need the single closest entity
	# EXAMPLES: Auto-targeting, nearest pickup, closest enemy
	# -------------------------------------------------------------------------
	print("\nquery_nearest_one - Find single nearest enemy:")
	print("  USE CASE: Auto-aim, nearest pickup, primary target")
	var nearest = spatial.query_nearest_one(player_pos)
	if nearest >= 0:
		print("  Nearest enemy: index ", nearest, " at ", enemy_positions[nearest])
	else:
		print("  No enemies found")
	print("")


func batch_queries() -> void:
	print("--- Batch Queries ---")
	print("")
	print("When multiple entities need to query their surroundings,")
	print("batch queries are more efficient than individual calls.")
	print("Common in: RTS games, tower defense, swarm AI")
	print("")

	var spatial = SpatialHash2D.new()
	spatial.cell_size = 50.0

	# Create 100 enemy positions
	var enemy_positions = PackedVector2Array()
	for i in range(100):
		enemy_positions.append(Vector2(randf() * 1000, randf() * 1000))
	spatial.build(enemy_positions)

	# Multiple units need to query their surroundings
	# This is common in RTS games where each unit has awareness
	var unit_positions = PackedVector2Array([
		Vector2(100, 100),
		Vector2(500, 500),
		Vector2(900, 100)
	])

	# -------------------------------------------------------------------------
	# query_radius_batch_uniform - Same radius for all queries
	# USE WHEN: All units have the same detection/attack range
	# EXAMPLES: Infantry squads, same-type turrets, uniform sensors
	# -------------------------------------------------------------------------
	print("query_radius_batch_uniform - 3 units querying nearby enemies:")
	print("  USE CASE: All units with same detection range")
	var all_nearby = spatial.query_radius_batch_uniform(unit_positions, 150.0)
	for i in range(unit_positions.size()):
		print("  Unit ", i, " found ", all_nearby[i].size(), " enemies nearby")

	# -------------------------------------------------------------------------
	# query_radius_batch - Different radius per query
	# USE WHEN: Units have different ranges (e.g., different unit types)
	# EXAMPLES: Mixed unit types, upgraded units, variable sensors
	# -------------------------------------------------------------------------
	print("\nquery_radius_batch - Units with different detection ranges:")
	print("  USE CASE: Scout (long range), Tank (short range), etc.")
	var unit_ranges = PackedFloat32Array([100.0, 200.0, 300.0])
	var varied_nearby = spatial.query_radius_batch(unit_positions, unit_ranges)
	for i in range(unit_positions.size()):
		print("  Unit ", i, " (range ", unit_ranges[i], ") found ", varied_nearby[i].size(), " enemies")
	print("")


func fast_checks() -> void:
	print("--- Fast Checks (No Allocation) ---")
	print("")
	print("These methods answer yes/no questions without creating arrays.")
	print("Use them for conditions where you don't need the actual entities.")
	print("Much faster than query_radius().size() > 0")
	print("")

	var spatial = SpatialHash2D.new()
	spatial.cell_size = 50.0

	var enemy_positions = PackedVector2Array([
		Vector2(100, 100),
		Vector2(200, 200),
		Vector2(500, 500)
	])
	spatial.build(enemy_positions)

	var player_pos = Vector2(110, 110)
	var alert_range = 50.0
	var detection_range = 200.0

	# -------------------------------------------------------------------------
	# has_any_in_radius - Boolean existence check
	# USE WHEN: You just need to know IF something is nearby
	# EXAMPLES: Trigger alerts, enable/disable systems, guard conditions
	# -------------------------------------------------------------------------
	print("has_any_in_radius - Check if any enemy in alert range:")
	print("  USE CASE: Trigger alarm, wake up AI, enable targeting")
	if spatial.has_any_in_radius(player_pos, alert_range):
		print("  ALERT! Enemy detected within ", alert_range, " units!")
	else:
		print("  No enemies in immediate range")

	# -------------------------------------------------------------------------
	# count_in_radius - Get count without allocation
	# USE WHEN: You need to know HOW MANY, but not WHO
	# EXAMPLES: Threat level assessment, crowd density, balance checks
	# -------------------------------------------------------------------------
	print("\ncount_in_radius - Count enemies in detection range:")
	print("  USE CASE: Threat level, crowd size, balance decisions")
	var enemy_count = spatial.count_in_radius(player_pos, detection_range)
	print("  ", enemy_count, " enemies within ", detection_range, " units")

	# Demonstrate efficiency difference
	print("\n  PERFORMANCE TIP:")
	print("  GOOD: if spatial.has_any_in_radius(pos, range): ...")
	print("  SLOWER: if spatial.query_radius(pos, range).size() > 0: ...")
	print("  The first avoids creating an array when you don't need it.")
	print("")


func incremental_updates() -> void:
	print("--- Incremental Updates ---")
	print("")
	print("You have two strategies for updating the spatial hash:")
	print("1. Rebuild every frame (simple, good when many entities move)")
	print("2. Individual insert/update/remove (good when few change)")
	print("")

	var spatial = SpatialHash2D.new()
	spatial.cell_size = 50.0

	# Start with some positions
	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(100, 100)
	])
	spatial.build(positions)
	print("Initial items: query_radius(origin, 200) = ", spatial.query_radius(Vector2.ZERO, 200.0))

	# -------------------------------------------------------------------------
	# insert - Add a single new entity
	# USE WHEN: Spawning new entities mid-frame
	# EXAMPLES: Bullet spawning, enemy spawn, pickup creation
	# -------------------------------------------------------------------------
	print("\ninsert - Adding a new entity:")
	print("  USE CASE: Spawning bullets, enemies, pickups mid-frame")
	var new_idx = spatial.insert(Vector2(50, 50))
	print("  Inserted new item at (50, 50), got index: ", new_idx)
	print("  After insert: query_radius(origin, 200) = ", spatial.query_radius(Vector2.ZERO, 200.0))

	# -------------------------------------------------------------------------
	# update - Move an existing entity
	# USE WHEN: Entity moved and you need to update its cell
	# EXAMPLES: Player movement, enemy patrol, projectile update
	# -------------------------------------------------------------------------
	print("\nupdate - Moving an existing entity:")
	print("  USE CASE: Entity moved, needs cell reassignment")
	spatial.update(0, Vector2(500, 500))  # Move item 0 far away
	print("  Updated item 0 to (500, 500)")
	print("  After update: query_radius(origin, 200) = ", spatial.query_radius(Vector2.ZERO, 200.0))

	# -------------------------------------------------------------------------
	# clear - Remove all entities
	# USE WHEN: Scene change, level reset, clearing temporary entities
	# -------------------------------------------------------------------------
	print("\nclear - Removing all entities:")
	print("  USE CASE: Level reset, clearing temporary structures")
	spatial.clear()
	print("  After clear: query_radius(origin, 200) = ", spatial.query_radius(Vector2.ZERO, 200.0))

	print("\n  STRATEGY GUIDE:")
	print("  - Many entities moving? -> Rebuild every frame")
	print("  - Few entities moving? -> Use update() on those that moved")
	print("  - Spawning/despawning? -> Use insert() and rebuild when needed")
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Enemy Detection System ---")
	print("")
	print("This shows a complete turret targeting system.")
	print("The turret needs to:")
	print("  1. Quickly check if any enemies are in range")
	print("  2. Find all valid targets")
	print("  3. Select the best target (lowest HP)")
	print("")

	var spatial = SpatialHash2D.new()
	spatial.cell_size = 100.0  # Detection range is ~50, so 2x

	# Enemy positions
	var enemies = [
		{"name": "Grunt", "pos": Vector2(100, 100), "hp": 50},
		{"name": "Archer", "pos": Vector2(150, 80), "hp": 30},
		{"name": "Tank", "pos": Vector2(500, 500), "hp": 200},
		{"name": "Scout", "pos": Vector2(120, 120), "hp": 20},
		{"name": "Boss", "pos": Vector2(1000, 1000), "hp": 500}
	]

	var enemy_positions = PackedVector2Array()
	for enemy in enemies:
		enemy_positions.append(enemy.pos)
	spatial.build(enemy_positions)

	# Player turret at (100, 100)
	var turret_pos = Vector2(100, 100)
	var turret_range = 100.0

	print("Turret at ", turret_pos, " with range ", turret_range)

	# Step 1: Quick check - is anything in range?
	# This is cheap and avoids unnecessary work
	print("\nStep 1: Quick existence check")
	if not spatial.has_any_in_radius(turret_pos, turret_range):
		print("  No targets in range - turret idle")
		return
	print("  Targets detected! Proceeding to targeting...")

	# Step 2: Get all targets in range
	print("\nStep 2: Get all valid targets")
	var targets_in_range = spatial.query_radius(turret_pos, turret_range)
	print("  Targets in range: ", targets_in_range.size())

	# Step 3: Select best target (game logic - lowest HP)
	print("\nStep 3: Select best target (focus fire on lowest HP)")
	var best_target_idx = -1
	var lowest_hp = INF
	for i in targets_in_range:
		var hp = enemies[i].hp
		if hp < lowest_hp:
			lowest_hp = hp
			best_target_idx = i

	if best_target_idx >= 0:
		print("  Targeting: ", enemies[best_target_idx].name,
			  " at ", enemies[best_target_idx].pos,
			  " (HP: ", enemies[best_target_idx].hp, ")")

	# Alternative: Get nearest for different AI
	print("\nAlternative: Nearest target (for different AI)")
	var nearest_target = spatial.query_nearest_one(turret_pos)
	print("  Nearest target: ", enemies[nearest_target].name)

	print("\n  USAGE PATTERN:")
	print("  1. Build spatial hash once per frame with all enemy positions")
	print("  2. Each turret queries its own range")
	print("  3. Apply game logic to choose from candidates")
	print("")

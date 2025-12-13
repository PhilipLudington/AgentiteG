extends SceneTree
## =============================================================================
## SpatialHash3D Example - Fast O(1) spatial lookups for 3D positions
## =============================================================================
##
## WHEN TO USE:
## ------------
## SpatialHash3D is the 3D equivalent of SpatialHash2D. Use it when you need
## fast "what's nearby" queries in 3D space.
##
## PERFECT FOR:
## - Space games (find nearby ships, asteroids, projectiles)
## - 3D shooters (enemy detection, projectile proximity)
## - Flight simulators (traffic detection, collision avoidance)
## - 3D sandbox games (entity queries in voxel-like worlds)
## - Any 3D game with many entities that need spatial queries
##
## NOT IDEAL FOR:
## - 2D games (use SpatialHash2D - it's faster)
## - Static point clouds (use KDTree3D for better nearest-neighbor)
## - Highly clustered data (consider Octree)
##
## PERFORMANCE:
## - Same O(1) average query time as 2D version
## - Memory scales with entity count, not world size
## - Cell size tuning is equally important
##
## CELL SIZE FOR 3D:
## - Space games with large distances: 100-500 units
## - Room-scale games: 10-50 units
## - Rule of thumb: 1-2x your typical query radius
##
## Run with: godot --headless -s examples/example_spatial_hash_3d.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("SpatialHash3D Feature Showcase")
	print("=" .repeat(60))
	print("")

	basic_usage()
	query_methods()
	batch_queries()
	space_combat_scenario()

	print("\nAll examples completed!")
	quit(0)


func basic_usage() -> void:
	print("--- Basic Usage ---")
	print("")
	print("SpatialHash3D works identically to SpatialHash2D but in 3D space.")
	print("The third dimension adds volume, so cell_size affects a cube.")
	print("")

	# Create a 3D spatial hash
	var spatial = SpatialHash3D.new()
	print("Default cell_size: ", spatial.cell_size)

	# For space games with large distances, use larger cells
	# This prevents checking too many cells on large-radius queries
	spatial.cell_size = 200.0
	print("Cell size set to: ", spatial.cell_size)
	print("  TIP: For space games, use larger cells (100-500)")
	print("  TIP: For indoor games, use smaller cells (10-50)")

	# Build from an array of 3D positions
	var positions = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(100, 50, 100),
		Vector3(500, 200, 500),
		Vector3(1000, 0, 1000)
	])
	spatial.build(positions)
	print("Built spatial hash with ", positions.size(), " items")
	print("")


func query_methods() -> void:
	print("--- Query Methods ---")
	print("")
	print("3D queries use spheres (query_radius) and boxes (query_box)")
	print("instead of circles and rectangles.")
	print("")

	var spatial = SpatialHash3D.new()
	spatial.cell_size = 100.0

	# Setup - asteroids floating in space
	var asteroid_positions = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(50, 30, 20),
		Vector3(100, 0, 100),
		Vector3(500, 200, 300),
		Vector3(1000, 500, 1000)
	])
	spatial.build(asteroid_positions)

	var ship_pos = Vector3(0, 0, 0)

	# -------------------------------------------------------------------------
	# query_radius (Sphere Query)
	# USE WHEN: Finding entities within a spherical volume
	# EXAMPLES: Sensor range, weapon range, explosion radius
	# -------------------------------------------------------------------------
	print("query_radius (Sphere Query):")
	print("  USE CASE: Sensor range, explosion blast, detection sphere")
	var nearby = spatial.query_radius(ship_pos, 150.0)
	print("  Found ", nearby.size(), " asteroids within 150 units")

	# -------------------------------------------------------------------------
	# query_box (AABB Query)
	# USE WHEN: Finding entities in a box-shaped volume
	# EXAMPLES: Sector scanning, frustum culling (approximation), zone checks
	# -------------------------------------------------------------------------
	print("\nquery_box (AABB Query):")
	print("  USE CASE: Sector scan, zone check, frustum approximation")
	var sector = AABB(Vector3(-50, -50, -50), Vector3(200, 200, 200))
	var in_sector = spatial.query_box(sector)
	print("  Found ", in_sector.size(), " asteroids in sector")

	# -------------------------------------------------------------------------
	# query_nearest - K nearest in 3D
	# Same as 2D, but considers all three dimensions
	# -------------------------------------------------------------------------
	print("\nquery_nearest - Find 3 nearest asteroids:")
	print("  USE CASE: Closest threats, nearby resources, target priority")
	var nearest_3 = spatial.query_nearest(ship_pos, 3)
	for i in nearest_3:
		var dist = asteroid_positions[i].distance_to(ship_pos)
		print("  Asteroid ", i, " at distance ", "%.2f" % dist)

	# -------------------------------------------------------------------------
	# query_nearest_one - Single nearest in 3D
	# -------------------------------------------------------------------------
	print("\nquery_nearest_one - Find closest asteroid:")
	print("  USE CASE: Auto-target, closest obstacle, nearest pickup")
	var nearest = spatial.query_nearest_one(ship_pos)
	if nearest >= 0:
		print("  Closest: asteroid ", nearest, " at ", asteroid_positions[nearest])
	print("")


func batch_queries() -> void:
	print("--- Batch Queries ---")
	print("")
	print("In 3D games, batch queries are even more valuable because")
	print("3D calculations are more expensive than 2D.")
	print("")

	var spatial = SpatialHash3D.new()
	spatial.cell_size = 100.0

	# Create enemy ships
	var enemy_positions = PackedVector3Array()
	for i in range(50):
		enemy_positions.append(Vector3(
			randf() * 1000 - 500,
			randf() * 200 - 100,
			randf() * 1000 - 500
		))
	spatial.build(enemy_positions)

	# Fleet of friendly ships
	var fleet_positions = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(100, 0, 0),
		Vector3(-100, 0, 0)
	])

	# -------------------------------------------------------------------------
	# Batch queries - All ships scan simultaneously
	# Much faster than querying one at a time
	# -------------------------------------------------------------------------
	print("query_radius_batch_uniform - Fleet scanning for enemies:")
	print("  USE CASE: Multiple ships with same sensor range")
	var scan_range = 200.0
	var all_contacts = spatial.query_radius_batch_uniform(fleet_positions, scan_range)
	for i in range(fleet_positions.size()):
		print("  Ship ", i, " detected ", all_contacts[i].size(), " contacts")

	# Different sensor capabilities
	print("\nquery_radius_batch - Ships with different sensors:")
	print("  USE CASE: Scout ship (long range) vs Fighter (short range)")
	var sensor_ranges = PackedFloat32Array([100.0, 200.0, 300.0])
	var varied_contacts = spatial.query_radius_batch(fleet_positions, sensor_ranges)
	for i in range(fleet_positions.size()):
		print("  Ship ", i, " (sensor range ", sensor_ranges[i], ") found ", varied_contacts[i].size(), " contacts")
	print("")


func space_combat_scenario() -> void:
	print("--- Game Scenario: Space Combat ---")
	print("")
	print("This demonstrates a complete space combat targeting system.")
	print("In 3D space games, efficient spatial queries are critical")
	print("because the search volume grows cubically with distance.")
	print("")

	var spatial = SpatialHash3D.new()
	spatial.cell_size = 200.0  # Large for space scale

	# Enemy fleet
	var enemies = [
		{"name": "Fighter-1", "pos": Vector3(100, 20, 50), "threat": 1},
		{"name": "Fighter-2", "pos": Vector3(150, -10, 80), "threat": 1},
		{"name": "Cruiser", "pos": Vector3(300, 0, 200), "threat": 3},
		{"name": "Carrier", "pos": Vector3(800, 50, 600), "threat": 5},
		{"name": "Station", "pos": Vector3(2000, 0, 2000), "threat": 10}
	]

	var enemy_positions = PackedVector3Array()
	for enemy in enemies:
		enemy_positions.append(enemy.pos)
	spatial.build(enemy_positions)

	# Player ship with weapons
	var player_ship = Vector3(0, 0, 0)
	var weapons_range = 250.0
	var sensor_range = 500.0

	print("Player ship at ", player_ship)
	print("Weapons range: ", weapons_range, ", Sensor range: ", sensor_range)

	# Quick check - any immediate threats?
	print("\nStep 1: Threat detection")
	if spatial.has_any_in_radius(player_ship, weapons_range):
		print("  WARNING: Enemies in weapons range!")
	else:
		print("  No immediate threats")

	# Count threats at different ranges
	print("\nStep 2: Situation awareness")
	var immediate_threats = spatial.count_in_radius(player_ship, weapons_range)
	var detected_contacts = spatial.count_in_radius(player_ship, sensor_range)
	print("  Immediate threats (weapons range): ", immediate_threats)
	print("  Total contacts (sensor range): ", detected_contacts)

	# Get targets for weapons
	print("\nStep 3: Target acquisition")
	var targets = spatial.query_radius(player_ship, weapons_range)
	print("  Targets in weapons range:")
	for i in targets:
		var dist = enemies[i].pos.distance_to(player_ship)
		print("    ", enemies[i].name, " - Distance: ", "%.0f" % dist, ", Threat: ", enemies[i].threat)

	# Priority targeting - closest enemy
	print("\nStep 4: Primary target selection")
	var primary_target = spatial.query_nearest_one(player_ship)
	print("  Primary target: ", enemies[primary_target].name)

	# Tactical scan - check frontal sector
	print("\nStep 5: Sector scan (AABB query)")
	print("  Scanning frontal sector...")
	var frontal_sector = AABB(Vector3(0, -100, -100), Vector3(500, 200, 200))
	var ahead = spatial.query_box(frontal_sector)
	print("  Enemies in frontal sector: ", ahead.size())

	print("\n  3D GAME TIPS:")
	print("  - Use larger cell sizes for space games (100-500)")
	print("  - query_box is faster than query_radius for box shapes")
	print("  - Batch queries save significant time with many ships")
	print("  - has_any_in_radius for early-out checks")
	print("")

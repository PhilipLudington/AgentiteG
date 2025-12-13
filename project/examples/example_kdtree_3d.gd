extends SceneTree
## =============================================================================
## KDTree3D Example - Fast nearest neighbor queries in 3D
## =============================================================================
##
## WHEN TO USE:
## ------------
## KDTree3D is the 3D equivalent of KDTree2D - optimized for finding exact
## nearest neighbors in three-dimensional space.
##
## PERFECT FOR:
## - 3D targeting systems (find nearest enemy ship)
## - Resource gathering (find nearest asteroid/ore)
## - AI perception (which sounds/objects are closest?)
## - Static 3D point clouds (star maps, waypoint networks)
## - Any 3D game needing exact nearest-neighbor queries
##
## COMPARISON WITH SPATIALHASH3D:
## - KDTree3D: Better for nearest-neighbor, static data, exact results
## - SpatialHash3D: Better for radius queries, dynamic data, frequent updates
##
## PERFORMANCE:
## - Build: O(n log n) - do infrequently
## - Query nearest: O(log n) - very fast
## - Query radius: O(log n + k)
##
## KEY INSIGHT:
## In 3D, the performance advantage of KDTree over brute force is even
## greater because brute force has to check all points in 3D space.
## KDTree prunes entire regions of 3D space efficiently.
##
## Run with: godot --headless -s examples/example_kdtree_3d.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("KDTree3D Feature Showcase")
	print("=" .repeat(60))
	print("")

	basic_usage()
	nearest_neighbor_queries()
	space_scenario()

	print("\nAll examples completed!")
	quit(0)


func basic_usage() -> void:
	print("--- Basic Usage ---")
	print("")
	print("KDTree3D partitions 3D space along x, y, and z axes alternately.")
	print("This creates an efficient search structure for 3D points.")
	print("")

	var kdtree = KDTree3D.new()

	# Build from 3D positions
	var positions = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(10, 10, 10),
		Vector3(100, 50, 100),
		Vector3(-50, 20, -50)
	])
	kdtree.build(positions)

	print("Built KD-Tree with ", kdtree.size(), " points")
	print("  TIP: Rebuild when data changes significantly")
	print("  TIP: For frequently changing data, use SpatialHash3D")
	print("")


func nearest_neighbor_queries() -> void:
	print("--- Nearest Neighbor Queries ---")
	print("")
	print("Finding nearest neighbors in 3D is computationally expensive.")
	print("KDTree3D makes this O(log n) instead of O(n).")
	print("")

	var kdtree = KDTree3D.new()

	var points = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(10, 0, 0),
		Vector3(0, 10, 0),
		Vector3(0, 0, 10),
		Vector3(100, 100, 100)
	])
	kdtree.build(points)

	var query = Vector3(5, 5, 5)

	# -------------------------------------------------------------------------
	# query_nearest_one - Find single closest point
	# USE WHEN: Auto-targeting, nearest resource, closest threat
	# -------------------------------------------------------------------------
	print("query_nearest_one from ", query, ":")
	print("  USE CASE: Find nearest asteroid to mine, closest enemy to target")
	var nearest = kdtree.query_nearest_one(query)
	print("  Nearest: index ", nearest, " at ", points[nearest])
	print("  Distance: ", "%.2f" % points[nearest].distance_to(query))

	# -------------------------------------------------------------------------
	# query_nearest - Find k closest points
	# USE WHEN: Multiple targets, priority list, k-nearest analysis
	# -------------------------------------------------------------------------
	print("\nquery_nearest (k=3) from ", query, ":")
	print("  USE CASE: Lock onto 3 nearest targets, prioritize closest threats")
	var k_nearest = kdtree.query_nearest(query, 3)
	for i in k_nearest:
		var dist = points[i].distance_to(query)
		print("  Index ", i, " at ", points[i], " - Distance: ", "%.2f" % dist)

	# -------------------------------------------------------------------------
	# query_radius - Find all points within distance
	# USE WHEN: Sensor range, explosion radius, area of effect
	# -------------------------------------------------------------------------
	print("\nquery_radius (r=15) from ", query, ":")
	print("  USE CASE: Find all objects within sensor range")
	var in_radius = kdtree.query_radius(query, 15.0)
	print("  Found ", in_radius.size(), " points: ", in_radius)

	print("\n  3D PERFORMANCE NOTE:")
	print("  - Brute force nearest in 3D: O(n) distance calculations")
	print("  - KDTree3D nearest: O(log n) - huge savings for large datasets")
	print("")


func space_scenario() -> void:
	print("--- Game Scenario: Space Mining ---")
	print("")
	print("KDTree3D is ideal for space games because:")
	print("  1. Asteroids/resources are relatively static")
	print("  2. Finding 'nearest X' is a common operation")
	print("  3. 3D space has enormous search volumes")
	print("")

	var kdtree = KDTree3D.new()

	# Asteroids in a field
	var asteroids = [
		{"type": "Iron", "pos": Vector3(100, 20, 50), "value": 10},
		{"type": "Gold", "pos": Vector3(150, -10, 80), "value": 50},
		{"type": "Platinum", "pos": Vector3(200, 30, 100), "value": 100},
		{"type": "Ice", "pos": Vector3(80, 0, 30), "value": 5},
		{"type": "Titanium", "pos": Vector3(500, 100, 400), "value": 75}
	]

	var asteroid_positions = PackedVector3Array()
	for a in asteroids:
		asteroid_positions.append(a.pos)
	kdtree.build(asteroid_positions)

	# Mining ship
	var ship_pos = Vector3(120, 10, 60)
	var mining_range = 100.0

	print("Mining ship at ", ship_pos)
	print("Mining laser range: ", mining_range)

	print("\nAsteroid field:")
	for a in asteroids:
		var dist = a.pos.distance_to(ship_pos)
		print("  ", a.type, " at ", a.pos, " - Distance: ", "%.0f" % dist, ", Value: ", a.value)

	# Find closest asteroid to mine
	print("\n--- NEAREST ASTEROID ---")
	print("  USE CASE: Auto-select mining target")
	var closest = kdtree.query_nearest_one(ship_pos)
	print("  Closest: ", asteroids[closest].type,
		  " (distance: ", "%.0f" % asteroids[closest].pos.distance_to(ship_pos), ")")

	# Find all asteroids in mining range
	print("\n--- ASTEROIDS IN RANGE ---")
	print("  USE CASE: Show minable resources on HUD")
	var in_range = kdtree.query_radius(ship_pos, mining_range)
	print("  Asteroids in mining range: ", in_range.size())
	var total_value = 0
	for idx in in_range:
		var a = asteroids[idx]
		var dist = a.pos.distance_to(ship_pos)
		print("    ", a.type, " - Distance: ", "%.0f" % dist, ", Value: ", a.value)
		total_value += a.value
	print("  Total potential value: ", total_value)

	# Prioritize: find 3 nearest for mining queue
	print("\n--- MINING QUEUE (3 nearest) ---")
	print("  USE CASE: Auto-queue next mining targets")
	var top_3 = kdtree.query_nearest(ship_pos, 3)
	for i in range(top_3.size()):
		var idx = top_3[i]
		print("  ", i + 1, ". ", asteroids[idx].type, " (value: ", asteroids[idx].value, ")")

	# Find most valuable in range
	print("\n--- BEST TARGET IN RANGE ---")
	print("  USE CASE: Prioritize high-value asteroids")
	var best_idx = -1
	var best_value = 0
	for idx in in_range:
		if asteroids[idx].value > best_value:
			best_value = asteroids[idx].value
			best_idx = idx

	if best_idx >= 0:
		print("  Best target: ", asteroids[best_idx].type, " (value: ", best_value, ")")

	print("\n  WHEN TO REBUILD KDTree3D:")
	print("  - Asteroid destroyed -> Rebuild")
	print("  - New asteroids spawned -> Rebuild")
	print("  - Ship moved significantly -> No rebuild needed (just query)")
	print("")

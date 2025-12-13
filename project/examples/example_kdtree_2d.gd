extends SceneTree
## =============================================================================
## KDTree2D Example - Fast nearest neighbor queries in 2D
## =============================================================================
##
## WHEN TO USE:
## ------------
## KDTree2D is optimized for finding the EXACT nearest neighbors to a point.
## It builds a balanced tree structure that makes nearest-neighbor queries
## very efficient, especially for static or rarely-changing data.
##
## PERFECT FOR:
## - Static point sets (map POIs, fixed spawn points, waypoints)
## - Exact k-nearest neighbor queries
## - Missile/projectile homing on nearest target
## - Finding closest pickup/resource/ally
## - Any scenario where data doesn't change every frame
##
## COMPARISON WITH SPATIALHASH2D:
## - KDTree2D: Better for nearest-neighbor, static data, exact results
## - SpatialHash2D: Better for radius queries, dynamic data, approximate OK
##
## PERFORMANCE:
## - Build: O(n log n) - slower than SpatialHash2D, do infrequently
## - Query nearest: O(log n) - very fast
## - Query radius: O(log n + k) where k = results
##
## KEY INSIGHT:
## Build the tree once (or when data changes), then query many times.
## If your data changes every frame, prefer SpatialHash2D instead.
##
## Run with: godot --headless -s examples/example_kdtree_2d.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("KDTree2D Feature Showcase")
	print("=" .repeat(60))
	print("")

	basic_usage()
	nearest_neighbor_queries()
	radius_queries()
	batch_queries()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func basic_usage() -> void:
	print("--- Basic Usage ---")
	print("")
	print("KDTree2D builds a balanced binary tree that recursively")
	print("partitions 2D space. This structure enables O(log n) queries.")
	print("Build once, query many times.")
	print("")

	# Create a KD-Tree
	var kdtree = KDTree2D.new()

	# Build from points - this constructs a balanced tree
	# Note: Building is O(n log n), so do this infrequently
	var points = PackedVector2Array([
		Vector2(0, 0),
		Vector2(10, 0),
		Vector2(5, 5),
		Vector2(100, 100),
		Vector2(50, 75)
	])
	kdtree.build(points)

	print("Built KD-Tree with ", kdtree.size(), " points")
	print("  TIP: Build once when data changes, not every frame")
	print("  TIP: For frequently changing data, use SpatialHash2D instead")
	print("")


func nearest_neighbor_queries() -> void:
	print("--- Nearest Neighbor Queries ---")
	print("")
	print("This is where KDTree2D shines. It finds the EXACT nearest")
	print("neighbors efficiently, unlike SpatialHash2D which may miss some.")
	print("")

	var kdtree = KDTree2D.new()

	# Create scattered points
	var points = PackedVector2Array([
		Vector2(0, 0),      # 0
		Vector2(10, 10),    # 1
		Vector2(50, 50),    # 2
		Vector2(100, 0),    # 3
		Vector2(0, 100)     # 4
	])
	kdtree.build(points)

	var query_point = Vector2(8, 12)

	# -------------------------------------------------------------------------
	# query_nearest_one - Find single closest point
	# USE WHEN: You need THE closest entity, guaranteed
	# EXAMPLES: Homing missile, auto-aim, nearest pickup
	# -------------------------------------------------------------------------
	print("query_nearest_one - Find nearest to ", query_point, ":")
	print("  USE CASE: Homing missile, auto-aim, 'interact with nearest'")
	var nearest = kdtree.query_nearest_one(query_point)
	print("  Nearest: index ", nearest, " at ", points[nearest])
	print("  Distance: ", "%.2f" % points[nearest].distance_to(query_point))

	# -------------------------------------------------------------------------
	# query_nearest - Find k closest points
	# USE WHEN: You need the closest N entities for priority/selection
	# EXAMPLES: Multi-target missiles, top 5 threats, nearby allies
	# -------------------------------------------------------------------------
	print("\nquery_nearest - Find 3 nearest to ", query_point, ":")
	print("  USE CASE: Multi-target lock, closest N threats, nearby allies")
	var k_nearest = kdtree.query_nearest(query_point, 3)
	print("  Indices: ", k_nearest)
	for i in k_nearest:
		var dist = points[i].distance_to(query_point)
		print("    Index ", i, " at ", points[i], " - Distance: ", "%.2f" % dist)

	print("\n  WHY KDTREE FOR NEAREST?")
	print("  - Guaranteed to find the actual nearest (no approximation)")
	print("  - Results sorted by distance")
	print("  - O(log n) complexity vs O(n) brute force")
	print("")


func radius_queries() -> void:
	print("--- Radius Queries ---")
	print("")
	print("KDTree2D can also do radius queries, but SpatialHash2D is often")
	print("faster for this. Use KDTree2D radius queries when you already")
	print("have the tree built for nearest-neighbor queries.")
	print("")

	var kdtree = KDTree2D.new()

	var points = PackedVector2Array([
		Vector2(0, 0),
		Vector2(5, 5),
		Vector2(10, 0),
		Vector2(50, 50),
		Vector2(100, 100)
	])
	kdtree.build(points)

	var center = Vector2(5, 5)
	var radius = 15.0

	print("query_radius - Find all points within ", radius, " of ", center, ":")
	print("  USE CASE: When you have a KDTree anyway and need radius query")
	var in_radius = kdtree.query_radius(center, radius)
	print("  Found ", in_radius.size(), " points: ", in_radius)
	for i in in_radius:
		print("    Index ", i, " at ", points[i])

	print("\n  NOTE: For radius-only queries, SpatialHash2D is typically faster.")
	print("  Use KDTree2D when you need both nearest AND radius queries.")
	print("")


func batch_queries() -> void:
	print("--- Batch Queries ---")
	print("")
	print("When multiple seekers need to find their nearest target,")
	print("batch queries avoid repeated tree traversal overhead.")
	print("")

	var kdtree = KDTree2D.new()

	# Create many target points
	var targets = PackedVector2Array()
	for i in range(100):
		targets.append(Vector2(randf() * 500, randf() * 500))
	kdtree.build(targets)

	# Query points for multiple seekers
	var seekers = PackedVector2Array([
		Vector2(100, 100),
		Vector2(250, 250),
		Vector2(400, 400)
	])

	# -------------------------------------------------------------------------
	# query_nearest_one_batch - Find nearest for multiple points at once
	# USE WHEN: Multiple entities need their nearest target
	# EXAMPLES: Fleet targeting, swarm AI, multiple turrets
	# -------------------------------------------------------------------------
	print("query_nearest_one_batch - Nearest targets for ", seekers.size(), " seekers:")
	print("  USE CASE: Each seeker finds its closest target")
	var nearest_targets = kdtree.query_nearest_one_batch(seekers)
	for i in range(seekers.size()):
		print("  Seeker ", i, " at ", seekers[i], " -> Target ", nearest_targets[i], " at ", targets[nearest_targets[i]])

	print("\n  BATCH ADVANTAGE:")
	print("  - Single function call for multiple queries")
	print("  - Potential cache locality benefits")
	print("  - Cleaner code than looping")
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Missile Targeting ---")
	print("")
	print("KDTree2D is ideal for homing missiles because:")
	print("  1. Target positions don't change rapidly")
	print("  2. We need EXACT nearest (not approximate)")
	print("  3. Multiple missiles may need targets simultaneously")
	print("")

	var kdtree = KDTree2D.new()

	# Fleet of enemy ships (relatively static targets)
	var enemy_ships = [
		{"name": "Scout-1", "pos": Vector2(150, 50), "value": 1},
		{"name": "Scout-2", "pos": Vector2(180, 80), "value": 1},
		{"name": "Destroyer", "pos": Vector2(300, 150), "value": 3},
		{"name": "Cruiser", "pos": Vector2(450, 300), "value": 5},
		{"name": "Carrier", "pos": Vector2(700, 500), "value": 10}
	]

	var ship_positions = PackedVector2Array()
	for ship in enemy_ships:
		ship_positions.append(ship.pos)
	kdtree.build(ship_positions)

	print("Enemy fleet:")
	for ship in enemy_ships:
		print("  ", ship.name, " at ", ship.pos, " (value: ", ship.value, ")")

	# Missiles launched from different positions
	var missiles = [
		{"pos": Vector2(100, 100), "type": "Seeker"},
		{"pos": Vector2(200, 200), "type": "Salvo"},
		{"pos": Vector2(400, 400), "type": "Heavy"}
	]

	print("\nMissile targeting (each finds nearest):")
	for missile in missiles:
		# Each missile homes on its nearest target
		var target_idx = kdtree.query_nearest_one(missile.pos)
		var target = enemy_ships[target_idx]
		var dist = target.pos.distance_to(missile.pos)
		print("  ", missile.type, " missile at ", missile.pos,
			  " -> ", target.name, " (distance: ", "%.0f" % dist, ")")

	# Multi-warhead missile - targets 3 nearest
	print("\nMulti-warhead missile from (150, 100):")
	print("  USE CASE: Cluster bomb, shotgun spread, area denial")
	var warhead_pos = Vector2(150, 100)
	var targets_3 = kdtree.query_nearest(warhead_pos, 3)
	for idx in targets_3:
		print("  Warhead -> ", enemy_ships[idx].name)

	# Area weapon with radius
	print("\nArea denial weapon (radius 200) from (200, 150):")
	print("  USE CASE: EMP burst, flak, area effect")
	var aoe_center = Vector2(200, 150)
	var in_range = kdtree.query_radius(aoe_center, 200.0)
	print("  Ships in range: ", in_range.size())
	for idx in in_range:
		print("    ", enemy_ships[idx].name)

	print("\n  WHEN TO REBUILD:")
	print("  - Ships destroyed -> Rebuild tree")
	print("  - Significant time passed -> Rebuild if ships moved")
	print("  - New ships spawned -> Rebuild tree")
	print("  Don't rebuild every frame - that defeats the purpose!")
	print("")

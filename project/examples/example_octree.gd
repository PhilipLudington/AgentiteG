extends SceneTree
## =============================================================================
## Octree Example - Adaptive spatial subdivision for 3D
## =============================================================================
##
## WHEN TO USE:
## ------------
## Octree is the 3D equivalent of QuadTree - it adaptively subdivides 3D space
## based on where entities are located. Efficient for clustered distributions.
##
## PERFECT FOR:
## - Space games with clustered objects (asteroid fields, debris clouds)
## - 3D games with non-uniform entity distributions
## - Large 3D worlds with localized activity
## - When you need to visualize/debug spatial partitioning
## - Broad-phase collision detection in 3D
##
## COMPARISON WITH OTHER 3D STRUCTURES:
## - SpatialHash3D: Better for uniform distributions, simpler
## - KDTree3D: Better for nearest-neighbor queries
## - Octree: Best for clustered data, hierarchical queries, visualization
##
## HOW IT WORKS:
## - Divides 3D space into 8 octants (like QuadTree divides into 4 quadrants)
## - Only subdivides where entities exist
## - Dense regions get more subdivision, sparse regions stay coarse
##
## PERFORMANCE:
## - Build: O(n log n) - depends on distribution
## - Query: O(log n + k) - skips empty octants
## - Memory: Adapts to data distribution
##
## Run with: godot --headless -s examples/example_octree.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("Octree Feature Showcase")
	print("=" .repeat(60))
	print("")

	basic_usage()
	query_methods()
	space_scenario()

	print("\nAll examples completed!")
	quit(0)


func basic_usage() -> void:
	print("--- Basic Usage ---")
	print("")
	print("Octree recursively divides 3D space into 8 octants.")
	print("Each octant can be further subdivided where entities cluster.")
	print("")

	var octree = Octree.new()

	# IMPORTANT: Set bounds BEFORE building
	# This defines the 3D volume the octree covers
	var world_bounds = AABB(Vector3.ZERO, Vector3(1000, 1000, 1000))
	octree.set_bounds(world_bounds)

	# Configuration options
	octree.max_depth = 8              # Maximum subdivision depth
	octree.max_items_per_node = 8     # Items before a node subdivides

	print("Bounds: ", world_bounds)
	print("Max depth: ", octree.max_depth)
	print("  TIP: Higher depth = more precision but more memory")
	print("Max items per node: ", octree.max_items_per_node)
	print("  TIP: Lower = more subdivision, higher = flatter tree")

	# Build from 3D positions
	var positions = PackedVector3Array([
		Vector3(100, 100, 100),
		Vector3(150, 120, 110),
		Vector3(800, 800, 800),
		Vector3(500, 500, 500)
	])
	octree.build(positions)

	print("Built octree with ", octree.size(), " items")
	print("")


func query_methods() -> void:
	print("--- Query Methods ---")
	print("")
	print("Octree supports box (AABB) and sphere queries.")
	print("The hierarchical structure quickly prunes empty regions.")
	print("")

	var octree = Octree.new()
	octree.set_bounds(AABB(Vector3.ZERO, Vector3(1000, 1000, 1000)))

	# Create clustered 3D distribution (where Octree excels)
	var positions = PackedVector3Array()

	# Cluster 1: Space station area
	print("Creating clustered 3D data (where Octree excels)...")
	print("  Cluster 1: 15 objects near (100, 100, 100)")
	for i in range(15):
		positions.append(Vector3(
			100 + randf() * 50,
			100 + randf() * 50,
			100 + randf() * 50
		))

	# Cluster 2: Distant asteroid field
	print("  Cluster 2: 10 objects near (800, 800, 800)")
	for i in range(10):
		positions.append(Vector3(
			800 + randf() * 100,
			800 + randf() * 100,
			800 + randf() * 100
		))

	octree.build(positions)
	print("Built octree with ", positions.size(), " items")

	# -------------------------------------------------------------------------
	# query_box (AABB query)
	# USE WHEN: Finding entities in a rectangular volume
	# EXAMPLES: Frustum culling (approximate), sector scan, zone check
	# -------------------------------------------------------------------------
	print("\nquery_box (AABB query):")
	print("  USE CASE: Sector scan, frustum culling, rectangular zone")
	var query_box = AABB(Vector3(50, 50, 50), Vector3(150, 150, 150))
	var in_box = octree.query_box(query_box)
	print("  Box ", query_box, " contains ", in_box.size(), " items")

	# -------------------------------------------------------------------------
	# query_radius (Sphere query)
	# USE WHEN: Finding entities within a spherical volume
	# EXAMPLES: Sensor range, explosion radius, gravitational influence
	# -------------------------------------------------------------------------
	print("\nquery_radius (Sphere query):")
	print("  USE CASE: Sensor range, explosion blast, area of effect")
	var center = Vector3(100, 100, 100)
	var radius = 100.0
	var in_radius = octree.query_radius(center, radius)
	print("  Sphere at ", center, " radius ", radius, " contains ", in_radius.size(), " items")

	# -------------------------------------------------------------------------
	# get_node_bounds - Get all node AABBs for debug visualization
	# USE WHEN: Debugging, visualizing tree structure
	# -------------------------------------------------------------------------
	print("\nget_node_bounds (for debug visualization):")
	print("  USE CASE: Draw octree structure for debugging")
	var node_bounds = octree.get_node_bounds()
	print("  Octree has ", node_bounds.size(), " nodes")
	print("  Draw these AABBs to visualize the tree structure")

	print("\n  OCTREE ADVANTAGE:")
	print("  With clustered data, Octree skips empty octants.")
	print("  Dense regions are subdivided, sparse regions are coarse.")
	print("")


func space_scenario() -> void:
	print("--- Game Scenario: 3D Space Station Defense ---")
	print("")
	print("Octree is ideal for space games because:")
	print("  1. Objects cluster (asteroid fields, debris, formations)")
	print("  2. 3D space is huge - need to skip empty regions")
	print("  3. Box queries match sensor/weapon volumes")
	print("")

	var octree = Octree.new()
	octree.set_bounds(AABB(Vector3(-500, -500, -500), Vector3(1000, 1000, 1000)))

	# Space objects with different types
	var objects = [
		# Station components (clustered at origin)
		{"name": "Station Core", "pos": Vector3(0, 0, 0), "type": "station"},
		{"name": "Solar Panel 1", "pos": Vector3(50, 30, 0), "type": "structure"},
		{"name": "Solar Panel 2", "pos": Vector3(-50, 30, 0), "type": "structure"},
		{"name": "Docking Bay", "pos": Vector3(0, -40, 20), "type": "structure"},
		# Nearby asteroids
		{"name": "Asteroid 1", "pos": Vector3(200, 100, 50), "type": "asteroid"},
		{"name": "Asteroid 2", "pos": Vector3(180, 120, 80), "type": "asteroid"},
		# Threats
		{"name": "Enemy Fighter", "pos": Vector3(150, 80, 30), "type": "enemy"},
		{"name": "Enemy Bomber", "pos": Vector3(300, 150, 100), "type": "enemy"},
		# Friendly
		{"name": "Friendly Patrol", "pos": Vector3(-100, 50, -50), "type": "ally"}
	]

	var object_positions = PackedVector3Array()
	for obj in objects:
		object_positions.append(obj.pos)
	octree.build(object_positions)

	print("Space objects:")
	for obj in objects:
		print("  ", obj.name, " (", obj.type, ") at ", obj.pos)

	# Station defense turret
	var turret_pos = Vector3(0, 0, 0)
	var turret_range = 200.0

	print("\n--- TURRET DEFENSE SYSTEM ---")
	print("Turret at ", turret_pos, " with range ", turret_range)

	# Query sphere for all objects in range
	print("\nObjects in turret range (sphere query):")
	var in_range = octree.query_radius(turret_pos, turret_range)
	for idx in in_range:
		var obj = objects[idx]
		var dist = obj.pos.distance_to(turret_pos)
		print("  ", obj.name, " (", obj.type, ") - Distance: ", "%.0f" % dist)

	# Count threats in range
	var threat_count = 0
	var closest_threat_dist = INF
	var closest_threat_name = ""
	for idx in in_range:
		var obj = objects[idx]
		if obj.type == "enemy":
			threat_count += 1
			var dist = obj.pos.distance_to(turret_pos)
			if dist < closest_threat_dist:
				closest_threat_dist = dist
				closest_threat_name = obj.name

	print("\nThreat analysis:")
	print("  Enemies in range: ", threat_count)
	if threat_count > 0:
		print("  Closest threat: ", closest_threat_name, " at ", "%.0f" % closest_threat_dist, " units")

	# Scan a sector (box query)
	print("\n--- SECTOR SCAN (box query) ---")
	print("Scanning forward sector...")
	var forward_sector = AABB(Vector3(0, -100, -100), Vector3(400, 200, 200))
	var in_sector = octree.query_box(forward_sector)
	print("Objects in forward sector: ", in_sector.size())
	for idx in in_sector:
		print("  ", objects[idx].name)

	# Debug visualization info
	print("\n--- DEBUG INFO ---")
	var nodes = octree.get_node_bounds()
	print("Octree structure: ", nodes.size(), " nodes")
	print("  Use get_node_bounds() to draw debug wireframes")

	print("\n  3D GAME TIPS:")
	print("  - Use query_box for rectangular sensors/zones")
	print("  - Use query_radius for spherical sensors/weapons")
	print("  - Dense clusters create more octree nodes")
	print("  - Empty space costs almost nothing")
	print("")

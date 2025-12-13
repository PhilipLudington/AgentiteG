extends SceneTree
## =============================================================================
## CollisionOps Example - Batch collision detection
## =============================================================================
##
## WHEN TO USE:
## ------------
## CollisionOps provides high-performance collision detection for many objects.
## Use when you need to check collisions between hundreds or thousands of entities.
##
## PERFECT FOR:
## - Bullet hell games (thousands of projectiles)
## - Physics simulations (particle systems)
## - RTS games (unit selection, area effects)
## - Tower defense (projectile vs enemy checks)
## - Any game with many colliding objects
##
## WHEN TO USE GODOT'S PHYSICS INSTEAD:
## - You need physics responses (bouncing, sliding)
## - You need collision layers/masks
## - You need complex shapes (not circles/boxes)
## - You have few objects (<100)
##
## COLLISION TYPES:
## - Point containment: Is point inside shape?
## - Circle/sphere tests: Overlapping circular hitboxes
## - Ray tests: Laser beams, projectile traces
## - Segment tests: Line-based collision
##
## ALL METHODS ARE STATIC - call directly on CollisionOps class.
##
## Run with: godot --headless -s examples/example_collision_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("CollisionOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	point_containment()
	circle_collision()
	self_collision()
	ray_intersection()
	segment_intersection()
	closest_queries()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func point_containment() -> void:
	print("--- Point Containment Tests ---")
	print("")
	print("Check which points are inside various shapes.")
	print("USE CASES: Culling, selection, zone detection")
	print("")

	var points = PackedVector2Array([
		Vector2(50, 50),    # 0: Inside rect and circle
		Vector2(150, 50),   # 1: Inside rect only
		Vector2(250, 250),  # 2: Outside both
		Vector2(75, 75)     # 3: Inside both
	])

	# -------------------------------------------------------------------------
	# points_in_rect - Filter points inside a rectangle
	# USE WHEN: Viewport culling, rectangular selection, zone checks
	# -------------------------------------------------------------------------
	var rect = Rect2(0, 0, 200, 100)
	print("points_in_rect (", rect, "):")
	print("  USE FOR: Viewport culling, rectangular selection")
	var in_rect = CollisionOps.points_in_rect(points, rect)
	print("  Points inside: ", in_rect)

	# Points in circle
	var center = Vector2(50, 50)
	var radius = 50.0
	print("\npoints_in_circle (center=", center, ", r=", radius, "):")
	var in_circle = CollisionOps.points_in_circle(points, center, radius)
	print("  Points inside: ", in_circle)

	# Points in polygon
	var polygon = PackedVector2Array([
		Vector2(0, 0), Vector2(100, 0), Vector2(100, 100), Vector2(0, 100)
	])
	print("\npoints_in_polygon (square 0-100):")
	var in_poly = CollisionOps.points_in_polygon(points, polygon)
	print("  Points inside: ", in_poly)

	# 3D versions
	print("\n3D containment:")
	var points_3d = PackedVector3Array([
		Vector3(50, 50, 50), Vector3(150, 50, 50)
	])
	var aabb = AABB(Vector3.ZERO, Vector3(100, 100, 100))
	var in_aabb = CollisionOps.points_in_aabb(points_3d, aabb)
	print("  points_in_aabb: ", in_aabb)

	var in_sphere = CollisionOps.points_in_sphere(points_3d, Vector3(50, 50, 50), 30.0)
	print("  points_in_sphere: ", in_sphere)
	print("")


func circle_collision() -> void:
	print("--- Circle vs Circle Collision ---")
	print("")
	print("The workhorse of bullet hell and action games.")
	print("Tests all circles in group A against all circles in group B.")
	print("")

	# -------------------------------------------------------------------------
	# circles_vs_circles - Group A vs Group B collision
	# USE WHEN: Bullets vs enemies, items vs player, projectiles vs targets
	# OUTPUT: Pairs of indices [a0, b0, a1, b1, ...] that collide
	# -------------------------------------------------------------------------

	# Group A: bullets
	var bullets = PackedVector2Array([
		Vector2(0, 0),
		Vector2(100, 0),
		Vector2(50, 50)
	])
	var bullet_radius = 5.0

	# Group B: enemies
	var enemies = PackedVector2Array([
		Vector2(3, 0),     # Collides with bullet 0
		Vector2(200, 0),   # No collision
		Vector2(55, 50)    # Collides with bullet 2
	])
	var enemy_radius = 10.0

	print("SCENARIO: Bullets (group A) vs Enemies (group B)")
	print("Bullets: ", bullets, " (radius ", bullet_radius, ")")
	print("Enemies: ", enemies, " (radius ", enemy_radius, ")")

	# Uniform radius per group (faster)
	print("\ncircles_vs_circles_uniform:")
	var pairs = CollisionOps.circles_vs_circles_uniform(
		bullets, bullet_radius,
		enemies, enemy_radius
	)
	print("  Collision pairs: ", pairs)
	print("  Format: [bullet_idx, enemy_idx, ...]")

	# Process collisions
	for i in range(0, pairs.size(), 2):
		var bullet_idx = pairs[i]
		var enemy_idx = pairs[i + 1]
		print("  Bullet ", bullet_idx, " hit enemy ", enemy_idx)

	# Variable radii
	print("\ncircles_vs_circles (variable radii):")
	var radii_a = PackedFloat32Array([5.0, 5.0, 5.0])
	var radii_b = PackedFloat32Array([10.0, 5.0, 15.0])
	var var_pairs = CollisionOps.circles_vs_circles(bullets, radii_a, enemies, radii_b)
	print("  Pairs: ", var_pairs)
	print("")


func self_collision() -> void:
	print("--- Self Collision (within same group) ---")
	print("")
	print("Find overlapping pairs within a single group.")
	print("USE CASES: Unit stacking prevention, boid separation, clustering")
	print("")

	# -------------------------------------------------------------------------
	# circles_self_collision - Find overlapping pairs in one group
	# USE WHEN: Preventing overlap, separating stacked units, physics
	# OUTPUT: Pairs of indices [i, j, ...] where i < j (no duplicates)
	# -------------------------------------------------------------------------
	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(5, 0),    # Overlaps with 0
		Vector2(100, 0),
		Vector2(102, 0)   # Overlaps with 2
	])
	var radius = 10.0

	print("SCENARIO: Find units that are too close together")
	print("Positions: ", positions, " (radius ", radius, ")")

	# Uniform radius
	print("\ncircles_self_collision_uniform:")
	var pairs = CollisionOps.circles_self_collision_uniform(positions, radius)
	print("  Overlapping pairs: ", pairs)

	# Each pair appears once (not duplicated)
	for i in range(0, pairs.size(), 2):
		print("  Entity ", pairs[i], " overlaps entity ", pairs[i + 1])

	# Variable radii
	print("\ncircles_self_collision (variable radii):")
	var radii = PackedFloat32Array([10.0, 10.0, 5.0, 5.0])
	var var_pairs = CollisionOps.circles_self_collision(positions, radii)
	print("  Pairs: ", var_pairs)
	print("")


func ray_intersection() -> void:
	print("--- Ray Intersection ---")
	print("")
	print("Cast rays and find what they hit.")
	print("USE CASES: Laser weapons, line of sight, hitscan bullets")
	print("")

	# -------------------------------------------------------------------------
	# Ray vs circles - Find what a ray hits
	# USE WHEN: Laser weapons, line of sight, hitscan bullets
	# OUTPUT: Distance to each circle (INF if no hit)
	# -------------------------------------------------------------------------
	var circles = PackedVector2Array([
		Vector2(50, 0),
		Vector2(100, 0),
		Vector2(150, 20)
	])
	var radii = PackedFloat32Array([10.0, 10.0, 10.0])

	var ray_origin = Vector2(0, 0)
	var ray_dir = Vector2(1, 0)

	print("SCENARIO: Laser beam scanning for targets")
	print("Ray from ", ray_origin, " direction ", ray_dir)
	print("Circles: ", circles, " radii: ", radii)

	# Distance to each circle
	print("\nray_vs_circles:")
	var distances = CollisionOps.ray_vs_circles(ray_origin, ray_dir, circles, radii)
	print("  Distances: ", distances)
	print("  (INF = no intersection)")

	# First circle hit
	print("\nray_first_circle (max_distance=200):")
	var hit_idx = CollisionOps.ray_first_circle(ray_origin, ray_dir, 200.0, circles, radii)
	print("  First hit: index ", hit_idx)
	if hit_idx >= 0:
		print("  Circle at ", circles[hit_idx], " hit at distance ", distances[hit_idx])

	# 3D ray vs spheres
	print("\n3D: ray_first_sphere:")
	var spheres = PackedVector3Array([Vector3(50, 0, 0), Vector3(100, 0, 0)])
	var sphere_radii = PackedFloat32Array([10.0, 10.0])
	var hit_3d = CollisionOps.ray_first_sphere(
		Vector3(0, 0, 0), Vector3(1, 0, 0), 200.0,
		spheres, sphere_radii
	)
	print("  First hit: index ", hit_3d)
	print("")


func segment_intersection() -> void:
	print("--- Segment Intersection ---")

	# Segment set A
	var starts_a = PackedVector2Array([
		Vector2(0, 0),
		Vector2(0, 100)
	])
	var ends_a = PackedVector2Array([
		Vector2(100, 100),
		Vector2(100, 100)
	])

	# Segment set B
	var starts_b = PackedVector2Array([
		Vector2(0, 100),
		Vector2(50, 0)
	])
	var ends_b = PackedVector2Array([
		Vector2(100, 0),
		Vector2(50, 200)
	])

	print("Segments A:")
	for i in range(starts_a.size()):
		print("  ", i, ": ", starts_a[i], " -> ", ends_a[i])

	print("Segments B:")
	for i in range(starts_b.size()):
		print("  ", i, ": ", starts_b[i], " -> ", ends_b[i])

	# Find intersecting pairs
	print("\nsegments_intersect:")
	var pairs = CollisionOps.segments_intersect(starts_a, ends_a, starts_b, ends_b)
	print("  Intersecting pairs: ", pairs)
	print("  Format: [A_idx, B_idx, ...]")

	# Get intersection points
	print("\nsegment_intersection_points:")
	var points = CollisionOps.segment_intersection_points(starts_a, ends_a, starts_b, ends_b)
	print("  Intersection points: ", points)

	# Self-intersection
	print("\nsegments_self_intersect (within single set):")
	var self_pairs = CollisionOps.segments_self_intersect(starts_a, ends_a)
	print("  Self-intersections: ", self_pairs)
	print("")


func closest_queries() -> void:
	print("--- Closest Point Queries ---")

	var query_points = PackedVector2Array([
		Vector2(0, 0),
		Vector2(100, 100)
	])

	var circles = PackedVector2Array([
		Vector2(50, 0),
		Vector2(100, 0),
		Vector2(200, 200)
	])
	var radii = PackedFloat32Array([10.0, 10.0, 10.0])

	print("Query points: ", query_points)
	print("Circles: ", circles)

	# Find closest circle to each point
	print("\nclosest_circle_indices_uniform:")
	var closest = CollisionOps.closest_circle_indices_uniform(query_points, circles, 10.0)
	print("  Closest circle for each query: ", closest)

	for i in range(query_points.size()):
		print("  Point ", query_points[i], " -> Circle ", closest[i], " at ", circles[closest[i]])

	# Variable radii version
	print("\nclosest_circle_indices (variable radii):")
	var closest_var = CollisionOps.closest_circle_indices(query_points, circles, radii)
	print("  Results: ", closest_var)
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Bullet Hell Collision ---")

	# Player bullets
	var bullets = PackedVector2Array()
	for i in range(20):
		bullets.append(Vector2(randf() * 400, randf() * 300))
	var bullet_radius = 3.0

	# Enemies
	var enemies = PackedVector2Array([
		Vector2(100, 150),
		Vector2(200, 100),
		Vector2(300, 200),
		Vector2(150, 250)
	])
	var enemy_radius = 20.0

	print("Bullets: ", bullets.size(), " (radius ", bullet_radius, ")")
	print("Enemies: ", enemies.size(), " (radius ", enemy_radius, ")")

	# Check collisions
	print("\n1. Checking bullet-enemy collisions:")
	var hits = CollisionOps.circles_vs_circles_uniform(
		bullets, bullet_radius,
		enemies, enemy_radius
	)
	print("   Total hits: ", hits.size() / 2)

	# Process hits
	var bullets_to_remove = []
	var enemy_hit_counts = [0, 0, 0, 0]
	for i in range(0, hits.size(), 2):
		var bullet_idx = hits[i]
		var enemy_idx = hits[i + 1]
		bullets_to_remove.append(bullet_idx)
		enemy_hit_counts[enemy_idx] += 1

	print("   Enemy hit counts: ", enemy_hit_counts)

	# Player ship
	var player_pos = Vector2(200, 280)
	var player_radius = 5.0  # Hitbox

	# Enemy bullets
	var enemy_bullets = PackedVector2Array()
	for i in range(50):
		enemy_bullets.append(Vector2(randf() * 400, randf() * 300))
	var enemy_bullet_radius = 4.0

	print("\n2. Checking player vs enemy bullets:")
	var player_hits = CollisionOps.circles_vs_circles_uniform(
		PackedVector2Array([player_pos]), player_radius,
		enemy_bullets, enemy_bullet_radius
	)
	if player_hits.size() > 0:
		print("   PLAYER HIT! ", player_hits.size() / 2, " bullets")
	else:
		print("   Player safe")

	# Viewport culling
	print("\n3. Viewport culling:")
	var viewport = Rect2(0, 0, 400, 300)
	var on_screen = CollisionOps.points_in_rect(bullets, viewport)
	print("   Bullets on screen: ", on_screen.size(), " / ", bullets.size())

	# Laser weapon (ray)
	print("\n4. Laser weapon:")
	var laser_origin = player_pos
	var laser_dir = Vector2(0, -1)  # Shooting up
	var laser_range = 300.0

	var laser_hit = CollisionOps.ray_first_circle(
		laser_origin, laser_dir, laser_range,
		enemies, PackedFloat32Array([enemy_radius, enemy_radius, enemy_radius, enemy_radius])
	)

	if laser_hit >= 0:
		print("   Laser hit enemy ", laser_hit, " at ", enemies[laser_hit])
	else:
		print("   Laser missed")

	# Bomb explosion (circular AoE)
	print("\n5. Bomb explosion:")
	var bomb_pos = Vector2(200, 150)
	var bomb_radius = 80.0

	var caught_enemies = CollisionOps.points_in_circle(enemies, bomb_pos, bomb_radius)
	var caught_bullets = CollisionOps.points_in_circle(enemy_bullets, bomb_pos, bomb_radius)

	print("   Enemies caught: ", caught_enemies.size())
	print("   Enemy bullets cleared: ", caught_bullets.size())

	print("")

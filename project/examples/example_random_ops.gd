extends SceneTree
## =============================================================================
## RandomOps Example - High-performance bulk random generation
## =============================================================================
##
## WHEN TO USE:
## ------------
## RandomOps is designed for generating large quantities of random values at once.
## It uses xoshiro256** PRNG which is both fast and high-quality.
##
## PERFECT FOR:
## - Procedural generation (spawning enemies, placing trees, loot drops)
## - Particle systems (random directions, speeds, lifetimes)
## - Shuffling decks, spawn queues, or turn orders
## - Blue noise distributions (Poisson disk sampling)
## - Monte Carlo simulations
## - Any system needing reproducible randomness (replays, seeds)
##
## NOT IDEAL FOR:
## - Single random values (just use Godot's built-in randf())
## - Cryptographic purposes (use a crypto-safe RNG instead)
##
## KEY FEATURES:
## - Deterministic seeding for reproducible results
## - Bulk generation (100x faster than loops for large arrays)
## - Spatial distributions (points in shapes)
## - Weighted selection for loot tables
## - Poisson disk sampling for natural-looking placement
##
## INSTANCE VS STATIC:
## - RandomOps is INSTANCED (unlike most other AgentiteG classes)
## - Create instance -> seed it -> generate values
## - Each instance has its own independent state
##
## Run with: godot --headless -s examples/example_random_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("RandomOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	seeding()
	float_generation()
	integer_generation()
	points_in_shapes()
	shuffling()
	weighted_selection()
	distributions()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func seeding() -> void:
	print("--- Seeding ---")
	print("")
	print("Deterministic seeding is ESSENTIAL for:")
	print("  - Procedural worlds (same seed = same world)")
	print("  - Replays (replay same random events)")
	print("  - Debugging (reproduce exact bug conditions)")
	print("  - Multiplayer sync (all clients generate same 'random' values)")
	print("")

	var rng = RandomOps.new()

	# -------------------------------------------------------------------------
	# Deterministic seed - same seed = same sequence
	# USE WHEN: Reproducible procedural content, replays, testing
	# -------------------------------------------------------------------------
	rng.seed(12345)
	var seq1 = rng.randf_array(5)

	rng.seed(12345)  # Reset to same seed
	var seq2 = rng.randf_array(5)

	print("Deterministic seeding:")
	print("  Seed 12345, sequence 1: ", seq1)
	print("  Seed 12345, sequence 2: ", seq2)
	print("  Identical: ", seq1 == seq2)

	# Non-deterministic (time-based)
	rng.seed_from_time()
	print("\nseed_from_time() for non-deterministic behavior")
	print("")


func float_generation() -> void:
	print("--- Float Generation ---")
	print("")
	print("Bulk float generation is faster than calling randf() in a loop.")
	print("USE CASES: Particle lifetimes, spawn delays, damage variance")
	print("")

	var rng = RandomOps.new()
	rng.seed(42)

	# -------------------------------------------------------------------------
	# randf_array - Generate floats in [0, 1)
	# USE WHEN: Probability checks, interpolation factors, normalized values
	# -------------------------------------------------------------------------
	var floats = rng.randf_array(10)
	print("randf_array(10) - values in [0, 1):")
	print("  ", floats)

	# -------------------------------------------------------------------------
	# randf_range_array - Generate floats in custom range
	# USE WHEN: Spawn heights, damage ranges, speeds with variance
	# -------------------------------------------------------------------------
	var ranged = rng.randf_range_array(10, 10.0, 20.0)
	print("\nrandf_range_array(10, 10, 20) - values in [10, 20):")
	print("  ", ranged)
	print("")


func integer_generation() -> void:
	print("--- Integer Generation ---")
	print("")
	print("Integer generation for indices, counts, and discrete choices.")
	print("")

	var rng = RandomOps.new()
	rng.seed(42)

	# -------------------------------------------------------------------------
	# randi_array - Generate random 32-bit integers
	# USE WHEN: Hash seeds, unique IDs, bit manipulation
	# -------------------------------------------------------------------------
	var ints = rng.randi_array(10)
	print("randi_array(10) - random 32-bit integers:")
	print("  ", ints)

	# -------------------------------------------------------------------------
	# randi_range_array - Generate integers in inclusive range
	# USE WHEN: Dice rolls, enemy counts, discrete selections
	# -------------------------------------------------------------------------
	var dice = rng.randi_range_array(10, 1, 6)
	print("\nrandi_range_array(10, 1, 6) - dice rolls:")
	print("  ", dice)
	print("")


func points_in_shapes() -> void:
	print("--- Points in Shapes ---")
	print("")
	print("Generate random positions within geometric shapes.")
	print("CRITICAL for: Spawn systems, particle emitters, area effects")
	print("")

	var rng = RandomOps.new()
	rng.seed(42)

	# -------------------------------------------------------------------------
	# rand_points_in_rect - Random points in rectangle
	# USE WHEN: Rectangular spawn zones, UI particle effects
	# -------------------------------------------------------------------------
	var rect = Rect2(0, 0, 100, 100)
	var rect_points = rng.rand_points_in_rect(5, rect)
	print("rand_points_in_rect(5, ", rect, "):")
	print("  ", rect_points)

	# -------------------------------------------------------------------------
	# rand_points_in_circle - Uniform distribution inside circle
	# USE WHEN: Circular spawn zones, explosion debris, AoE effects
	# NOTE: Uses proper uniform distribution (not biased toward center)
	# -------------------------------------------------------------------------
	var circle_points = rng.rand_points_in_circle(5, Vector2(50, 50), 25.0)
	print("\nrand_points_in_circle(5, center=(50,50), r=25):")
	print("  ", circle_points)

	# -------------------------------------------------------------------------
	# rand_points_on_circle - Points ON the circle edge (not inside)
	# USE WHEN: Ring formations, orbital spawns, radial patterns
	# -------------------------------------------------------------------------
	var perimeter_points = rng.rand_points_on_circle(5, Vector2.ZERO, 10.0)
	print("\nrand_points_on_circle(5, center=origin, r=10):")
	print("  ", perimeter_points)
	for p in perimeter_points:
		print("    Distance from center: ", "%.4f" % p.length())

	# -------------------------------------------------------------------------
	# 3D shape variants - Same concepts in 3D space
	# USE WHEN: Space games, volumetric effects, 3D spawn systems
	# -------------------------------------------------------------------------
	var box = AABB(Vector3.ZERO, Vector3(100, 100, 100))
	var box_points = rng.rand_points_in_box(3, box)
	print("\nrand_points_in_box(3, AABB):")
	print("  ", box_points)

	var sphere_points = rng.rand_points_in_sphere(3, Vector3.ZERO, 10.0)
	print("\nrand_points_in_sphere(3, origin, r=10):")
	print("  ", sphere_points)
	print("")


func shuffling() -> void:
	print("--- Shuffling ---")
	print("")
	print("Fisher-Yates shuffling for unbiased randomization.")
	print("USE CASES: Card decks, spawn orders, playlist shuffles")
	print("")

	var rng = RandomOps.new()
	rng.seed(42)

	# -------------------------------------------------------------------------
	# shuffle_indices - Get shuffled index array
	# USE WHEN: Randomizing order without copying data
	# -------------------------------------------------------------------------
	var indices = rng.shuffle_indices(10)
	print("shuffle_indices(10):")
	print("  ", indices)

	# -------------------------------------------------------------------------
	# shuffle_floats / shuffle_ints - Shuffle actual arrays
	# USE WHEN: Direct array shuffling needed
	# -------------------------------------------------------------------------
	var values = PackedFloat32Array([1, 2, 3, 4, 5])
	var shuffled = rng.shuffle_floats(values)
	print("\nshuffle_floats([1,2,3,4,5]):")
	print("  Original: ", values)
	print("  Shuffled: ", shuffled)

	# Shuffle int array
	var int_values = PackedInt32Array([10, 20, 30, 40, 50])
	var shuffled_ints = rng.shuffle_ints(int_values)
	print("\nshuffle_ints([10,20,30,40,50]):")
	print("  Shuffled: ", shuffled_ints)
	print("")


func weighted_selection() -> void:
	print("--- Weighted Selection ---")
	print("")
	print("Weighted random selection for loot tables, enemy spawns, etc.")
	print("Weights don't need to sum to 1.0 - they're normalized automatically.")
	print("")

	var rng = RandomOps.new()
	rng.seed(42)

	# -------------------------------------------------------------------------
	# weighted_choice - Select indices based on weights
	# USE WHEN: Loot drops, enemy type selection, random events
	# Weights are relative (don't need to sum to 1.0)
	# -------------------------------------------------------------------------
	var weights = PackedFloat32Array([0.50, 0.30, 0.15, 0.05])
	var rarity_names = ["Common", "Uncommon", "Rare", "Legendary"]

	print("Loot weights: Common=50%, Uncommon=30%, Rare=15%, Legendary=5%")

	# Single selection
	var single = rng.weighted_choice_one(weights)
	print("\nweighted_choice_one: ", rarity_names[single])

	# Multiple selections
	var choices = rng.weighted_choice(weights, 100)
	var counts = [0, 0, 0, 0]
	for c in choices:
		counts[c] += 1

	print("\nweighted_choice(100 samples):")
	for i in range(4):
		print("  ", rarity_names[i], ": ", counts[i], "%")
	print("")


func distributions() -> void:
	print("--- Special Distributions ---")
	print("")
	print("Special distributions for specific game mechanics.")
	print("")

	var rng = RandomOps.new()
	rng.seed(42)

	# -------------------------------------------------------------------------
	# rand_directions_2d/3d - Random unit vectors
	# USE WHEN: Particle directions, scatter patterns, AI wander
	# All vectors have length 1.0 (normalized)
	# -------------------------------------------------------------------------
	var dirs_2d = rng.rand_directions_2d(5)
	print("rand_directions_2d(5) - unit vectors:")
	print("  ", dirs_2d)
	print("  Lengths: ", dirs_2d[0].length(), ", ", dirs_2d[1].length())

	var dirs_3d = rng.rand_directions_3d(3)
	print("\nrand_directions_3d(3):")
	print("  ", dirs_3d)

	# -------------------------------------------------------------------------
	# randn_array - Gaussian (normal) distribution
	# USE WHEN: Natural variation, bell curves, realistic randomness
	# Standard normal: mean=0, std_dev=1
	# -------------------------------------------------------------------------
	var gaussian = rng.randn_array(10)
	print("\nrandn_array(10) - standard normal (mean=0, std=1):")
	print("  ", gaussian)

	var gaussian_custom = rng.randn_array_params(10, 100.0, 15.0)
	print("\nrandn_array_params(10, mean=100, std=15):")
	print("  ", gaussian_custom)

	# -------------------------------------------------------------------------
	# poisson_disk_2d - Blue noise distribution
	# USE WHEN: Natural-looking placement (trees, rocks, NPCs)
	# Guarantees minimum spacing between all points
	# Third parameter (k) is attempts per point - higher = denser fill
	# -------------------------------------------------------------------------
	print("\npoisson_disk_2d - evenly spaced random points:")
	print("  PERFECT FOR: Tree placement, item spawns, crowd distribution")
	var bounds = Rect2(0, 0, 100, 100)
	var min_distance = 15.0
	var poisson = rng.poisson_disk_2d(bounds, min_distance, 30)
	print("  Bounds: ", bounds, ", Min distance: ", min_distance)
	print("  Generated ", poisson.size(), " points")

	# Verify minimum distance
	var min_found = INF
	for i in range(poisson.size()):
		for j in range(i + 1, poisson.size()):
			var d = poisson[i].distance_to(poisson[j])
			if d < min_found:
				min_found = d
	print("  Minimum distance between points: ", "%.2f" % min_found)
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Procedural Spawning ---")
	print("")
	print("This demonstrates a complete procedural generation workflow.")
	print("The same seed will always produce the same world - essential for")
	print("multiplayer sync, replays, and shareable world seeds.")
	print("")

	var rng = RandomOps.new()
	rng.seed(12345)  # Deterministic for reproducible levels

	print("World generation seed: 12345")
	print("  TIP: Share this seed to let others play the same generated world")

	# Tree placement with blue noise (evenly spaced)
	print("\n1. Forest generation (Poisson disk):")
	var forest_bounds = Rect2(0, 0, 500, 500)
	var tree_spacing = 20.0
	var tree_positions = rng.poisson_disk_2d(forest_bounds, tree_spacing, 30)
	print("   Generated ", tree_positions.size(), " trees")
	print("   First 3: ", tree_positions.slice(0, 3))

	# Enemy wave spawning
	print("\n2. Enemy wave spawning:")
	var spawn_center = Vector2(250, 250)
	var spawn_radius = 50.0
	var wave_size = 10

	var enemy_spawns = rng.rand_points_in_circle(wave_size, spawn_center, spawn_radius)
	print("   ", wave_size, " enemies spawning around ", spawn_center)
	print("   Spawn positions: ", enemy_spawns.slice(0, 3), "...")

	# Loot drops with weighted rarity
	print("\n3. Loot chest contents:")
	var loot_weights = PackedFloat32Array([0.5, 0.3, 0.15, 0.05])
	var loot_names = ["Gold", "Potion", "Weapon", "Artifact"]
	var chest_contents = rng.weighted_choice(loot_weights, 5)
	print("   Chest contains:")
	for idx in chest_contents:
		print("     - ", loot_names[idx])

	# Particle effect directions
	print("\n4. Explosion particle directions:")
	var particle_count = 20
	var directions = rng.rand_directions_2d(particle_count)
	var speeds = rng.randf_range_array(particle_count, 50.0, 150.0)
	print("   ", particle_count, " particles")
	print("   Sample directions: ", directions.slice(0, 3))
	print("   Sample speeds: ", speeds.slice(0, 3))

	# Random patrol waypoints
	print("\n5. Guard patrol waypoints:")
	var patrol_area = Rect2(100, 100, 300, 200)
	var waypoints = rng.rand_points_in_rect(4, patrol_area)
	print("   Patrol area: ", patrol_area)
	print("   Waypoints: ", waypoints)

	# Shuffled spawn order
	print("\n6. Randomized spawn order:")
	var spawn_queue = PackedInt32Array([0, 1, 2, 3, 4])  # Enemy type IDs
	var shuffled_queue = rng.shuffle_ints(spawn_queue)
	print("   Original order: ", spawn_queue)
	print("   Randomized: ", shuffled_queue)

	print("")

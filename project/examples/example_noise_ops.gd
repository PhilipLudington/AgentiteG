extends SceneTree
## =============================================================================
## NoiseOps Example - Procedural noise generation
## =============================================================================
##
## WHEN TO USE:
## ------------
## NoiseOps generates coherent noise - random values that vary smoothly in space.
## Essential for procedural content that looks natural rather than random.
##
## PERFECT FOR:
## - Terrain heightmaps (hills, mountains, valleys)
## - Texture generation (clouds, marble, wood grain)
## - Animation variation (camera shake, wind effects)
## - Cave systems and dungeon generation
## - Biome boundaries and transitions
## - Water/fire/smoke effects
##
## NOISE TYPES EXPLAINED:
## - PERLIN: Classic gradient noise, smooth hills and valleys
## - SIMPLEX: Faster Perlin, fewer artifacts, better in higher dimensions
## - WORLEY: Cellular/Voronoi noise, great for stones, cells, cracks
## - FBM: Layered noise for natural detail (terrain, clouds)
## - RIDGED: Sharp ridges (mountain ranges, rivers)
## - TURBULENCE: Absolute value noise (fire, smoke, clouds)
##
## KEY CONCEPTS:
## - Frequency: How "zoomed in" the noise is (lower = larger features)
## - Octaves: Number of detail layers (more = more detail but slower)
## - Persistence: How much each octave contributes (0.5 is standard)
## - Lacunarity: Frequency multiplier per octave (2.0 is standard)
##
## INSTANCE VS STATIC:
## - NoiseOps is INSTANCED (like RandomOps)
## - Create instance -> configure -> sample
##
## Run with: godot --headless -s examples/example_noise_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("NoiseOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	configuration()
	basic_noise_types()
	fractal_variants()
	grid_sampling()
	domain_warping()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func configuration() -> void:
	print("--- Configuration ---")
	print("")
	print("Configure noise BEFORE sampling. Same config = same results.")
	print("")

	var noise = NoiseOps.new()

	# -------------------------------------------------------------------------
	# set_seed - Deterministic noise generation
	# USE WHEN: Reproducible worlds, shareable seeds
	# -------------------------------------------------------------------------
	noise.set_seed(12345)
	print("Seed: 12345")
	print("  TIP: Same seed + config = identical terrain everywhere")

	# Octaves: number of noise layers (1-16)
	noise.set_octaves(4)
	print("Octaves: 4 (more = more detail, slower)")

	# Persistence: amplitude decay per octave (0-1)
	noise.set_persistence(0.5)
	print("Persistence: 0.5 (lower = smoother)")

	# Lacunarity: frequency growth per octave
	noise.set_lacunarity(2.0)
	print("Lacunarity: 2.0 (higher = more detail)")

	# Frequency: base scale
	noise.set_frequency(0.1)
	print("Frequency: 0.1 (lower = larger features)")
	print("")


func basic_noise_types() -> void:
	print("--- Basic Noise Types ---")
	print("")
	print("Each noise type has different visual characteristics.")
	print("Choose based on what you're generating.")
	print("")

	var noise = NoiseOps.new()
	noise.set_seed(42)
	noise.set_frequency(0.1)

	var sample_pos = Vector2(10.5, 20.3)

	# -------------------------------------------------------------------------
	# PERLIN NOISE - Classic smooth gradient noise
	# USE WHEN: Terrain, clouds, gentle variations
	# OUTPUT: Approximately [-1, 1]
	# -------------------------------------------------------------------------
	print("PERLIN NOISE (smooth, continuous):")
	print("  USE FOR: Rolling hills, cloud density, gentle height variation")
	var perlin = noise.perlin_2d(sample_pos)
	print("  perlin_2d", sample_pos, " = ", "%.4f" % perlin)
	print("  Range: approximately [-1, 1]")

	# Verify determinism
	var perlin2 = noise.perlin_2d(sample_pos)
	print("  Deterministic: ", abs(perlin - perlin2) < 0.0001)

	# -------------------------------------------------------------------------
	# SIMPLEX NOISE - Improved Perlin
	# USE WHEN: Same as Perlin but need better performance
	# ADVANTAGE: Faster, no directional artifacts, scales better to 3D/4D
	# -------------------------------------------------------------------------
	print("\nSIMPLEX NOISE (faster, less directional artifacts):")
	print("  USE FOR: Same as Perlin, but faster and cleaner")
	var simplex = noise.simplex_2d(sample_pos)
	print("  simplex_2d", sample_pos, " = ", "%.4f" % simplex)

	# -------------------------------------------------------------------------
	# WORLEY NOISE - Cellular/Voronoi noise
	# USE WHEN: Organic cells, cracked earth, stone textures
	# OUTPUT: Distance to nearest cell center (0 at center, higher at edges)
	# -------------------------------------------------------------------------
	print("\nWORLEY NOISE (cellular/voronoi-like):")
	print("  USE FOR: Cobblestones, cracked earth, cell structures, caves")
	var worley = noise.worley_2d(sample_pos)
	print("  worley_2d", sample_pos, " = ", "%.4f" % worley)
	print("  Range: [0, ~1.5] (distance to cell center)")
	print("  TIP: Threshold at low values for cave systems")

	# 3D versions
	print("\n3D versions:")
	var pos_3d = Vector3(5.0, 5.0, 5.0)
	print("  perlin_3d", pos_3d, " = ", "%.4f" % noise.perlin_3d(pos_3d))
	print("  simplex_3d", pos_3d, " = ", "%.4f" % noise.simplex_3d(pos_3d))
	print("  worley_3d", pos_3d, " = ", "%.4f" % noise.worley_3d(pos_3d))
	print("")


func fractal_variants() -> void:
	print("--- Fractal Variants ---")
	print("")
	print("Fractal noise adds multiple layers of detail at different scales.")
	print("This creates natural-looking complexity from simple algorithms.")
	print("")

	var noise = NoiseOps.new()
	noise.set_seed(42)
	noise.set_octaves(4)  # 4 layers of detail
	noise.set_frequency(0.05)  # Large base features

	var pos = Vector2(10.0, 10.0)

	# -------------------------------------------------------------------------
	# FBM (Fractal Brownian Motion) - The workhorse of procedural generation
	# USE WHEN: Natural terrain, clouds, anything organic-looking
	# Combines multiple octaves of noise for natural detail
	# -------------------------------------------------------------------------
	print("FBM (natural terrain, clouds):")
	print("  USE FOR: Base terrain, cloud textures, organic surfaces")
	var fbm = noise.fbm_2d(pos)
	print("  fbm_2d", pos, " = ", "%.4f" % fbm)

	# -------------------------------------------------------------------------
	# RIDGED MULTIFRACTAL - Sharp ridges and valleys
	# USE WHEN: Mountain ranges, river networks, lightning
	# Creates sharp peaks where noise crosses zero
	# -------------------------------------------------------------------------
	print("\nRIDGED (mountains, rivers):")
	print("  USE FOR: Mountain ridges, river beds, cracks, lightning")
	var ridged = noise.ridged_2d(pos)
	print("  ridged_2d", pos, " = ", "%.4f" % ridged)

	# -------------------------------------------------------------------------
	# TURBULENCE - Absolute value noise (always positive)
	# USE WHEN: Fire, smoke, marble textures
	# Takes absolute value of each octave - creates "billowy" effect
	# -------------------------------------------------------------------------
	print("\nTURBULENCE (fire, smoke, clouds):")
	print("  USE FOR: Fire, smoke, marble veins, turbulent water")
	var turb = noise.turbulence_2d(pos)
	print("  turbulence_2d", pos, " = ", "%.4f" % turb)
	print("  Always positive (uses absolute values)")
	print("")


func grid_sampling() -> void:
	print("--- Grid Sampling (Heightmaps) ---")
	print("")
	print("Grid sampling generates a 2D array of noise values.")
	print("Perfect for heightmaps, texture generation, and tile maps.")
	print("")

	var noise = NoiseOps.new()
	noise.set_seed(42)
	noise.set_octaves(4)
	noise.set_frequency(0.05)

	# -------------------------------------------------------------------------
	# Grid sampling - Generate heightmap in one call
	# USE WHEN: Terrain generation, texture creation, chunk loading
	# Much faster than sampling each point individually
	# -------------------------------------------------------------------------
	var origin = Vector2.ZERO
	var cell_size = Vector2(1, 1)
	var width = 4
	var height = 4

	print("Perlin 2D grid (", width, "x", height, "):")
	print("  TIP: Use for terrain chunks, texture generation")
	var grid = noise.perlin_2d_grid(origin, cell_size, width, height)
	print("  Size: ", grid.size(), " values (row-major order)")

	# Display as grid
	print("  Grid values:")
	for y in range(height):
		var row = "    "
		for x in range(width):
			row += "%6.2f " % grid[y * width + x]
		print(row)

	# FBM grid for terrain
	print("\nFBM 2D grid (terrain heightmap):")
	var fbm_grid = noise.fbm_2d_grid(origin, cell_size, width, height)
	print("  Grid values:")
	for y in range(height):
		var row = "    "
		for x in range(width):
			row += "%6.2f " % fbm_grid[y * width + x]
		print(row)

	# Batch sampling at arbitrary positions
	print("\nBatch sampling at specific positions:")
	var sample_points = PackedVector2Array([
		Vector2(0, 0), Vector2(5, 5), Vector2(10, 10)
	])
	var batch_values = noise.fbm_2d_batch(sample_points)
	for i in range(sample_points.size()):
		print("  ", sample_points[i], " = ", "%.4f" % batch_values[i])
	print("")


func domain_warping() -> void:
	print("--- Domain Warping ---")
	print("")
	print("Domain warping distorts coordinates before sampling noise.")
	print("Creates organic, flowing shapes instead of regular patterns.")
	print("")

	var noise = NoiseOps.new()
	noise.set_seed(42)
	noise.set_frequency(0.1)

	# -------------------------------------------------------------------------
	# warp_2d - Distort sample positions using noise
	# USE WHEN: Organic biome boundaries, flowing rivers, realistic coastlines
	# Strength controls how much distortion is applied
	# -------------------------------------------------------------------------
	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(10, 0),
		Vector2(0, 10),
		Vector2(10, 10)
	])

	print("Domain warping USE CASES:")
	print("  - Organic biome transitions (not grid-aligned)")
	print("  - Natural coastlines and rivers")
	print("  - Swirling cloud/smoke patterns")
	print("")

	print("Original positions: ", positions)

	var warp_strength = 5.0
	var warped = noise.warp_2d(positions, warp_strength)

	print("warp_2d (strength=", warp_strength, "):")
	print("  Warped positions: ", warped)
	print("  TIP: Higher strength = more distortion")

	# Sample noise at warped positions for organic shapes
	var values_original = noise.fbm_2d_batch(positions)
	var values_warped = noise.fbm_2d_batch(warped)

	print("\nFBM at original vs warped positions:")
	for i in range(positions.size()):
		print("  ", positions[i], ": ", "%.4f" % values_original[i],
			  " -> warped: ", "%.4f" % values_warped[i])
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Terrain Generation ---")

	var noise = NoiseOps.new()
	noise.set_seed(world_seed())
	print("World seed: ", world_seed())

	# Terrain layer 1: Base terrain (large features)
	print("\n1. Base terrain (hills and valleys):")
	noise.set_frequency(0.01)
	noise.set_octaves(4)
	var base = noise.fbm_2d(Vector2(100, 100))
	print("   FBM at (100,100): ", "%.4f" % base)

	# Terrain layer 2: Mountains (ridged noise)
	print("\n2. Mountain ridges:")
	noise.set_frequency(0.02)
	var mountains = noise.ridged_2d(Vector2(100, 100))
	print("   Ridged at (100,100): ", "%.4f" % mountains)

	# Terrain layer 3: Detail (high frequency)
	print("\n3. Surface detail:")
	noise.set_frequency(0.1)
	noise.set_octaves(2)
	var detail = noise.perlin_2d(Vector2(100, 100))
	print("   Detail at (100,100): ", "%.4f" % detail)

	# Combine layers
	var final_height = base * 0.6 + mountains * 0.3 + detail * 0.1
	print("\n   Combined height: ", "%.4f" % final_height)

	# Cave system using Worley noise
	print("\n4. Cave system (Worley noise):")
	noise.set_frequency(0.05)
	var cave = noise.worley_3d(Vector3(50, 10, 50))
	print("   Worley 3D value: ", "%.4f" % cave)
	print("   Cave threshold: 0.3 (values below = air)")
	print("   Is cave: ", cave < 0.3)

	# Generate a small heightmap
	print("\n5. 8x8 heightmap preview:")
	noise.set_frequency(0.1)
	noise.set_octaves(4)
	var heightmap = noise.fbm_2d_grid(Vector2.ZERO, Vector2(1, 1), 8, 8)

	# Visualize with ASCII
	var chars = " .:-=+*#@"
	for y in range(8):
		var row = "   "
		for x in range(8):
			var val = heightmap[y * 8 + x]
			var normalized = (val + 1.0) / 2.0  # Map -1..1 to 0..1
			var char_idx = int(clamp(normalized * (chars.length() - 1), 0, chars.length() - 1))
			row += chars[char_idx]
		print(row)

	# Biome blending with domain warping
	print("\n6. Organic biome boundaries (domain warping):")
	var biome_samples = PackedVector2Array([
		Vector2(0, 0), Vector2(50, 0), Vector2(100, 0)
	])
	var warped = noise.warp_2d(biome_samples, 10.0)
	print("   Original: ", biome_samples)
	print("   Warped:   ", warped)

	print("")


func world_seed() -> int:
	return 42

extends SceneTree
## =============================================================================
## BatchOps Example - Steering behaviors and game loop operations
## =============================================================================
##
## WHEN TO USE:
## ------------
## BatchOps handles the core game loop operations that run every frame:
## updating positions, velocities, and implementing AI steering behaviors.
##
## PERFECT FOR:
## - Boids/flocking simulations
## - RTS unit movement
## - Crowd simulations
## - Swarm AI (drones, fish, birds)
## - Any game updating many entities per frame
##
## KEY BEHAVIORS:
## - SEEK: Move toward a target at max speed
## - FLEE: Move away from a threat at max speed
## - ARRIVE: Move toward target, slowing down as you approach
## - SEPARATION: Push apart entities that are too close
## - COHESION: Steer toward the center of nearby entities
## - ALIGNMENT: Match velocity of nearby entities
## - FLOCK: Combined separation + cohesion + alignment
##
## PERFORMANCE NOTE:
## Flocking behaviors (separation, cohesion, alignment) are O(n²).
## For >500 entities, use SpatialHash2D to find neighbors first.
##
## All methods are STATIC - call on the class directly.
## Most 2D methods have 3D equivalents (replace _2d with _3d).
##
## Run with: godot --headless -s examples/example_batch_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("BatchOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	velocity_updates()
	steering_behaviors()
	flocking_behaviors()
	obstacle_avoidance()
	game_scenario_boids()

	print("\nAll examples completed!")
	quit(0)


func velocity_updates() -> void:
	print("--- Velocity / Position Updates ---")
	print("")
	print("These are the fundamental operations of any physics loop.")
	print("They replace the common pattern of looping through entities.")
	print("")

	var positions = PackedVector2Array([
		Vector2(0, 0), Vector2(100, 100)
	])
	var velocities = PackedVector2Array([
		Vector2(100, 50), Vector2(-20, 30)
	])
	var accelerations = PackedVector2Array([
		Vector2(0, 10), Vector2(5, -5)  # Gravity-like
	])
	var delta = 0.1

	print("Positions: ", positions)
	print("Velocities: ", velocities)
	print("Delta: ", delta)

	# -------------------------------------------------------------------------
	# apply_velocities - Update positions based on velocity
	# USE WHEN: Every frame to move entities
	# REPLACES: for i in range(n): positions[i] += velocities[i] * delta
	# -------------------------------------------------------------------------
	print("\napply_velocities_2d:")
	print("  USE CASE: Main physics loop position update")
	print("  REPLACES: for i: positions[i] += velocities[i] * delta")
	var new_positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
	print("  New positions: ", new_positions)

	# -------------------------------------------------------------------------
	# apply_accelerations - Update velocities based on acceleration
	# USE WHEN: Applying forces (gravity, thrust, steering)
	# REPLACES: for i in range(n): velocities[i] += accel[i] * delta
	# -------------------------------------------------------------------------
	print("\napply_accelerations_2d:")
	print("  USE CASE: Applying forces to velocities")
	print("  REPLACES: for i: velocities[i] += accel[i] * delta")
	var new_velocities = BatchOps.apply_accelerations_2d(velocities, accelerations, delta)
	print("  New velocities: ", new_velocities)

	# -------------------------------------------------------------------------
	# limit_velocity - Clamp velocity to max speed
	# USE WHEN: Preventing entities from exceeding speed limit
	# -------------------------------------------------------------------------
	print("\nlimit_velocity_2d:")
	print("  USE CASE: Enforce maximum speed")
	var fast_velocities = PackedVector2Array([Vector2(100, 0), Vector2(5, 5)])
	var max_speed = 20.0
	print("  Input: ", fast_velocities, " (max=", max_speed, ")")
	print("  Output: ", BatchOps.limit_velocity_2d(fast_velocities, max_speed))

	# -------------------------------------------------------------------------
	# limit_velocity_range - Clamp to min AND max speed
	# USE WHEN: Preventing both stopping and overspeeding
	# -------------------------------------------------------------------------
	print("\nlimit_velocity_range_2d:")
	print("  USE CASE: Entities must keep moving (min speed > 0)")
	var slow_velocities = PackedVector2Array([Vector2(2, 0), Vector2(50, 0)])
	print("  Input: ", slow_velocities, " (min=5, max=20)")
	print("  Output: ", BatchOps.limit_velocity_range_2d(slow_velocities, 5.0, 20.0))
	print("")


func steering_behaviors() -> void:
	print("--- Steering Behaviors ---")
	print("")
	print("Steering behaviors return DESIRED VELOCITY, not forces.")
	print("To steer smoothly: steering = desired - current_velocity")
	print("Then apply steering as an acceleration.")
	print("")

	var positions = PackedVector2Array([
		Vector2(0, 0), Vector2(100, 0), Vector2(50, 100)
	])
	var max_speed = 10.0

	# -------------------------------------------------------------------------
	# SEEK - Move toward target at full speed
	# USE WHEN: Chasing, pursuing, moving to waypoint
	# Returns desired velocity pointing at target
	# -------------------------------------------------------------------------
	print("SEEK - Move toward targets at max speed:")
	print("  USE CASE: Enemy chasing player, unit moving to waypoint")
	var seek_targets = PackedVector2Array([
		Vector2(100, 100), Vector2(0, 100), Vector2(50, 0)
	])
	var desired = BatchOps.seek_batch(positions, seek_targets, max_speed)
	print("  Positions: ", positions)
	print("  Targets: ", seek_targets)
	print("  Desired velocities: ", desired)

	# -------------------------------------------------------------------------
	# FLEE - Move away from threat at full speed
	# USE WHEN: Running away, avoiding danger zones
	# Opposite of seek
	# -------------------------------------------------------------------------
	print("\nFLEE - Move away from threats at max speed:")
	print("  USE CASE: Civilian fleeing danger, retreating unit")
	var threats = PackedVector2Array([
		Vector2(10, 10), Vector2(90, 10), Vector2(50, 90)
	])
	var flee_vel = BatchOps.flee_batch(positions, threats, max_speed)
	print("  Threats: ", threats)
	print("  Desired velocities: ", flee_vel)

	# -------------------------------------------------------------------------
	# ARRIVE - Move toward target, slowing down when close
	# USE WHEN: Smooth stopping at destination
	# Better than seek for final approach
	# -------------------------------------------------------------------------
	print("\nARRIVE - Move toward target, slow down when close:")
	print("  USE CASE: Unit reaching destination smoothly")
	var slowing_radius = 50.0

	var arrive_positions = PackedVector2Array([
		Vector2(0, 0),      # Far from target
		Vector2(90, 0)      # Close to target (within slowing radius)
	])
	var arrive_targets = PackedVector2Array([
		Vector2(100, 0),    # 100 units away
		Vector2(100, 0)     # 10 units away
	])

	var arrive_vel = BatchOps.arrive_batch(arrive_positions, arrive_targets, max_speed, slowing_radius)
	print("  Slowing radius: ", slowing_radius)
	print("  Positions: ", arrive_positions)
	print("  Targets: ", arrive_targets)
	print("  Desired velocities: ", arrive_vel)
	print("  NOTE: Second entity is slower (within slowing radius)")
	print("")


func flocking_behaviors() -> void:
	print("--- Flocking Behaviors (Boids) ---")
	print("")
	print("Craig Reynolds' boids algorithm has three rules:")
	print("  1. SEPARATION: Don't crowd neighbors")
	print("  2. COHESION: Steer toward center of flock")
	print("  3. ALIGNMENT: Match velocity of neighbors")
	print("")
	print("WARNING: These are O(n²). For >500 entities, use spatial hashing.")
	print("")

	# Small flock of 4 boids
	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(10, 0),
		Vector2(5, 10),
		Vector2(100, 100)  # Far away - won't affect others
	])
	var velocities = PackedVector2Array([
		Vector2(1, 0),
		Vector2(1, 0),
		Vector2(0, 1),
		Vector2(-1, -1)
	])

	print("Positions: ", positions)
	print("Velocities: ", velocities)

	# -------------------------------------------------------------------------
	# SEPARATION - Push apart entities that are too close
	# USE WHEN: Preventing overlap, maintaining personal space
	# Force is stronger when closer
	# -------------------------------------------------------------------------
	print("\nSEPARATION - Push apart nearby boids:")
	print("  USE CASE: Prevent units from stacking, maintain formation gaps")
	var sep_radius = 20.0
	var sep_strength = 1.0
	var sep_forces = BatchOps.separation_2d(positions, sep_radius, sep_strength)
	print("  Radius: ", sep_radius, ", Strength: ", sep_strength)
	print("  Forces: ", sep_forces)
	print("  NOTE: Boid 3 (at 100,100) has zero force - no neighbors")

	# -------------------------------------------------------------------------
	# COHESION - Steer toward center of nearby flock
	# USE WHEN: Keeping group together
	# -------------------------------------------------------------------------
	print("\nCOHESION - Steer toward local center of mass:")
	print("  USE CASE: Keep flocks together, group movement")
	var coh_radius = 50.0
	var coh_strength = 0.5
	var coh_forces = BatchOps.cohesion_2d(positions, coh_radius, coh_strength)
	print("  Radius: ", coh_radius, ", Strength: ", coh_strength)
	print("  Forces: ", coh_forces)

	# -------------------------------------------------------------------------
	# ALIGNMENT - Match velocity of neighbors
	# USE WHEN: Coordinated movement, schooling fish
	# -------------------------------------------------------------------------
	print("\nALIGNMENT - Match neighbor velocities:")
	print("  USE CASE: Schooling fish, flying formations")
	var align_radius = 30.0
	var aligned = BatchOps.alignment_2d(positions, velocities, align_radius)
	print("  Radius: ", align_radius)
	print("  Aligned velocities: ", aligned)

	# -------------------------------------------------------------------------
	# FLOCK - All three combined (most efficient)
	# USE WHEN: You want complete boids behavior
	# Single pass is faster than calling three separate functions
	# -------------------------------------------------------------------------
	print("\nFLOCK - Combined separation + cohesion + alignment:")
	print("  USE CASE: Complete boids simulation in one call")
	var combined = BatchOps.flock_2d(
		positions, velocities,
		15.0,   # Separation radius
		50.0,   # Cohesion radius
		30.0,   # Alignment radius
		2.0,    # Separation strength (usually highest)
		1.0,    # Cohesion strength
		1.0     # Alignment strength
	)
	print("  Combined forces: ", combined)
	print("")


func obstacle_avoidance() -> void:
	print("--- Obstacle Avoidance ---")
	print("")
	print("Simple raycast-style avoidance for circular obstacles.")
	print("Checks if entity will collide based on current velocity.")
	print("")

	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(0, 50)
	])
	var velocities = PackedVector2Array([
		Vector2(10, 0),  # Moving right toward obstacle
		Vector2(10, 0)   # Moving right, will miss
	])

	# Circular obstacles
	var obstacle_centers = PackedVector2Array([
		Vector2(50, 0)   # Obstacle directly ahead of first entity
	])
	var obstacle_radii = PackedFloat32Array([15.0])

	print("Entity positions: ", positions)
	print("Entity velocities: ", velocities)
	print("Obstacle at: ", obstacle_centers[0], " radius: ", obstacle_radii[0])

	var lookahead = 80.0
	var strength = 3.0

	# -------------------------------------------------------------------------
	# avoid_circles - Steer around circular obstacles
	# USE WHEN: Simple obstacle avoidance
	# NOTE: For complex obstacles, use navmesh or pathfinding
	# -------------------------------------------------------------------------
	print("\navoid_circles_2d:")
	print("  USE CASE: Simple obstacle avoidance for moving entities")
	var avoidance = BatchOps.avoid_circles_2d(
		positions, velocities,
		obstacle_centers, obstacle_radii,
		lookahead, strength
	)
	print("  Lookahead: ", lookahead, ", Strength: ", strength)
	print("  Avoidance forces: ", avoidance)
	print("  NOTE: First entity gets avoidance force (headed toward obstacle)")
	print("")


func game_scenario_boids() -> void:
	print("--- Game Scenario: Boids Simulation Frame ---")
	print("")
	print("This shows a complete boids update loop.")
	print("The pattern: calculate forces -> apply -> limit -> move")
	print("")

	# Flock of 5 birds
	var positions = PackedVector2Array([
		Vector2(100, 100),
		Vector2(120, 105),
		Vector2(110, 120),
		Vector2(95, 110),
		Vector2(115, 95)
	])

	var velocities = PackedVector2Array([
		Vector2(5, 2),
		Vector2(4, 3),
		Vector2(6, 1),
		Vector2(5, 4),
		Vector2(3, 2)
	])

	var max_speed = 10.0
	var delta = 0.016  # ~60 FPS

	print("Initial state:")
	print("  Positions: ", positions)
	print("  Velocities: ", velocities)

	# STEP 1: Calculate flocking forces
	print("\n--- STEP 1: Calculate flocking forces ---")
	var flock_forces = BatchOps.flock_2d(
		positions, velocities,
		20.0,   # Separation: stay 20 units apart
		80.0,   # Cohesion: be aware of flock within 80 units
		50.0,   # Alignment: match velocity within 50 units
		2.0,    # Separation is strong (priority)
		0.8,    # Cohesion is moderate
		1.0     # Alignment is normal
	)
	print("  Flock forces: ", flock_forces)

	# STEP 2: Apply forces as acceleration
	print("\n--- STEP 2: Apply forces (as acceleration) ---")
	velocities = BatchOps.apply_accelerations_2d(velocities, flock_forces, delta)
	print("  After acceleration: ", velocities)

	# STEP 3: Limit speed
	print("\n--- STEP 3: Limit speed ---")
	velocities = BatchOps.limit_velocity_2d(velocities, max_speed)
	print("  After speed limit: ", velocities)

	# STEP 4: Update positions
	print("\n--- STEP 4: Update positions ---")
	positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
	print("  New positions: ", positions)

	# Simulate multiple frames
	print("\n--- Simulating 10 more frames... ---")
	for frame in range(10):
		flock_forces = BatchOps.flock_2d(
			positions, velocities,
			20.0, 80.0, 50.0,
			2.0, 0.8, 1.0
		)
		velocities = BatchOps.apply_accelerations_2d(velocities, flock_forces, delta)
		velocities = BatchOps.limit_velocity_2d(velocities, max_speed)
		positions = BatchOps.apply_velocities_2d(positions, velocities, delta)

	print("Final positions: ", positions)

	# Calculate flock center and spread
	var center = Vector2.ZERO
	for pos in positions:
		center += pos
	center /= positions.size()
	print("Flock center: ", center)

	print("\n  BOIDS LOOP SUMMARY:")
	print("  1. flock_2d() -> Get combined steering forces")
	print("  2. apply_accelerations_2d() -> Update velocities")
	print("  3. limit_velocity_2d() -> Enforce speed limit")
	print("  4. apply_velocities_2d() -> Update positions")
	print("")
	print("  TUNING TIPS:")
	print("  - Separation strength highest -> prevents overlap")
	print("  - Smaller separation radius -> tighter formations")
	print("  - Larger cohesion radius -> bigger flocks")
	print("  - Alignment creates smooth, coordinated movement")
	print("")

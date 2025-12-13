extends SceneTree
## =============================================================================
## MathOps Example - Batch vector and math operations
## =============================================================================
##
## WHEN TO USE:
## ------------
## MathOps provides SIMD-optimized batch operations on vectors.
## Use it whenever you're doing the same math operation on many vectors.
##
## PERFECT FOR:
## - Batch movement calculations (direction, distance, lerp)
## - Physics simulations (normalize, scale, clamp)
## - Rendering prep (transforms, projections)
## - AI calculations (angles, dot products for vision cones)
## - Any per-entity vector math in a hot loop
##
## KEY PATTERNS:
## - All operations work on arrays, not individual vectors
## - Most have 2D and 3D versions (suffix _2d or _3d)
## - Results are new arrays (original unchanged)
##
## PERFORMANCE:
## - 10-30x faster than GDScript loops
## - Especially beneficial for 100+ entities
## - SIMD optimization where available
##
## STATIC METHODS: Call on class directly
##   MathOps.normalize_batch_2d(vectors)
## NOT: var ops = MathOps.new()  # Wrong!
##
## Run with: godot --headless -s examples/example_math_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("MathOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	normalization()
	dot_and_cross()
	lengths_and_distances()
	interpolation()
	vector_arithmetic()
	directions_and_angles()
	projections_and_reflections()
	transform_operations()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func normalization() -> void:
	print("--- Normalization ---")
	print("")
	print("Normalizing vectors is one of the most common operations.")
	print("MathOps handles zero vectors safely (returns zero, not NaN).")
	print("")

	var vectors = PackedVector2Array([
		Vector2(3, 4),    # Length 5
		Vector2(0, 10),   # Length 10
		Vector2(0, 0),    # Zero vector (safe!)
		Vector2(1, 1)     # Length sqrt(2)
	])

	# -------------------------------------------------------------------------
	# normalize_batch - Make all vectors unit length
	# USE WHEN: You need direction without magnitude
	# EXAMPLES: Movement direction, facing direction, force direction
	# -------------------------------------------------------------------------
	print("normalize_batch_2d:")
	print("  USE CASE: Get direction vectors for movement/aiming")
	print("  Original: ", vectors)
	var normalized = MathOps.normalize_batch_2d(vectors)
	print("  Normalized: ", normalized)
	print("  NOTE: Zero vectors stay zero (safe handling)")

	# Verify lengths
	var lengths = MathOps.length_batch_2d(normalized)
	print("  Lengths after normalize: ", lengths)
	print("")


func dot_and_cross() -> void:
	print("--- Dot and Cross Products ---")
	print("")
	print("Dot product: How aligned are two vectors? (-1 to 1)")
	print("Cross product (2D): Which side is B relative to A?")
	print("")

	var a = PackedVector2Array([Vector2(1, 0), Vector2(1, 0), Vector2(1, 1)])
	var b = PackedVector2Array([Vector2(1, 0), Vector2(0, 1), Vector2(1, 1)])

	print("Vectors a: ", a)
	print("Vectors b: ", b)

	# -------------------------------------------------------------------------
	# dot_batch - Dot product of paired vectors
	# USE WHEN: Checking alignment, vision cones, angle comparison
	# RESULT: 1 = same direction, 0 = perpendicular, -1 = opposite
	# -------------------------------------------------------------------------
	print("\ndot_batch_2d:")
	print("  USE CASE: Vision cones, checking if target is ahead")
	var dots = MathOps.dot_batch_2d(a, b)
	print("  Dot products: ", dots)
	print("  Interpretation:")
	print("    (1,0)·(1,0) = 1.0 (parallel, same direction)")
	print("    (1,0)·(0,1) = 0.0 (perpendicular)")
	print("    (1,1)·(1,1) = 2.0 (same, but not normalized)")

	# -------------------------------------------------------------------------
	# cross_batch (2D) - "Cross product" gives Z component
	# USE WHEN: Determining winding order, which side something is on
	# RESULT: Positive = B is counter-clockwise from A
	# -------------------------------------------------------------------------
	print("\ncross_batch_2d:")
	print("  USE CASE: Winding order, left/right detection")
	var crosses = MathOps.cross_batch_2d(a, b)
	print("  Cross products: ", crosses)
	print("  Positive = B is counter-clockwise from A")
	print("")


func lengths_and_distances() -> void:
	print("--- Lengths and Distances ---")
	print("")
	print("Length/distance calculations are expensive (sqrt).")
	print("Use squared versions when possible (comparing distances).")
	print("")

	var vectors = PackedVector2Array([
		Vector2(3, 4), Vector2(5, 12), Vector2(0, 0)
	])

	print("Vectors: ", vectors)

	# -------------------------------------------------------------------------
	# length_batch - Length of each vector
	# USE WHEN: You need actual distances/speeds
	# -------------------------------------------------------------------------
	print("\nlength_batch_2d:")
	print("  USE CASE: Calculate actual speed, distance values")
	print("  Lengths: ", MathOps.length_batch_2d(vectors))

	# -------------------------------------------------------------------------
	# length_squared_batch - Length² (no sqrt - faster!)
	# USE WHEN: Comparing distances (which is closer?)
	# TIP: dist² < threshold² works the same as dist < threshold
	# -------------------------------------------------------------------------
	print("\nlength_squared_batch_2d:")
	print("  USE CASE: Distance comparisons (avoid sqrt)")
	print("  Lengths²: ", MathOps.length_squared_batch_2d(vectors))
	print("  TIP: For comparisons, compare squared values!")

	# Pairwise distances
	var positions_a = PackedVector2Array([Vector2(0, 0), Vector2(10, 0)])
	var positions_b = PackedVector2Array([Vector2(3, 4), Vector2(10, 10)])

	print("\nPairwise distances:")
	print("  Positions A: ", positions_a)
	print("  Positions B: ", positions_b)
	print("  distance_batch_2d: ", MathOps.distance_batch_2d(positions_a, positions_b))
	print("  distance_squared_batch_2d: ", MathOps.distance_squared_batch_2d(positions_a, positions_b))

	# -------------------------------------------------------------------------
	# distance_matrix - All pairwise distances (expensive!)
	# USE WHEN: You need distance between every pair
	# RESULT: Flat array [a0→b0, a0→b1, a1→b0, a1→b1, ...]
	# -------------------------------------------------------------------------
	print("\ndistance_matrix_2d (all pairs):")
	print("  USE CASE: Clustering, all-pairs collision, assignment")
	var matrix = MathOps.distance_matrix_2d(positions_a, positions_b)
	print("  A[0]→B[0]: ", matrix[0], ", A[0]→B[1]: ", matrix[1])
	print("  A[1]→B[0]: ", matrix[2], ", A[1]→B[1]: ", matrix[3])
	print("")


func interpolation() -> void:
	print("--- Interpolation ---")
	print("")
	print("Lerp (linear interpolation) blends between two values.")
	print("t=0 gives start, t=1 gives end, t=0.5 gives midpoint.")
	print("")

	var starts = PackedVector2Array([Vector2(0, 0), Vector2(100, 100)])
	var ends = PackedVector2Array([Vector2(100, 0), Vector2(200, 200)])

	print("Starts: ", starts)
	print("Ends: ", ends)

	# -------------------------------------------------------------------------
	# lerp_batch - Same t for all vectors
	# USE WHEN: Uniform animation progress, global blend factor
	# -------------------------------------------------------------------------
	print("\nlerp_batch_2d (uniform t=0.5):")
	print("  USE CASE: All entities at same animation frame")
	var mid = MathOps.lerp_batch_2d(starts, ends, 0.5)
	print("  Midpoints: ", mid)

	# -------------------------------------------------------------------------
	# lerp_batch_weights - Different t per vector
	# USE WHEN: Per-entity animation progress, staggered transitions
	# -------------------------------------------------------------------------
	print("\nlerp_batch_2d_weights (per-entity t):")
	print("  USE CASE: Each entity at different animation progress")
	var t_values = PackedFloat32Array([0.25, 0.75])
	var weighted = MathOps.lerp_batch_2d_weights(starts, ends, t_values)
	print("  t values: ", t_values)
	print("  Results: ", weighted)
	print("")


func vector_arithmetic() -> void:
	print("--- Vector Arithmetic ---")
	print("")
	print("Basic math operations on vector arrays.")
	print("Faster than looping in GDScript.")
	print("")

	var a = PackedVector2Array([Vector2(10, 20), Vector2(30, 40)])
	var b = PackedVector2Array([Vector2(5, 5), Vector2(10, 10)])

	print("a: ", a)
	print("b: ", b)

	# -------------------------------------------------------------------------
	# add/sub batch - Element-wise addition/subtraction
	# USE WHEN: Combining forces, calculating offsets
	# -------------------------------------------------------------------------
	print("\nadd_batch_2d: ", MathOps.add_batch_2d(a, b))
	print("sub_batch_2d: ", MathOps.sub_batch_2d(a, b))

	# -------------------------------------------------------------------------
	# scale_batch - Multiply all vectors by scalar
	# USE WHEN: Applying speed, force multiplier, delta time
	# -------------------------------------------------------------------------
	print("\nscale_batch_2d (×2): ", MathOps.scale_batch_2d(a, 2.0))

	# -------------------------------------------------------------------------
	# scale_batch_weights - Different scalar per vector
	# USE WHEN: Per-entity speed, individual force magnitudes
	# -------------------------------------------------------------------------
	print("\nscale_batch_2d_weights:")
	print("  USE CASE: Each entity has different speed")
	var scales = PackedFloat32Array([0.5, 2.0])
	print("  Scales: ", scales)
	print("  Result: ", MathOps.scale_batch_2d_weights(a, scales))

	# -------------------------------------------------------------------------
	# clamp_length_batch - Limit vector magnitude
	# USE WHEN: Speed limits, force caps
	# -------------------------------------------------------------------------
	print("\nclamp_length_batch_2d:")
	print("  USE CASE: Enforce maximum speed")
	var velocities = PackedVector2Array([Vector2(100, 0), Vector2(5, 0), Vector2(50, 50)])
	print("  Input: ", velocities)
	print("  Clamped to max 30: ", MathOps.clamp_length_batch_2d(velocities, 30.0))

	# -------------------------------------------------------------------------
	# clamp_length_range_batch - Min AND max length
	# USE WHEN: Entities must keep moving, prevent both stop and overspeed
	# -------------------------------------------------------------------------
	print("\nclamp_length_range_batch_2d:")
	print("  USE CASE: Minimum speed + maximum speed")
	print("  Clamped to [10, 30]: ", MathOps.clamp_length_range_batch_2d(velocities, 10.0, 30.0))
	print("")


func directions_and_angles() -> void:
	print("--- Directions and Angles ---")
	print("")
	print("Convert between positions, directions, and angles.")
	print("")

	# -------------------------------------------------------------------------
	# direction_batch - Normalized direction from A to B
	# USE WHEN: Which way to move/aim/look
	# -------------------------------------------------------------------------
	print("direction_batch_2d:")
	print("  USE CASE: Calculate direction from entity to target")
	var froms = PackedVector2Array([Vector2(0, 0), Vector2(10, 10)])
	var tos = PackedVector2Array([Vector2(10, 0), Vector2(10, 20)])
	print("  From: ", froms)
	print("  To: ", tos)
	print("  Directions: ", MathOps.direction_batch_2d(froms, tos))

	# -------------------------------------------------------------------------
	# angle_batch - Angle of each vector (radians)
	# USE WHEN: Converting velocity to rotation, direction to angle
	# RESULT: Radians, 0 = right, PI/2 = up
	# -------------------------------------------------------------------------
	print("\nangle_batch_2d:")
	print("  USE CASE: Set sprite rotation from velocity")
	var vectors = PackedVector2Array([
		Vector2(1, 0),   # 0 radians (right)
		Vector2(0, 1),   # PI/2 radians (up)
		Vector2(-1, 0),  # PI radians (left)
		Vector2(1, 1)    # PI/4 radians (diagonal)
	])
	print("  Vectors: ", vectors)
	var angles = MathOps.angle_batch_2d(vectors)
	print("  Angles (radians): ", angles)
	print("  Angles (degrees): ", angles[0] * 180/PI, ", ", angles[1] * 180/PI, ", ",
		  angles[2] * 180/PI, ", ", "%.1f" % (angles[3] * 180/PI))

	# -------------------------------------------------------------------------
	# from_angle_batch - Create vectors from angles
	# USE WHEN: Creating directions from rotations
	# -------------------------------------------------------------------------
	print("\nfrom_angle_batch:")
	print("  USE CASE: Spread shot, radial patterns")
	var input_angles = PackedFloat32Array([0, PI/2, PI])
	print("  Input angles: 0, PI/2, PI")
	print("  Vectors: ", MathOps.from_angle_batch(input_angles))

	# -------------------------------------------------------------------------
	# angle_between_batch - Angle between pairs of vectors
	# USE WHEN: Vision cones, checking alignment
	# RESULT: Always positive (unsigned angle)
	# -------------------------------------------------------------------------
	print("\nangle_between_batch_2d:")
	print("  USE CASE: Is target within vision cone?")
	var v1 = PackedVector2Array([Vector2(1, 0), Vector2(1, 0)])
	var v2 = PackedVector2Array([Vector2(0, 1), Vector2(1, 1)])
	print("  Pairs: (1,0)↔(0,1), (1,0)↔(1,1)")
	var between = MathOps.angle_between_batch_2d(v1, v2)
	print("  Angles: ", between, " radians")
	print("  In degrees: ", between[0] * 180/PI, ", ", "%.1f" % (between[1] * 180/PI))
	print("")


func projections_and_reflections() -> void:
	print("--- Projections and Reflections ---")
	print("")
	print("Useful for physics, shadows, and bouncing.")
	print("")

	# -------------------------------------------------------------------------
	# project_batch - Project vectors onto other vectors
	# USE WHEN: Shadow direction, component extraction
	# -------------------------------------------------------------------------
	print("project_batch_2d:")
	print("  USE CASE: Get velocity component along slope")
	var vectors = PackedVector2Array([Vector2(3, 4), Vector2(5, 5)])
	var onto = PackedVector2Array([Vector2(1, 0), Vector2(0, 1)])
	print("  Vectors: ", vectors)
	print("  Project onto: ", onto)
	print("  Results: ", MathOps.project_batch_2d(vectors, onto))
	print("  (3,4) onto (1,0) = (3,0) - x component only")

	# -------------------------------------------------------------------------
	# reflect_batch - Reflect vectors off surfaces
	# USE WHEN: Bouncing, mirrors, ricochets
	# INPUT: Incoming velocity, surface normal
	# -------------------------------------------------------------------------
	print("\nreflect_batch_2d:")
	print("  USE CASE: Ball bouncing off walls")
	var velocities = PackedVector2Array([Vector2(1, -1), Vector2(1, 0)])
	var normals = PackedVector2Array([Vector2(0, 1), Vector2(-1, 0)])  # Floor, right wall
	print("  Incoming: ", velocities)
	print("  Surface normals: ", normals)
	print("  Reflected: ", MathOps.reflect_batch_2d(velocities, normals))
	print("")


func transform_operations() -> void:
	print("--- Transform Operations ---")
	print("")
	print("Apply Transform2D to many points at once.")
	print("Useful for batch sprite positioning, collision shapes.")
	print("")

	var points = PackedVector2Array([Vector2(0, 0), Vector2(10, 10), Vector2(5, 0)])

	# Create a transform: rotate 90 degrees and translate
	var xform = Transform2D(PI/2, Vector2(100, 50))

	print("Points: ", points)
	print("Transform: 90° rotation + translate(100, 50)")

	# -------------------------------------------------------------------------
	# transform_2d_batch - Apply transform to all points
	# USE WHEN: Moving collision shapes, sprite offsets
	# -------------------------------------------------------------------------
	print("\ntransform_2d_batch:")
	print("  USE CASE: Transform all hitbox vertices")
	var transformed = MathOps.transform_2d_batch(xform, points)
	print("  Transformed: ", transformed)

	# -------------------------------------------------------------------------
	# transform_2d_batch_inverse - Apply inverse transform
	# USE WHEN: Converting world coords to local coords
	# -------------------------------------------------------------------------
	print("\ntransform_2d_batch_inverse:")
	print("  USE CASE: Convert world positions to local space")
	var inverse = MathOps.transform_2d_batch_inverse(xform, transformed)
	print("  Back to original: ", inverse)
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Steering Forces ---")
	print("")
	print("This shows batch vector math for steering AI.")
	print("Multiple units seeking targets with steering forces.")
	print("")

	# Multiple units seeking targets
	var positions = PackedVector2Array([
		Vector2(0, 0),
		Vector2(100, 100),
		Vector2(200, 50)
	])

	var targets = PackedVector2Array([
		Vector2(50, 50),
		Vector2(150, 80),
		Vector2(180, 100)
	])

	var velocities = PackedVector2Array([
		Vector2(10, 0),
		Vector2(-5, 5),
		Vector2(0, -10)
	])

	var max_speed = 20.0
	var max_force = 5.0
	var delta = 0.016  # ~60 FPS

	print("Positions: ", positions)
	print("Targets: ", targets)
	print("Current velocities: ", velocities)

	# STEP 1: Direction to targets
	print("\n--- STEP 1: Calculate direction to targets ---")
	var directions = MathOps.direction_batch_2d(positions, targets)
	print("  Directions: ", directions)

	# STEP 2: Desired velocity (direction × max_speed)
	print("\n--- STEP 2: Desired velocity (direction × speed) ---")
	var desired = MathOps.scale_batch_2d(directions, max_speed)
	print("  Desired: ", desired)

	# STEP 3: Steering force (desired - current)
	print("\n--- STEP 3: Steering force (desired - current) ---")
	var steering = MathOps.sub_batch_2d(desired, velocities)
	print("  Raw steering: ", steering)

	# STEP 4: Limit steering force
	print("\n--- STEP 4: Limit steering force ---")
	steering = MathOps.clamp_length_batch_2d(steering, max_force)
	print("  Clamped: ", steering)

	# STEP 5: Apply steering (velocity += steering × delta)
	print("\n--- STEP 5: Apply steering to velocity ---")
	var accel = MathOps.scale_batch_2d(steering, delta)
	var new_velocities = MathOps.add_batch_2d(velocities, accel)
	new_velocities = MathOps.clamp_length_batch_2d(new_velocities, max_speed)
	print("  New velocities: ", new_velocities)

	# STEP 6: Apply velocities (position += velocity × delta)
	print("\n--- STEP 6: Update positions ---")
	var movement = MathOps.scale_batch_2d(new_velocities, delta)
	var new_positions = MathOps.add_batch_2d(positions, movement)
	print("  New positions: ", new_positions)

	print("\n  STEERING PATTERN:")
	print("  1. direction_batch_2d -> Where to go")
	print("  2. scale_batch_2d -> Desired velocity")
	print("  3. sub_batch_2d -> Steering = desired - current")
	print("  4. clamp_length_batch_2d -> Limit force")
	print("  5. scale + add -> Apply force")
	print("  6. scale + add -> Move")
	print("")

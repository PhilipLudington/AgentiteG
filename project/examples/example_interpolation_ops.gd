extends SceneTree
## =============================================================================
## InterpolationOps Example - Easing, bezier, and spline functions
## =============================================================================
##
## WHEN TO USE:
## ------------
## InterpolationOps provides smooth transitions, curves, and animation helpers.
## Essential for polished game feel and procedural animation.
##
## PERFECT FOR:
## - UI animations (health bars, menus, transitions)
## - Camera movements (smooth follow, dolly shots)
## - Projectile arcs (thrown objects, ballistic paths)
## - Character animation (procedural movement)
## - Difficulty curves and progression systems
## - Patrol paths and AI movement
##
## KEY CONCEPTS:
## - EASING: Smooth start/end of animations (not linear)
## - BEZIER: Curved paths with control points
## - CATMULL-ROM: Smooth curve through waypoints
##
## EASING TYPES:
## - ease_in_*: Slow start, fast end (accelerating)
## - ease_out_*: Fast start, slow end (decelerating)
## - ease_in_out_*: Slow start and end (smooth both ways)
## - Types: quad, cubic, quart, quint, sine, expo, circ, back, elastic, bounce
##
## ALL METHODS ARE STATIC - call directly on InterpolationOps class.
##
## Run with: godot --headless -s examples/example_interpolation_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("InterpolationOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	utility_functions()
	easing_functions()
	bezier_curves()
	catmull_rom_splines()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func utility_functions() -> void:
	print("--- Utility Functions ---")
	print("")
	print("Helper functions for generating and transforming values.")
	print("")

	# -------------------------------------------------------------------------
	# linspace - Generate evenly spaced values
	# USE WHEN: Animation keyframes, sampling curves, progress tracking
	# -------------------------------------------------------------------------
	print("linspace - generate evenly spaced values:")
	print("  USE FOR: Animation frames, curve sampling, progress steps")
	print("")
	print("linspace(0, 1, 5):")
	var t = InterpolationOps.linspace(0.0, 1.0, 5)
	print("  ", t)

	print("\nlinspace(0, 100, 6):")
	var range100 = InterpolationOps.linspace(0.0, 100.0, 6)
	print("  ", range100)

	# Remap values
	print("\nremap - change value range:")
	var values = PackedFloat32Array([0, 25, 50, 75, 100])
	var remapped = InterpolationOps.remap(values, 0.0, 100.0, -1.0, 1.0)
	print("  [0-100] -> [-1, 1]: ", remapped)

	# Clamp array
	print("\nclamp_array:")
	var unclamped = PackedFloat32Array([-0.5, 0.3, 0.7, 1.5])
	var clamped = InterpolationOps.clamp_array(unclamped, 0.0, 1.0)
	print("  Input: ", unclamped)
	print("  Clamped [0,1]: ", clamped)
	print("")


func easing_functions() -> void:
	print("--- Easing Functions ---")
	print("")
	print("Easing makes animations feel natural instead of robotic.")
	print("Linear motion feels wrong - real objects accelerate and decelerate.")
	print("")

	# -------------------------------------------------------------------------
	# EASING GUIDE:
	# - ease_in_*: Slow start (use when something is getting going)
	# - ease_out_*: Slow end (use when something is stopping)
	# - ease_in_out_*: Slow both ends (smooth transitions)
	#
	# Common choices:
	# - UI animations: ease_out_quad or ease_out_cubic
	# - Bounce effects: ease_out_bounce or ease_out_elastic
	# - Smooth transitions: ease_in_out_sine or smoothstep
	# -------------------------------------------------------------------------
	var t = InterpolationOps.linspace(0.0, 1.0, 5)
	print("t values (input): ", t)

	# Smoothstep
	print("\nsmoothstep (general-purpose smooth):")
	print("  USE FOR: Generic smooth transitions")
	var smooth = InterpolationOps.smoothstep(t)
	print("  ", smooth)

	# Quad easing
	print("\nQuadratic easing:")
	print("  ease_in_quad:  ", InterpolationOps.ease_in_quad(t))
	print("  ease_out_quad: ", InterpolationOps.ease_out_quad(t))
	print("  ease_in_out_quad: ", InterpolationOps.ease_in_out_quad(t))

	# Cubic easing
	print("\nCubic easing:")
	print("  ease_in_cubic:  ", InterpolationOps.ease_in_cubic(t))
	print("  ease_out_cubic: ", InterpolationOps.ease_out_cubic(t))
	print("  ease_in_out_cubic: ", InterpolationOps.ease_in_out_cubic(t))

	# Elastic easing (bouncy)
	print("\nElastic easing (bouncy):")
	print("  ease_out_elastic: ", InterpolationOps.ease_out_elastic(t))

	# Bounce easing
	print("\nBounce easing:")
	print("  ease_out_bounce: ", InterpolationOps.ease_out_bounce(t))

	# Back easing (overshoots)
	print("\nBack easing (overshoots):")
	print("  ease_out_back: ", InterpolationOps.ease_out_back(t))

	# Sine easing (smooth)
	print("\nSine easing:")
	print("  ease_in_out_sine: ", InterpolationOps.ease_in_out_sine(t))

	# Available easing functions:
	print("\nAll available easing types:")
	print("  ease_in/out/in_out_quad, cubic, quart, quint")
	print("  ease_in/out/in_out_sine, expo, circ, back, elastic, bounce")
	print("")


func bezier_curves() -> void:
	print("--- Bezier Curves ---")
	print("")
	print("Bezier curves create smooth paths controlled by anchor points.")
	print("The curve is 'pulled' toward control points but doesn't pass through them.")
	print("")

	# -------------------------------------------------------------------------
	# bezier_cubic_2d - Cubic bezier curve (4 points)
	# USE WHEN: Camera paths, projectile arcs, UI transitions
	# POINTS: Start, Control1, Control2, End
	# The curve passes through Start and End, is pulled by Control1/2
	# -------------------------------------------------------------------------
	var p0 = Vector2(0, 0)      # Start
	var p1 = Vector2(30, 100)   # Control 1 (pulls curve)
	var p2 = Vector2(70, 100)   # Control 2 (pulls curve)
	var p3 = Vector2(100, 0)    # End

	print("Bezier USE CASES:")
	print("  - Camera dolly paths")
	print("  - Projectile arcs (grenade throws)")
	print("  - UI element transitions")
	print("  - Smooth connector lines")
	print("")

	print("Cubic Bezier control points:")
	print("  P0 (start): ", p0)
	print("  P1 (control): ", p1)
	print("  P2 (control): ", p2)
	print("  P3 (end): ", p3)

	# Sample the curve
	var t = InterpolationOps.linspace(0.0, 1.0, 11)
	var curve = InterpolationOps.bezier_cubic_2d(p0, p1, p2, p3, t)

	print("\nbezier_cubic_2d (11 samples):")
	for i in range(curve.size()):
		print("  t=%.1f: " % t[i], curve[i])

	# 3D bezier
	print("\nbezier_cubic_3d:")
	var p0_3d = Vector3(0, 0, 0)
	var p1_3d = Vector3(30, 50, 20)
	var p2_3d = Vector3(70, 50, 20)
	var p3_3d = Vector3(100, 0, 0)

	var t_3d = PackedFloat32Array([0.0, 0.5, 1.0])
	var curve_3d = InterpolationOps.bezier_cubic_3d(p0_3d, p1_3d, p2_3d, p3_3d, t_3d)
	print("  Samples: ", curve_3d)
	print("")


func catmull_rom_splines() -> void:
	print("--- Catmull-Rom Splines ---")
	print("")
	print("Catmull-Rom splines pass through ALL control points smoothly.")
	print("Unlike Bezier, every point is on the curve - perfect for waypoints.")
	print("")

	# -------------------------------------------------------------------------
	# catmull_rom_2d - Smooth curve through waypoints
	# USE WHEN: Patrol paths, roller coasters, smooth paths through nodes
	# ADVANTAGE: Curve passes through every control point
	# Bezier pulls toward points; Catmull-Rom passes through them
	# -------------------------------------------------------------------------
	var control_points = PackedVector2Array([
		Vector2(0, 50),
		Vector2(25, 0),
		Vector2(50, 100),
		Vector2(75, 50),
		Vector2(100, 75)
	])

	print("Catmull-Rom USE CASES:")
	print("  - Enemy patrol paths (waypoints)")
	print("  - Roller coaster/rail tracks")
	print("  - Camera follow paths")
	print("  - Drawing smooth lines through user clicks")
	print("")

	print("Control points (curve passes through ALL of them):")
	for i in range(control_points.size()):
		print("  P", i, ": ", control_points[i])

	# Generate spline with 5 samples per segment
	var samples_per_segment = 5
	var spline = InterpolationOps.catmull_rom_2d(control_points, samples_per_segment)

	print("\ncatmull_rom_2d (", samples_per_segment, " samples/segment):")
	print("  Total points: ", spline.size())
	print("  First few: ", spline.slice(0, 6))

	# 3D spline
	print("\ncatmull_rom_3d:")
	var control_3d = PackedVector3Array([
		Vector3(0, 0, 0),
		Vector3(25, 50, 10),
		Vector3(50, 0, 20),
		Vector3(75, 50, 30),
		Vector3(100, 0, 0)
	])
	var spline_3d = InterpolationOps.catmull_rom_3d(control_3d, 3)
	print("  Total points: ", spline_3d.size())
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Animation and Movement ---")

	# 1. UI Animation with easing
	print("1. Health bar animation (depleting 100 -> 30):")
	var frames = 10
	var t = InterpolationOps.linspace(0.0, 1.0, frames)

	# Different feel with different easing
	var linear = t
	var eased = InterpolationOps.ease_out_quad(t)

	print("   Linear interpolation:")
	var health_linear = InterpolationOps.remap(linear, 0.0, 1.0, 100.0, 30.0)
	print("   ", health_linear)

	print("   Ease-out (smooth deceleration):")
	var health_eased = InterpolationOps.remap(eased, 0.0, 1.0, 100.0, 30.0)
	print("   ", health_eased)

	# 2. Camera shake with elastic
	print("\n2. Camera shake recovery:")
	var shake_t = InterpolationOps.linspace(0.0, 1.0, 8)
	var shake_recovery = InterpolationOps.ease_out_elastic(shake_t)
	print("   Elastic recovery: ", shake_recovery)
	print("   (Oscillates before settling)")

	# 3. Bounce effect
	print("\n3. Object drop with bounce:")
	var drop_t = InterpolationOps.linspace(0.0, 1.0, 10)
	var bounce = InterpolationOps.ease_out_bounce(drop_t)
	var y_positions = InterpolationOps.remap(bounce, 0.0, 1.0, 0.0, 100.0)
	print("   Y positions (0=top, 100=ground): ", y_positions)

	# 4. Smooth camera path (Bezier)
	print("\n4. Camera dolly path (Bezier):")
	var cam_start = Vector2(0, 100)
	var cam_ctrl1 = Vector2(200, 150)
	var cam_ctrl2 = Vector2(300, 50)
	var cam_end = Vector2(500, 100)

	var path_t = InterpolationOps.linspace(0.0, 1.0, 5)
	var cam_path = InterpolationOps.bezier_cubic_2d(cam_start, cam_ctrl1, cam_ctrl2, cam_end, path_t)
	print("   Camera positions: ", cam_path)

	# 5. Enemy patrol path (Catmull-Rom)
	print("\n5. Enemy patrol path (Catmull-Rom spline):")
	var waypoints = PackedVector2Array([
		Vector2(100, 100),
		Vector2(200, 80),
		Vector2(300, 120),
		Vector2(200, 150)
	])
	print("   Waypoints: ", waypoints)

	var patrol_path = InterpolationOps.catmull_rom_2d(waypoints, 8)
	print("   Smooth path points: ", patrol_path.size())
	print("   First segment: ", patrol_path.slice(0, 5))

	# 6. Difficulty curve
	print("\n6. Difficulty curve (level 1-10):")
	var levels = InterpolationOps.linspace(0.0, 1.0, 10)
	var difficulty_linear = InterpolationOps.remap(levels, 0.0, 1.0, 1.0, 100.0)
	var difficulty_expo = InterpolationOps.ease_in_expo(levels)
	var difficulty_curved = InterpolationOps.remap(difficulty_expo, 0.0, 1.0, 1.0, 100.0)

	print("   Linear difficulty: ", difficulty_linear)
	print("   Exponential (slow start): ", difficulty_curved)

	# 7. Projectile arc
	print("\n7. Projectile arc (parabola via Bezier):")
	var throw_start = Vector2(0, 100)
	var throw_apex = Vector2(50, 30)  # Control points for arc
	var throw_end = Vector2(100, 100)

	# Use control points to create arc
	var arc_t = InterpolationOps.linspace(0.0, 1.0, 6)
	var projectile_path = InterpolationOps.bezier_cubic_2d(
		throw_start,
		Vector2(25, 20),   # Pull up early
		Vector2(75, 20),   # Pull up late
		throw_end,
		arc_t
	)
	print("   Arc positions (Y inverted): ", projectile_path)

	print("")

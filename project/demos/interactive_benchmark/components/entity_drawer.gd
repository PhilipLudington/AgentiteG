class_name EntityDrawer
extends Node2D
## Draws entities and debug visualizations for benchmark scenarios.

const DEFAULT_RADIUS := 3.0
const DEFAULT_COLOR := Color.WHITE

var scenario: BenchmarkScenario


func _draw() -> void:
	if scenario == null:
		return

	var data := scenario.get_draw_data()

	# Draw debug rects first (obstacles, spatial grid cells, etc.)
	var rects: Array = data.get("rects", [])
	for rect in rects:
		if rect is Rect2:
			# Filled dark rectangle for obstacles
			draw_rect(rect, Color(0.15, 0.15, 0.2, 0.9), true)
			# Lighter outline
			draw_rect(rect, Color(0.3, 0.3, 0.4, 0.8), false, 1.0)

	# Draw debug lines
	var lines: PackedVector2Array = data.get("lines", PackedVector2Array())
	if lines.size() >= 2:
		draw_multiline(lines, Color(0.5, 0.5, 0.5, 0.5), 1.0)

	# Draw debug circles (e.g., query radius)
	var circles: Array = data.get("circles", [])
	for circle_data in circles:
		if circle_data is Dictionary:
			var center: Vector2 = circle_data.get("center", Vector2.ZERO)
			var radius: float = circle_data.get("radius", 10.0)
			var color: Color = circle_data.get("color", Color.WHITE)
			var filled: bool = circle_data.get("filled", false)
			if filled:
				draw_circle(center, radius, color)
			else:
				_draw_circle_outline(center, radius, color, 2.0)

	# Draw triangles (for boids/flocking)
	var triangles: Array = data.get("triangles", [])
	for tri in triangles:
		if tri is Dictionary:
			var points: PackedVector2Array = tri.get("points", PackedVector2Array())
			var color: Color = tri.get("color", Color.WHITE)
			if points.size() >= 3:
				draw_colored_polygon(points, color)

	# Draw entities as circles
	var positions: PackedVector2Array = data.get("positions", PackedVector2Array())
	var colors_arr: PackedColorArray = data.get("colors", PackedColorArray())
	var radii: PackedFloat32Array = data.get("radii", PackedFloat32Array())

	for i in positions.size():
		var pos: Vector2 = positions[i]
		var color: Color = colors_arr[i] if i < colors_arr.size() else DEFAULT_COLOR
		var radius: float = radii[i] if i < radii.size() else DEFAULT_RADIUS
		draw_circle(pos, radius, color)


func _draw_circle_outline(center: Vector2, radius: float, color: Color, width: float = 1.0) -> void:
	var segments := 32
	var points := PackedVector2Array()
	points.resize(segments + 1)

	for i in segments + 1:
		var angle := TAU * i / segments
		points[i] = center + Vector2(cos(angle), sin(angle)) * radius

	for i in segments:
		draw_line(points[i], points[i + 1], color, width)

class_name FrameGraph
extends Control
## Displays a real-time frame time graph showing performance history.

const HISTORY_LENGTH := 120  # 2 seconds at 60fps
const TARGET_FRAME_TIME := 16.67  # 60fps target in ms
const MAX_FRAME_TIME := 50.0  # Scale to show up to ~20fps

var frame_times := PackedFloat32Array()
var current_index := 0

# Colors
var bg_color := Color(0.1, 0.1, 0.15)
var grid_color := Color(0.25, 0.25, 0.3)
var target_color := Color(0.6, 0.6, 0.2, 0.8)
var good_color := Color(0.2, 0.8, 0.3)
var bad_color := Color(0.9, 0.3, 0.2)


func _ready() -> void:
	frame_times.resize(HISTORY_LENGTH)
	for i in HISTORY_LENGTH:
		frame_times[i] = TARGET_FRAME_TIME
	custom_minimum_size = Vector2(280, 80)


func add_frame_time(delta_ms: float) -> void:
	# Shift history left and add new value at the end
	for i in range(HISTORY_LENGTH - 1):
		frame_times[i] = frame_times[i + 1]
	frame_times[HISTORY_LENGTH - 1] = delta_ms
	queue_redraw()


func _draw() -> void:
	var rect := Rect2(Vector2.ZERO, size)

	# Background
	draw_rect(rect, bg_color, true)

	# Grid lines at 16.67ms (60fps) and 33.33ms (30fps)
	var target_y := size.y * (1.0 - TARGET_FRAME_TIME / MAX_FRAME_TIME)
	var half_y := size.y * (1.0 - 33.33 / MAX_FRAME_TIME)

	draw_line(Vector2(0, target_y), Vector2(size.x, target_y), target_color, 1.0)
	draw_line(Vector2(0, half_y), Vector2(size.x, half_y), grid_color, 1.0)

	# Frame time graph
	var x_step := size.x / float(HISTORY_LENGTH - 1)

	# Build points array
	var points := PackedVector2Array()
	points.resize(HISTORY_LENGTH)

	for i in HISTORY_LENGTH:
		var x := i * x_step
		var normalized := clampf(frame_times[i] / MAX_FRAME_TIME, 0.0, 1.0)
		var y := size.y * (1.0 - normalized)
		points[i] = Vector2(x, y)

	# Draw line segments with color based on performance
	for i in range(1, points.size()):
		var color := good_color if frame_times[i] <= TARGET_FRAME_TIME else bad_color
		draw_line(points[i - 1], points[i], color, 2.0)

	# Draw border
	draw_rect(rect, grid_color, false, 1.0)

	# Labels
	var font := ThemeDB.fallback_font
	var font_size := 10
	draw_string(font, Vector2(4, target_y - 2), "60fps", HORIZONTAL_ALIGNMENT_LEFT, -1, font_size, target_color)
	draw_string(font, Vector2(4, half_y - 2), "30fps", HORIZONTAL_ALIGNMENT_LEFT, -1, font_size, grid_color)

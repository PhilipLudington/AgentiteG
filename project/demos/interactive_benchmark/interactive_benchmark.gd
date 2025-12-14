extends Node2D
## Interactive Benchmark Demo for AgentiteG
##
## Showcases AgentiteG's performance advantages through real-time,
## visual comparisons against GDScript implementations.
##
## Run with: godot --path project demos/interactive_benchmark/interactive_benchmark.tscn

# UI references
@onready var scenario_dropdown: OptionButton = $CanvasLayer/UIPanel/MarginContainer/VBox/ScenarioDropdown
@onready var entity_slider: HSlider = $CanvasLayer/UIPanel/MarginContainer/VBox/EntitySlider
@onready var entity_count_label: Label = $CanvasLayer/UIPanel/MarginContainer/VBox/EntityCountLabel
@onready var continuous_toggle: CheckButton = $CanvasLayer/UIPanel/MarginContainer/VBox/ContinuousToggle
@onready var gd_time_label: Label = $CanvasLayer/UIPanel/MarginContainer/VBox/GDTimeLabel
@onready var ag_time_label: Label = $CanvasLayer/UIPanel/MarginContainer/VBox/AGTimeLabel
@onready var speedup_label: Label = $CanvasLayer/UIPanel/MarginContainer/VBox/SpeedupLabel
@onready var fps_label: Label = $CanvasLayer/UIPanel/MarginContainer/VBox/FPSLabel
@onready var frame_graph: FrameGraph = $CanvasLayer/UIPanel/MarginContainer/VBox/FrameGraph
@onready var description_label: RichTextLabel = $CanvasLayer/UIPanel/MarginContainer/VBox/DescriptionLabel
@onready var entity_drawer: EntityDrawer = $VisualizationArea/EntityDrawer

# Benchmark config
const TIMING_ITERATIONS := 3  # Iterations for timing accuracy
const BENCHMARK_INTERVAL := 30  # Only run full benchmark every N frames
const UI_PANEL_WIDTH := 320.0
const WINDOW_SIZE := Vector2i(1920, 1080)  # 1080p resolution

# Scenario management
var scenarios: Array[BenchmarkScenario] = []
var current_scenario: BenchmarkScenario
var current_scenario_index := 0
var frame_counter := 0

# Visualization bounds (area to the right of UI panel)
var visualization_bounds: Rect2


func _ready() -> void:
	# Set window to 1080p on primary monitor
	DisplayServer.window_set_size(WINDOW_SIZE)
	var screen_size := DisplayServer.screen_get_size(0)  # Primary monitor
	var screen_pos := DisplayServer.screen_get_position(0)
	DisplayServer.window_set_position(
		screen_pos + (screen_size - WINDOW_SIZE) / 2
	)

	# Wait a frame for window to resize before calculating bounds
	await get_tree().process_frame

	_calculate_bounds()
	_setup_scenarios()
	_connect_signals()
	_switch_scenario(0)


func _calculate_bounds() -> void:
	var viewport_size := get_viewport_rect().size
	visualization_bounds = Rect2(
		0,  # Entity drawer is offset by UI_PANEL_WIDTH via parent position
		0,
		viewport_size.x - UI_PANEL_WIDTH - 20,
		viewport_size.y - 20
	)


func _setup_scenarios() -> void:
	# Import scenario classes (they will be created when loaded)
	var scenario_classes := [
		preload("res://demos/interactive_benchmark/scenarios/spatial_query_scenario.gd"),
		preload("res://demos/interactive_benchmark/scenarios/array_filter_scenario.gd"),
		preload("res://demos/interactive_benchmark/scenarios/distance_scenario.gd"),
		preload("res://demos/interactive_benchmark/scenarios/steering_scenario.gd"),
		preload("res://demos/interactive_benchmark/scenarios/flocking_scenario.gd"),
		preload("res://demos/interactive_benchmark/scenarios/collision_scenario.gd"),
		preload("res://demos/interactive_benchmark/scenarios/pathfinding_scenario.gd"),
	]

	for scenario_class in scenario_classes:
		var scenario: BenchmarkScenario = scenario_class.new()
		scenarios.append(scenario)
		scenario_dropdown.add_item(scenario.get_name())


func _connect_signals() -> void:
	scenario_dropdown.item_selected.connect(_on_scenario_selected)
	entity_slider.value_changed.connect(_on_entity_slider_changed)


func _process(delta: float) -> void:
	# Update FPS display
	var fps := Engine.get_frames_per_second()
	var frame_ms := delta * 1000.0
	fps_label.text = "%d FPS (%.1f ms)" % [fps, frame_ms]
	frame_graph.add_frame_time(frame_ms)

	if continuous_toggle.button_pressed and current_scenario:
		frame_counter += 1

		# Run full benchmark only periodically to avoid frame drops
		if frame_counter >= BENCHMARK_INTERVAL:
			frame_counter = 0
			_run_benchmark()
		else:
			# Just run AgentiteG once for smooth visualization
			current_scenario.run_agentiteg()

		current_scenario.update_visualization(delta)
		entity_drawer.queue_redraw()


func _input(event: InputEvent) -> void:
	if event is InputEventKey and event.pressed:
		match event.keycode:
			# Number keys 1-7 for scenarios
			KEY_1: _switch_scenario(0)
			KEY_2: _switch_scenario(1)
			KEY_3: _switch_scenario(2)
			KEY_4: _switch_scenario(3)
			KEY_5: _switch_scenario(4)
			KEY_6: _switch_scenario(5)
			KEY_7: _switch_scenario(6)
			# +/- for entity count
			KEY_EQUAL, KEY_KP_ADD:
				entity_slider.value = minf(entity_slider.value + 500, entity_slider.max_value)
			KEY_MINUS, KEY_KP_SUBTRACT:
				entity_slider.value = maxf(entity_slider.value - 500, entity_slider.min_value)
			# Space to toggle continuous mode
			KEY_SPACE:
				continuous_toggle.button_pressed = not continuous_toggle.button_pressed
			# R to reinitialize
			KEY_R:
				_reinitialize_scenario()


func _run_benchmark() -> void:
	if not current_scenario:
		return

	# Run GDScript timing
	var gd_start := Time.get_ticks_usec()
	for i in TIMING_ITERATIONS:
		current_scenario.run_gdscript()
	var gd_end := Time.get_ticks_usec()
	var gd_time := (gd_end - gd_start) / 1000.0 / TIMING_ITERATIONS  # ms

	# Run AgentiteG timing
	var ag_start := Time.get_ticks_usec()
	for i in TIMING_ITERATIONS:
		current_scenario.run_agentiteg()
	var ag_end := Time.get_ticks_usec()
	var ag_time := (ag_end - ag_start) / 1000.0 / TIMING_ITERATIONS  # ms

	# Calculate speedup
	var speedup := gd_time / ag_time if ag_time > 0.001 else 0.0

	# Update UI
	gd_time_label.text = "GDScript:  %.3f ms" % gd_time
	ag_time_label.text = "AgentiteG: %.3f ms" % ag_time
	speedup_label.text = "Speedup:   %.1fx" % speedup

	# Color code speedup
	if speedup >= 10.0:
		speedup_label.add_theme_color_override("font_color", Color(0.2, 0.9, 0.3))
	elif speedup >= 2.0:
		speedup_label.add_theme_color_override("font_color", Color(0.9, 0.9, 0.2))
	else:
		speedup_label.add_theme_color_override("font_color", Color(0.9, 0.5, 0.2))


func _switch_scenario(index: int) -> void:
	if index < 0 or index >= scenarios.size():
		return

	current_scenario_index = index
	current_scenario = scenarios[index]
	scenario_dropdown.select(index)

	_reinitialize_scenario()


func _reinitialize_scenario() -> void:
	if not current_scenario:
		return

	current_scenario.initialize(int(entity_slider.value), visualization_bounds)
	entity_drawer.scenario = current_scenario
	description_label.text = current_scenario.get_description()

	# Run initial benchmark
	_run_benchmark()
	entity_drawer.queue_redraw()


func _on_scenario_selected(index: int) -> void:
	_switch_scenario(index)


func _on_entity_slider_changed(value: float) -> void:
	var count := int(value)
	entity_count_label.text = "Entities: %d" % count

	# Check for flocking scenario entity cap
	if current_scenario and current_scenario.get_name() == "Flocking (Boids)":
		if count > 2000:
			entity_count_label.text = "Entities: %d (capped: 2000)" % count
			count = 2000

	if current_scenario:
		current_scenario.initialize(count, visualization_bounds)
		entity_drawer.scenario = current_scenario
		_run_benchmark()
		entity_drawer.queue_redraw()

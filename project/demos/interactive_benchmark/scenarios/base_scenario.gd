class_name BenchmarkScenario
extends RefCounted
## Base class for all benchmark scenarios.
## Each scenario compares a GDScript implementation against AgentiteG.

var entity_count: int = 1000
var positions: PackedVector2Array
var velocities: PackedVector2Array
var colors: PackedColorArray
var visualization_bounds: Rect2

## Results from last benchmark run
var gdscript_result: Variant
var agentiteg_result: Variant


## Initialize the scenario with entity count and bounds.
func initialize(count: int, bounds: Rect2) -> void:
	entity_count = count
	visualization_bounds = bounds
	_setup_entities()


## Override in subclasses to set up entity positions, velocities, etc.
func _setup_entities() -> void:
	positions = PackedVector2Array()
	positions.resize(entity_count)
	velocities = PackedVector2Array()
	velocities.resize(entity_count)
	colors = PackedColorArray()
	colors.resize(entity_count)

	for i in entity_count:
		positions[i] = Vector2(
			visualization_bounds.position.x + randf() * visualization_bounds.size.x,
			visualization_bounds.position.y + randf() * visualization_bounds.size.y
		)
		velocities[i] = Vector2.ZERO
		colors[i] = Color.WHITE


## Run the GDScript implementation. Override in subclasses.
func run_gdscript() -> void:
	pass


## Run the AgentiteG implementation. Override in subclasses.
func run_agentiteg() -> void:
	pass


## Update visualization state (for animated scenarios). Override in subclasses.
func update_visualization(_delta: float) -> void:
	pass


## Get the scenario name for the UI dropdown.
func get_name() -> String:
	return "Base Scenario"


## Get a description of what this scenario benchmarks.
func get_description() -> String:
	return "Base scenario description."


## Return data for visualization.
## Override in subclasses to add scenario-specific visuals.
func get_draw_data() -> Dictionary:
	return {
		"positions": positions,
		"colors": colors,
		"radii": PackedFloat32Array(),  # Per-entity radii (optional)
		"lines": PackedVector2Array(),  # Debug lines (pairs)
		"rects": [],  # Array of Rect2 for debug
		"circles": [],  # Array of {center: Vector2, radius: float, color: Color}
		"triangles": [],  # Array of PackedVector2Array (3 points each)
	}

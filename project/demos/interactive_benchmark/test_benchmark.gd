extends SceneTree
## Quick test to verify all benchmark scenarios load and run correctly.
## Run with: godot --headless -s demos/interactive_benchmark/test_benchmark.gd

func _init() -> void:
	print("=" .repeat(60))
	print("Interactive Benchmark Demo - Test")
	print("=" .repeat(60))

	var all_passed := true

	all_passed = test_spatial_query() and all_passed
	all_passed = test_array_filter() and all_passed
	all_passed = test_distance() and all_passed
	all_passed = test_steering() and all_passed
	all_passed = test_flocking() and all_passed
	all_passed = test_collision() and all_passed
	all_passed = test_pathfinding() and all_passed

	print("")
	if all_passed:
		print("All benchmark scenario tests PASSED!")
		quit(0)
	else:
		print("Some tests FAILED!")
		quit(1)


func test_spatial_query() -> bool:
	print("\n--- Testing Spatial Query Scenario ---")
	var SpatialQueryScenario := preload("res://demos/interactive_benchmark/scenarios/spatial_query_scenario.gd")
	var scenario = SpatialQueryScenario.new()

	scenario.initialize(1000, Rect2(0, 0, 800, 600))
	print("  Initialized with 1000 entities")

	scenario.run_gdscript()
	print("  GDScript run: found %d entities" % scenario.gdscript_result.size())

	scenario.run_agentiteg()
	print("  AgentiteG run: found %d entities" % scenario.agentiteg_result.size())

	var passed: bool = scenario.gdscript_result.size() == scenario.agentiteg_result.size()
	print("  Result: %s" % ("PASS" if passed else "FAIL - counts differ"))
	return passed


func test_array_filter() -> bool:
	print("\n--- Testing Array Filter Scenario ---")
	var ArrayFilterScenario := preload("res://demos/interactive_benchmark/scenarios/array_filter_scenario.gd")
	var scenario = ArrayFilterScenario.new()

	scenario.initialize(1000, Rect2(0, 0, 800, 600))
	print("  Initialized with 1000 entities")

	scenario.run_gdscript()
	print("  GDScript run: filtered %d entities" % scenario.gdscript_result.size())

	scenario.run_agentiteg()
	print("  AgentiteG run: filtered %d entities" % scenario.agentiteg_result.size())

	var passed: bool = scenario.gdscript_result.size() == scenario.agentiteg_result.size()
	print("  Result: %s" % ("PASS" if passed else "FAIL - counts differ"))
	return passed


func test_distance() -> bool:
	print("\n--- Testing Distance Scenario ---")
	var DistanceScenario := preload("res://demos/interactive_benchmark/scenarios/distance_scenario.gd")
	var scenario = DistanceScenario.new()

	scenario.initialize(1000, Rect2(0, 0, 800, 600))
	print("  Initialized with 1000 entities")

	scenario.run_gdscript()
	print("  GDScript run: found %d nearest" % scenario.gdscript_result.size())

	scenario.run_agentiteg()
	print("  AgentiteG run: found %d nearest" % scenario.agentiteg_result.size())

	var passed: bool = scenario.gdscript_result.size() == scenario.agentiteg_result.size()
	print("  Result: %s" % ("PASS" if passed else "FAIL - counts differ"))
	return passed


func test_steering() -> bool:
	print("\n--- Testing Steering Scenario ---")
	var SteeringScenario := preload("res://demos/interactive_benchmark/scenarios/steering_scenario.gd")
	var scenario = SteeringScenario.new()

	scenario.initialize(500, Rect2(0, 0, 800, 600))
	print("  Initialized with 500 entities")

	scenario.run_gdscript()
	print("  GDScript run: computed %d steering forces" % scenario.gdscript_result.size())

	scenario.run_agentiteg()
	print("  AgentiteG run: computed %d steering forces" % scenario.agentiteg_result.size())

	var passed: bool = scenario.gdscript_result.size() == scenario.agentiteg_result.size()
	print("  Result: %s" % ("PASS" if passed else "FAIL - counts differ"))
	return passed


func test_flocking() -> bool:
	print("\n--- Testing Flocking Scenario ---")
	var FlockingScenario := preload("res://demos/interactive_benchmark/scenarios/flocking_scenario.gd")
	var scenario = FlockingScenario.new()

	scenario.initialize(200, Rect2(0, 0, 800, 600))  # Small count for O(n^2)
	print("  Initialized with 200 entities (capped)")

	scenario.run_gdscript()
	print("  GDScript run: computed %d flock forces" % scenario.gdscript_result.size())

	scenario.run_agentiteg()
	print("  AgentiteG run: computed %d flock forces" % scenario.agentiteg_result.size())

	var passed: bool = scenario.gdscript_result.size() == scenario.agentiteg_result.size()
	print("  Result: %s" % ("PASS" if passed else "FAIL - counts differ"))
	return passed


func test_collision() -> bool:
	print("\n--- Testing Collision Scenario ---")
	var CollisionScenario := preload("res://demos/interactive_benchmark/scenarios/collision_scenario.gd")
	var scenario = CollisionScenario.new()

	scenario.initialize(500, Rect2(0, 0, 800, 600))
	print("  Initialized with 500 entities")

	scenario.run_gdscript()
	var gd_pairs: int = scenario.gdscript_result.size() / 2
	print("  GDScript run: found %d collision pairs" % gd_pairs)

	scenario.run_agentiteg()
	var ag_pairs: int = scenario.agentiteg_result.size() / 2
	print("  AgentiteG run: found %d collision pairs" % ag_pairs)

	var passed: bool = gd_pairs == ag_pairs
	print("  Result: %s" % ("PASS" if passed else "FAIL - pair counts differ"))
	return passed


func test_pathfinding() -> bool:
	print("\n--- Testing Pathfinding Scenario ---")
	var PathfindingScenario := preload("res://demos/interactive_benchmark/scenarios/pathfinding_scenario.gd")
	var scenario = PathfindingScenario.new()

	scenario.initialize(100, Rect2(0, 0, 800, 600))
	print("  Initialized with grid-based pathfinding")

	scenario.run_gdscript()
	print("  GDScript run: generated flow field (%d cells)" % scenario.gdscript_result.size())

	scenario.run_agentiteg()
	print("  AgentiteG run: generated flow field (%d cells)" % scenario.agentiteg_result.size())

	var passed: bool = scenario.gdscript_result.size() == scenario.agentiteg_result.size()
	print("  Result: %s" % ("PASS" if passed else "FAIL - cell counts differ"))
	return passed

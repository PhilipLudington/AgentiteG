extends SceneTree
## =============================================================================
## ArrayOps Example - Fast array filtering, sorting, and reduction
## =============================================================================
##
## WHEN TO USE:
## ------------
## ArrayOps replaces GDScript loops over PackedArrays with native C++ code.
## Use it whenever you're processing arrays with 100+ elements per frame.
##
## PERFECT FOR:
## - Filtering entities by HP, distance, team, state
## - Sorting by priority, distance, score
## - Finding min/max/sum/mean of large arrays
## - Selecting subsets of data (like numpy indexing)
## - Any array operation in a hot path
##
## WHEN TO AVOID:
## - Small arrays (<50 elements) - GDScript is fine
## - Complex filtering logic - custom GDScript may be clearer
## - One-time operations - optimization isn't needed
##
## PERFORMANCE GAINS:
## - Filter: 15-20x faster than GDScript
## - Sort: 10-15x faster than GDScript
## - Reduce: 20-30x faster than GDScript
##
## KEY INSIGHT:
## All methods are STATIC - call them on the class directly:
##   ArrayOps.filter_gt_float(values, threshold)
## NOT: var ops = ArrayOps.new() # Wrong!
##
## Run with: godot --headless -s examples/example_array_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("ArrayOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	filter_operations()
	sort_operations()
	reduce_operations()
	select_operations()
	find_and_count_operations()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func filter_operations() -> void:
	print("--- Filter Operations ---")
	print("")
	print("Filters return INDICES of matching elements, not the elements themselves.")
	print("This lets you use the same indices across multiple related arrays.")
	print("")

	# Float comparisons - common for HP, damage, speed, etc.
	var health = PackedFloat32Array([100, 45, 80, 15, 60, 5, 90])
	print("Health values: ", health)

	# -------------------------------------------------------------------------
	# Float comparison filters
	# USE WHEN: Filtering by HP, damage, distance, speed, etc.
	# -------------------------------------------------------------------------
	print("\nFloat filters (returns indices, not values):")

	var critical = ArrayOps.filter_lt_float(health, 20.0)
	print("  filter_lt_float(health, 20): ", critical)
	print("    USE CASE: Find critically wounded units")

	var healthy = ArrayOps.filter_gte_float(health, 80.0)
	print("  filter_gte_float(health, 80): ", healthy)
	print("    USE CASE: Find units at full/near-full health")

	var mid_health = ArrayOps.filter_range_float(health, 40.0, 70.0)
	print("  filter_range_float(health, 40, 70): ", mid_health)
	print("    USE CASE: Find wounded but not critical units")

	# -------------------------------------------------------------------------
	# Int comparison filters
	# USE WHEN: Filtering by team ID, level, state enum, etc.
	# -------------------------------------------------------------------------
	var team_ids = PackedInt32Array([1, 1, 2, 1, 2, 2, 1])
	print("\nInt filters:")

	var team_1 = ArrayOps.filter_eq_int(team_ids, 1)
	print("  filter_eq_int(team_ids, 1): ", team_1)
	print("    USE CASE: Get all units on team 1")

	var levels = PackedInt32Array([5, 12, 3, 20, 8, 15, 1])
	var high_level = ArrayOps.filter_gte_int(levels, 10)
	print("  filter_gte_int(levels, 10): ", high_level)
	print("    USE CASE: Find high-level enemies")

	# -------------------------------------------------------------------------
	# Spatial filters
	# USE WHEN: Finding entities on screen, in range, in zone
	# -------------------------------------------------------------------------
	var positions = PackedVector2Array([
		Vector2(50, 50), Vector2(150, 150), Vector2(500, 500),
		Vector2(75, 80), Vector2(1000, 1000)
	])
	print("\nSpatial filters:")

	var viewport = Rect2(0, 0, 200, 200)
	var on_screen = ArrayOps.filter_in_rect(positions, viewport)
	print("  filter_in_rect(positions, viewport): ", on_screen)
	print("    USE CASE: Find entities visible in viewport")

	var near_center = ArrayOps.filter_in_radius(positions, Vector2(100, 100), 100.0)
	print("  filter_in_radius(positions, center, 100): ", near_center)
	print("    USE CASE: Find entities within attack range")
	print("")


func sort_operations() -> void:
	print("--- Sort Operations ---")
	print("")
	print("Two types: sort (returns sorted copy) and argsort (returns sort order).")
	print("Argsort is powerful because you can apply the order to related arrays.")
	print("")

	var values = PackedFloat32Array([3.5, 1.2, 4.8, 2.1, 5.0])
	print("Original values: ", values)

	# -------------------------------------------------------------------------
	# sort - Returns a sorted copy
	# USE WHEN: You just need values in order
	# -------------------------------------------------------------------------
	print("\nsort_floats (returns sorted copy):")
	var sorted_asc = ArrayOps.sort_floats(values)
	print("  Ascending: ", sorted_asc)
	var sorted_desc = ArrayOps.sort_floats(values, true)
	print("  Descending: ", sorted_desc)

	# -------------------------------------------------------------------------
	# argsort - Returns indices that would sort
	# USE WHEN: You need to sort multiple arrays the same way
	# -------------------------------------------------------------------------
	var priorities = PackedFloat32Array([30, 10, 50, 20, 40])
	print("\nargsort_floats (returns INDICES that would sort):")
	print("  Priorities: ", priorities)

	var order = ArrayOps.argsort_floats(priorities)
	print("  Order (ascending): ", order)
	print("    Index ", order[0], " has lowest priority (", priorities[order[0]], ")")

	var order_desc = ArrayOps.argsort_floats(priorities, true)
	print("  Order (descending): ", order_desc)
	print("    Index ", order_desc[0], " has highest priority (", priorities[order_desc[0]], ")")

	# -------------------------------------------------------------------------
	# argsort_by_distance - Sort by distance from a point
	# USE WHEN: Target prioritization, nearest-first processing
	# -------------------------------------------------------------------------
	var positions = PackedVector2Array([
		Vector2(100, 0), Vector2(10, 0), Vector2(50, 0), Vector2(200, 0)
	])
	var origin = Vector2.ZERO
	print("\nargsort_by_distance (spatial sorting):")
	print("  Positions: ", positions)
	print("  Origin: ", origin)

	var by_distance = ArrayOps.argsort_by_distance(positions, origin)
	print("  Order by distance: ", by_distance)
	print("    Closest: index ", by_distance[0], " at ", positions[by_distance[0]])

	# -------------------------------------------------------------------------
	# reorder - Apply sort order to other arrays
	# USE WHEN: Sorting multiple related arrays together
	# -------------------------------------------------------------------------
	print("\nReorder (apply sort order to related arrays):")
	print("  USE CASE: Sort enemies by distance, keep HP array aligned")
	var entity_ids = PackedFloat32Array([1.0, 2.0, 3.0, 4.0])
	var sorted_ids = ArrayOps.reorder_floats(entity_ids, by_distance)
	print("  Original IDs: ", entity_ids)
	print("  Reordered by distance: ", sorted_ids)
	print("")


func reduce_operations() -> void:
	print("--- Reduce Operations ---")
	print("")
	print("Reduce operations collapse an array to a single value.")
	print("Common uses: total damage, average health, finding extremes.")
	print("")

	var damage = PackedFloat32Array([25.5, 30.0, 15.5, 40.0, 10.0])
	print("Damage values: ", damage)

	# -------------------------------------------------------------------------
	# sum - Total of all values
	# USE WHEN: Total damage, combined score, summed forces
	# -------------------------------------------------------------------------
	var total = ArrayOps.sum_floats(damage)
	print("\nsum_floats: ", total)
	print("  USE CASE: Total damage dealt, combined resources")

	# -------------------------------------------------------------------------
	# min/max - Extreme values
	# USE WHEN: Finding weakest/strongest, cheapest/most expensive
	# -------------------------------------------------------------------------
	var min_dmg = ArrayOps.min_float(damage)
	var max_dmg = ArrayOps.max_float(damage)
	print("min_float: ", min_dmg, ", max_float: ", max_dmg)
	print("  USE CASE: Damage range for scaling")

	# -------------------------------------------------------------------------
	# mean - Average value
	# USE WHEN: Average health, mean score, typical value
	# -------------------------------------------------------------------------
	var avg = ArrayOps.mean_floats(damage)
	print("mean_floats: ", avg)
	print("  USE CASE: Average damage for balance tuning")

	# -------------------------------------------------------------------------
	# argmin/argmax - INDEX of min/max
	# USE WHEN: Finding which entity has the extreme value
	# -------------------------------------------------------------------------
	var weakest_idx = ArrayOps.argmin_floats(damage)
	var strongest_idx = ArrayOps.argmax_floats(damage)
	print("argmin_floats: index ", weakest_idx, " (value ", damage[weakest_idx], ")")
	print("argmax_floats: index ", strongest_idx, " (value ", damage[strongest_idx], ")")
	print("  USE CASE: Find which enemy is weakest to focus fire")

	# -------------------------------------------------------------------------
	# Vector reductions - sum and mean for positions/forces
	# USE WHEN: Center of mass, combined force vector
	# -------------------------------------------------------------------------
	var forces = PackedVector2Array([
		Vector2(10, 0), Vector2(0, 10), Vector2(-5, 5), Vector2(5, -5)
	])
	var total_force = ArrayOps.sum_vector2(forces)
	var center = ArrayOps.mean_vector2(forces)
	print("\nVector reductions:")
	print("  Forces: ", forces)
	print("  sum_vector2: ", total_force, " (combined force)")
	print("  mean_vector2: ", center, " (center point)")
	print("")


func select_operations() -> void:
	print("--- Select Operations (numpy-style indexing) ---")
	print("")
	print("Select extracts elements at specific indices.")
	print("Like numpy: values[indices] or values[[0, 3, 7]]")
	print("")

	var all_values = PackedFloat32Array([10, 20, 30, 40, 50, 60, 70])
	var indices = PackedInt32Array([1, 3, 5])

	print("All values: ", all_values)
	print("Indices to select: ", indices)

	# -------------------------------------------------------------------------
	# select - Pick elements by index
	# USE WHEN: Get values for filtered/sorted indices
	# -------------------------------------------------------------------------
	var selected = ArrayOps.select_floats(all_values, indices)
	print("\nselect_floats: ", selected)
	print("  USE CASE: Get HP values for enemies in range")

	# Works for positions too
	var all_positions = PackedVector2Array([
		Vector2(0, 0), Vector2(10, 10), Vector2(20, 20),
		Vector2(30, 30), Vector2(40, 40)
	])
	var selected_pos = ArrayOps.select_vector2(all_positions, indices)
	print("\nselect_vector2 at indices ", indices, ": ", selected_pos)
	print("  USE CASE: Get positions of specific entities")

	print("\n  PATTERN: Filter -> Select")
	print("    indices = ArrayOps.filter_gt_float(hp, 50)")
	print("    positions = ArrayOps.select_vector2(all_pos, indices)")
	print("")


func find_and_count_operations() -> void:
	print("--- Find and Count Operations ---")
	print("")
	print("Find returns the first match. Count returns how many match.")
	print("Both avoid creating full result arrays when you don't need them.")
	print("")

	var values = PackedFloat32Array([5, 10, 15, 10, 25, 30, 10])
	print("Values: ", values)

	# -------------------------------------------------------------------------
	# find_first - Index of first match
	# USE WHEN: Looking for any one match, early exit
	# -------------------------------------------------------------------------
	var first_over_20 = ArrayOps.find_first_gt_float(values, 20.0)
	print("\nfind_first_gt_float(values, 20): index ", first_over_20)
	print("  USE CASE: Find any enemy above HP threshold")

	# -------------------------------------------------------------------------
	# count - How many match (no allocation)
	# USE WHEN: You need the count, not the actual entities
	# -------------------------------------------------------------------------
	var count_over_10 = ArrayOps.count_gt_float(values, 10.0)
	print("count_gt_float(values, 10): ", count_over_10)
	print("  USE CASE: How many enemies have HP > 10?")

	# Int versions
	var ids = PackedInt32Array([1, 2, 1, 3, 1, 2, 1])
	var count_id_1 = ArrayOps.count_eq_int(ids, 1)
	print("\ncount_eq_int(ids, 1): ", count_id_1)
	print("  USE CASE: How many units on team 1?")

	# Spatial count
	var positions = PackedVector2Array([
		Vector2(0, 0), Vector2(5, 5), Vector2(50, 50), Vector2(100, 100)
	])
	var nearby_count = ArrayOps.count_in_radius(positions, Vector2.ZERO, 20.0)
	print("count_in_radius from origin, r=20: ", nearby_count)
	print("  USE CASE: How many enemies nearby? (for UI)")
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Target Prioritization ---")
	print("")
	print("This shows a complete targeting algorithm using ArrayOps.")
	print("Goal: Find the weakest enemy in attack range.")
	print("")

	# Enemy data stored in parallel arrays (ECS-style)
	var enemies = [
		{"name": "Grunt-1", "hp": 30, "pos": Vector2(100, 50)},
		{"name": "Grunt-2", "hp": 50, "pos": Vector2(200, 80)},
		{"name": "Elite", "hp": 100, "pos": Vector2(150, 60)},
		{"name": "Grunt-3", "hp": 10, "pos": Vector2(500, 300)},
		{"name": "Boss", "hp": 500, "pos": Vector2(120, 70)}
	]

	# Extract into parallel arrays (common game pattern)
	var healths = PackedFloat32Array()
	var positions = PackedVector2Array()
	for e in enemies:
		healths.append(e.hp)
		positions.append(e.pos)

	var attacker_pos = Vector2(100, 50)
	var attack_range = 150.0

	print("Attacker at ", attacker_pos, " with range ", attack_range)
	print("\nEnemies:")
	for e in enemies:
		print("  ", e.name, " HP:", e.hp, " at ", e.pos)

	# STEP 1: Filter to in-range enemies
	print("\n--- STEP 1: Find enemies in range ---")
	var in_range = ArrayOps.filter_in_radius(positions, attacker_pos, attack_range)
	print("  Indices in range: ", in_range)
	print("  Count: ", in_range.size())

	if in_range.is_empty():
		print("  No targets in range!")
		return

	# STEP 2: Get health values of in-range enemies only
	print("\n--- STEP 2: Get HP of in-range enemies ---")
	var in_range_health = ArrayOps.select_floats(healths, in_range)
	print("  In-range health values: ", in_range_health)

	# STEP 3: Find weakest among in-range (focus fire strategy)
	print("\n--- STEP 3: Find weakest (focus fire) ---")
	var weakest_local = ArrayOps.argmin_floats(in_range_health)
	var weakest_global = in_range[weakest_local]  # Convert back to global index
	print("  Weakest local index: ", weakest_local)
	print("  Weakest global index: ", weakest_global)
	print("  TARGET: ", enemies[weakest_global].name,
		  " (HP: ", enemies[weakest_global].hp, ")")

	# ALTERNATIVE: Sort by HP (show priority list)
	print("\n--- ALTERNATIVE: Priority list (weakest first) ---")
	var by_hp = ArrayOps.argsort_floats(in_range_health)
	print("  Priority order:")
	for i in by_hp:
		var global_idx = in_range[i]
		print("    ", enemies[global_idx].name, " HP:", enemies[global_idx].hp)

	# ALTERNATIVE: Nearest target
	print("\n--- ALTERNATIVE: Nearest target ---")
	var in_range_positions = ArrayOps.select_vector2(positions, in_range)
	var by_distance = ArrayOps.argsort_by_distance(in_range_positions, attacker_pos)
	var nearest_global = in_range[by_distance[0]]
	print("  Nearest: ", enemies[nearest_global].name)

	print("\n  PATTERN SUMMARY:")
	print("  1. filter_in_radius -> Get candidates")
	print("  2. select_floats -> Get their attributes")
	print("  3. argmin/argsort -> Find best target")
	print("  4. Map back to global index")
	print("")

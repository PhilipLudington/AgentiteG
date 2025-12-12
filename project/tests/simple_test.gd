extends SceneTree

func _init():
	print("Testing AgentiteG...")

	# Test if classes exist
	print("Checking SpatialHash2D...")
	var spatial = SpatialHash2D.new()
	print("SpatialHash2D created: ", spatial)
	print("Cell size: ", spatial.cell_size)

	print("Checking ArrayOps...")
	var values = PackedFloat32Array([3.0, 1.0, 2.0])
	var result = ArrayOps.filter_gt_float(values, 1.5)
	print("Filter result: ", result)

	print("All tests passed!")
	quit(0)

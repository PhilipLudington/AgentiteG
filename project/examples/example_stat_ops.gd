extends SceneTree
## =============================================================================
## StatOps Example - Statistical operations and analytics
## =============================================================================
##
## WHEN TO USE:
## ------------
## StatOps provides statistical analysis for game analytics, balancing, and AI.
## Use to understand distributions, detect anomalies, and smooth data.
##
## PERFECT FOR:
## - Game analytics (player behavior, session times, scores)
## - Balance testing (damage distributions, difficulty analysis)
## - AI decision making (weighted choices, trend detection)
## - Data smoothing (noisy sensor data, score averaging)
## - Leaderboard systems (percentiles, rankings)
## - Procedural generation (distribution control)
##
## KEY OPERATIONS:
## - Descriptive: mean, median, std_dev, variance, min_max
## - Percentiles: q1, median, q3, percentile(p)
## - Normalization: min_max (0-1), z-score (mean=0, std=1)
## - Running stats: moving average, EMA, cumsum
## - Outlier detection: z-score method, IQR method
## - Histograms: distribution visualization
## - Correlation: relationship between variables
##
## ALL METHODS ARE STATIC - call directly on StatOps class.
##
## Run with: godot --headless -s examples/example_stat_ops.gd
## =============================================================================

func _init() -> void:
	print("=" .repeat(60))
	print("StatOps Feature Showcase")
	print("=" .repeat(60))
	print("")

	descriptive_stats()
	percentiles()
	normalization()
	running_stats()
	outlier_detection()
	histograms()
	correlation()
	game_scenario()

	print("\nAll examples completed!")
	quit(0)


func descriptive_stats() -> void:
	print("--- Descriptive Statistics ---")
	print("")
	print("Understand the shape and spread of your data.")
	print("Essential for game balancing and analytics.")
	print("")

	# -------------------------------------------------------------------------
	# Descriptive statistics - Summarize datasets
	# USE WHEN: Analyzing player scores, damage distributions, session times
	# WHEN TO USE EACH:
	#   - mean: Average value (sensitive to outliers)
	#   - median: Middle value (robust to outliers)
	#   - std_dev: How spread out values are
	# -------------------------------------------------------------------------
	var scores = PackedFloat32Array([72, 85, 90, 65, 78, 92, 88, 76, 83, 70])
	print("Descriptive stats USE CASES:")
	print("  - Average player score (mean)")
	print("  - Typical session time (median - ignore outliers)")
	print("  - Score consistency (std_dev)")
	print("")

	print("Scores: ", scores)

	# Basic stats
	print("\nBasic statistics:")
	print("  sum: ", StatOps.sum(scores))
	print("  mean: ", StatOps.mean(scores))
	print("  median: ", StatOps.median(scores))
	print("  std_dev: ", "%.2f" % StatOps.std_dev(scores))
	print("  variance: ", "%.2f" % StatOps.variance(scores))

	# Range
	var mm = StatOps.min_max(scores)
	print("  min_max: ", mm, " (x=min, y=max)")
	print("  range: ", mm.y - mm.x)
	print("")


func percentiles() -> void:
	print("--- Percentiles and Quartiles ---")
	print("")
	print("Percentiles tell you where a value ranks in a distribution.")
	print("Essential for leaderboards, player segmentation, and difficulty.")
	print("")

	# -------------------------------------------------------------------------
	# Percentiles - Where does a value rank?
	# USE WHEN: Leaderboards, player tiers, difficulty brackets
	# INTERPRETATION:
	#   - 50th percentile = median (half above, half below)
	#   - 95th percentile = top 5%
	#   - Q1/Q3 = bottom/top quartile boundaries
	# -------------------------------------------------------------------------
	var data = PackedFloat32Array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
	print("Percentile USE CASES:")
	print("  - Leaderboard ranking ('You are in top 5%%!')")
	print("  - Player segmentation (casual/regular/hardcore)")
	print("  - Difficulty tiers")
	print("")

	print("Data: ", data)

	# Quartiles
	print("\nQuartiles:")
	print("  Q1 (25th): ", StatOps.q1(data))
	print("  Q2 (50th/median): ", StatOps.median(data))
	print("  Q3 (75th): ", StatOps.q3(data))
	print("  IQR: ", StatOps.q3(data) - StatOps.q1(data))

	# Arbitrary percentiles
	print("\nPercentiles:")
	print("  10th: ", StatOps.percentile(data, 10.0))
	print("  50th: ", StatOps.percentile(data, 50.0))
	print("  90th: ", StatOps.percentile(data, 90.0))
	print("  99th: ", StatOps.percentile(data, 99.0))
	print("")


func normalization() -> void:
	print("--- Normalization ---")
	print("")
	print("Scale values to standard ranges for comparison or processing.")
	print("")

	# -------------------------------------------------------------------------
	# Normalization - Scale values to standard ranges
	# USE WHEN: Comparing different metrics, ML inputs, health bars
	# TYPES:
	#   - min_max: Scale to [0, 1] - good for display (health bars)
	#   - z_score: Mean=0, std=1 - good for comparison/ML
	# -------------------------------------------------------------------------
	var values = PackedFloat32Array([10, 20, 30, 40, 50])
	print("Normalization USE CASES:")
	print("  - Health bar display (normalize to 0-1)")
	print("  - Comparing different metrics (z-score)")
	print("  - Machine learning inputs")
	print("")

	print("Original values: ", values)

	# Min-max normalization [0, 1]
	var minmax = StatOps.normalize_min_max(values)
	print("\nnormalize_min_max [0, 1]: ", minmax)

	# Z-score normalization (mean=0, std=1)
	var zscore = StatOps.normalize_z_score(values)
	print("normalize_z_score (mean=0, std=1): ", zscore)

	# Verify z-score properties
	print("  Normalized mean: ", "%.4f" % StatOps.mean(zscore))
	print("  Normalized std: ", "%.4f" % StatOps.std_dev(zscore))
	print("")


func running_stats() -> void:
	print("--- Running/Moving Statistics ---")
	print("")
	print("Track trends over time by smoothing noisy data.")
	print("Essential for time series, progress tracking, and trend detection.")
	print("")

	# -------------------------------------------------------------------------
	# Running/Moving statistics - Smooth noisy time series
	# USE WHEN: Tracking trends, smoothing noisy data, progress bars
	# TYPES:
	#   - running_mean: Average of last N values (simple smoothing)
	#   - EMA: Exponential moving average (recent values weighted more)
	#   - cumsum: Running total (XP, score accumulation)
	# -------------------------------------------------------------------------
	var data = PackedFloat32Array([1, 3, 2, 5, 4, 6, 8, 7, 9, 10])
	print("Running stats USE CASES:")
	print("  - Smooth noisy sensor data (running_mean)")
	print("  - Trend detection (EMA)")
	print("  - XP/score accumulation (cumsum)")
	print("")

	print("Data: ", data)

	# Cumulative sum
	var cumsum = StatOps.cumsum(data)
	print("\ncumsum: ", cumsum)

	# Running mean (window size)
	var window = 3
	var running_mean = StatOps.running_mean(data, window)
	print("\nrunning_mean (window=", window, "): ", running_mean)

	# Exponential moving average
	var alpha = 0.3
	var ema = StatOps.exponential_moving_average(data, alpha)
	print("\nexponential_moving_average (alpha=", alpha, "):")
	print("  ", ema)
	print("  (Higher alpha = more weight on recent values)")
	print("")


func outlier_detection() -> void:
	print("--- Outlier Detection ---")
	print("")
	print("Find unusual values that don't fit the pattern.")
	print("Essential for data cleaning, cheat detection, and anomaly finding.")
	print("")

	# -------------------------------------------------------------------------
	# Outlier detection - Find anomalous values
	# USE WHEN: Cheat detection, data cleaning, anomaly alerts
	# METHODS:
	#   - z-score: Values more than N standard deviations from mean
	#   - IQR: Values outside Q1 - 1.5*IQR or Q3 + 1.5*IQR
	# -------------------------------------------------------------------------
	var data = PackedFloat32Array([10, 12, 11, 13, 9, 11, 100, 12, 10, -50])
	print("Outlier detection USE CASES:")
	print("  - Cheat detection (impossible scores)")
	print("  - Data cleaning (sensor errors)")
	print("  - Anomaly alerts (unusual player behavior)")
	print("")

	print("Data: ", data)
	print("Obvious outliers: 100 (at index 6), -50 (at index 9)")

	# Z-score method
	var threshold = 2.0
	var zscore_outliers = StatOps.outliers_zscore(data, threshold)
	print("\noutliers_zscore (threshold=", threshold, "):")
	print("  Outlier indices: ", zscore_outliers)

	# IQR method
	var iqr_multiplier = 1.5
	var iqr_outliers = StatOps.outliers_iqr(data, iqr_multiplier)
	print("\noutliers_iqr (multiplier=", iqr_multiplier, "):")
	print("  Outlier indices: ", iqr_outliers)
	print("")


func histograms() -> void:
	print("--- Histograms ---")
	print("")
	print("Visualize how values are distributed across ranges.")
	print("Essential for understanding player behavior patterns.")
	print("")

	# -------------------------------------------------------------------------
	# Histograms - Distribution visualization
	# USE WHEN: Analyzing score distributions, session times, item usage
	# OUTPUT: Count of values in each bin (range)
	# -------------------------------------------------------------------------
	var data = PackedFloat32Array([
		15, 22, 35, 42, 18, 55, 67, 28, 73, 45,
		32, 58, 41, 63, 25, 48, 71, 39, 52, 33
	])
	print("Histogram USE CASES:")
	print("  - Score distribution analysis")
	print("  - Session time patterns")
	print("  - Item usage frequency")
	print("")

	print("Data: ", data)

	# Create histogram
	var num_bins = 5
	var hist = StatOps.histogram(data, num_bins)
	print("\nhistogram (", num_bins, " bins):")
	print("  Bin counts: ", hist)

	# Get bin edges
	var mm = StatOps.min_max(data)
	var edges = StatOps.bin_edges(mm.x, mm.y, num_bins)
	print("  Bin edges: ", edges)

	# Display histogram
	print("\n  Visual:")
	for i in range(hist.size()):
		var bar = "*".repeat(hist[i])
		print("    [%.0f-%.0f]: %s (%d)" % [edges[i], edges[i+1], bar, hist[i]])
	print("")


func correlation() -> void:
	print("--- Correlation ---")
	print("")
	print("Measure the relationship between two variables.")
	print("Essential for understanding cause-and-effect in game data.")
	print("")

	# -------------------------------------------------------------------------
	# Correlation - Measure relationship between variables
	# USE WHEN: Analyzing relationships (time vs score, level vs deaths)
	# OUTPUT: -1 to +1
	#   +1: Perfect positive (X goes up, Y goes up)
	#   -1: Perfect negative (X goes up, Y goes down)
	#    0: No relationship
	# -------------------------------------------------------------------------
	var x = PackedFloat32Array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
	var y_positive = PackedFloat32Array([2, 4, 5, 4, 5, 7, 8, 9, 10, 11])  # Positive correlation
	var y_negative = PackedFloat32Array([10, 9, 8, 8, 7, 5, 4, 3, 2, 1])    # Negative correlation
	var y_none = PackedFloat32Array([5, 8, 3, 7, 4, 9, 2, 6, 8, 5])         # No correlation

	print("Correlation USE CASES:")
	print("  - Does play time correlate with skill? (time vs score)")
	print("  - Does difficulty affect retention? (level vs quit rate)")
	print("  - Does spending correlate with engagement?")
	print("")

	print("X: ", x)
	print("Y (positive): ", y_positive)
	print("Y (negative): ", y_negative)
	print("Y (none): ", y_none)

	print("\nPearson correlation coefficients:")
	print("  X vs Y_positive: ", "%.3f" % StatOps.correlation(x, y_positive))
	print("  X vs Y_negative: ", "%.3f" % StatOps.correlation(x, y_negative))
	print("  X vs Y_none: ", "%.3f" % StatOps.correlation(x, y_none))

	print("\nInterpretation:")
	print("  +1.0 = perfect positive, -1.0 = perfect negative, 0 = none")
	print("")


func game_scenario() -> void:
	print("--- Game Scenario: Player Analytics ---")

	# Simulate player session data
	var session_times = PackedFloat32Array([  # Minutes per session
		45, 32, 60, 28, 55, 120, 38, 42, 51, 35,
		48, 65, 30, 44, 52, 180, 40, 37, 58, 33
	])

	var player_scores = PackedFloat32Array([
		1200, 850, 2100, 920, 1850, 3500, 1100, 1300, 1650, 980,
		1420, 2200, 890, 1280, 1720, 4200, 1150, 1050, 1900, 960
	])

	print("1. Session Time Analysis:")
	print("   Sessions: ", session_times.size())
	print("   Mean: ", "%.1f" % StatOps.mean(session_times), " minutes")
	print("   Median: ", StatOps.median(session_times), " minutes")
	print("   Std Dev: ", "%.1f" % StatOps.std_dev(session_times), " minutes")

	var time_mm = StatOps.min_max(session_times)
	print("   Range: ", time_mm.x, " - ", time_mm.y, " minutes")

	# Detect unusual sessions
	var unusual = StatOps.outliers_zscore(session_times, 2.0)
	print("   Unusual sessions (z > 2): ", unusual)
	if unusual.size() > 0:
		print("   (Sessions: ", session_times[unusual[0]], " min)")

	# Player segmentation using percentiles
	print("\n2. Player Segmentation by Score:")
	var p25 = StatOps.percentile(player_scores, 25.0)
	var p75 = StatOps.percentile(player_scores, 75.0)
	print("   Bottom 25%% (casual): < ", p25)
	print("   Middle 50%% (regular): ", p25, " - ", p75)
	print("   Top 25%% (hardcore): > ", p75)

	# Score distribution
	print("\n3. Score Distribution:")
	var hist = StatOps.histogram(player_scores, 5)
	var edges = StatOps.bin_edges(
		StatOps.min_max(player_scores).x,
		StatOps.min_max(player_scores).y,
		5
	)
	for i in range(hist.size()):
		print("   ", "%.0f" % edges[i], "-", "%.0f" % edges[i+1], ": ", hist[i], " players")

	# Correlation analysis
	print("\n4. Time vs Score Correlation:")
	var r = StatOps.correlation(session_times, player_scores)
	print("   Correlation coefficient: ", "%.3f" % r)
	if r > 0.7:
		print("   Strong positive: longer sessions = higher scores")
	elif r > 0.3:
		print("   Moderate positive: some relationship")
	elif r > -0.3:
		print("   Weak/no correlation")
	else:
		print("   Negative correlation")

	# Trend analysis with EMA
	print("\n5. Revenue Trend (last 10 days):")
	var daily_revenue = PackedFloat32Array([
		100, 120, 95, 140, 130, 145, 160, 155, 170, 180
	])
	var ema = StatOps.exponential_moving_average(daily_revenue, 0.3)
	print("   Daily: ", daily_revenue)
	print("   Smoothed (EMA): ", ema)
	print("   Trend: ", "%.1f" % daily_revenue[0], " -> ", "%.1f" % daily_revenue[9],
		  " (", "%.1f" % ((daily_revenue[9] / daily_revenue[0] - 1) * 100), "%% change)")

	# Normalize for leaderboard percentile
	print("\n6. Leaderboard Percentile:")
	var my_score = 1500.0
	# Count how many scores are below mine
	var below = 0
	for s in player_scores:
		if s < my_score:
			below += 1
	var percentile_rank = (float(below) / player_scores.size()) * 100
	print("   My score: ", my_score)
	print("   Percentile: ", "%.0f" % percentile_rank, "th")

	# Cumulative progress
	print("\n7. Cumulative XP Progress:")
	var xp_gains = PackedFloat32Array([100, 150, 80, 200, 120, 180, 90, 250, 110, 160])
	var cumulative = StatOps.cumsum(xp_gains)
	print("   XP gains: ", xp_gains)
	print("   Total XP: ", cumulative)

	print("")

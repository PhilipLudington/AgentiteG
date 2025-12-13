# StatOps

Statistical operations for game analytics, balancing, and data analysis.

## When to Use

- Game analytics (player behavior, session times, scores)
- Balance testing (damage distributions, difficulty analysis)
- Leaderboard systems (percentiles, rankings)
- Data smoothing (noisy sensor data, trend detection)
- Outlier/cheat detection

All methods are **static** - call directly on the class.

## Descriptive Statistics

### Basic Statistics

```gdscript
var scores = PackedFloat32Array([72, 85, 90, 65, 78])

StatOps.sum(scores)       # Total: 390
StatOps.mean(scores)      # Average: 78
StatOps.median(scores)    # Middle value: 78
StatOps.std_dev(scores)   # Standard deviation
StatOps.variance(scores)  # Variance
StatOps.min_max(scores)   # Vector2(min, max)
```

### When to Use Mean vs Median

- **Mean**: Average value. Sensitive to outliers.
- **Median**: Middle value. Robust to outliers.

```gdscript
var session_times = PackedFloat32Array([30, 45, 35, 40, 500])  # 500 is outlier
StatOps.mean(session_times)    # ~130 (skewed by outlier)
StatOps.median(session_times)  # 40 (represents typical user better)
```

## Percentiles and Quartiles

#### `percentile(data: PackedFloat32Array, p: float) -> float`
Find the value at a given percentile (0-100).

```gdscript
var p90 = StatOps.percentile(scores, 90.0)  # Top 10% threshold
```

#### `q1(data) / median(data) / q3(data)`
Quartile values (25th, 50th, 75th percentiles).

```gdscript
# Player segmentation
var q1 = StatOps.q1(scores)   # Bottom 25% threshold
var q3 = StatOps.q3(scores)   # Top 25% threshold

if player_score < q1:
    tier = "Casual"
elif player_score > q3:
    tier = "Hardcore"
else:
    tier = "Regular"
```

## Normalization

#### `normalize_min_max(data: PackedFloat32Array) -> PackedFloat32Array`
Scale values to [0, 1] range.

```gdscript
var health_normalized = StatOps.normalize_min_max(health_values)
# Now all values are 0.0 to 1.0, perfect for progress bars
```

#### `normalize_z_score(data: PackedFloat32Array) -> PackedFloat32Array`
Transform to mean=0, std_dev=1. Good for comparing different metrics.

```gdscript
var z_scores = StatOps.normalize_z_score(values)
# Values are now in "standard deviations from mean"
```

## Running Statistics

For smoothing noisy data and detecting trends.

#### `cumsum(data: PackedFloat32Array) -> PackedFloat32Array`
Cumulative sum (running total).

```gdscript
var xp_gains = PackedFloat32Array([100, 150, 80, 200])
var total_xp = StatOps.cumsum(xp_gains)  # [100, 250, 330, 530]
```

#### `running_mean(data: PackedFloat32Array, window: int) -> PackedFloat32Array`
Moving average over a window.

```gdscript
var smoothed = StatOps.running_mean(noisy_data, 5)  # 5-sample window
```

#### `exponential_moving_average(data: PackedFloat32Array, alpha: float) -> PackedFloat32Array`
EMA gives more weight to recent values. Alpha 0-1 (higher = more recent weight).

```gdscript
var trend = StatOps.exponential_moving_average(daily_revenue, 0.3)
```

## Outlier Detection

Find unusual values (cheaters, errors, anomalies).

#### `outliers_zscore(data: PackedFloat32Array, threshold: float) -> PackedInt32Array`
Find indices where |z-score| > threshold.

```gdscript
var cheaters = StatOps.outliers_zscore(scores, 3.0)  # 3 std devs from mean
for idx in cheaters:
    flag_for_review(players[idx])
```

#### `outliers_iqr(data: PackedFloat32Array, multiplier: float) -> PackedInt32Array`
Find indices outside Q1 - multiplier*IQR or Q3 + multiplier*IQR.

```gdscript
var outliers = StatOps.outliers_iqr(data, 1.5)  # Standard IQR method
```

## Histograms

Visualize distributions.

#### `histogram(data: PackedFloat32Array, num_bins: int) -> PackedInt32Array`
Count values in each bin.

```gdscript
var hist = StatOps.histogram(scores, 5)  # 5 bins
# Returns count in each bin
```

#### `bin_edges(min: float, max: float, num_bins: int) -> PackedFloat32Array`
Get the edges of histogram bins.

```gdscript
var mm = StatOps.min_max(scores)
var edges = StatOps.bin_edges(mm.x, mm.y, 5)  # [min, ..., max]
```

## Correlation

Measure relationships between variables.

#### `correlation(x: PackedFloat32Array, y: PackedFloat32Array) -> float`
Pearson correlation coefficient (-1 to +1).

```gdscript
var r = StatOps.correlation(play_time, skill_rating)
# +1: Strong positive (more time = higher skill)
# -1: Strong negative (inverse relationship)
#  0: No relationship
```

## Example: Player Analytics

```gdscript
func analyze_session_data(sessions: PackedFloat32Array):
    print("Sessions analyzed: ", sessions.size())
    print("Mean duration: ", StatOps.mean(sessions), " min")
    print("Median duration: ", StatOps.median(sessions), " min")
    print("Std dev: ", StatOps.std_dev(sessions), " min")

    # Find unusual sessions (potential bots or AFK)
    var unusual = StatOps.outliers_zscore(sessions, 2.0)
    if unusual.size() > 0:
        print("Flagged sessions: ", unusual.size())

    # Player segmentation
    var casual_threshold = StatOps.percentile(sessions, 25)
    var hardcore_threshold = StatOps.percentile(sessions, 75)
    print("Casual: < ", casual_threshold, " min")
    print("Hardcore: > ", hardcore_threshold, " min")

func analyze_score_vs_time(scores: PackedFloat32Array, times: PackedFloat32Array):
    var r = StatOps.correlation(times, scores)
    if r > 0.7:
        print("Strong positive: play time correlates with score")
    elif r < -0.7:
        print("Strong negative: inverse relationship")
    else:
        print("Weak correlation: ", r)

func show_score_distribution(scores: PackedFloat32Array):
    var hist = StatOps.histogram(scores, 10)
    var mm = StatOps.min_max(scores)
    var edges = StatOps.bin_edges(mm.x, mm.y, 10)

    for i in range(hist.size()):
        var bar = "*".repeat(hist[i])
        print("[%.0f-%.0f]: %s" % [edges[i], edges[i+1], bar])
```

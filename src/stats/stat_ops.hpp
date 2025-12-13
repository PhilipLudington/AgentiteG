/**
 * StatOps - Statistical operations for data analysis
 *
 * High-performance statistical computations on arrays.
 *
 * Ideal for:
 * - Analytics and metrics
 * - AI behavior analysis
 * - Game balance tuning
 * - Performance profiling
 *
 * Usage:
 *   var health_values = PackedFloat32Array([...])
 *   var avg = StatOps.mean(health_values)
 *   var mid = StatOps.median(health_values)
 *   var outliers = StatOps.outliers_iqr(health_values, 1.5)
 */

#ifndef AGENTITE_STAT_OPS_HPP
#define AGENTITE_STAT_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/vector2.hpp>

namespace godot {

class StatOps : public RefCounted {
    GDCLASS(StatOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // === Descriptive Statistics ===

    // Central tendency
    static float mean(const PackedFloat32Array& values);
    static float median(const PackedFloat32Array& values);
    static float mode(const PackedFloat32Array& values, float bin_size);

    // Spread
    static float variance(const PackedFloat32Array& values);
    static float std_dev(const PackedFloat32Array& values);

    // Percentiles
    static float percentile(const PackedFloat32Array& values, float p);  // p in 0-100
    static PackedFloat32Array percentiles(const PackedFloat32Array& values,
                                           const PackedFloat32Array& ps);  // Multiple percentiles

    // Quartiles (convenience methods)
    static float q1(const PackedFloat32Array& values);  // 25th percentile
    static float q2(const PackedFloat32Array& values);  // 50th (median)
    static float q3(const PackedFloat32Array& values);  // 75th percentile

    // === Range Statistics ===

    // Min and max
    static float min_value(const PackedFloat32Array& values);
    static float max_value(const PackedFloat32Array& values);
    static Vector2 min_max(const PackedFloat32Array& values);  // Returns (min, max)

    // Range
    static float range(const PackedFloat32Array& values);      // max - min
    static float iqr(const PackedFloat32Array& values);        // Interquartile range (Q3 - Q1)

    // Sum and product
    static float sum(const PackedFloat32Array& values);
    static float product(const PackedFloat32Array& values);

    // === Histograms ===

    // Count values in bins
    static PackedInt32Array histogram(const PackedFloat32Array& values, int32_t bin_count);
    static PackedInt32Array histogram_range(const PackedFloat32Array& values,
                                             float min_val, float max_val, int32_t bin_count);

    // Get bin edges for a histogram
    static PackedFloat32Array bin_edges(float min_val, float max_val, int32_t bin_count);

    // === Correlation ===

    // Pearson correlation coefficient
    static float correlation(const PackedFloat32Array& x, const PackedFloat32Array& y);

    // Covariance
    static float covariance(const PackedFloat32Array& x, const PackedFloat32Array& y);

    // === Normalization ===

    // Min-max normalization (scales to [0, 1])
    static PackedFloat32Array normalize_min_max(const PackedFloat32Array& values);

    // Z-score normalization (mean=0, std=1)
    static PackedFloat32Array normalize_z_score(const PackedFloat32Array& values);

    // === Running Statistics ===

    // Moving window operations
    static PackedFloat32Array running_mean(const PackedFloat32Array& values, int32_t window);
    static PackedFloat32Array running_max(const PackedFloat32Array& values, int32_t window);
    static PackedFloat32Array running_min(const PackedFloat32Array& values, int32_t window);
    static PackedFloat32Array running_sum(const PackedFloat32Array& values, int32_t window);
    static PackedFloat32Array running_std(const PackedFloat32Array& values, int32_t window);

    // Exponential moving average
    static PackedFloat32Array exponential_moving_average(const PackedFloat32Array& values, float alpha);

    // Cumulative sum
    static PackedFloat32Array cumsum(const PackedFloat32Array& values);

    // === Outlier Detection ===

    // IQR method: values outside [Q1 - k*IQR, Q3 + k*IQR]
    static PackedInt32Array outliers_iqr(const PackedFloat32Array& values, float k);

    // Z-score method: values with |z| > threshold
    static PackedInt32Array outliers_zscore(const PackedFloat32Array& values, float threshold);

    // Modified Z-score (more robust to outliers)
    static PackedInt32Array outliers_modified_zscore(const PackedFloat32Array& values, float threshold);

    // === Utility ===

    // Count values matching a condition
    static int32_t count_equal(const PackedFloat32Array& values, float target);
    static int32_t count_greater(const PackedFloat32Array& values, float threshold);
    static int32_t count_less(const PackedFloat32Array& values, float threshold);
    static int32_t count_in_range(const PackedFloat32Array& values, float min_val, float max_val);

    // Unique values
    static PackedFloat32Array unique(const PackedFloat32Array& values);
    static int32_t count_unique(const PackedFloat32Array& values);
};

}

#endif // AGENTITE_STAT_OPS_HPP

/**
 * StatOps Implementation
 */

#include "stat_ops.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <algorithm>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <limits>
#include <deque>
#include <set>

namespace godot {

void StatOps::_bind_methods() {
    // Descriptive statistics
    ClassDB::bind_static_method("StatOps", D_METHOD("mean", "values"), &StatOps::mean);
    ClassDB::bind_static_method("StatOps", D_METHOD("median", "values"), &StatOps::median);
    ClassDB::bind_static_method("StatOps", D_METHOD("mode", "values", "bin_size"), &StatOps::mode);
    ClassDB::bind_static_method("StatOps", D_METHOD("variance", "values"), &StatOps::variance);
    ClassDB::bind_static_method("StatOps", D_METHOD("std_dev", "values"), &StatOps::std_dev);
    ClassDB::bind_static_method("StatOps", D_METHOD("percentile", "values", "p"), &StatOps::percentile);
    ClassDB::bind_static_method("StatOps", D_METHOD("percentiles", "values", "ps"), &StatOps::percentiles);
    ClassDB::bind_static_method("StatOps", D_METHOD("q1", "values"), &StatOps::q1);
    ClassDB::bind_static_method("StatOps", D_METHOD("q2", "values"), &StatOps::q2);
    ClassDB::bind_static_method("StatOps", D_METHOD("q3", "values"), &StatOps::q3);

    // Range statistics
    ClassDB::bind_static_method("StatOps", D_METHOD("min_value", "values"), &StatOps::min_value);
    ClassDB::bind_static_method("StatOps", D_METHOD("max_value", "values"), &StatOps::max_value);
    ClassDB::bind_static_method("StatOps", D_METHOD("min_max", "values"), &StatOps::min_max);
    ClassDB::bind_static_method("StatOps", D_METHOD("range", "values"), &StatOps::range);
    ClassDB::bind_static_method("StatOps", D_METHOD("iqr", "values"), &StatOps::iqr);
    ClassDB::bind_static_method("StatOps", D_METHOD("sum", "values"), &StatOps::sum);
    ClassDB::bind_static_method("StatOps", D_METHOD("product", "values"), &StatOps::product);

    // Histograms
    ClassDB::bind_static_method("StatOps", D_METHOD("histogram", "values", "bin_count"), &StatOps::histogram);
    ClassDB::bind_static_method("StatOps", D_METHOD("histogram_range", "values", "min_val", "max_val", "bin_count"), &StatOps::histogram_range);
    ClassDB::bind_static_method("StatOps", D_METHOD("bin_edges", "min_val", "max_val", "bin_count"), &StatOps::bin_edges);

    // Correlation
    ClassDB::bind_static_method("StatOps", D_METHOD("correlation", "x", "y"), &StatOps::correlation);
    ClassDB::bind_static_method("StatOps", D_METHOD("covariance", "x", "y"), &StatOps::covariance);

    // Normalization
    ClassDB::bind_static_method("StatOps", D_METHOD("normalize_min_max", "values"), &StatOps::normalize_min_max);
    ClassDB::bind_static_method("StatOps", D_METHOD("normalize_z_score", "values"), &StatOps::normalize_z_score);

    // Running statistics
    ClassDB::bind_static_method("StatOps", D_METHOD("running_mean", "values", "window"), &StatOps::running_mean);
    ClassDB::bind_static_method("StatOps", D_METHOD("running_max", "values", "window"), &StatOps::running_max);
    ClassDB::bind_static_method("StatOps", D_METHOD("running_min", "values", "window"), &StatOps::running_min);
    ClassDB::bind_static_method("StatOps", D_METHOD("running_sum", "values", "window"), &StatOps::running_sum);
    ClassDB::bind_static_method("StatOps", D_METHOD("running_std", "values", "window"), &StatOps::running_std);
    ClassDB::bind_static_method("StatOps", D_METHOD("exponential_moving_average", "values", "alpha"), &StatOps::exponential_moving_average);
    ClassDB::bind_static_method("StatOps", D_METHOD("cumsum", "values"), &StatOps::cumsum);

    // Outlier detection
    ClassDB::bind_static_method("StatOps", D_METHOD("outliers_iqr", "values", "k"), &StatOps::outliers_iqr);
    ClassDB::bind_static_method("StatOps", D_METHOD("outliers_zscore", "values", "threshold"), &StatOps::outliers_zscore);
    ClassDB::bind_static_method("StatOps", D_METHOD("outliers_modified_zscore", "values", "threshold"), &StatOps::outliers_modified_zscore);

    // Utility
    ClassDB::bind_static_method("StatOps", D_METHOD("count_equal", "values", "target"), &StatOps::count_equal);
    ClassDB::bind_static_method("StatOps", D_METHOD("count_greater", "values", "threshold"), &StatOps::count_greater);
    ClassDB::bind_static_method("StatOps", D_METHOD("count_less", "values", "threshold"), &StatOps::count_less);
    ClassDB::bind_static_method("StatOps", D_METHOD("count_in_range", "values", "min_val", "max_val"), &StatOps::count_in_range);
    ClassDB::bind_static_method("StatOps", D_METHOD("unique", "values"), &StatOps::unique);
    ClassDB::bind_static_method("StatOps", D_METHOD("count_unique", "values"), &StatOps::count_unique);
}

// === Descriptive Statistics ===

float StatOps::mean(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;

    const float* v = values.ptr();
    double sum = 0.0;
    for (int32_t i = 0; i < n; i++) {
        sum += v[i];
    }

    return static_cast<float>(sum / n);
}

float StatOps::median(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;

    std::vector<float> sorted(values.ptr(), values.ptr() + n);
    std::sort(sorted.begin(), sorted.end());

    if (n % 2 == 0) {
        return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0f;
    } else {
        return sorted[n / 2];
    }
}

float StatOps::mode(const PackedFloat32Array& values, float bin_size) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;
    if (bin_size <= 0.0f) bin_size = 1.0f;

    const float* v = values.ptr();
    std::unordered_map<int64_t, int32_t> bins;

    for (int32_t i = 0; i < n; i++) {
        int64_t bin = static_cast<int64_t>(std::floor(v[i] / bin_size));
        bins[bin]++;
    }

    int64_t mode_bin = 0;
    int32_t mode_count = 0;

    for (const auto& pair : bins) {
        if (pair.second > mode_count) {
            mode_count = pair.second;
            mode_bin = pair.first;
        }
    }

    return (mode_bin + 0.5f) * bin_size;
}

float StatOps::variance(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n < 2) return 0.0f;

    const float* v = values.ptr();

    // Two-pass algorithm for numerical stability
    double m = mean(values);
    double sum_sq = 0.0;

    for (int32_t i = 0; i < n; i++) {
        double diff = v[i] - m;
        sum_sq += diff * diff;
    }

    return static_cast<float>(sum_sq / (n - 1));  // Sample variance (Bessel's correction)
}

float StatOps::std_dev(const PackedFloat32Array& values) {
    return std::sqrt(variance(values));
}

float StatOps::percentile(const PackedFloat32Array& values, float p) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;

    // Clamp p to [0, 100]
    if (p < 0.0f) p = 0.0f;
    if (p > 100.0f) p = 100.0f;

    std::vector<float> sorted(values.ptr(), values.ptr() + n);
    std::sort(sorted.begin(), sorted.end());

    // Linear interpolation method
    float rank = (p / 100.0f) * (n - 1);
    int32_t lower = static_cast<int32_t>(std::floor(rank));
    int32_t upper = static_cast<int32_t>(std::ceil(rank));

    if (lower == upper) {
        return sorted[lower];
    }

    float frac = rank - lower;
    return sorted[lower] * (1.0f - frac) + sorted[upper] * frac;
}

PackedFloat32Array StatOps::percentiles(const PackedFloat32Array& values,
                                         const PackedFloat32Array& ps) {
    PackedFloat32Array result;
    int32_t n = values.size();
    int32_t np = ps.size();

    if (n == 0 || np == 0) return result;

    result.resize(np);
    float* r_ptr = result.ptrw();

    std::vector<float> sorted(values.ptr(), values.ptr() + n);
    std::sort(sorted.begin(), sorted.end());

    const float* p_ptr = ps.ptr();

    for (int32_t i = 0; i < np; i++) {
        float p = p_ptr[i];
        if (p < 0.0f) p = 0.0f;
        if (p > 100.0f) p = 100.0f;

        float rank = (p / 100.0f) * (n - 1);
        int32_t lower = static_cast<int32_t>(std::floor(rank));
        int32_t upper = static_cast<int32_t>(std::ceil(rank));

        if (lower == upper) {
            r_ptr[i] = sorted[lower];
        } else {
            float frac = rank - lower;
            r_ptr[i] = sorted[lower] * (1.0f - frac) + sorted[upper] * frac;
        }
    }

    return result;
}

float StatOps::q1(const PackedFloat32Array& values) {
    return percentile(values, 25.0f);
}

float StatOps::q2(const PackedFloat32Array& values) {
    return percentile(values, 50.0f);
}

float StatOps::q3(const PackedFloat32Array& values) {
    return percentile(values, 75.0f);
}

// === Range Statistics ===

float StatOps::min_value(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;

    const float* v = values.ptr();
    float min_v = v[0];

    for (int32_t i = 1; i < n; i++) {
        if (v[i] < min_v) min_v = v[i];
    }

    return min_v;
}

float StatOps::max_value(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;

    const float* v = values.ptr();
    float max_v = v[0];

    for (int32_t i = 1; i < n; i++) {
        if (v[i] > max_v) max_v = v[i];
    }

    return max_v;
}

Vector2 StatOps::min_max(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return Vector2(0, 0);

    const float* v = values.ptr();
    float min_v = v[0];
    float max_v = v[0];

    for (int32_t i = 1; i < n; i++) {
        if (v[i] < min_v) min_v = v[i];
        if (v[i] > max_v) max_v = v[i];
    }

    return Vector2(min_v, max_v);
}

float StatOps::range(const PackedFloat32Array& values) {
    Vector2 mm = min_max(values);
    return mm.y - mm.x;
}

float StatOps::iqr(const PackedFloat32Array& values) {
    return q3(values) - q1(values);
}

float StatOps::sum(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;

    const float* v = values.ptr();
    double total = 0.0;

    for (int32_t i = 0; i < n; i++) {
        total += v[i];
    }

    return static_cast<float>(total);
}

float StatOps::product(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return 0.0f;

    const float* v = values.ptr();
    double prod = 1.0;

    for (int32_t i = 0; i < n; i++) {
        prod *= v[i];
    }

    return static_cast<float>(prod);
}

// === Histograms ===

PackedInt32Array StatOps::histogram(const PackedFloat32Array& values, int32_t bin_count) {
    Vector2 mm = min_max(values);
    return histogram_range(values, mm.x, mm.y, bin_count);
}

PackedInt32Array StatOps::histogram_range(const PackedFloat32Array& values,
                                           float min_val, float max_val, int32_t bin_count) {
    PackedInt32Array result;

    if (bin_count < 1) return result;

    result.resize(bin_count);
    int32_t* r_ptr = result.ptrw();

    for (int32_t i = 0; i < bin_count; i++) {
        r_ptr[i] = 0;
    }

    int32_t n = values.size();
    if (n == 0) return result;

    const float* v = values.ptr();
    float range_val = max_val - min_val;

    if (range_val <= 0.0f) {
        // All values go in first bin
        r_ptr[0] = n;
        return result;
    }

    for (int32_t i = 0; i < n; i++) {
        float normalized = (v[i] - min_val) / range_val;
        int32_t bin = static_cast<int32_t>(normalized * bin_count);

        // Clamp to valid range (handles edge cases)
        if (bin < 0) bin = 0;
        if (bin >= bin_count) bin = bin_count - 1;

        r_ptr[bin]++;
    }

    return result;
}

PackedFloat32Array StatOps::bin_edges(float min_val, float max_val, int32_t bin_count) {
    PackedFloat32Array result;

    if (bin_count < 1) return result;

    result.resize(bin_count + 1);
    float* r_ptr = result.ptrw();

    float step = (max_val - min_val) / bin_count;

    for (int32_t i = 0; i <= bin_count; i++) {
        r_ptr[i] = min_val + step * i;
    }

    return result;
}

// === Correlation ===

float StatOps::correlation(const PackedFloat32Array& x, const PackedFloat32Array& y) {
    int32_t n = x.size();
    if (n != y.size() || n < 2) return 0.0f;

    const float* x_ptr = x.ptr();
    const float* y_ptr = y.ptr();

    // Calculate means
    double x_mean = 0.0, y_mean = 0.0;
    for (int32_t i = 0; i < n; i++) {
        x_mean += x_ptr[i];
        y_mean += y_ptr[i];
    }
    x_mean /= n;
    y_mean /= n;

    // Calculate covariance and standard deviations
    double cov = 0.0, x_var = 0.0, y_var = 0.0;
    for (int32_t i = 0; i < n; i++) {
        double dx = x_ptr[i] - x_mean;
        double dy = y_ptr[i] - y_mean;
        cov += dx * dy;
        x_var += dx * dx;
        y_var += dy * dy;
    }

    double denom = std::sqrt(x_var * y_var);
    if (denom < 1e-10) return 0.0f;

    return static_cast<float>(cov / denom);
}

float StatOps::covariance(const PackedFloat32Array& x, const PackedFloat32Array& y) {
    int32_t n = x.size();
    if (n != y.size() || n < 2) return 0.0f;

    const float* x_ptr = x.ptr();
    const float* y_ptr = y.ptr();

    double x_mean = 0.0, y_mean = 0.0;
    for (int32_t i = 0; i < n; i++) {
        x_mean += x_ptr[i];
        y_mean += y_ptr[i];
    }
    x_mean /= n;
    y_mean /= n;

    double cov = 0.0;
    for (int32_t i = 0; i < n; i++) {
        cov += (x_ptr[i] - x_mean) * (y_ptr[i] - y_mean);
    }

    return static_cast<float>(cov / (n - 1));  // Sample covariance
}

// === Normalization ===

PackedFloat32Array StatOps::normalize_min_max(const PackedFloat32Array& values) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0) return result;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    Vector2 mm = min_max(values);
    float range_val = mm.y - mm.x;

    if (range_val < 1e-10f) {
        for (int32_t i = 0; i < n; i++) {
            r_ptr[i] = 0.5f;
        }
    } else {
        for (int32_t i = 0; i < n; i++) {
            r_ptr[i] = (v[i] - mm.x) / range_val;
        }
    }

    return result;
}

PackedFloat32Array StatOps::normalize_z_score(const PackedFloat32Array& values) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0) return result;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    float m = mean(values);
    float s = std_dev(values);

    if (s < 1e-10f) {
        for (int32_t i = 0; i < n; i++) {
            r_ptr[i] = 0.0f;
        }
    } else {
        for (int32_t i = 0; i < n; i++) {
            r_ptr[i] = (v[i] - m) / s;
        }
    }

    return result;
}

// === Running Statistics ===

PackedFloat32Array StatOps::running_mean(const PackedFloat32Array& values, int32_t window) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0 || window < 1) return result;
    if (window > n) window = n;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    // Initial sum for first window
    double sum = 0.0;
    for (int32_t i = 0; i < window; i++) {
        sum += v[i];
    }

    // First window - fewer elements
    for (int32_t i = 0; i < window; i++) {
        double local_sum = 0.0;
        int32_t count = 0;
        for (int32_t j = std::max(0, i - window + 1); j <= i; j++) {
            local_sum += v[j];
            count++;
        }
        r_ptr[i] = static_cast<float>(local_sum / count);
    }

    // Sliding window
    for (int32_t i = window; i < n; i++) {
        sum = sum - v[i - window] + v[i];
        r_ptr[i] = static_cast<float>(sum / window);
    }

    return result;
}

PackedFloat32Array StatOps::running_max(const PackedFloat32Array& values, int32_t window) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0 || window < 1) return result;
    if (window > n) window = n;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    std::deque<int32_t> deq;  // Stores indices of potential max values

    for (int32_t i = 0; i < n; i++) {
        // Remove elements outside window
        while (!deq.empty() && deq.front() <= i - window) {
            deq.pop_front();
        }

        // Remove elements smaller than current (they can't be max)
        while (!deq.empty() && v[deq.back()] <= v[i]) {
            deq.pop_back();
        }

        deq.push_back(i);
        r_ptr[i] = v[deq.front()];
    }

    return result;
}

PackedFloat32Array StatOps::running_min(const PackedFloat32Array& values, int32_t window) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0 || window < 1) return result;
    if (window > n) window = n;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    std::deque<int32_t> deq;

    for (int32_t i = 0; i < n; i++) {
        while (!deq.empty() && deq.front() <= i - window) {
            deq.pop_front();
        }

        while (!deq.empty() && v[deq.back()] >= v[i]) {
            deq.pop_back();
        }

        deq.push_back(i);
        r_ptr[i] = v[deq.front()];
    }

    return result;
}

PackedFloat32Array StatOps::running_sum(const PackedFloat32Array& values, int32_t window) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0 || window < 1) return result;
    if (window > n) window = n;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    double sum = 0.0;

    for (int32_t i = 0; i < n; i++) {
        sum += v[i];
        if (i >= window) {
            sum -= v[i - window];
        }
        r_ptr[i] = static_cast<float>(sum);
    }

    return result;
}

PackedFloat32Array StatOps::running_std(const PackedFloat32Array& values, int32_t window) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0 || window < 2) return result;
    if (window > n) window = n;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    // Use Welford's online algorithm for numerical stability
    for (int32_t i = 0; i < n; i++) {
        int32_t start = std::max(0, i - window + 1);
        int32_t count = i - start + 1;

        if (count < 2) {
            r_ptr[i] = 0.0f;
            continue;
        }

        double m = 0.0, m2 = 0.0;
        for (int32_t j = start; j <= i; j++) {
            double delta = v[j] - m;
            m += delta / (j - start + 1);
            m2 += delta * (v[j] - m);
        }

        r_ptr[i] = static_cast<float>(std::sqrt(m2 / (count - 1)));
    }

    return result;
}

PackedFloat32Array StatOps::exponential_moving_average(const PackedFloat32Array& values, float alpha) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0) return result;

    // Clamp alpha to [0, 1]
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    r_ptr[0] = v[0];
    float inv_alpha = 1.0f - alpha;

    for (int32_t i = 1; i < n; i++) {
        r_ptr[i] = alpha * v[i] + inv_alpha * r_ptr[i - 1];
    }

    return result;
}

PackedFloat32Array StatOps::cumsum(const PackedFloat32Array& values) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0) return result;

    result.resize(n);
    float* r_ptr = result.ptrw();
    const float* v = values.ptr();

    double sum = 0.0;
    for (int32_t i = 0; i < n; i++) {
        sum += v[i];
        r_ptr[i] = static_cast<float>(sum);
    }

    return result;
}

// === Outlier Detection ===

PackedInt32Array StatOps::outliers_iqr(const PackedFloat32Array& values, float k) {
    PackedInt32Array result;
    int32_t n = values.size();
    if (n < 4) return result;

    float q1_val = q1(values);
    float q3_val = q3(values);
    float iqr_val = q3_val - q1_val;

    float lower_bound = q1_val - k * iqr_val;
    float upper_bound = q3_val + k * iqr_val;

    const float* v = values.ptr();

    for (int32_t i = 0; i < n; i++) {
        if (v[i] < lower_bound || v[i] > upper_bound) {
            result.push_back(i);
        }
    }

    return result;
}

PackedInt32Array StatOps::outliers_zscore(const PackedFloat32Array& values, float threshold) {
    PackedInt32Array result;
    int32_t n = values.size();
    if (n < 2) return result;

    float m = mean(values);
    float s = std_dev(values);

    if (s < 1e-10f) return result;

    const float* v = values.ptr();

    for (int32_t i = 0; i < n; i++) {
        float z = std::abs((v[i] - m) / s);
        if (z > threshold) {
            result.push_back(i);
        }
    }

    return result;
}

PackedInt32Array StatOps::outliers_modified_zscore(const PackedFloat32Array& values, float threshold) {
    PackedInt32Array result;
    int32_t n = values.size();
    if (n < 2) return result;

    // Use median and MAD (Median Absolute Deviation) for robustness
    float med = median(values);

    // Calculate MAD
    PackedFloat32Array deviations;
    deviations.resize(n);
    float* d_ptr = deviations.ptrw();
    const float* v = values.ptr();

    for (int32_t i = 0; i < n; i++) {
        d_ptr[i] = std::abs(v[i] - med);
    }

    float mad = median(deviations);

    if (mad < 1e-10f) return result;

    // Modified z-score uses 0.6745 as consistency constant
    float scale = 0.6745f / mad;

    for (int32_t i = 0; i < n; i++) {
        float modified_z = std::abs((v[i] - med) * scale);
        if (modified_z > threshold) {
            result.push_back(i);
        }
    }

    return result;
}

// === Utility ===

int32_t StatOps::count_equal(const PackedFloat32Array& values, float target) {
    int32_t n = values.size();
    const float* v = values.ptr();
    int32_t count = 0;

    for (int32_t i = 0; i < n; i++) {
        if (std::abs(v[i] - target) < 1e-10f) {
            count++;
        }
    }

    return count;
}

int32_t StatOps::count_greater(const PackedFloat32Array& values, float threshold) {
    int32_t n = values.size();
    const float* v = values.ptr();
    int32_t count = 0;

    for (int32_t i = 0; i < n; i++) {
        if (v[i] > threshold) {
            count++;
        }
    }

    return count;
}

int32_t StatOps::count_less(const PackedFloat32Array& values, float threshold) {
    int32_t n = values.size();
    const float* v = values.ptr();
    int32_t count = 0;

    for (int32_t i = 0; i < n; i++) {
        if (v[i] < threshold) {
            count++;
        }
    }

    return count;
}

int32_t StatOps::count_in_range(const PackedFloat32Array& values, float min_val, float max_val) {
    int32_t n = values.size();
    const float* v = values.ptr();
    int32_t count = 0;

    for (int32_t i = 0; i < n; i++) {
        if (v[i] >= min_val && v[i] <= max_val) {
            count++;
        }
    }

    return count;
}

PackedFloat32Array StatOps::unique(const PackedFloat32Array& values) {
    PackedFloat32Array result;
    int32_t n = values.size();
    if (n == 0) return result;

    std::set<float> seen;
    const float* v = values.ptr();

    for (int32_t i = 0; i < n; i++) {
        if (seen.insert(v[i]).second) {
            result.push_back(v[i]);
        }
    }

    return result;
}

int32_t StatOps::count_unique(const PackedFloat32Array& values) {
    int32_t n = values.size();
    if (n == 0) return 0;

    std::set<float> seen;
    const float* v = values.ptr();

    for (int32_t i = 0; i < n; i++) {
        seen.insert(v[i]);
    }

    return static_cast<int32_t>(seen.size());
}

}

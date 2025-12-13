/**
 * InterpolationOps Implementation
 */

#include "interpolation_ops.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <cmath>

namespace godot {

// Constants for easing
static const float PI = 3.14159265358979323846f;
static const float HALF_PI = PI / 2.0f;
static const float BACK_C1 = 1.70158f;
static const float BACK_C2 = BACK_C1 * 1.525f;
static const float BACK_C3 = BACK_C1 + 1.0f;

void InterpolationOps::_bind_methods() {
    // Quadratic
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_quad", "t"), &InterpolationOps::ease_in_quad);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_quad", "t"), &InterpolationOps::ease_out_quad);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_quad", "t"), &InterpolationOps::ease_in_out_quad);

    // Cubic
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_cubic", "t"), &InterpolationOps::ease_in_cubic);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_cubic", "t"), &InterpolationOps::ease_out_cubic);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_cubic", "t"), &InterpolationOps::ease_in_out_cubic);

    // Quartic
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_quart", "t"), &InterpolationOps::ease_in_quart);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_quart", "t"), &InterpolationOps::ease_out_quart);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_quart", "t"), &InterpolationOps::ease_in_out_quart);

    // Quintic
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_quint", "t"), &InterpolationOps::ease_in_quint);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_quint", "t"), &InterpolationOps::ease_out_quint);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_quint", "t"), &InterpolationOps::ease_in_out_quint);

    // Sine
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_sine", "t"), &InterpolationOps::ease_in_sine);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_sine", "t"), &InterpolationOps::ease_out_sine);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_sine", "t"), &InterpolationOps::ease_in_out_sine);

    // Exponential
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_expo", "t"), &InterpolationOps::ease_in_expo);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_expo", "t"), &InterpolationOps::ease_out_expo);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_expo", "t"), &InterpolationOps::ease_in_out_expo);

    // Circular
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_circ", "t"), &InterpolationOps::ease_in_circ);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_circ", "t"), &InterpolationOps::ease_out_circ);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_circ", "t"), &InterpolationOps::ease_in_out_circ);

    // Back
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_back", "t"), &InterpolationOps::ease_in_back);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_back", "t"), &InterpolationOps::ease_out_back);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_back", "t"), &InterpolationOps::ease_in_out_back);

    // Elastic
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_elastic", "t"), &InterpolationOps::ease_in_elastic);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_elastic", "t"), &InterpolationOps::ease_out_elastic);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_elastic", "t"), &InterpolationOps::ease_in_out_elastic);

    // Bounce
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_bounce", "t"), &InterpolationOps::ease_in_bounce);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_out_bounce", "t"), &InterpolationOps::ease_out_bounce);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("ease_in_out_bounce", "t"), &InterpolationOps::ease_in_out_bounce);

    // Bezier
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("bezier_quadratic_2d", "p0", "p1", "p2", "t"), &InterpolationOps::bezier_quadratic_2d);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("bezier_quadratic_3d", "p0", "p1", "p2", "t"), &InterpolationOps::bezier_quadratic_3d);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("bezier_cubic_2d", "p0", "p1", "p2", "p3", "t"), &InterpolationOps::bezier_cubic_2d);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("bezier_cubic_3d", "p0", "p1", "p2", "p3", "t"), &InterpolationOps::bezier_cubic_3d);

    // Splines
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("catmull_rom_2d", "control_points", "samples_per_segment"), &InterpolationOps::catmull_rom_2d);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("catmull_rom_3d", "control_points", "samples_per_segment"), &InterpolationOps::catmull_rom_3d);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("bspline_2d", "control_points", "degree", "total_samples"), &InterpolationOps::bspline_2d);

    // Utility
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("remap", "values", "in_min", "in_max", "out_min", "out_max"), &InterpolationOps::remap);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("clamp_array", "values", "min_val", "max_val"), &InterpolationOps::clamp_array);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("smoothstep", "t"), &InterpolationOps::smoothstep);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("smootherstep", "t"), &InterpolationOps::smootherstep);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("lerp_floats", "a", "b", "t"), &InterpolationOps::lerp_floats);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("lerp_floats_array", "a", "b", "t"), &InterpolationOps::lerp_floats_array);
    ClassDB::bind_static_method("InterpolationOps", D_METHOD("linspace", "start", "end", "count"), &InterpolationOps::linspace);
}

// === Quadratic ===

PackedFloat32Array InterpolationOps::ease_in_quad(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = t_ptr[i] * t_ptr[i];
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_quad(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = 1.0f - t_ptr[i];
        r_ptr[i] = 1.0f - x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_quad(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x < 0.5f) {
            r_ptr[i] = 2.0f * x * x;
        } else {
            float f = -2.0f * x + 2.0f;
            r_ptr[i] = 1.0f - f * f / 2.0f;
        }
    }

    return result;
}

// === Cubic ===

PackedFloat32Array InterpolationOps::ease_in_cubic(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = t_ptr[i] * t_ptr[i] * t_ptr[i];
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_cubic(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = 1.0f - t_ptr[i];
        r_ptr[i] = 1.0f - x * x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_cubic(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x < 0.5f) {
            r_ptr[i] = 4.0f * x * x * x;
        } else {
            float f = -2.0f * x + 2.0f;
            r_ptr[i] = 1.0f - f * f * f / 2.0f;
        }
    }

    return result;
}

// === Quartic ===

PackedFloat32Array InterpolationOps::ease_in_quart(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        r_ptr[i] = x * x * x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_quart(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = 1.0f - t_ptr[i];
        r_ptr[i] = 1.0f - x * x * x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_quart(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x < 0.5f) {
            r_ptr[i] = 8.0f * x * x * x * x;
        } else {
            float f = -2.0f * x + 2.0f;
            r_ptr[i] = 1.0f - f * f * f * f / 2.0f;
        }
    }

    return result;
}

// === Quintic ===

PackedFloat32Array InterpolationOps::ease_in_quint(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        r_ptr[i] = x * x * x * x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_quint(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = 1.0f - t_ptr[i];
        r_ptr[i] = 1.0f - x * x * x * x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_quint(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x < 0.5f) {
            r_ptr[i] = 16.0f * x * x * x * x * x;
        } else {
            float f = -2.0f * x + 2.0f;
            r_ptr[i] = 1.0f - f * f * f * f * f / 2.0f;
        }
    }

    return result;
}

// === Sine ===

PackedFloat32Array InterpolationOps::ease_in_sine(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = 1.0f - std::cos(t_ptr[i] * HALF_PI);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_sine(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = std::sin(t_ptr[i] * HALF_PI);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_sine(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = -(std::cos(PI * t_ptr[i]) - 1.0f) / 2.0f;
    }

    return result;
}

// === Exponential ===

PackedFloat32Array InterpolationOps::ease_in_expo(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = (t_ptr[i] == 0.0f) ? 0.0f : std::pow(2.0f, 10.0f * t_ptr[i] - 10.0f);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_expo(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = (t_ptr[i] == 1.0f) ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t_ptr[i]);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_expo(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x == 0.0f) {
            r_ptr[i] = 0.0f;
        } else if (x == 1.0f) {
            r_ptr[i] = 1.0f;
        } else if (x < 0.5f) {
            r_ptr[i] = std::pow(2.0f, 20.0f * x - 10.0f) / 2.0f;
        } else {
            r_ptr[i] = (2.0f - std::pow(2.0f, -20.0f * x + 10.0f)) / 2.0f;
        }
    }

    return result;
}

// === Circular ===

PackedFloat32Array InterpolationOps::ease_in_circ(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        r_ptr[i] = 1.0f - std::sqrt(1.0f - x * x);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_circ(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i] - 1.0f;
        r_ptr[i] = std::sqrt(1.0f - x * x);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_circ(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x < 0.5f) {
            r_ptr[i] = (1.0f - std::sqrt(1.0f - 4.0f * x * x)) / 2.0f;
        } else {
            float f = -2.0f * x + 2.0f;
            r_ptr[i] = (std::sqrt(1.0f - f * f) + 1.0f) / 2.0f;
        }
    }

    return result;
}

// === Back ===

PackedFloat32Array InterpolationOps::ease_in_back(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        r_ptr[i] = BACK_C3 * x * x * x - BACK_C1 * x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_back(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i] - 1.0f;
        r_ptr[i] = 1.0f + BACK_C3 * x * x * x + BACK_C1 * x * x;
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_back(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x < 0.5f) {
            float f = 2.0f * x;
            r_ptr[i] = (f * f * ((BACK_C2 + 1.0f) * f - BACK_C2)) / 2.0f;
        } else {
            float f = 2.0f * x - 2.0f;
            r_ptr[i] = (f * f * ((BACK_C2 + 1.0f) * f + BACK_C2) + 2.0f) / 2.0f;
        }
    }

    return result;
}

// === Elastic ===

PackedFloat32Array InterpolationOps::ease_in_elastic(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    const float c4 = (2.0f * PI) / 3.0f;

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x == 0.0f) {
            r_ptr[i] = 0.0f;
        } else if (x == 1.0f) {
            r_ptr[i] = 1.0f;
        } else {
            r_ptr[i] = -std::pow(2.0f, 10.0f * x - 10.0f) * std::sin((x * 10.0f - 10.75f) * c4);
        }
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_elastic(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    const float c4 = (2.0f * PI) / 3.0f;

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x == 0.0f) {
            r_ptr[i] = 0.0f;
        } else if (x == 1.0f) {
            r_ptr[i] = 1.0f;
        } else {
            r_ptr[i] = std::pow(2.0f, -10.0f * x) * std::sin((x * 10.0f - 0.75f) * c4) + 1.0f;
        }
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_elastic(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    const float c5 = (2.0f * PI) / 4.5f;

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x == 0.0f) {
            r_ptr[i] = 0.0f;
        } else if (x == 1.0f) {
            r_ptr[i] = 1.0f;
        } else if (x < 0.5f) {
            r_ptr[i] = -(std::pow(2.0f, 20.0f * x - 10.0f) * std::sin((20.0f * x - 11.125f) * c5)) / 2.0f;
        } else {
            r_ptr[i] = (std::pow(2.0f, -20.0f * x + 10.0f) * std::sin((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
        }
    }

    return result;
}

// === Bounce ===

// Helper function for bounce calculation
static float bounce_out_single(float x) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;

    if (x < 1.0f / d1) {
        return n1 * x * x;
    } else if (x < 2.0f / d1) {
        x -= 1.5f / d1;
        return n1 * x * x + 0.75f;
    } else if (x < 2.5f / d1) {
        x -= 2.25f / d1;
        return n1 * x * x + 0.9375f;
    } else {
        x -= 2.625f / d1;
        return n1 * x * x + 0.984375f;
    }
}

PackedFloat32Array InterpolationOps::ease_in_bounce(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = 1.0f - bounce_out_single(1.0f - t_ptr[i]);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_out_bounce(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = bounce_out_single(t_ptr[i]);
    }

    return result;
}

PackedFloat32Array InterpolationOps::ease_in_out_bounce(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        if (x < 0.5f) {
            r_ptr[i] = (1.0f - bounce_out_single(1.0f - 2.0f * x)) / 2.0f;
        } else {
            r_ptr[i] = (1.0f + bounce_out_single(2.0f * x - 1.0f)) / 2.0f;
        }
    }

    return result;
}

// === Bezier Curves ===

PackedVector2Array InterpolationOps::bezier_quadratic_2d(const Vector2& p0, const Vector2& p1,
                                                          const Vector2& p2, const PackedFloat32Array& t) {
    PackedVector2Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    Vector2* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float u = t_ptr[i];
        float u2 = u * u;
        float inv_u = 1.0f - u;
        float inv_u2 = inv_u * inv_u;

        r_ptr[i] = inv_u2 * p0 + 2.0f * inv_u * u * p1 + u2 * p2;
    }

    return result;
}

PackedVector3Array InterpolationOps::bezier_quadratic_3d(const Vector3& p0, const Vector3& p1,
                                                          const Vector3& p2, const PackedFloat32Array& t) {
    PackedVector3Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    Vector3* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float u = t_ptr[i];
        float u2 = u * u;
        float inv_u = 1.0f - u;
        float inv_u2 = inv_u * inv_u;

        r_ptr[i] = inv_u2 * p0 + 2.0f * inv_u * u * p1 + u2 * p2;
    }

    return result;
}

PackedVector2Array InterpolationOps::bezier_cubic_2d(const Vector2& p0, const Vector2& p1,
                                                      const Vector2& p2, const Vector2& p3,
                                                      const PackedFloat32Array& t) {
    PackedVector2Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    Vector2* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float u = t_ptr[i];
        float u2 = u * u;
        float u3 = u2 * u;
        float inv_u = 1.0f - u;
        float inv_u2 = inv_u * inv_u;
        float inv_u3 = inv_u2 * inv_u;

        r_ptr[i] = inv_u3 * p0 + 3.0f * inv_u2 * u * p1 + 3.0f * inv_u * u2 * p2 + u3 * p3;
    }

    return result;
}

PackedVector3Array InterpolationOps::bezier_cubic_3d(const Vector3& p0, const Vector3& p1,
                                                      const Vector3& p2, const Vector3& p3,
                                                      const PackedFloat32Array& t) {
    PackedVector3Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    Vector3* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float u = t_ptr[i];
        float u2 = u * u;
        float u3 = u2 * u;
        float inv_u = 1.0f - u;
        float inv_u2 = inv_u * inv_u;
        float inv_u3 = inv_u2 * inv_u;

        r_ptr[i] = inv_u3 * p0 + 3.0f * inv_u2 * u * p1 + 3.0f * inv_u * u2 * p2 + u3 * p3;
    }

    return result;
}

// === Splines ===

PackedVector2Array InterpolationOps::catmull_rom_2d(const PackedVector2Array& control_points,
                                                     int32_t samples_per_segment) {
    PackedVector2Array result;

    int32_t n = control_points.size();
    if (n < 4 || samples_per_segment < 1) {
        return result;
    }

    const Vector2* cp = control_points.ptr();

    // Number of segments is n-3 (need 4 points per segment)
    int32_t num_segments = n - 3;
    result.resize(num_segments * samples_per_segment + 1);
    Vector2* r_ptr = result.ptrw();

    int32_t out_idx = 0;

    for (int32_t seg = 0; seg < num_segments; seg++) {
        const Vector2& p0 = cp[seg];
        const Vector2& p1 = cp[seg + 1];
        const Vector2& p2 = cp[seg + 2];
        const Vector2& p3 = cp[seg + 3];

        for (int32_t s = 0; s < samples_per_segment; s++) {
            float t = static_cast<float>(s) / samples_per_segment;
            float t2 = t * t;
            float t3 = t2 * t;

            // Catmull-Rom basis matrix
            Vector2 point = 0.5f * (
                (2.0f * p1) +
                (-p0 + p2) * t +
                (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
            );

            r_ptr[out_idx++] = point;
        }
    }

    // Add final point
    r_ptr[out_idx] = cp[n - 2];

    return result;
}

PackedVector3Array InterpolationOps::catmull_rom_3d(const PackedVector3Array& control_points,
                                                     int32_t samples_per_segment) {
    PackedVector3Array result;

    int32_t n = control_points.size();
    if (n < 4 || samples_per_segment < 1) {
        return result;
    }

    const Vector3* cp = control_points.ptr();

    int32_t num_segments = n - 3;
    result.resize(num_segments * samples_per_segment + 1);
    Vector3* r_ptr = result.ptrw();

    int32_t out_idx = 0;

    for (int32_t seg = 0; seg < num_segments; seg++) {
        const Vector3& p0 = cp[seg];
        const Vector3& p1 = cp[seg + 1];
        const Vector3& p2 = cp[seg + 2];
        const Vector3& p3 = cp[seg + 3];

        for (int32_t s = 0; s < samples_per_segment; s++) {
            float t = static_cast<float>(s) / samples_per_segment;
            float t2 = t * t;
            float t3 = t2 * t;

            Vector3 point = 0.5f * (
                (2.0f * p1) +
                (-p0 + p2) * t +
                (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
            );

            r_ptr[out_idx++] = point;
        }
    }

    r_ptr[out_idx] = cp[n - 2];

    return result;
}

PackedVector2Array InterpolationOps::bspline_2d(const PackedVector2Array& control_points,
                                                 int32_t degree, int32_t total_samples) {
    PackedVector2Array result;

    int32_t n = control_points.size();
    if (n < degree + 1 || total_samples < 2) {
        return result;
    }

    // Clamp degree
    if (degree < 1) degree = 1;
    if (degree > n - 1) degree = n - 1;

    result.resize(total_samples);
    Vector2* r_ptr = result.ptrw();
    const Vector2* cp = control_points.ptr();

    // Create uniform knot vector
    int32_t num_knots = n + degree + 1;
    std::vector<float> knots(num_knots);

    for (int32_t i = 0; i < num_knots; i++) {
        if (i < degree + 1) {
            knots[i] = 0.0f;
        } else if (i >= n) {
            knots[i] = static_cast<float>(n - degree);
        } else {
            knots[i] = static_cast<float>(i - degree);
        }
    }

    float t_max = knots[n];

    for (int32_t s = 0; s < total_samples; s++) {
        float t = (static_cast<float>(s) / (total_samples - 1)) * t_max;

        // De Boor's algorithm
        // Find knot span
        int32_t k = degree;
        for (int32_t i = degree; i < n; i++) {
            if (t >= knots[i] && t < knots[i + 1]) {
                k = i;
                break;
            }
        }
        if (t >= t_max) k = n - 1;

        // Copy affected control points
        std::vector<Vector2> d(degree + 1);
        for (int32_t j = 0; j <= degree; j++) {
            d[j] = cp[j + k - degree];
        }

        // De Boor recursion
        for (int32_t r = 1; r <= degree; r++) {
            for (int32_t j = degree; j >= r; j--) {
                float alpha = (t - knots[j + k - degree]) /
                              (knots[j + 1 + k - r] - knots[j + k - degree]);
                d[j] = (1.0f - alpha) * d[j - 1] + alpha * d[j];
            }
        }

        r_ptr[s] = d[degree];
    }

    return result;
}

// === Utility Functions ===

PackedFloat32Array InterpolationOps::remap(const PackedFloat32Array& values,
                                            float in_min, float in_max,
                                            float out_min, float out_max) {
    PackedFloat32Array result;
    int32_t n = values.size();
    result.resize(n);

    const float* v_ptr = values.ptr();
    float* r_ptr = result.ptrw();

    float in_range = in_max - in_min;
    float out_range = out_max - out_min;

    if (std::abs(in_range) < 1e-10f) {
        // Avoid division by zero
        for (int32_t i = 0; i < n; i++) {
            r_ptr[i] = out_min;
        }
    } else {
        float scale = out_range / in_range;
        for (int32_t i = 0; i < n; i++) {
            r_ptr[i] = (v_ptr[i] - in_min) * scale + out_min;
        }
    }

    return result;
}

PackedFloat32Array InterpolationOps::clamp_array(const PackedFloat32Array& values,
                                                  float min_val, float max_val) {
    PackedFloat32Array result;
    int32_t n = values.size();
    result.resize(n);

    const float* v_ptr = values.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float v = v_ptr[i];
        if (v < min_val) v = min_val;
        else if (v > max_val) v = max_val;
        r_ptr[i] = v;
    }

    return result;
}

PackedFloat32Array InterpolationOps::smoothstep(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        // Clamp to [0, 1]
        if (x < 0.0f) x = 0.0f;
        else if (x > 1.0f) x = 1.0f;
        // 3x^2 - 2x^3
        r_ptr[i] = x * x * (3.0f - 2.0f * x);
    }

    return result;
}

PackedFloat32Array InterpolationOps::smootherstep(const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = t.size();
    result.resize(n);

    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        float x = t_ptr[i];
        // Clamp to [0, 1]
        if (x < 0.0f) x = 0.0f;
        else if (x > 1.0f) x = 1.0f;
        // 6x^5 - 15x^4 + 10x^3
        r_ptr[i] = x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
    }

    return result;
}

PackedFloat32Array InterpolationOps::lerp_floats(const PackedFloat32Array& a, const PackedFloat32Array& b,
                                                  float t) {
    PackedFloat32Array result;
    int32_t n = a.size();
    if (n != b.size()) {
        return result;
    }

    result.resize(n);
    const float* a_ptr = a.ptr();
    const float* b_ptr = b.ptr();
    float* r_ptr = result.ptrw();

    float inv_t = 1.0f - t;
    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = a_ptr[i] * inv_t + b_ptr[i] * t;
    }

    return result;
}

PackedFloat32Array InterpolationOps::lerp_floats_array(const PackedFloat32Array& a, const PackedFloat32Array& b,
                                                        const PackedFloat32Array& t) {
    PackedFloat32Array result;
    int32_t n = a.size();
    if (n != b.size() || n != t.size()) {
        return result;
    }

    result.resize(n);
    const float* a_ptr = a.ptr();
    const float* b_ptr = b.ptr();
    const float* t_ptr = t.ptr();
    float* r_ptr = result.ptrw();

    for (int32_t i = 0; i < n; i++) {
        r_ptr[i] = a_ptr[i] * (1.0f - t_ptr[i]) + b_ptr[i] * t_ptr[i];
    }

    return result;
}

PackedFloat32Array InterpolationOps::linspace(float start, float end, int32_t count) {
    PackedFloat32Array result;

    if (count < 1) {
        return result;
    }

    result.resize(count);
    float* r_ptr = result.ptrw();

    if (count == 1) {
        r_ptr[0] = start;
        return result;
    }

    float step = (end - start) / (count - 1);
    for (int32_t i = 0; i < count; i++) {
        r_ptr[i] = start + step * i;
    }

    return result;
}

}

/**
 * BatchOps - Implementation
 */

#include "batch_ops.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <cmath>
#include <algorithm>

namespace godot {

void BatchOps::_bind_methods() {
    // Velocity / Position updates
    ClassDB::bind_static_method("BatchOps", D_METHOD("apply_velocities_2d", "positions", "velocities", "delta"),
        &BatchOps::apply_velocities_2d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("apply_velocities_3d", "positions", "velocities", "delta"),
        &BatchOps::apply_velocities_3d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("apply_accelerations_2d", "velocities", "accelerations", "delta"),
        &BatchOps::apply_accelerations_2d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("apply_accelerations_3d", "velocities", "accelerations", "delta"),
        &BatchOps::apply_accelerations_3d);

    // Steering behaviors
    ClassDB::bind_static_method("BatchOps", D_METHOD("seek_batch", "positions", "targets", "max_speed"),
        &BatchOps::seek_batch);
    ClassDB::bind_static_method("BatchOps", D_METHOD("seek_batch_3d", "positions", "targets", "max_speed"),
        &BatchOps::seek_batch_3d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("flee_batch", "positions", "threats", "max_speed"),
        &BatchOps::flee_batch);
    ClassDB::bind_static_method("BatchOps", D_METHOD("flee_batch_3d", "positions", "threats", "max_speed"),
        &BatchOps::flee_batch_3d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("arrive_batch", "positions", "targets", "max_speed", "slowing_radius"),
        &BatchOps::arrive_batch);
    ClassDB::bind_static_method("BatchOps", D_METHOD("arrive_batch_3d", "positions", "targets", "max_speed", "slowing_radius"),
        &BatchOps::arrive_batch_3d);

    // Flocking / Separation
    ClassDB::bind_static_method("BatchOps", D_METHOD("separation_2d", "positions", "radius", "strength"),
        &BatchOps::separation_2d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("separation_3d", "positions", "radius", "strength"),
        &BatchOps::separation_3d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("separation_2d_radii", "positions", "radii", "strength"),
        &BatchOps::separation_2d_radii);

    // Cohesion
    ClassDB::bind_static_method("BatchOps", D_METHOD("cohesion_2d", "positions", "radius", "strength"),
        &BatchOps::cohesion_2d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("cohesion_3d", "positions", "radius", "strength"),
        &BatchOps::cohesion_3d);

    // Alignment
    ClassDB::bind_static_method("BatchOps", D_METHOD("alignment_2d", "positions", "velocities", "radius"),
        &BatchOps::alignment_2d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("alignment_3d", "positions", "velocities", "radius"),
        &BatchOps::alignment_3d);

    // Combined flocking
    ClassDB::bind_static_method("BatchOps", D_METHOD("flock_2d", "positions", "velocities",
        "separation_radius", "cohesion_radius", "alignment_radius",
        "separation_strength", "cohesion_strength", "alignment_strength"),
        &BatchOps::flock_2d);

    // Wander
    ClassDB::bind_static_method("BatchOps", D_METHOD("wander_2d", "forward_directions", "wander_angles",
        "circle_distance", "circle_radius", "angle_change"),
        &BatchOps::wander_2d);

    // Obstacle avoidance
    ClassDB::bind_static_method("BatchOps", D_METHOD("avoid_circles_2d", "positions", "velocities",
        "obstacle_centers", "obstacle_radii", "lookahead_distance", "avoidance_strength"),
        &BatchOps::avoid_circles_2d);

    // Velocity limiting
    ClassDB::bind_static_method("BatchOps", D_METHOD("limit_velocity_2d", "velocities", "max_speed"),
        &BatchOps::limit_velocity_2d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("limit_velocity_3d", "velocities", "max_speed"),
        &BatchOps::limit_velocity_3d);
    ClassDB::bind_static_method("BatchOps", D_METHOD("limit_velocity_range_2d", "velocities", "min_speed", "max_speed"),
        &BatchOps::limit_velocity_range_2d);
}

// ========== VELOCITY / POSITION UPDATES ==========

PackedVector2Array BatchOps::apply_velocities_2d(
    const PackedVector2Array& positions,
    const PackedVector2Array& velocities,
    float delta
) {
    int64_t count = positions.size();
    if (count == 0 || velocities.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    const Vector2* vel_ptr = velocities.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        res_ptr[i] = pos_ptr[i] + vel_ptr[i] * delta;
    }

    return result;
}

PackedVector3Array BatchOps::apply_velocities_3d(
    const PackedVector3Array& positions,
    const PackedVector3Array& velocities,
    float delta
) {
    int64_t count = positions.size();
    if (count == 0 || velocities.size() != count) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* pos_ptr = positions.ptr();
    const Vector3* vel_ptr = velocities.ptr();
    Vector3* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        res_ptr[i] = pos_ptr[i] + vel_ptr[i] * delta;
    }

    return result;
}

PackedVector2Array BatchOps::apply_accelerations_2d(
    const PackedVector2Array& velocities,
    const PackedVector2Array& accelerations,
    float delta
) {
    int64_t count = velocities.size();
    if (count == 0 || accelerations.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* vel_ptr = velocities.ptr();
    const Vector2* acc_ptr = accelerations.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        res_ptr[i] = vel_ptr[i] + acc_ptr[i] * delta;
    }

    return result;
}

PackedVector3Array BatchOps::apply_accelerations_3d(
    const PackedVector3Array& velocities,
    const PackedVector3Array& accelerations,
    float delta
) {
    int64_t count = velocities.size();
    if (count == 0 || accelerations.size() != count) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* vel_ptr = velocities.ptr();
    const Vector3* acc_ptr = accelerations.ptr();
    Vector3* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        res_ptr[i] = vel_ptr[i] + acc_ptr[i] * delta;
    }

    return result;
}

// ========== STEERING BEHAVIORS ==========

PackedVector2Array BatchOps::seek_batch(
    const PackedVector2Array& positions,
    const PackedVector2Array& targets,
    float max_speed
) {
    int64_t count = positions.size();
    if (count == 0 || targets.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    const Vector2* tgt_ptr = targets.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        Vector2 direction = tgt_ptr[i] - pos_ptr[i];
        float length = direction.length();
        if (length > 0.0001f) {
            res_ptr[i] = (direction / length) * max_speed;
        } else {
            res_ptr[i] = Vector2();
        }
    }

    return result;
}

PackedVector3Array BatchOps::seek_batch_3d(
    const PackedVector3Array& positions,
    const PackedVector3Array& targets,
    float max_speed
) {
    int64_t count = positions.size();
    if (count == 0 || targets.size() != count) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* pos_ptr = positions.ptr();
    const Vector3* tgt_ptr = targets.ptr();
    Vector3* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        Vector3 direction = tgt_ptr[i] - pos_ptr[i];
        float length = direction.length();
        if (length > 0.0001f) {
            res_ptr[i] = (direction / length) * max_speed;
        } else {
            res_ptr[i] = Vector3();
        }
    }

    return result;
}

PackedVector2Array BatchOps::flee_batch(
    const PackedVector2Array& positions,
    const PackedVector2Array& threats,
    float max_speed
) {
    int64_t count = positions.size();
    if (count == 0 || threats.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    const Vector2* thr_ptr = threats.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        Vector2 direction = pos_ptr[i] - thr_ptr[i];
        float length = direction.length();
        if (length > 0.0001f) {
            res_ptr[i] = (direction / length) * max_speed;
        } else {
            res_ptr[i] = Vector2();
        }
    }

    return result;
}

PackedVector3Array BatchOps::flee_batch_3d(
    const PackedVector3Array& positions,
    const PackedVector3Array& threats,
    float max_speed
) {
    int64_t count = positions.size();
    if (count == 0 || threats.size() != count) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* pos_ptr = positions.ptr();
    const Vector3* thr_ptr = threats.ptr();
    Vector3* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        Vector3 direction = pos_ptr[i] - thr_ptr[i];
        float length = direction.length();
        if (length > 0.0001f) {
            res_ptr[i] = (direction / length) * max_speed;
        } else {
            res_ptr[i] = Vector3();
        }
    }

    return result;
}

PackedVector2Array BatchOps::arrive_batch(
    const PackedVector2Array& positions,
    const PackedVector2Array& targets,
    float max_speed,
    float slowing_radius
) {
    int64_t count = positions.size();
    if (count == 0 || targets.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    const Vector2* tgt_ptr = targets.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        Vector2 to_target = tgt_ptr[i] - pos_ptr[i];
        float distance = to_target.length();

        if (distance < 0.0001f) {
            res_ptr[i] = Vector2();
        } else {
            float speed;
            if (distance < slowing_radius) {
                // Ramp down speed as we get closer
                speed = max_speed * (distance / slowing_radius);
            } else {
                speed = max_speed;
            }
            res_ptr[i] = (to_target / distance) * speed;
        }
    }

    return result;
}

PackedVector3Array BatchOps::arrive_batch_3d(
    const PackedVector3Array& positions,
    const PackedVector3Array& targets,
    float max_speed,
    float slowing_radius
) {
    int64_t count = positions.size();
    if (count == 0 || targets.size() != count) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* pos_ptr = positions.ptr();
    const Vector3* tgt_ptr = targets.ptr();
    Vector3* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        Vector3 to_target = tgt_ptr[i] - pos_ptr[i];
        float distance = to_target.length();

        if (distance < 0.0001f) {
            res_ptr[i] = Vector3();
        } else {
            float speed;
            if (distance < slowing_radius) {
                speed = max_speed * (distance / slowing_radius);
            } else {
                speed = max_speed;
            }
            res_ptr[i] = (to_target / distance) * speed;
        }
    }

    return result;
}

// ========== FLOCKING / SEPARATION ==========

PackedVector2Array BatchOps::separation_2d(
    const PackedVector2Array& positions,
    float radius,
    float strength
) {
    int64_t count = positions.size();
    if (count == 0) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    Vector2* res_ptr = result.ptrw();

    float radius_sq = radius * radius;

    for (int64_t i = 0; i < count; ++i) {
        Vector2 force;
        const Vector2& my_pos = pos_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            Vector2 diff = my_pos - pos_ptr[j];
            float dist_sq = diff.length_squared();

            if (dist_sq < radius_sq && dist_sq > 0.0001f) {
                float dist = std::sqrt(dist_sq);
                // Force inversely proportional to distance
                float factor = (radius - dist) / radius;
                force += (diff / dist) * factor * strength;
            }
        }

        res_ptr[i] = force;
    }

    return result;
}

PackedVector3Array BatchOps::separation_3d(
    const PackedVector3Array& positions,
    float radius,
    float strength
) {
    int64_t count = positions.size();
    if (count == 0) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* pos_ptr = positions.ptr();
    Vector3* res_ptr = result.ptrw();

    float radius_sq = radius * radius;

    for (int64_t i = 0; i < count; ++i) {
        Vector3 force;
        const Vector3& my_pos = pos_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            Vector3 diff = my_pos - pos_ptr[j];
            float dist_sq = diff.length_squared();

            if (dist_sq < radius_sq && dist_sq > 0.0001f) {
                float dist = std::sqrt(dist_sq);
                float factor = (radius - dist) / radius;
                force += (diff / dist) * factor * strength;
            }
        }

        res_ptr[i] = force;
    }

    return result;
}

PackedVector2Array BatchOps::separation_2d_radii(
    const PackedVector2Array& positions,
    const PackedFloat32Array& radii,
    float strength
) {
    int64_t count = positions.size();
    if (count == 0 || radii.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    const float* rad_ptr = radii.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        Vector2 force;
        const Vector2& my_pos = pos_ptr[i];
        float my_radius = rad_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            float combined_radius = my_radius + rad_ptr[j];
            float combined_radius_sq = combined_radius * combined_radius;

            Vector2 diff = my_pos - pos_ptr[j];
            float dist_sq = diff.length_squared();

            if (dist_sq < combined_radius_sq && dist_sq > 0.0001f) {
                float dist = std::sqrt(dist_sq);
                float factor = (combined_radius - dist) / combined_radius;
                force += (diff / dist) * factor * strength;
            }
        }

        res_ptr[i] = force;
    }

    return result;
}

// ========== COHESION ==========

PackedVector2Array BatchOps::cohesion_2d(
    const PackedVector2Array& positions,
    float radius,
    float strength
) {
    int64_t count = positions.size();
    if (count == 0) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    Vector2* res_ptr = result.ptrw();

    float radius_sq = radius * radius;

    for (int64_t i = 0; i < count; ++i) {
        Vector2 center_of_mass;
        int neighbor_count = 0;
        const Vector2& my_pos = pos_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            float dist_sq = (my_pos - pos_ptr[j]).length_squared();
            if (dist_sq < radius_sq) {
                center_of_mass += pos_ptr[j];
                neighbor_count++;
            }
        }

        if (neighbor_count > 0) {
            center_of_mass /= static_cast<float>(neighbor_count);
            Vector2 direction = center_of_mass - my_pos;
            float dist = direction.length();
            if (dist > 0.0001f) {
                res_ptr[i] = (direction / dist) * strength;
            } else {
                res_ptr[i] = Vector2();
            }
        } else {
            res_ptr[i] = Vector2();
        }
    }

    return result;
}

PackedVector3Array BatchOps::cohesion_3d(
    const PackedVector3Array& positions,
    float radius,
    float strength
) {
    int64_t count = positions.size();
    if (count == 0) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* pos_ptr = positions.ptr();
    Vector3* res_ptr = result.ptrw();

    float radius_sq = radius * radius;

    for (int64_t i = 0; i < count; ++i) {
        Vector3 center_of_mass;
        int neighbor_count = 0;
        const Vector3& my_pos = pos_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            float dist_sq = (my_pos - pos_ptr[j]).length_squared();
            if (dist_sq < radius_sq) {
                center_of_mass += pos_ptr[j];
                neighbor_count++;
            }
        }

        if (neighbor_count > 0) {
            center_of_mass /= static_cast<float>(neighbor_count);
            Vector3 direction = center_of_mass - my_pos;
            float dist = direction.length();
            if (dist > 0.0001f) {
                res_ptr[i] = (direction / dist) * strength;
            } else {
                res_ptr[i] = Vector3();
            }
        } else {
            res_ptr[i] = Vector3();
        }
    }

    return result;
}

// ========== ALIGNMENT ==========

PackedVector2Array BatchOps::alignment_2d(
    const PackedVector2Array& positions,
    const PackedVector2Array& velocities,
    float radius
) {
    int64_t count = positions.size();
    if (count == 0 || velocities.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    const Vector2* vel_ptr = velocities.ptr();
    Vector2* res_ptr = result.ptrw();

    float radius_sq = radius * radius;

    for (int64_t i = 0; i < count; ++i) {
        Vector2 avg_velocity;
        int neighbor_count = 0;
        const Vector2& my_pos = pos_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            float dist_sq = (my_pos - pos_ptr[j]).length_squared();
            if (dist_sq < radius_sq) {
                avg_velocity += vel_ptr[j];
                neighbor_count++;
            }
        }

        if (neighbor_count > 0) {
            res_ptr[i] = avg_velocity / static_cast<float>(neighbor_count);
        } else {
            res_ptr[i] = vel_ptr[i];  // Keep current velocity if no neighbors
        }
    }

    return result;
}

PackedVector3Array BatchOps::alignment_3d(
    const PackedVector3Array& positions,
    const PackedVector3Array& velocities,
    float radius
) {
    int64_t count = positions.size();
    if (count == 0 || velocities.size() != count) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* pos_ptr = positions.ptr();
    const Vector3* vel_ptr = velocities.ptr();
    Vector3* res_ptr = result.ptrw();

    float radius_sq = radius * radius;

    for (int64_t i = 0; i < count; ++i) {
        Vector3 avg_velocity;
        int neighbor_count = 0;
        const Vector3& my_pos = pos_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            float dist_sq = (my_pos - pos_ptr[j]).length_squared();
            if (dist_sq < radius_sq) {
                avg_velocity += vel_ptr[j];
                neighbor_count++;
            }
        }

        if (neighbor_count > 0) {
            res_ptr[i] = avg_velocity / static_cast<float>(neighbor_count);
        } else {
            res_ptr[i] = vel_ptr[i];
        }
    }

    return result;
}

// ========== COMBINED FLOCKING ==========

PackedVector2Array BatchOps::flock_2d(
    const PackedVector2Array& positions,
    const PackedVector2Array& velocities,
    float separation_radius,
    float cohesion_radius,
    float alignment_radius,
    float separation_strength,
    float cohesion_strength,
    float alignment_strength
) {
    int64_t count = positions.size();
    if (count == 0 || velocities.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* pos_ptr = positions.ptr();
    const Vector2* vel_ptr = velocities.ptr();
    Vector2* res_ptr = result.ptrw();

    float sep_radius_sq = separation_radius * separation_radius;
    float coh_radius_sq = cohesion_radius * cohesion_radius;
    float align_radius_sq = alignment_radius * alignment_radius;

    for (int64_t i = 0; i < count; ++i) {
        Vector2 separation_force;
        Vector2 cohesion_center;
        Vector2 alignment_velocity;
        int sep_count = 0;
        int coh_count = 0;
        int align_count = 0;

        const Vector2& my_pos = pos_ptr[i];

        for (int64_t j = 0; j < count; ++j) {
            if (i == j) continue;

            Vector2 diff = my_pos - pos_ptr[j];
            float dist_sq = diff.length_squared();

            // Separation
            if (dist_sq < sep_radius_sq && dist_sq > 0.0001f) {
                float dist = std::sqrt(dist_sq);
                float factor = (separation_radius - dist) / separation_radius;
                separation_force += (diff / dist) * factor;
                sep_count++;
            }

            // Cohesion
            if (dist_sq < coh_radius_sq) {
                cohesion_center += pos_ptr[j];
                coh_count++;
            }

            // Alignment
            if (dist_sq < align_radius_sq) {
                alignment_velocity += vel_ptr[j];
                align_count++;
            }
        }

        Vector2 total_force;

        // Apply separation
        if (sep_count > 0) {
            total_force += separation_force * separation_strength;
        }

        // Apply cohesion
        if (coh_count > 0) {
            cohesion_center /= static_cast<float>(coh_count);
            Vector2 direction = cohesion_center - my_pos;
            float dist = direction.length();
            if (dist > 0.0001f) {
                total_force += (direction / dist) * cohesion_strength;
            }
        }

        // Apply alignment
        if (align_count > 0) {
            alignment_velocity /= static_cast<float>(align_count);
            Vector2 align_diff = alignment_velocity - vel_ptr[i];
            total_force += align_diff * alignment_strength;
        }

        res_ptr[i] = total_force;
    }

    return result;
}

// ========== WANDER ==========

PackedVector2Array BatchOps::wander_2d(
    const PackedVector2Array& forward_directions,
    const PackedFloat32Array& wander_angles,
    float circle_distance,
    float circle_radius,
    float angle_change
) {
    int64_t count = forward_directions.size();
    if (count == 0 || wander_angles.size() != count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* fwd_ptr = forward_directions.ptr();
    const float* ang_ptr = wander_angles.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < count; ++i) {
        // Circle center is ahead of entity
        Vector2 forward = fwd_ptr[i];
        float fwd_len = forward.length();
        if (fwd_len < 0.0001f) {
            res_ptr[i] = Vector2();
            continue;
        }

        Vector2 fwd_norm = forward / fwd_len;
        Vector2 circle_center = fwd_norm * circle_distance;

        // Point on circle based on wander angle
        float angle = ang_ptr[i];
        Vector2 displacement = Vector2(
            std::cos(angle) * circle_radius,
            std::sin(angle) * circle_radius
        );

        res_ptr[i] = (circle_center + displacement).normalized();
    }

    return result;
}

// ========== OBSTACLE AVOIDANCE ==========

PackedVector2Array BatchOps::avoid_circles_2d(
    const PackedVector2Array& positions,
    const PackedVector2Array& velocities,
    const PackedVector2Array& obstacle_centers,
    const PackedFloat32Array& obstacle_radii,
    float lookahead_distance,
    float avoidance_strength
) {
    int64_t entity_count = positions.size();
    int64_t obstacle_count = obstacle_centers.size();

    if (entity_count == 0 || velocities.size() != entity_count ||
        obstacle_radii.size() != obstacle_count) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(entity_count);

    const Vector2* pos_ptr = positions.ptr();
    const Vector2* vel_ptr = velocities.ptr();
    const Vector2* obs_ptr = obstacle_centers.ptr();
    const float* rad_ptr = obstacle_radii.ptr();
    Vector2* res_ptr = result.ptrw();

    for (int64_t i = 0; i < entity_count; ++i) {
        Vector2 avoidance_force;
        Vector2 pos = pos_ptr[i];
        Vector2 vel = vel_ptr[i];

        float vel_len = vel.length();
        if (vel_len < 0.0001f) {
            res_ptr[i] = Vector2();
            continue;
        }

        Vector2 ahead = pos + vel.normalized() * lookahead_distance;
        Vector2 ahead_half = pos + vel.normalized() * (lookahead_distance * 0.5f);

        // Find closest threatening obstacle
        float closest_dist_sq = lookahead_distance * lookahead_distance * 4.0f;
        int closest_obs = -1;

        for (int64_t j = 0; j < obstacle_count; ++j) {
            Vector2 obs_center = obs_ptr[j];
            float obs_radius = rad_ptr[j];

            // Check if ahead point is inside obstacle
            float dist_sq = (ahead - obs_center).length_squared();
            float dist_half_sq = (ahead_half - obs_center).length_squared();
            float obs_radius_sq = obs_radius * obs_radius;

            if (dist_sq < obs_radius_sq || dist_half_sq < obs_radius_sq) {
                float check_dist = (pos - obs_center).length_squared();
                if (check_dist < closest_dist_sq) {
                    closest_dist_sq = check_dist;
                    closest_obs = j;
                }
            }
        }

        if (closest_obs >= 0) {
            // Steer away from closest obstacle
            Vector2 away = ahead - obs_ptr[closest_obs];
            float away_len = away.length();
            if (away_len > 0.0001f) {
                avoidance_force = (away / away_len) * avoidance_strength;
            }
        }

        res_ptr[i] = avoidance_force;
    }

    return result;
}

// ========== VELOCITY LIMITING ==========

PackedVector2Array BatchOps::limit_velocity_2d(
    const PackedVector2Array& velocities,
    float max_speed
) {
    int64_t count = velocities.size();
    if (count == 0) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* vel_ptr = velocities.ptr();
    Vector2* res_ptr = result.ptrw();

    float max_speed_sq = max_speed * max_speed;

    for (int64_t i = 0; i < count; ++i) {
        Vector2 vel = vel_ptr[i];
        float speed_sq = vel.length_squared();

        if (speed_sq > max_speed_sq) {
            res_ptr[i] = vel.normalized() * max_speed;
        } else {
            res_ptr[i] = vel;
        }
    }

    return result;
}

PackedVector3Array BatchOps::limit_velocity_3d(
    const PackedVector3Array& velocities,
    float max_speed
) {
    int64_t count = velocities.size();
    if (count == 0) {
        return PackedVector3Array();
    }

    PackedVector3Array result;
    result.resize(count);

    const Vector3* vel_ptr = velocities.ptr();
    Vector3* res_ptr = result.ptrw();

    float max_speed_sq = max_speed * max_speed;

    for (int64_t i = 0; i < count; ++i) {
        Vector3 vel = vel_ptr[i];
        float speed_sq = vel.length_squared();

        if (speed_sq > max_speed_sq) {
            res_ptr[i] = vel.normalized() * max_speed;
        } else {
            res_ptr[i] = vel;
        }
    }

    return result;
}

PackedVector2Array BatchOps::limit_velocity_range_2d(
    const PackedVector2Array& velocities,
    float min_speed,
    float max_speed
) {
    int64_t count = velocities.size();
    if (count == 0) {
        return PackedVector2Array();
    }

    PackedVector2Array result;
    result.resize(count);

    const Vector2* vel_ptr = velocities.ptr();
    Vector2* res_ptr = result.ptrw();

    float min_speed_sq = min_speed * min_speed;
    float max_speed_sq = max_speed * max_speed;

    for (int64_t i = 0; i < count; ++i) {
        Vector2 vel = vel_ptr[i];
        float speed_sq = vel.length_squared();

        if (speed_sq > max_speed_sq) {
            res_ptr[i] = vel.normalized() * max_speed;
        } else if (speed_sq > 0.0001f && speed_sq < min_speed_sq) {
            res_ptr[i] = vel.normalized() * min_speed;
        } else {
            res_ptr[i] = vel;
        }
    }

    return result;
}

}

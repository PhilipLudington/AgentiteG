/**
 * BatchOps - Batch operations for common game patterns
 *
 * Provides high-performance batch operations for game logic that
 * typically runs every frame on many entities.
 *
 * Usage:
 *   # Apply velocities to positions
 *   positions = BatchOps.apply_velocities_2d(positions, velocities, delta)
 *
 *   # Steering: seek targets
 *   accelerations = BatchOps.seek_batch(positions, targets, max_speed)
 *
 *   # Steering: arrive at targets (slow down near goal)
 *   accelerations = BatchOps.arrive_batch(positions, targets, max_speed, slowing_radius)
 *
 *   # Separation: push apart overlapping entities
 *   forces = BatchOps.separation_2d(positions, radius, strength)
 */

#ifndef AGENTITE_BATCH_OPS_HPP
#define AGENTITE_BATCH_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class BatchOps : public RefCounted {
    GDCLASS(BatchOps, RefCounted)

protected:
    static void _bind_methods();

public:
    // ========== VELOCITY / POSITION UPDATES ==========
    // Apply velocities to positions: result[i] = positions[i] + velocities[i] * delta
    static PackedVector2Array apply_velocities_2d(
        const PackedVector2Array& positions,
        const PackedVector2Array& velocities,
        float delta
    );
    static PackedVector3Array apply_velocities_3d(
        const PackedVector3Array& positions,
        const PackedVector3Array& velocities,
        float delta
    );

    // Apply accelerations to velocities: result[i] = velocities[i] + accelerations[i] * delta
    static PackedVector2Array apply_accelerations_2d(
        const PackedVector2Array& velocities,
        const PackedVector2Array& accelerations,
        float delta
    );
    static PackedVector3Array apply_accelerations_3d(
        const PackedVector3Array& velocities,
        const PackedVector3Array& accelerations,
        float delta
    );

    // ========== STEERING BEHAVIORS ==========
    // All steering methods return desired velocity vectors (not accelerations)
    // To get acceleration: (desired - current_velocity).limit(max_force)

    // Seek: move toward targets at max speed
    // Returns desired velocity: (target - position).normalized() * max_speed
    static PackedVector2Array seek_batch(
        const PackedVector2Array& positions,
        const PackedVector2Array& targets,
        float max_speed
    );
    static PackedVector3Array seek_batch_3d(
        const PackedVector3Array& positions,
        const PackedVector3Array& targets,
        float max_speed
    );

    // Flee: move away from threats at max speed
    // Returns desired velocity: (position - threat).normalized() * max_speed
    static PackedVector2Array flee_batch(
        const PackedVector2Array& positions,
        const PackedVector2Array& threats,
        float max_speed
    );
    static PackedVector3Array flee_batch_3d(
        const PackedVector3Array& positions,
        const PackedVector3Array& threats,
        float max_speed
    );

    // Arrive: move toward targets, slowing down as we approach
    // Returns desired velocity that ramps down within slowing_radius
    static PackedVector2Array arrive_batch(
        const PackedVector2Array& positions,
        const PackedVector2Array& targets,
        float max_speed,
        float slowing_radius
    );
    static PackedVector3Array arrive_batch_3d(
        const PackedVector3Array& positions,
        const PackedVector3Array& targets,
        float max_speed,
        float slowing_radius
    );

    // ========== FLOCKING / SEPARATION ==========
    // Separation: repel from nearby entities
    // Returns separation force for each entity
    // Uses brute force O(n^2) - for large counts, use spatial hash version

    static PackedVector2Array separation_2d(
        const PackedVector2Array& positions,
        float radius,
        float strength
    );
    static PackedVector3Array separation_3d(
        const PackedVector3Array& positions,
        float radius,
        float strength
    );

    // Separation with different radii per entity
    static PackedVector2Array separation_2d_radii(
        const PackedVector2Array& positions,
        const PackedFloat32Array& radii,
        float strength
    );

    // ========== COHESION ==========
    // Cohesion: steer toward center of nearby entities
    // Returns direction toward local center of mass

    static PackedVector2Array cohesion_2d(
        const PackedVector2Array& positions,
        float radius,
        float strength
    );
    static PackedVector3Array cohesion_3d(
        const PackedVector3Array& positions,
        float radius,
        float strength
    );

    // ========== ALIGNMENT ==========
    // Alignment: match velocity of nearby entities
    // Returns average velocity of neighbors within radius

    static PackedVector2Array alignment_2d(
        const PackedVector2Array& positions,
        const PackedVector2Array& velocities,
        float radius
    );
    static PackedVector3Array alignment_3d(
        const PackedVector3Array& positions,
        const PackedVector3Array& velocities,
        float radius
    );

    // ========== COMBINED FLOCKING ==========
    // Boids-style flocking: separation + cohesion + alignment
    // Returns combined steering force

    static PackedVector2Array flock_2d(
        const PackedVector2Array& positions,
        const PackedVector2Array& velocities,
        float separation_radius,
        float cohesion_radius,
        float alignment_radius,
        float separation_strength,
        float cohesion_strength,
        float alignment_strength
    );

    // ========== WANDER ==========
    // Wander: semi-random movement with smoothness
    // angles: current wander angles (updated in place conceptually - returns new angles via second return)
    // Returns desired velocity directions

    static PackedVector2Array wander_2d(
        const PackedVector2Array& forward_directions,
        const PackedFloat32Array& wander_angles,
        float circle_distance,
        float circle_radius,
        float angle_change
    );

    // ========== OBSTACLE AVOIDANCE ==========
    // Simple obstacle avoidance using raycast-style detection
    // Returns avoidance forces (zero if no obstacle ahead)

    static PackedVector2Array avoid_circles_2d(
        const PackedVector2Array& positions,
        const PackedVector2Array& velocities,
        const PackedVector2Array& obstacle_centers,
        const PackedFloat32Array& obstacle_radii,
        float lookahead_distance,
        float avoidance_strength
    );

    // ========== VELOCITY LIMITING ==========
    // Limit velocity magnitudes
    static PackedVector2Array limit_velocity_2d(
        const PackedVector2Array& velocities,
        float max_speed
    );
    static PackedVector3Array limit_velocity_3d(
        const PackedVector3Array& velocities,
        float max_speed
    );

    // Limit velocity with minimum speed (prevents stopping)
    static PackedVector2Array limit_velocity_range_2d(
        const PackedVector2Array& velocities,
        float min_speed,
        float max_speed
    );
};

}

#endif // AGENTITE_BATCH_OPS_HPP

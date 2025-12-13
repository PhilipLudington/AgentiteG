/**
 * RandomOps - High-performance random number generation
 *
 * Provides bulk random number generation optimized for game development.
 * Uses xoshiro256** for fast, high-quality randomness.
 *
 * Usage:
 *   var rng = RandomOps.new()
 *   rng.seed(12345)
 *
 *   # Generate many random floats at once
 *   var values = rng.randf_array(1000)
 *
 *   # Random points in shapes
 *   var spawn_points = rng.rand_points_in_circle(100, center, radius)
 *
 *   # Weighted selection for loot tables
 *   var drops = rng.weighted_choice(loot_weights, 5)
 */

#ifndef AGENTITE_RANDOM_OPS_HPP
#define AGENTITE_RANDOM_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/aabb.hpp>

#include <cstdint>

namespace godot {

class RandomOps : public RefCounted {
    GDCLASS(RandomOps, RefCounted)

private:
    // xoshiro256** state (4 x 64-bit)
    uint64_t state[4];

    // Internal RNG functions
    uint64_t next();
    double next_double();  // 0.0 to 1.0
    float next_float();    // 0.0 to 1.0

protected:
    static void _bind_methods();

public:
    RandomOps();
    ~RandomOps();

    // ========== SEEDING ==========
    // Seed with specific value (deterministic)
    void seed(int64_t seed_value);
    // Seed from current time (non-deterministic)
    void seed_from_time();

    // ========== BULK FLOAT GENERATION ==========
    // Generate count floats in range [0.0, 1.0)
    PackedFloat32Array randf_array(int count);
    // Generate count floats in range [min, max)
    PackedFloat32Array randf_range_array(int count, float min_val, float max_val);

    // ========== BULK INTEGER GENERATION ==========
    // Generate count random 32-bit integers
    PackedInt32Array randi_array(int count);
    // Generate count integers in range [min, max] (inclusive)
    PackedInt32Array randi_range_array(int count, int min_val, int max_val);

    // ========== RANDOM POINTS IN SHAPES ==========
    // Random points uniformly distributed in a rectangle
    PackedVector2Array rand_points_in_rect(int count, const Rect2& rect);
    // Random points uniformly distributed in a 3D box
    PackedVector3Array rand_points_in_box(int count, const AABB& box);
    // Random points uniformly distributed in a circle
    PackedVector2Array rand_points_in_circle(int count, const Vector2& center, float radius);
    // Random points uniformly distributed in a sphere
    PackedVector3Array rand_points_in_sphere(int count, const Vector3& center, float radius);
    // Random points on circle perimeter
    PackedVector2Array rand_points_on_circle(int count, const Vector2& center, float radius);
    // Random points on sphere surface
    PackedVector3Array rand_points_on_sphere(int count, const Vector3& center, float radius);

    // ========== SHUFFLING ==========
    // Return shuffled indices 0..count-1 (Fisher-Yates)
    PackedInt32Array shuffle_indices(int count);
    // Shuffle float array (returns new array)
    PackedFloat32Array shuffle_floats(const PackedFloat32Array& values);
    // Shuffle int array (returns new array)
    PackedInt32Array shuffle_ints(const PackedInt32Array& values);

    // ========== WEIGHTED SELECTION ==========
    // Select count indices with probability proportional to weights
    PackedInt32Array weighted_choice(const PackedFloat32Array& weights, int count);
    // Select single index
    int weighted_choice_one(const PackedFloat32Array& weights);

    // ========== POISSON DISK SAMPLING ==========
    // Generate blue noise distributed points in 2D rectangle
    // min_distance: minimum spacing between points
    // max_attempts: attempts per active point (default 30)
    PackedVector2Array poisson_disk_2d(const Rect2& bounds, float min_distance, int max_attempts = 30);
    // Generate blue noise distributed points in 3D box
    PackedVector3Array poisson_disk_3d(const AABB& bounds, float min_distance, int max_attempts = 30);

    // ========== RANDOM DIRECTIONS ==========
    // Random unit vectors (uniformly distributed on unit circle/sphere)
    PackedVector2Array rand_directions_2d(int count);
    PackedVector3Array rand_directions_3d(int count);

    // ========== GAUSSIAN/NORMAL DISTRIBUTION ==========
    // Generate normally distributed values (mean=0, std=1)
    PackedFloat32Array randn_array(int count);
    // Generate normally distributed values with custom mean and std
    PackedFloat32Array randn_array_params(int count, float mean, float std_dev);
};

}

#endif // AGENTITE_RANDOM_OPS_HPP

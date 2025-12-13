/**
 * NoiseOps - Procedural noise generation
 *
 * Provides high-performance noise functions for procedural generation.
 * Supports Perlin, Simplex, and Worley (cellular) noise with fractal variants.
 *
 * Usage:
 *   var noise = NoiseOps.new()
 *   noise.set_seed(12345)
 *   noise.set_octaves(4)
 *
 *   # Single value
 *   var value = noise.perlin_2d(Vector2(x, y))
 *
 *   # Batch evaluation for terrain generation
 *   var heights = noise.perlin_2d_batch(sample_points)
 *
 *   # Grid evaluation (optimized for heightmaps)
 *   var heightmap = noise.perlin_2d_grid(origin, cell_size, width, height)
 */

#ifndef AGENTITE_NOISE_OPS_HPP
#define AGENTITE_NOISE_OPS_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cstdint>

namespace godot {

class NoiseOps : public RefCounted {
    GDCLASS(NoiseOps, RefCounted)

private:
    // Permutation table (256 + 256 for wrapping)
    uint8_t perm[512];
    uint8_t perm12[512]; // perm mod 12 for 3D gradients

    // Noise parameters
    int octaves;
    float persistence;
    float lacunarity;
    float frequency;
    int noise_seed;

    // Internal gradient/noise functions
    float grad2(int hash, float x, float y) const;
    float grad3(int hash, float x, float y, float z) const;

    // Single octave noise
    float perlin_2d_single(float x, float y) const;
    float perlin_3d_single(float x, float y, float z) const;
    float simplex_2d_single(float x, float y) const;
    float simplex_3d_single(float x, float y, float z) const;
    float worley_2d_single(float x, float y) const;
    float worley_3d_single(float x, float y, float z) const;

    // Rebuild permutation table
    void rebuild_permutation();

protected:
    static void _bind_methods();

public:
    NoiseOps();
    ~NoiseOps();

    // ========== CONFIGURATION ==========
    void set_seed(int seed);
    int get_seed() const;

    void set_octaves(int num_octaves);
    int get_octaves() const;

    void set_persistence(float p);
    float get_persistence() const;

    void set_lacunarity(float l);
    float get_lacunarity() const;

    void set_frequency(float f);
    float get_frequency() const;

    // ========== PERLIN NOISE ==========
    // Single value evaluation
    float perlin_2d(const Vector2& pos);
    float perlin_3d(const Vector3& pos);

    // Batch evaluation
    PackedFloat32Array perlin_2d_batch(const PackedVector2Array& positions);
    PackedFloat32Array perlin_3d_batch(const PackedVector3Array& positions);

    // Grid evaluation (row-major, optimized for heightmaps)
    // Returns width * height values starting at origin
    PackedFloat32Array perlin_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height);
    PackedFloat32Array perlin_3d_grid(const Vector3& origin, const Vector3& cell_size, int width, int height, int depth);

    // ========== SIMPLEX NOISE ==========
    // Simplex is faster than Perlin and has fewer artifacts
    float simplex_2d(const Vector2& pos);
    float simplex_3d(const Vector3& pos);

    PackedFloat32Array simplex_2d_batch(const PackedVector2Array& positions);
    PackedFloat32Array simplex_3d_batch(const PackedVector3Array& positions);

    PackedFloat32Array simplex_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height);

    // ========== WORLEY (CELLULAR) NOISE ==========
    // Returns distance to nearest feature point
    float worley_2d(const Vector2& pos);
    float worley_3d(const Vector3& pos);

    PackedFloat32Array worley_2d_batch(const PackedVector2Array& positions);
    PackedFloat32Array worley_3d_batch(const PackedVector3Array& positions);

    PackedFloat32Array worley_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height);

    // ========== FRACTAL VARIANTS ==========
    // Fractal Brownian Motion (sum of octaves)
    float fbm_2d(const Vector2& pos);
    float fbm_3d(const Vector3& pos);
    PackedFloat32Array fbm_2d_batch(const PackedVector2Array& positions);
    PackedFloat32Array fbm_3d_batch(const PackedVector3Array& positions);
    PackedFloat32Array fbm_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height);

    // Ridged multifractal (abs, inverted, for ridges/mountains)
    float ridged_2d(const Vector2& pos);
    float ridged_3d(const Vector3& pos);
    PackedFloat32Array ridged_2d_batch(const PackedVector2Array& positions);
    PackedFloat32Array ridged_3d_batch(const PackedVector3Array& positions);
    PackedFloat32Array ridged_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height);

    // Turbulence (sum of absolute values)
    float turbulence_2d(const Vector2& pos);
    float turbulence_3d(const Vector3& pos);
    PackedFloat32Array turbulence_2d_batch(const PackedVector2Array& positions);
    PackedFloat32Array turbulence_3d_batch(const PackedVector3Array& positions);

    // ========== DOMAIN WARPING ==========
    // Warp positions using noise for organic distortion
    PackedVector2Array warp_2d(const PackedVector2Array& positions, float strength);
    PackedVector3Array warp_3d(const PackedVector3Array& positions, float strength);
};

}

#endif // AGENTITE_NOISE_OPS_HPP

/**
 * NoiseOps Implementation
 *
 * Provides Perlin, Simplex, and Worley noise implementations with
 * fractal variants (FBM, ridged, turbulence).
 */

#include "noise_ops.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <cmath>
#include <algorithm>

namespace godot {

// ========== CONSTANTS ==========

// Simplex skew factors
static const float F2 = 0.3660254037844386f;  // (sqrt(3) - 1) / 2
static const float G2 = 0.21132486540518713f; // (3 - sqrt(3)) / 6
static const float F3 = 1.0f / 3.0f;
static const float G3 = 1.0f / 6.0f;

// 2D gradient vectors (8 directions)
static const float GRAD2[8][2] = {
    { 1, 0}, { 0, 1}, {-1, 0}, { 0,-1},
    { 1, 1}, {-1, 1}, { 1,-1}, {-1,-1}
};

// 3D gradient vectors (12 directions)
static const float GRAD3[12][3] = {
    { 1, 1, 0}, {-1, 1, 0}, { 1,-1, 0}, {-1,-1, 0},
    { 1, 0, 1}, {-1, 0, 1}, { 1, 0,-1}, {-1, 0,-1},
    { 0, 1, 1}, { 0,-1, 1}, { 0, 1,-1}, { 0,-1,-1}
};

// ========== HELPER FUNCTIONS ==========

static inline float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

static inline int fast_floor(float x) {
    int xi = static_cast<int>(x);
    return x < xi ? xi - 1 : xi;
}

// ========== BINDING ==========

void NoiseOps::_bind_methods() {
    // Configuration
    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &NoiseOps::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &NoiseOps::get_seed);
    ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &NoiseOps::set_octaves);
    ClassDB::bind_method(D_METHOD("get_octaves"), &NoiseOps::get_octaves);
    ClassDB::bind_method(D_METHOD("set_persistence", "persistence"), &NoiseOps::set_persistence);
    ClassDB::bind_method(D_METHOD("get_persistence"), &NoiseOps::get_persistence);
    ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &NoiseOps::set_lacunarity);
    ClassDB::bind_method(D_METHOD("get_lacunarity"), &NoiseOps::get_lacunarity);
    ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &NoiseOps::set_frequency);
    ClassDB::bind_method(D_METHOD("get_frequency"), &NoiseOps::get_frequency);

    // Perlin noise
    ClassDB::bind_method(D_METHOD("perlin_2d", "pos"), &NoiseOps::perlin_2d);
    ClassDB::bind_method(D_METHOD("perlin_3d", "pos"), &NoiseOps::perlin_3d);
    ClassDB::bind_method(D_METHOD("perlin_2d_batch", "positions"), &NoiseOps::perlin_2d_batch);
    ClassDB::bind_method(D_METHOD("perlin_3d_batch", "positions"), &NoiseOps::perlin_3d_batch);
    ClassDB::bind_method(D_METHOD("perlin_2d_grid", "origin", "cell_size", "width", "height"), &NoiseOps::perlin_2d_grid);
    ClassDB::bind_method(D_METHOD("perlin_3d_grid", "origin", "cell_size", "width", "height", "depth"), &NoiseOps::perlin_3d_grid);

    // Simplex noise
    ClassDB::bind_method(D_METHOD("simplex_2d", "pos"), &NoiseOps::simplex_2d);
    ClassDB::bind_method(D_METHOD("simplex_3d", "pos"), &NoiseOps::simplex_3d);
    ClassDB::bind_method(D_METHOD("simplex_2d_batch", "positions"), &NoiseOps::simplex_2d_batch);
    ClassDB::bind_method(D_METHOD("simplex_3d_batch", "positions"), &NoiseOps::simplex_3d_batch);
    ClassDB::bind_method(D_METHOD("simplex_2d_grid", "origin", "cell_size", "width", "height"), &NoiseOps::simplex_2d_grid);

    // Worley noise
    ClassDB::bind_method(D_METHOD("worley_2d", "pos"), &NoiseOps::worley_2d);
    ClassDB::bind_method(D_METHOD("worley_3d", "pos"), &NoiseOps::worley_3d);
    ClassDB::bind_method(D_METHOD("worley_2d_batch", "positions"), &NoiseOps::worley_2d_batch);
    ClassDB::bind_method(D_METHOD("worley_3d_batch", "positions"), &NoiseOps::worley_3d_batch);
    ClassDB::bind_method(D_METHOD("worley_2d_grid", "origin", "cell_size", "width", "height"), &NoiseOps::worley_2d_grid);

    // FBM
    ClassDB::bind_method(D_METHOD("fbm_2d", "pos"), &NoiseOps::fbm_2d);
    ClassDB::bind_method(D_METHOD("fbm_3d", "pos"), &NoiseOps::fbm_3d);
    ClassDB::bind_method(D_METHOD("fbm_2d_batch", "positions"), &NoiseOps::fbm_2d_batch);
    ClassDB::bind_method(D_METHOD("fbm_3d_batch", "positions"), &NoiseOps::fbm_3d_batch);
    ClassDB::bind_method(D_METHOD("fbm_2d_grid", "origin", "cell_size", "width", "height"), &NoiseOps::fbm_2d_grid);

    // Ridged
    ClassDB::bind_method(D_METHOD("ridged_2d", "pos"), &NoiseOps::ridged_2d);
    ClassDB::bind_method(D_METHOD("ridged_3d", "pos"), &NoiseOps::ridged_3d);
    ClassDB::bind_method(D_METHOD("ridged_2d_batch", "positions"), &NoiseOps::ridged_2d_batch);
    ClassDB::bind_method(D_METHOD("ridged_3d_batch", "positions"), &NoiseOps::ridged_3d_batch);
    ClassDB::bind_method(D_METHOD("ridged_2d_grid", "origin", "cell_size", "width", "height"), &NoiseOps::ridged_2d_grid);

    // Turbulence
    ClassDB::bind_method(D_METHOD("turbulence_2d", "pos"), &NoiseOps::turbulence_2d);
    ClassDB::bind_method(D_METHOD("turbulence_3d", "pos"), &NoiseOps::turbulence_3d);
    ClassDB::bind_method(D_METHOD("turbulence_2d_batch", "positions"), &NoiseOps::turbulence_2d_batch);
    ClassDB::bind_method(D_METHOD("turbulence_3d_batch", "positions"), &NoiseOps::turbulence_3d_batch);

    // Domain warping
    ClassDB::bind_method(D_METHOD("warp_2d", "positions", "strength"), &NoiseOps::warp_2d);
    ClassDB::bind_method(D_METHOD("warp_3d", "positions", "strength"), &NoiseOps::warp_3d);

    // Properties
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves"), "set_octaves", "get_octaves");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "persistence"), "set_persistence", "get_persistence");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lacunarity"), "set_lacunarity", "get_lacunarity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency"), "set_frequency", "get_frequency");
}

NoiseOps::NoiseOps() {
    octaves = 4;
    persistence = 0.5f;
    lacunarity = 2.0f;
    frequency = 1.0f;
    noise_seed = 0;
    rebuild_permutation();
}

NoiseOps::~NoiseOps() {
}

void NoiseOps::rebuild_permutation() {
    // Initialize with identity
    for (int i = 0; i < 256; i++) {
        perm[i] = static_cast<uint8_t>(i);
    }

    // Shuffle using seed
    uint32_t s = static_cast<uint32_t>(noise_seed);
    for (int i = 255; i > 0; i--) {
        // Simple LCG for shuffling
        s = s * 1664525 + 1013904223;
        int j = (s >> 16) % (i + 1);
        std::swap(perm[i], perm[j]);
    }

    // Duplicate for wrapping
    for (int i = 0; i < 256; i++) {
        perm[i + 256] = perm[i];
        perm12[i] = perm[i] % 12;
        perm12[i + 256] = perm12[i];
    }
}

// ========== CONFIGURATION ==========

void NoiseOps::set_seed(int seed) {
    noise_seed = seed;
    rebuild_permutation();
}

int NoiseOps::get_seed() const {
    return noise_seed;
}

void NoiseOps::set_octaves(int num_octaves) {
    octaves = std::max(1, std::min(num_octaves, 16));
}

int NoiseOps::get_octaves() const {
    return octaves;
}

void NoiseOps::set_persistence(float p) {
    persistence = std::max(0.0f, std::min(p, 1.0f));
}

float NoiseOps::get_persistence() const {
    return persistence;
}

void NoiseOps::set_lacunarity(float l) {
    lacunarity = std::max(1.0f, l);
}

float NoiseOps::get_lacunarity() const {
    return lacunarity;
}

void NoiseOps::set_frequency(float f) {
    frequency = std::max(0.001f, f);
}

float NoiseOps::get_frequency() const {
    return frequency;
}

// ========== GRADIENT HELPERS ==========

float NoiseOps::grad2(int hash, float x, float y) const {
    int h = hash & 7;
    return GRAD2[h][0] * x + GRAD2[h][1] * y;
}

float NoiseOps::grad3(int hash, float x, float y, float z) const {
    int h = hash % 12;
    return GRAD3[h][0] * x + GRAD3[h][1] * y + GRAD3[h][2] * z;
}

// ========== PERLIN NOISE ==========

float NoiseOps::perlin_2d_single(float x, float y) const {
    int X = fast_floor(x) & 255;
    int Y = fast_floor(y) & 255;

    x -= fast_floor(x);
    y -= fast_floor(y);

    float u = fade(x);
    float v = fade(y);

    int A = perm[X] + Y;
    int B = perm[X + 1] + Y;

    return lerp(
        lerp(grad2(perm[A], x, y), grad2(perm[B], x - 1, y), u),
        lerp(grad2(perm[A + 1], x, y - 1), grad2(perm[B + 1], x - 1, y - 1), u),
        v
    );
}

float NoiseOps::perlin_3d_single(float x, float y, float z) const {
    int X = fast_floor(x) & 255;
    int Y = fast_floor(y) & 255;
    int Z = fast_floor(z) & 255;

    x -= fast_floor(x);
    y -= fast_floor(y);
    z -= fast_floor(z);

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int A = perm[X] + Y;
    int AA = perm[A] + Z;
    int AB = perm[A + 1] + Z;
    int B = perm[X + 1] + Y;
    int BA = perm[B] + Z;
    int BB = perm[B + 1] + Z;

    return lerp(
        lerp(
            lerp(grad3(perm[AA], x, y, z), grad3(perm[BA], x - 1, y, z), u),
            lerp(grad3(perm[AB], x, y - 1, z), grad3(perm[BB], x - 1, y - 1, z), u),
            v
        ),
        lerp(
            lerp(grad3(perm[AA + 1], x, y, z - 1), grad3(perm[BA + 1], x - 1, y, z - 1), u),
            lerp(grad3(perm[AB + 1], x, y - 1, z - 1), grad3(perm[BB + 1], x - 1, y - 1, z - 1), u),
            v
        ),
        w
    );
}

float NoiseOps::perlin_2d(const Vector2& pos) {
    return perlin_2d_single(pos.x * frequency, pos.y * frequency);
}

float NoiseOps::perlin_3d(const Vector3& pos) {
    return perlin_3d_single(pos.x * frequency, pos.y * frequency, pos.z * frequency);
}

PackedFloat32Array NoiseOps::perlin_2d_batch(const PackedVector2Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = perlin_2d_single(src[i].x * frequency, src[i].y * frequency);
    }

    return result;
}

PackedFloat32Array NoiseOps::perlin_3d_batch(const PackedVector3Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = perlin_3d_single(src[i].x * frequency, src[i].y * frequency, src[i].z * frequency);
    }

    return result;
}

PackedFloat32Array NoiseOps::perlin_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height) {
    if (width <= 0 || height <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(width * height);
    float* dst = result.ptrw();

    for (int y = 0; y < height; y++) {
        float py = (origin.y + y * cell_size.y) * frequency;
        for (int x = 0; x < width; x++) {
            float px = (origin.x + x * cell_size.x) * frequency;
            dst[y * width + x] = perlin_2d_single(px, py);
        }
    }

    return result;
}

PackedFloat32Array NoiseOps::perlin_3d_grid(const Vector3& origin, const Vector3& cell_size, int width, int height, int depth) {
    if (width <= 0 || height <= 0 || depth <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(width * height * depth);
    float* dst = result.ptrw();

    for (int z = 0; z < depth; z++) {
        float pz = (origin.z + z * cell_size.z) * frequency;
        for (int y = 0; y < height; y++) {
            float py = (origin.y + y * cell_size.y) * frequency;
            for (int x = 0; x < width; x++) {
                float px = (origin.x + x * cell_size.x) * frequency;
                dst[z * width * height + y * width + x] = perlin_3d_single(px, py, pz);
            }
        }
    }

    return result;
}

// ========== SIMPLEX NOISE ==========

float NoiseOps::simplex_2d_single(float x, float y) const {
    float s = (x + y) * F2;
    int i = fast_floor(x + s);
    int j = fast_floor(y + s);

    float t = (i + j) * G2;
    float X0 = i - t;
    float Y0 = j - t;
    float x0 = x - X0;
    float y0 = y - Y0;

    int i1, j1;
    if (x0 > y0) {
        i1 = 1; j1 = 0;
    } else {
        i1 = 0; j1 = 1;
    }

    float x1 = x0 - i1 + G2;
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2;
    float y2 = y0 - 1.0f + 2.0f * G2;

    int ii = i & 255;
    int jj = j & 255;

    float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f;

    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 >= 0.0f) {
        t0 *= t0;
        n0 = t0 * t0 * grad2(perm[ii + perm[jj]], x0, y0);
    }

    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 >= 0.0f) {
        t1 *= t1;
        n1 = t1 * t1 * grad2(perm[ii + i1 + perm[jj + j1]], x1, y1);
    }

    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 >= 0.0f) {
        t2 *= t2;
        n2 = t2 * t2 * grad2(perm[ii + 1 + perm[jj + 1]], x2, y2);
    }

    return 70.0f * (n0 + n1 + n2);
}

float NoiseOps::simplex_3d_single(float x, float y, float z) const {
    float s = (x + y + z) * F3;
    int i = fast_floor(x + s);
    int j = fast_floor(y + s);
    int k = fast_floor(z + s);

    float t = (i + j + k) * G3;
    float X0 = i - t;
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = x - X0;
    float y0 = y - Y0;
    float z0 = z - Z0;

    int i1, j1, k1;
    int i2, j2, k2;

    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
        } else if (x0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
        } else {
            i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
        }
    } else {
        if (y0 < z0) {
            i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
        } else if (x0 < z0) {
            i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
        } else {
            i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
        }
    }

    float x1 = x0 - i1 + G3;
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3;
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3;
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    int ii = i & 255;
    int jj = j & 255;
    int kk = k & 255;

    float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;

    float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 >= 0.0f) {
        t0 *= t0;
        n0 = t0 * t0 * grad3(perm[ii + perm[jj + perm[kk]]], x0, y0, z0);
    }

    float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 >= 0.0f) {
        t1 *= t1;
        n1 = t1 * t1 * grad3(perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]], x1, y1, z1);
    }

    float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 >= 0.0f) {
        t2 *= t2;
        n2 = t2 * t2 * grad3(perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]], x2, y2, z2);
    }

    float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 >= 0.0f) {
        t3 *= t3;
        n3 = t3 * t3 * grad3(perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]], x3, y3, z3);
    }

    return 32.0f * (n0 + n1 + n2 + n3);
}

float NoiseOps::simplex_2d(const Vector2& pos) {
    return simplex_2d_single(pos.x * frequency, pos.y * frequency);
}

float NoiseOps::simplex_3d(const Vector3& pos) {
    return simplex_3d_single(pos.x * frequency, pos.y * frequency, pos.z * frequency);
}

PackedFloat32Array NoiseOps::simplex_2d_batch(const PackedVector2Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = simplex_2d_single(src[i].x * frequency, src[i].y * frequency);
    }

    return result;
}

PackedFloat32Array NoiseOps::simplex_3d_batch(const PackedVector3Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = simplex_3d_single(src[i].x * frequency, src[i].y * frequency, src[i].z * frequency);
    }

    return result;
}

PackedFloat32Array NoiseOps::simplex_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height) {
    if (width <= 0 || height <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(width * height);
    float* dst = result.ptrw();

    for (int y = 0; y < height; y++) {
        float py = (origin.y + y * cell_size.y) * frequency;
        for (int x = 0; x < width; x++) {
            float px = (origin.x + x * cell_size.x) * frequency;
            dst[y * width + x] = simplex_2d_single(px, py);
        }
    }

    return result;
}

// ========== WORLEY (CELLULAR) NOISE ==========

float NoiseOps::worley_2d_single(float x, float y) const {
    int xi = fast_floor(x);
    int yi = fast_floor(y);

    float min_dist = 999999.0f;

    // Check 3x3 neighborhood
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int cx = xi + dx;
            int cy = yi + dy;

            // Hash cell to get feature point position
            int h = perm[(perm[(cx & 255)] + (cy & 255)) & 255];

            float fx = cx + (h & 15) / 15.0f;
            float fy = cy + ((h >> 4) & 15) / 15.0f;

            float dist_x = x - fx;
            float dist_y = y - fy;
            float dist = dist_x * dist_x + dist_y * dist_y;

            if (dist < min_dist) {
                min_dist = dist;
            }
        }
    }

    return std::sqrt(min_dist);
}

float NoiseOps::worley_3d_single(float x, float y, float z) const {
    int xi = fast_floor(x);
    int yi = fast_floor(y);
    int zi = fast_floor(z);

    float min_dist = 999999.0f;

    // Check 3x3x3 neighborhood
    for (int dz = -1; dz <= 1; dz++) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int cx = xi + dx;
                int cy = yi + dy;
                int cz = zi + dz;

                int h = perm[(perm[(perm[(cx & 255)] + (cy & 255)) & 255] + (cz & 255)) & 255];

                float fx = cx + (h & 15) / 15.0f;
                float fy = cy + ((h >> 4) & 15) / 15.0f;
                float fz = cz + ((h >> 8) % 16) / 15.0f;

                float dist_x = x - fx;
                float dist_y = y - fy;
                float dist_z = z - fz;
                float dist = dist_x * dist_x + dist_y * dist_y + dist_z * dist_z;

                if (dist < min_dist) {
                    min_dist = dist;
                }
            }
        }
    }

    return std::sqrt(min_dist);
}

float NoiseOps::worley_2d(const Vector2& pos) {
    return worley_2d_single(pos.x * frequency, pos.y * frequency);
}

float NoiseOps::worley_3d(const Vector3& pos) {
    return worley_3d_single(pos.x * frequency, pos.y * frequency, pos.z * frequency);
}

PackedFloat32Array NoiseOps::worley_2d_batch(const PackedVector2Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = worley_2d_single(src[i].x * frequency, src[i].y * frequency);
    }

    return result;
}

PackedFloat32Array NoiseOps::worley_3d_batch(const PackedVector3Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        dst[i] = worley_3d_single(src[i].x * frequency, src[i].y * frequency, src[i].z * frequency);
    }

    return result;
}

PackedFloat32Array NoiseOps::worley_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height) {
    if (width <= 0 || height <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(width * height);
    float* dst = result.ptrw();

    for (int y = 0; y < height; y++) {
        float py = (origin.y + y * cell_size.y) * frequency;
        for (int x = 0; x < width; x++) {
            float px = (origin.x + x * cell_size.x) * frequency;
            dst[y * width + x] = worley_2d_single(px, py);
        }
    }

    return result;
}

// ========== FRACTAL BROWNIAN MOTION ==========

float NoiseOps::fbm_2d(const Vector2& pos) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = frequency;
    float max_amp = 0.0f;

    for (int i = 0; i < octaves; i++) {
        sum += perlin_2d_single(pos.x * freq, pos.y * freq) * amp;
        max_amp += amp;
        amp *= persistence;
        freq *= lacunarity;
    }

    return sum / max_amp;
}

float NoiseOps::fbm_3d(const Vector3& pos) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = frequency;
    float max_amp = 0.0f;

    for (int i = 0; i < octaves; i++) {
        sum += perlin_3d_single(pos.x * freq, pos.y * freq, pos.z * freq) * amp;
        max_amp += amp;
        amp *= persistence;
        freq *= lacunarity;
    }

    return sum / max_amp;
}

PackedFloat32Array NoiseOps::fbm_2d_batch(const PackedVector2Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float sum = 0.0f;
        float amp = 1.0f;
        float freq = frequency;
        float max_amp = 0.0f;

        for (int o = 0; o < octaves; o++) {
            sum += perlin_2d_single(src[i].x * freq, src[i].y * freq) * amp;
            max_amp += amp;
            amp *= persistence;
            freq *= lacunarity;
        }

        dst[i] = sum / max_amp;
    }

    return result;
}

PackedFloat32Array NoiseOps::fbm_3d_batch(const PackedVector3Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float sum = 0.0f;
        float amp = 1.0f;
        float freq = frequency;
        float max_amp = 0.0f;

        for (int o = 0; o < octaves; o++) {
            sum += perlin_3d_single(src[i].x * freq, src[i].y * freq, src[i].z * freq) * amp;
            max_amp += amp;
            amp *= persistence;
            freq *= lacunarity;
        }

        dst[i] = sum / max_amp;
    }

    return result;
}

PackedFloat32Array NoiseOps::fbm_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height) {
    if (width <= 0 || height <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(width * height);
    float* dst = result.ptrw();

    for (int y = 0; y < height; y++) {
        float py = origin.y + y * cell_size.y;
        for (int x = 0; x < width; x++) {
            float px = origin.x + x * cell_size.x;

            float sum = 0.0f;
            float amp = 1.0f;
            float freq = frequency;
            float max_amp = 0.0f;

            for (int o = 0; o < octaves; o++) {
                sum += perlin_2d_single(px * freq, py * freq) * amp;
                max_amp += amp;
                amp *= persistence;
                freq *= lacunarity;
            }

            dst[y * width + x] = sum / max_amp;
        }
    }

    return result;
}

// ========== RIDGED MULTIFRACTAL ==========

float NoiseOps::ridged_2d(const Vector2& pos) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = frequency;
    float weight = 1.0f;

    for (int i = 0; i < octaves; i++) {
        float n = perlin_2d_single(pos.x * freq, pos.y * freq);
        n = 1.0f - std::abs(n);
        n = n * n;
        n = n * weight;
        weight = std::min(1.0f, std::max(0.0f, n * 2.0f));

        sum += n * amp;
        amp *= persistence;
        freq *= lacunarity;
    }

    return sum;
}

float NoiseOps::ridged_3d(const Vector3& pos) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = frequency;
    float weight = 1.0f;

    for (int i = 0; i < octaves; i++) {
        float n = perlin_3d_single(pos.x * freq, pos.y * freq, pos.z * freq);
        n = 1.0f - std::abs(n);
        n = n * n;
        n = n * weight;
        weight = std::min(1.0f, std::max(0.0f, n * 2.0f));

        sum += n * amp;
        amp *= persistence;
        freq *= lacunarity;
    }

    return sum;
}

PackedFloat32Array NoiseOps::ridged_2d_batch(const PackedVector2Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float sum = 0.0f;
        float amp = 1.0f;
        float freq = frequency;
        float weight = 1.0f;

        for (int o = 0; o < octaves; o++) {
            float n = perlin_2d_single(src[i].x * freq, src[i].y * freq);
            n = 1.0f - std::abs(n);
            n = n * n;
            n = n * weight;
            weight = std::min(1.0f, std::max(0.0f, n * 2.0f));

            sum += n * amp;
            amp *= persistence;
            freq *= lacunarity;
        }

        dst[i] = sum;
    }

    return result;
}

PackedFloat32Array NoiseOps::ridged_3d_batch(const PackedVector3Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float sum = 0.0f;
        float amp = 1.0f;
        float freq = frequency;
        float weight = 1.0f;

        for (int o = 0; o < octaves; o++) {
            float n = perlin_3d_single(src[i].x * freq, src[i].y * freq, src[i].z * freq);
            n = 1.0f - std::abs(n);
            n = n * n;
            n = n * weight;
            weight = std::min(1.0f, std::max(0.0f, n * 2.0f));

            sum += n * amp;
            amp *= persistence;
            freq *= lacunarity;
        }

        dst[i] = sum;
    }

    return result;
}

PackedFloat32Array NoiseOps::ridged_2d_grid(const Vector2& origin, const Vector2& cell_size, int width, int height) {
    if (width <= 0 || height <= 0) return PackedFloat32Array();

    PackedFloat32Array result;
    result.resize(width * height);
    float* dst = result.ptrw();

    for (int y = 0; y < height; y++) {
        float py = origin.y + y * cell_size.y;
        for (int x = 0; x < width; x++) {
            float px = origin.x + x * cell_size.x;

            float sum = 0.0f;
            float amp = 1.0f;
            float freq = frequency;
            float weight = 1.0f;

            for (int o = 0; o < octaves; o++) {
                float n = perlin_2d_single(px * freq, py * freq);
                n = 1.0f - std::abs(n);
                n = n * n;
                n = n * weight;
                weight = std::min(1.0f, std::max(0.0f, n * 2.0f));

                sum += n * amp;
                amp *= persistence;
                freq *= lacunarity;
            }

            dst[y * width + x] = sum;
        }
    }

    return result;
}

// ========== TURBULENCE ==========

float NoiseOps::turbulence_2d(const Vector2& pos) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = frequency;

    for (int i = 0; i < octaves; i++) {
        sum += std::abs(perlin_2d_single(pos.x * freq, pos.y * freq)) * amp;
        amp *= persistence;
        freq *= lacunarity;
    }

    return sum;
}

float NoiseOps::turbulence_3d(const Vector3& pos) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = frequency;

    for (int i = 0; i < octaves; i++) {
        sum += std::abs(perlin_3d_single(pos.x * freq, pos.y * freq, pos.z * freq)) * amp;
        amp *= persistence;
        freq *= lacunarity;
    }

    return sum;
}

PackedFloat32Array NoiseOps::turbulence_2d_batch(const PackedVector2Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector2* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float sum = 0.0f;
        float amp = 1.0f;
        float freq = frequency;

        for (int o = 0; o < octaves; o++) {
            sum += std::abs(perlin_2d_single(src[i].x * freq, src[i].y * freq)) * amp;
            amp *= persistence;
            freq *= lacunarity;
        }

        dst[i] = sum;
    }

    return result;
}

PackedFloat32Array NoiseOps::turbulence_3d_batch(const PackedVector3Array& positions) {
    int count = positions.size();
    PackedFloat32Array result;
    result.resize(count);

    const Vector3* src = positions.ptr();
    float* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float sum = 0.0f;
        float amp = 1.0f;
        float freq = frequency;

        for (int o = 0; o < octaves; o++) {
            sum += std::abs(perlin_3d_single(src[i].x * freq, src[i].y * freq, src[i].z * freq)) * amp;
            amp *= persistence;
            freq *= lacunarity;
        }

        dst[i] = sum;
    }

    return result;
}

// ========== DOMAIN WARPING ==========

PackedVector2Array NoiseOps::warp_2d(const PackedVector2Array& positions, float strength) {
    int count = positions.size();
    PackedVector2Array result;
    result.resize(count);

    const Vector2* src = positions.ptr();
    Vector2* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float wx = perlin_2d_single(src[i].x * frequency, src[i].y * frequency);
        float wy = perlin_2d_single(src[i].x * frequency + 5.2f, src[i].y * frequency + 1.3f);

        dst[i] = Vector2(src[i].x + wx * strength, src[i].y + wy * strength);
    }

    return result;
}

PackedVector3Array NoiseOps::warp_3d(const PackedVector3Array& positions, float strength) {
    int count = positions.size();
    PackedVector3Array result;
    result.resize(count);

    const Vector3* src = positions.ptr();
    Vector3* dst = result.ptrw();

    for (int i = 0; i < count; i++) {
        float wx = perlin_3d_single(src[i].x * frequency, src[i].y * frequency, src[i].z * frequency);
        float wy = perlin_3d_single(src[i].x * frequency + 5.2f, src[i].y * frequency + 1.3f, src[i].z * frequency + 2.8f);
        float wz = perlin_3d_single(src[i].x * frequency + 9.1f, src[i].y * frequency + 4.7f, src[i].z * frequency + 6.3f);

        dst[i] = Vector3(
            src[i].x + wx * strength,
            src[i].y + wy * strength,
            src[i].z + wz * strength
        );
    }

    return result;
}

}

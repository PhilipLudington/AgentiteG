/**
 * SpatialHash3D - Fast spatial queries for 3D games
 *
 * A hash grid implementation that provides O(1) average-case spatial lookups.
 * Ideal for:
 * - Finding nearby enemies/allies in 3D space
 * - Collision detection broad phase
 * - Range-based ability targeting
 * - Proximity alerts in 3D environments
 *
 * Usage:
 *   var spatial = SpatialHash3D.new()
 *   spatial.cell_size = 64.0
 *   spatial.build(positions)  # PackedVector3Array
 *   var nearby = spatial.query_radius(origin, radius)  # Returns indices
 */

#ifndef AGENTITE_SPATIAL_HASH_3D_HPP
#define AGENTITE_SPATIAL_HASH_3D_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/array.hpp>

#include <unordered_map>
#include <vector>

namespace godot {

class SpatialHash3D : public RefCounted {
    GDCLASS(SpatialHash3D, RefCounted)

private:
    float cell_size = 64.0f;
    std::unordered_map<uint64_t, std::vector<int32_t>> cells;
    PackedVector3Array stored_positions;
    int32_t item_count = 0;

    // Hash a position to a cell key
    uint64_t hash_position(const Vector3& pos) const;

    // Get cell coordinates
    void get_cell_coords(const Vector3& pos, int32_t& cx, int32_t& cy, int32_t& cz) const;

    // Combine cell coordinates into hash key
    uint64_t coords_to_key(int32_t cx, int32_t cy, int32_t cz) const;

protected:
    static void _bind_methods();

public:
    SpatialHash3D();
    ~SpatialHash3D();

    // Properties
    void set_cell_size(float p_size);
    float get_cell_size() const;

    // Get count of items in the hash
    int32_t get_count() const;

    // Build the hash from a position array
    // This clears existing data and rebuilds from scratch
    void build(const PackedVector3Array& positions);

    // Clear all data
    void clear();

    // Insert a single item (returns the index assigned)
    int32_t insert(const Vector3& position);

    // Update a single item's position
    void update(int32_t index, const Vector3& new_position);

    // Query: find all items within radius of origin
    // Returns PackedInt32Array of indices into the original positions array
    PackedInt32Array query_radius(const Vector3& origin, float radius) const;

    // Query: find all items within an AABB (axis-aligned bounding box)
    PackedInt32Array query_box(const AABB& box) const;

    // Query: find k nearest items to origin
    PackedInt32Array query_nearest(const Vector3& origin, int32_t k) const;

    // Query: find nearest single item (convenience method)
    int32_t query_nearest_one(const Vector3& origin) const;

    // Batch query: multiple radius queries at once
    // Returns Array of PackedInt32Array, one per query
    Array query_radius_batch(
        const PackedVector3Array& origins,
        const PackedFloat32Array& radii
    ) const;

    // Batch query: same radius for all queries
    Array query_radius_batch_uniform(
        const PackedVector3Array& origins,
        float radius
    ) const;

    // Check if any item is within radius (faster than query when you just need bool)
    bool has_any_in_radius(const Vector3& origin, float radius) const;

    // Count items in radius (faster than query().size() when you just need count)
    int32_t count_in_radius(const Vector3& origin, float radius) const;
};

}

#endif // AGENTITE_SPATIAL_HASH_3D_HPP

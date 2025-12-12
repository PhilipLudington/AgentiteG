/**
 * SpatialHash2D - Fast spatial queries for 2D games
 *
 * A hash grid implementation that provides O(1) average-case spatial lookups.
 * Ideal for:
 * - Finding nearby enemies/allies
 * - Collision detection broad phase
 * - Range-based ability targeting
 * - Proximity alerts
 *
 * Usage:
 *   var spatial = SpatialHash2D.new()
 *   spatial.cell_size = 64.0
 *   spatial.build(positions)  # PackedVector2Array
 *   var nearby = spatial.query_radius(origin, radius)  # Returns indices
 */

#ifndef AGENTITE_SPATIAL_HASH_2D_HPP
#define AGENTITE_SPATIAL_HASH_2D_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/array.hpp>

#include <unordered_map>
#include <vector>

namespace godot {

class SpatialHash2D : public RefCounted {
    GDCLASS(SpatialHash2D, RefCounted)

private:
    float cell_size = 64.0f;
    std::unordered_map<int64_t, std::vector<int32_t>> cells;
    PackedVector2Array stored_positions;
    int32_t item_count = 0;

    // Hash a position to a cell key
    int64_t hash_position(const Vector2& pos) const;

    // Get cell coordinates
    void get_cell_coords(const Vector2& pos, int32_t& cx, int32_t& cy) const;

protected:
    static void _bind_methods();

public:
    SpatialHash2D();
    ~SpatialHash2D();

    // Properties
    void set_cell_size(float p_size);
    float get_cell_size() const;

    // Get count of items in the hash
    int32_t get_count() const;

    // Build the hash from a position array
    // This clears existing data and rebuilds from scratch
    void build(const PackedVector2Array& positions);

    // Clear all data
    void clear();

    // Insert a single item (returns the index assigned)
    int32_t insert(const Vector2& position);

    // Update a single item's position
    void update(int32_t index, const Vector2& new_position);

    // Query: find all items within radius of origin
    // Returns PackedInt32Array of indices into the original positions array
    PackedInt32Array query_radius(const Vector2& origin, float radius) const;

    // Query: find all items within a rectangle
    PackedInt32Array query_rect(const Rect2& rect) const;

    // Query: find k nearest items to origin
    PackedInt32Array query_nearest(const Vector2& origin, int32_t k) const;

    // Query: find nearest single item (convenience method)
    int32_t query_nearest_one(const Vector2& origin) const;

    // Batch query: multiple radius queries at once
    // Returns Array of PackedInt32Array, one per query
    Array query_radius_batch(
        const PackedVector2Array& origins,
        const PackedFloat32Array& radii
    ) const;

    // Batch query: same radius for all queries
    Array query_radius_batch_uniform(
        const PackedVector2Array& origins,
        float radius
    ) const;

    // Check if any item is within radius (faster than query when you just need bool)
    bool has_any_in_radius(const Vector2& origin, float radius) const;

    // Count items in radius (faster than query().size() when you just need count)
    int32_t count_in_radius(const Vector2& origin, float radius) const;
};

}

#endif // AGENTITE_SPATIAL_HASH_2D_HPP

/**
 * KDTree2D - K-D Tree for fast nearest neighbor queries in 2D
 *
 * A static binary space partitioning tree that provides O(log n) average-case
 * nearest neighbor lookups. Unlike SpatialHash2D, this structure is optimized
 * for nearest-neighbor queries rather than range queries.
 *
 * Ideal for:
 * - Finding the k nearest neighbors
 * - Fast single nearest neighbor queries
 * - Scenarios where data doesn't change frequently (static builds)
 *
 * Trade-offs vs SpatialHash2D:
 * - Faster for k-nearest queries
 * - No insert/update (must rebuild for changes)
 * - More memory efficient for sparse distributions
 *
 * Usage:
 *   var kdtree = KDTree2D.new()
 *   kdtree.build(positions)  # PackedVector2Array
 *   var nearest = kdtree.query_nearest_one(target)  # Single nearest index
 *   var k_nearest = kdtree.query_nearest(target, k)  # k nearest indices
 */

#ifndef AGENTITE_KD_TREE_2D_HPP
#define AGENTITE_KD_TREE_2D_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/array.hpp>

#include <vector>
#include <queue>

namespace godot {

class KDTree2D : public RefCounted {
    GDCLASS(KDTree2D, RefCounted)

private:
    // Node structure for the KD-Tree
    struct Node {
        int32_t point_index;  // Index into original points array
        int32_t left;         // Index of left child (-1 if none)
        int32_t right;        // Index of right child (-1 if none)
        uint8_t axis;         // Split axis (0=x, 1=y)
    };

    std::vector<Node> nodes;
    PackedVector2Array stored_points;
    int32_t root_index = -1;

    // Build tree recursively, returns node index
    int32_t build_recursive(std::vector<int32_t>& indices, int32_t start, int32_t end, int depth);

    // Nearest neighbor search helpers
    void nearest_one_recursive(int32_t node_idx, const Vector2& target,
                                int32_t& best_idx, float& best_dist_sq) const;

    void nearest_k_recursive(int32_t node_idx, const Vector2& target, int32_t k,
                              std::priority_queue<std::pair<float, int32_t>>& heap) const;

    void radius_recursive(int32_t node_idx, const Vector2& target, float radius_sq,
                           PackedInt32Array& results) const;

protected:
    static void _bind_methods();

public:
    KDTree2D();
    ~KDTree2D();

    // Build the tree from a position array
    // This clears existing data and rebuilds from scratch
    void build(const PackedVector2Array& points);

    // Clear all data
    void clear();

    // Get number of points in the tree
    int32_t size() const;

    // Query: find nearest single point to target
    // Returns index into original points array, or -1 if empty
    int32_t query_nearest_one(const Vector2& point) const;

    // Query: find k nearest points to target
    // Returns PackedInt32Array of indices, sorted by distance (nearest first)
    PackedInt32Array query_nearest(const Vector2& point, int32_t k) const;

    // Query: find all points within radius
    // Returns PackedInt32Array of indices (not sorted)
    PackedInt32Array query_radius(const Vector2& point, float radius) const;

    // Batch query: find nearest one for multiple query points
    // Returns PackedInt32Array of indices, one per query point
    PackedInt32Array query_nearest_one_batch(const PackedVector2Array& points) const;

    // Batch query: find k nearest for multiple query points
    // Returns Array of PackedInt32Array, one per query point
    Array query_nearest_batch(const PackedVector2Array& points, int32_t k) const;
};

}

#endif // AGENTITE_KD_TREE_2D_HPP

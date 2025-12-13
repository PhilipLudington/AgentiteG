/**
 * Octree - Adaptive spatial subdivision for 3D
 *
 * A tree structure that recursively subdivides 3D space into octants.
 * The 3D equivalent of QuadTree.
 *
 * Ideal for:
 * - Non-uniform point distributions in 3D
 * - 3D collision detection broad phase
 * - Frustum culling
 * - Level-of-detail systems
 *
 * Usage:
 *   var octree = Octree.new()
 *   octree.set_bounds(AABB(Vector3.ZERO, Vector3(1000, 1000, 1000)))
 *   octree.build(positions)  # PackedVector3Array
 *   var in_area = octree.query_box(selection_box)
 */

#ifndef AGENTITE_OCTREE_HPP
#define AGENTITE_OCTREE_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/array.hpp>

#include <vector>
#include <memory>

namespace godot {

class Octree : public RefCounted {
    GDCLASS(Octree, RefCounted)

private:
    struct Node {
        AABB bounds;
        std::vector<int32_t> point_indices;  // Points stored at this node
        std::unique_ptr<Node> children[8];   // 8 octants
        bool is_leaf = true;

        Node(const AABB& b) : bounds(b) {}
    };

    std::unique_ptr<Node> root;
    PackedVector3Array stored_points;
    AABB tree_bounds;
    int32_t max_depth = 8;
    int32_t max_items_per_node = 8;
    int32_t item_count = 0;

    // Insert point into a node recursively
    void insert_into_node(Node* node, int32_t point_index, int depth);

    // Subdivide a node into 8 children
    void subdivide(Node* node, int depth);

    // Query helpers
    void query_box_recursive(const Node* node, const AABB& box, PackedInt32Array& results) const;
    void query_radius_recursive(const Node* node, const Vector3& center, float radius_sq,
                                 PackedInt32Array& results) const;

    // Get all node bounds for visualization
    void collect_node_bounds(const Node* node, Array& bounds) const;

    // Get octant index for a point (0-7)
    int get_octant(const AABB& bounds, const Vector3& point) const;

protected:
    static void _bind_methods();

public:
    Octree();
    ~Octree();

    // Configuration
    void set_bounds(const AABB& bounds);
    AABB get_bounds() const;

    void set_max_depth(int32_t depth);
    int32_t get_max_depth() const;

    void set_max_items_per_node(int32_t n);
    int32_t get_max_items_per_node() const;

    // Build the tree from a position array
    // This clears existing data and rebuilds from scratch
    void build(const PackedVector3Array& positions);

    // Insert a single point (returns the index assigned)
    int32_t insert(const Vector3& position);

    // Clear all data
    void clear();

    // Get number of points in the tree
    int32_t size() const;

    // Query: find all points within an AABB
    // Returns PackedInt32Array of indices
    PackedInt32Array query_box(const AABB& box) const;

    // Query: find all points within radius of center
    // Returns PackedInt32Array of indices
    PackedInt32Array query_radius(const Vector3& center, float radius) const;

    // Debug: get bounds of all nodes for visualization
    // Returns Array of AABB
    Array get_node_bounds() const;
};

}

#endif // AGENTITE_OCTREE_HPP

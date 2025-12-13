/**
 * QuadTree - Adaptive spatial subdivision for 2D
 *
 * A tree structure that recursively subdivides 2D space into quadrants.
 * Adapts to point density, subdividing only where needed.
 *
 * Ideal for:
 * - Non-uniform point distributions
 * - Collision detection broad phase
 * - Frustum culling
 * - Level-of-detail systems
 *
 * Trade-offs vs SpatialHash2D:
 * - Better for non-uniform distributions
 * - Adaptive memory usage
 * - Supports dynamic insertion
 * - Slightly slower for uniform distributions
 *
 * Usage:
 *   var quadtree = QuadTree.new()
 *   quadtree.set_bounds(Rect2(0, 0, 1000, 1000))
 *   quadtree.build(positions)  # PackedVector2Array
 *   var in_area = quadtree.query_rect(selection_rect)
 */

#ifndef AGENTITE_QUAD_TREE_HPP
#define AGENTITE_QUAD_TREE_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/array.hpp>

#include <vector>
#include <memory>

namespace godot {

class QuadTree : public RefCounted {
    GDCLASS(QuadTree, RefCounted)

private:
    struct Node {
        Rect2 bounds;
        std::vector<int32_t> point_indices;  // Points stored at this node
        std::unique_ptr<Node> children[4];   // NW, NE, SW, SE
        bool is_leaf = true;

        Node(const Rect2& b) : bounds(b) {}
    };

    std::unique_ptr<Node> root;
    PackedVector2Array stored_points;
    Rect2 tree_bounds;
    int32_t max_depth = 8;
    int32_t max_items_per_node = 8;
    int32_t item_count = 0;

    // Insert point into a node recursively
    void insert_into_node(Node* node, int32_t point_index, int depth);

    // Subdivide a node into 4 children
    void subdivide(Node* node, int depth);

    // Query helpers
    void query_rect_recursive(const Node* node, const Rect2& rect, PackedInt32Array& results) const;
    void query_radius_recursive(const Node* node, const Vector2& center, float radius_sq,
                                 PackedInt32Array& results) const;

    // Get all node bounds for visualization
    void collect_node_bounds(const Node* node, Array& bounds) const;

    // Get quadrant index for a point (0=NW, 1=NE, 2=SW, 3=SE)
    int get_quadrant(const Rect2& bounds, const Vector2& point) const;

protected:
    static void _bind_methods();

public:
    QuadTree();
    ~QuadTree();

    // Configuration
    void set_bounds(const Rect2& bounds);
    Rect2 get_bounds() const;

    void set_max_depth(int32_t depth);
    int32_t get_max_depth() const;

    void set_max_items_per_node(int32_t n);
    int32_t get_max_items_per_node() const;

    // Build the tree from a position array
    // This clears existing data and rebuilds from scratch
    void build(const PackedVector2Array& positions);

    // Insert a single point (returns the index assigned)
    int32_t insert(const Vector2& position);

    // Clear all data
    void clear();

    // Get number of points in the tree
    int32_t size() const;

    // Query: find all points within a rectangle
    // Returns PackedInt32Array of indices
    PackedInt32Array query_rect(const Rect2& rect) const;

    // Query: find all points within radius of center
    // Returns PackedInt32Array of indices
    PackedInt32Array query_radius(const Vector2& center, float radius) const;

    // Debug: get bounds of all nodes for visualization
    // Returns Array of Rect2
    Array get_node_bounds() const;
};

}

#endif // AGENTITE_QUAD_TREE_HPP

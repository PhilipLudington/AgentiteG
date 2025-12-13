/**
 * Octree Implementation
 */

#include "octree.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <algorithm>

namespace godot {

void Octree::_bind_methods() {
    // Configuration
    ClassDB::bind_method(D_METHOD("set_bounds", "bounds"), &Octree::set_bounds);
    ClassDB::bind_method(D_METHOD("get_bounds"), &Octree::get_bounds);
    ADD_PROPERTY(PropertyInfo(Variant::AABB, "bounds"), "set_bounds", "get_bounds");

    ClassDB::bind_method(D_METHOD("set_max_depth", "depth"), &Octree::set_max_depth);
    ClassDB::bind_method(D_METHOD("get_max_depth"), &Octree::get_max_depth);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_depth"), "set_max_depth", "get_max_depth");

    ClassDB::bind_method(D_METHOD("set_max_items_per_node", "count"), &Octree::set_max_items_per_node);
    ClassDB::bind_method(D_METHOD("get_max_items_per_node"), &Octree::get_max_items_per_node);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_items_per_node"), "set_max_items_per_node", "get_max_items_per_node");

    // Core methods
    ClassDB::bind_method(D_METHOD("build", "positions"), &Octree::build);
    ClassDB::bind_method(D_METHOD("insert", "position"), &Octree::insert);
    ClassDB::bind_method(D_METHOD("clear"), &Octree::clear);
    ClassDB::bind_method(D_METHOD("size"), &Octree::size);

    // Query methods
    ClassDB::bind_method(D_METHOD("query_box", "box"), &Octree::query_box);
    ClassDB::bind_method(D_METHOD("query_radius", "center", "radius"), &Octree::query_radius);

    // Debug
    ClassDB::bind_method(D_METHOD("get_node_bounds"), &Octree::get_node_bounds);
}

Octree::Octree() {
    tree_bounds = AABB(Vector3(), Vector3(1000, 1000, 1000));  // Default bounds
}

Octree::~Octree() {
    clear();
}

void Octree::set_bounds(const AABB& bounds) {
    tree_bounds = bounds;
    // Clear existing data when bounds change
    clear();
}

AABB Octree::get_bounds() const {
    return tree_bounds;
}

void Octree::set_max_depth(int32_t depth) {
    if (depth > 0 && depth <= 16) {
        max_depth = depth;
    }
}

int32_t Octree::get_max_depth() const {
    return max_depth;
}

void Octree::set_max_items_per_node(int32_t n) {
    if (n > 0) {
        max_items_per_node = n;
    }
}

int32_t Octree::get_max_items_per_node() const {
    return max_items_per_node;
}

void Octree::build(const PackedVector3Array& positions) {
    clear();

    int32_t n = positions.size();
    if (n == 0) {
        return;
    }

    stored_points = positions;

    // Auto-detect bounds if default or invalid
    if (tree_bounds.size.x <= 0 || tree_bounds.size.y <= 0 || tree_bounds.size.z <= 0) {
        const Vector3* pos_ptr = positions.ptr();
        Vector3 min_p = pos_ptr[0];
        Vector3 max_p = pos_ptr[0];

        for (int32_t i = 1; i < n; i++) {
            min_p.x = std::min(min_p.x, pos_ptr[i].x);
            min_p.y = std::min(min_p.y, pos_ptr[i].y);
            min_p.z = std::min(min_p.z, pos_ptr[i].z);
            max_p.x = std::max(max_p.x, pos_ptr[i].x);
            max_p.y = std::max(max_p.y, pos_ptr[i].y);
            max_p.z = std::max(max_p.z, pos_ptr[i].z);
        }

        // Add small padding
        Vector3 padding = (max_p - min_p) * 0.01f;
        tree_bounds = AABB(min_p - padding, (max_p - min_p) + padding * 2.0f);
    }

    // Create root node
    root = std::make_unique<Node>(tree_bounds);

    // Insert all points
    for (int32_t i = 0; i < n; i++) {
        insert_into_node(root.get(), i, 0);
    }

    item_count = n;
}

int32_t Octree::insert(const Vector3& position) {
    // Create root if needed
    if (!root) {
        root = std::make_unique<Node>(tree_bounds);
    }

    int32_t index = stored_points.size();
    stored_points.push_back(position);
    insert_into_node(root.get(), index, 0);
    item_count++;

    return index;
}

void Octree::insert_into_node(Node* node, int32_t point_index, int depth) {
    const Vector3& point = stored_points[point_index];

    // If point is outside bounds, skip
    if (!node->bounds.has_point(point)) {
        return;
    }

    // If this is a leaf node
    if (node->is_leaf) {
        node->point_indices.push_back(point_index);

        // Check if we need to subdivide
        if (static_cast<int32_t>(node->point_indices.size()) > max_items_per_node && depth < max_depth) {
            subdivide(node, depth);
        }
    } else {
        // Route to appropriate child
        int octant = get_octant(node->bounds, point);
        if (node->children[octant]) {
            insert_into_node(node->children[octant].get(), point_index, depth + 1);
        }
    }
}

void Octree::subdivide(Node* node, int depth) {
    const AABB& b = node->bounds;
    Vector3 center = b.position + b.size * 0.5f;
    Vector3 half_size = b.size * 0.5f;

    // Create 8 children
    // Octants are indexed by 3 bits: X (bit 0), Y (bit 1), Z (bit 2)
    // 0 = -X -Y -Z, 1 = +X -Y -Z, 2 = -X +Y -Z, 3 = +X +Y -Z
    // 4 = -X -Y +Z, 5 = +X -Y +Z, 6 = -X +Y +Z, 7 = +X +Y +Z
    for (int i = 0; i < 8; i++) {
        Vector3 child_pos;
        child_pos.x = (i & 1) ? center.x : b.position.x;
        child_pos.y = (i & 2) ? center.y : b.position.y;
        child_pos.z = (i & 4) ? center.z : b.position.z;
        node->children[i] = std::make_unique<Node>(AABB(child_pos, half_size));
    }

    node->is_leaf = false;

    // Re-insert all points from this node into children
    for (int32_t idx : node->point_indices) {
        const Vector3& point = stored_points[idx];
        int octant = get_octant(b, point);
        insert_into_node(node->children[octant].get(), idx, depth + 1);
    }

    // Clear points from this node
    node->point_indices.clear();
    node->point_indices.shrink_to_fit();
}

int Octree::get_octant(const AABB& bounds, const Vector3& point) const {
    Vector3 center = bounds.position + bounds.size * 0.5f;

    int octant = 0;
    if (point.x >= center.x) octant |= 1;
    if (point.y >= center.y) octant |= 2;
    if (point.z >= center.z) octant |= 4;

    return octant;
}

void Octree::clear() {
    root.reset();
    stored_points.resize(0);
    item_count = 0;
}

int32_t Octree::size() const {
    return item_count;
}

PackedInt32Array Octree::query_box(const AABB& box) const {
    PackedInt32Array results;

    if (!root) {
        return results;
    }

    query_box_recursive(root.get(), box, results);
    return results;
}

void Octree::query_box_recursive(const Node* node, const AABB& box, PackedInt32Array& results) const {
    // Check if query box intersects this node's bounds
    if (!node->bounds.intersects(box)) {
        return;
    }

    if (node->is_leaf) {
        // Check all points in this leaf
        for (int32_t idx : node->point_indices) {
            if (box.has_point(stored_points[idx])) {
                results.push_back(idx);
            }
        }
    } else {
        // Recurse into children
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                query_box_recursive(node->children[i].get(), box, results);
            }
        }
    }
}

PackedInt32Array Octree::query_radius(const Vector3& center, float radius) const {
    PackedInt32Array results;

    if (!root || radius <= 0.0f) {
        return results;
    }

    float radius_sq = radius * radius;
    query_radius_recursive(root.get(), center, radius_sq, results);
    return results;
}

void Octree::query_radius_recursive(const Node* node, const Vector3& center, float radius_sq,
                                     PackedInt32Array& results) const {
    // Check if sphere intersects this node's bounds
    // Use AABB of sphere for quick rejection
    float radius = std::sqrt(radius_sq);
    AABB sphere_bounds(
        Vector3(center.x - radius, center.y - radius, center.z - radius),
        Vector3(radius * 2, radius * 2, radius * 2)
    );

    if (!node->bounds.intersects(sphere_bounds)) {
        return;
    }

    if (node->is_leaf) {
        // Check all points in this leaf
        for (int32_t idx : node->point_indices) {
            if (center.distance_squared_to(stored_points[idx]) <= radius_sq) {
                results.push_back(idx);
            }
        }
    } else {
        // Recurse into children
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                query_radius_recursive(node->children[i].get(), center, radius_sq, results);
            }
        }
    }
}

Array Octree::get_node_bounds() const {
    Array bounds;

    if (root) {
        collect_node_bounds(root.get(), bounds);
    }

    return bounds;
}

void Octree::collect_node_bounds(const Node* node, Array& bounds) const {
    bounds.push_back(node->bounds);

    if (!node->is_leaf) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i]) {
                collect_node_bounds(node->children[i].get(), bounds);
            }
        }
    }
}

}

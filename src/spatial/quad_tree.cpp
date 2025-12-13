/**
 * QuadTree Implementation
 */

#include "quad_tree.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>

namespace godot {

void QuadTree::_bind_methods() {
    // Configuration
    ClassDB::bind_method(D_METHOD("set_bounds", "bounds"), &QuadTree::set_bounds);
    ClassDB::bind_method(D_METHOD("get_bounds"), &QuadTree::get_bounds);
    ADD_PROPERTY(PropertyInfo(Variant::RECT2, "bounds"), "set_bounds", "get_bounds");

    ClassDB::bind_method(D_METHOD("set_max_depth", "depth"), &QuadTree::set_max_depth);
    ClassDB::bind_method(D_METHOD("get_max_depth"), &QuadTree::get_max_depth);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_depth"), "set_max_depth", "get_max_depth");

    ClassDB::bind_method(D_METHOD("set_max_items_per_node", "count"), &QuadTree::set_max_items_per_node);
    ClassDB::bind_method(D_METHOD("get_max_items_per_node"), &QuadTree::get_max_items_per_node);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_items_per_node"), "set_max_items_per_node", "get_max_items_per_node");

    // Core methods
    ClassDB::bind_method(D_METHOD("build", "positions"), &QuadTree::build);
    ClassDB::bind_method(D_METHOD("insert", "position"), &QuadTree::insert);
    ClassDB::bind_method(D_METHOD("clear"), &QuadTree::clear);
    ClassDB::bind_method(D_METHOD("size"), &QuadTree::size);

    // Query methods
    ClassDB::bind_method(D_METHOD("query_rect", "rect"), &QuadTree::query_rect);
    ClassDB::bind_method(D_METHOD("query_radius", "center", "radius"), &QuadTree::query_radius);

    // Debug
    ClassDB::bind_method(D_METHOD("get_node_bounds"), &QuadTree::get_node_bounds);
}

QuadTree::QuadTree() {
    tree_bounds = Rect2(0, 0, 1000, 1000);  // Default bounds
}

QuadTree::~QuadTree() {
    clear();
}

void QuadTree::set_bounds(const Rect2& bounds) {
    tree_bounds = bounds;
    // Clear existing data when bounds change
    clear();
}

Rect2 QuadTree::get_bounds() const {
    return tree_bounds;
}

void QuadTree::set_max_depth(int32_t depth) {
    if (depth > 0 && depth <= 16) {
        max_depth = depth;
    }
}

int32_t QuadTree::get_max_depth() const {
    return max_depth;
}

void QuadTree::set_max_items_per_node(int32_t n) {
    if (n > 0) {
        max_items_per_node = n;
    }
}

int32_t QuadTree::get_max_items_per_node() const {
    return max_items_per_node;
}

void QuadTree::build(const PackedVector2Array& positions) {
    clear();

    int32_t n = positions.size();
    if (n == 0) {
        return;
    }

    stored_points = positions;

    // Auto-detect bounds if default
    if (tree_bounds.size.x <= 0 || tree_bounds.size.y <= 0) {
        const Vector2* pos_ptr = positions.ptr();
        Vector2 min_p = pos_ptr[0];
        Vector2 max_p = pos_ptr[0];

        for (int32_t i = 1; i < n; i++) {
            min_p.x = std::min(min_p.x, pos_ptr[i].x);
            min_p.y = std::min(min_p.y, pos_ptr[i].y);
            max_p.x = std::max(max_p.x, pos_ptr[i].x);
            max_p.y = std::max(max_p.y, pos_ptr[i].y);
        }

        // Add small padding
        Vector2 padding = (max_p - min_p) * 0.01f;
        tree_bounds = Rect2(min_p - padding, (max_p - min_p) + padding * 2.0f);
    }

    // Create root node
    root = std::make_unique<Node>(tree_bounds);

    // Insert all points
    for (int32_t i = 0; i < n; i++) {
        insert_into_node(root.get(), i, 0);
    }

    item_count = n;
}

int32_t QuadTree::insert(const Vector2& position) {
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

void QuadTree::insert_into_node(Node* node, int32_t point_index, int depth) {
    const Vector2& point = stored_points[point_index];

    // If point is outside bounds, skip (shouldn't happen normally)
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
        int quadrant = get_quadrant(node->bounds, point);
        if (node->children[quadrant]) {
            insert_into_node(node->children[quadrant].get(), point_index, depth + 1);
        }
    }
}

void QuadTree::subdivide(Node* node, int depth) {
    const Rect2& b = node->bounds;
    Vector2 center = b.position + b.size * 0.5f;
    Vector2 half_size = b.size * 0.5f;

    // Create children: NW, NE, SW, SE
    node->children[0] = std::make_unique<Node>(Rect2(b.position, half_size));  // NW
    node->children[1] = std::make_unique<Node>(Rect2(Vector2(center.x, b.position.y), half_size));  // NE
    node->children[2] = std::make_unique<Node>(Rect2(Vector2(b.position.x, center.y), half_size));  // SW
    node->children[3] = std::make_unique<Node>(Rect2(center, half_size));  // SE

    node->is_leaf = false;

    // Re-insert all points from this node into children
    for (int32_t idx : node->point_indices) {
        const Vector2& point = stored_points[idx];
        int quadrant = get_quadrant(b, point);
        insert_into_node(node->children[quadrant].get(), idx, depth + 1);
    }

    // Clear points from this node (they're now in children)
    node->point_indices.clear();
    node->point_indices.shrink_to_fit();
}

int QuadTree::get_quadrant(const Rect2& bounds, const Vector2& point) const {
    Vector2 center = bounds.position + bounds.size * 0.5f;

    // 0=NW (top-left), 1=NE (top-right), 2=SW (bottom-left), 3=SE (bottom-right)
    // Using screen coordinates where Y increases downward
    bool east = point.x >= center.x;
    bool south = point.y >= center.y;

    if (!east && !south) return 0;  // NW
    if (east && !south) return 1;   // NE
    if (!east && south) return 2;   // SW
    return 3;                        // SE
}

void QuadTree::clear() {
    root.reset();
    stored_points.resize(0);
    item_count = 0;
}

int32_t QuadTree::size() const {
    return item_count;
}

PackedInt32Array QuadTree::query_rect(const Rect2& rect) const {
    PackedInt32Array results;

    if (!root) {
        return results;
    }

    query_rect_recursive(root.get(), rect, results);
    return results;
}

void QuadTree::query_rect_recursive(const Node* node, const Rect2& rect, PackedInt32Array& results) const {
    // Check if query rect intersects this node's bounds
    if (!node->bounds.intersects(rect)) {
        return;
    }

    if (node->is_leaf) {
        // Check all points in this leaf
        for (int32_t idx : node->point_indices) {
            if (rect.has_point(stored_points[idx])) {
                results.push_back(idx);
            }
        }
    } else {
        // Recurse into children
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                query_rect_recursive(node->children[i].get(), rect, results);
            }
        }
    }
}

PackedInt32Array QuadTree::query_radius(const Vector2& center, float radius) const {
    PackedInt32Array results;

    if (!root || radius <= 0.0f) {
        return results;
    }

    float radius_sq = radius * radius;
    query_radius_recursive(root.get(), center, radius_sq, results);
    return results;
}

void QuadTree::query_radius_recursive(const Node* node, const Vector2& center, float radius_sq,
                                       PackedInt32Array& results) const {
    // Check if circle intersects this node's bounds
    // Use AABB of circle for quick rejection
    float radius = std::sqrt(radius_sq);
    Rect2 circle_bounds(center.x - radius, center.y - radius, radius * 2, radius * 2);

    if (!node->bounds.intersects(circle_bounds)) {
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
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                query_radius_recursive(node->children[i].get(), center, radius_sq, results);
            }
        }
    }
}

Array QuadTree::get_node_bounds() const {
    Array bounds;

    if (root) {
        collect_node_bounds(root.get(), bounds);
    }

    return bounds;
}

void QuadTree::collect_node_bounds(const Node* node, Array& bounds) const {
    bounds.push_back(node->bounds);

    if (!node->is_leaf) {
        for (int i = 0; i < 4; i++) {
            if (node->children[i]) {
                collect_node_bounds(node->children[i].get(), bounds);
            }
        }
    }
}

}

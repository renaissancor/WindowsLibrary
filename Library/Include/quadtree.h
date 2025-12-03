#pragma once 

// quadtree.h 

struct aabb {
	float min_x, min_y;
	float max_x, max_y;

	aabb() noexcept : min_x(0), min_y(0), max_x(0), max_y(0) {}
	aabb(float min_x_, float min_y_, float max_x_, float max_y_) noexcept
		: min_x(min_x_), min_y(min_y_), max_x(max_x_), max_y(max_y_) {
	}

	inline bool contains(float px, float py) const noexcept {
		return px >= min_x && px <= max_x &&
			py >= min_y && py <= max_y;
	}

	inline bool intersects(const aabb& other) const noexcept {
		return !(other.min_x > max_x || other.max_x < min_x ||
			other.min_y > max_y || other.max_y < min_y);
	}
};

template<typename T>
class quadtree {
private: 
	const int _max_objects; 
	const int _max_depth; 

	struct node {
		aabb boundary; 
		std::vector<std::pair<aabb, T>> objects;
		node* children[4];
		int depth;

		node(const aabb& boundary, int depth)
			: boundary(boundary), objects(), depth(depth)
		{
			children[0] = children[1] = children[2] = children[3] = nullptr;
		}

		~node() {
			for (int i = 0; i < 4; i++) if (children[i]) delete children[i];
		}

		inline bool is_leaf() const noexcept { return children[0] == nullptr; }
	};

	node* _root;

public: 

	
}; 
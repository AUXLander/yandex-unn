#include <utility>
#include <cstddef>
#include <iostream>
#include <limits>


namespace binary
{
	enum class side
	{
		left,
		right,
		parent
	};

	
template<class Tkey, class Tval>
struct node
{
	Tkey key;
	Tval val;

	node<Tkey, Tval>* left;
	node<Tkey, Tval>* right;
	node<Tkey, Tval>* parent;

	node(const Tkey& key, const Tval& val)
		: key(key), val(val), left{ nullptr }, right{ nullptr }, parent{ nullptr }, m_weight(0)
	{;}

	binary::side what_side(node<Tkey, Tval>* child) const
	{
		if (this->left == child)
		{
			return binary::side::left;
		}
		else if (this->right == child)
		{
			return binary::side::right;
		}
		else if (this == child)
		{
			return binary::side::parent;
		}

		throw "shit happend";

		return binary::side::parent;
	}

	node<Tkey, Tval>*& select(const binary::side side)
	{
		switch (side)
		{
			case binary::side::left:   return left;
			case binary::side::right:  return right;
			case binary::side::parent: return parent;
		}
	}

	node<Tkey, Tval>*& select(const binary::side side, const bool inverse)
	{
		switch (side)
		{
			case binary::side::left:   return inverse ? right : left;
			case binary::side::right:  return inverse ? left : right;
			case binary::side::parent: return parent;
		}
	}

	void connect(const binary::side side, node<Tkey, Tval>* item)
	{
		select(side) = item;

		if (item)
		{
			item->parent = this;
		}
	}

	bool connect_child(const binary::side side, node<Tkey, Tval>* item)
	{
		if (side != binary::side::parent)
		{
			connect(side, item);

			return true;
		}
		
		return false;
	}

	void swap(node<Tkey, Tval>* other)
	{
		std::swap(this->key, other->key);
		std::swap(this->val, other->val);

		std::swap(this->m_weight, other->m_weight);
	}

	inline bool is_node() const
	{
		return left || right;
	}

	inline bool is_leaf() const
	{
		return !left && !right;
	}

	inline static size_t get_weight(const node<Tkey, Tval>* root, const size_t upweight = 0)
	{
		return root ? root->get_weight() + upweight : 0;
	}

	inline size_t get_weight() const
	{
		return m_weight;
	}

	inline std::pair<int, int> get_balances() const
	{
		return std::make_pair<int, int>(get_weight(left), get_weight(right));
	}

	bool recalc_weight()
	{
		const size_t weight = get_weight(this->left, 1) + get_weight(this->right, 1);

		const bool result = (weight != m_weight);

		m_weight = weight;

		return result;
	}

private:
	size_t m_weight;
};


template<class Tkey, class Tval>
class tree
{
	node<Tkey, Tval>* m_root;

public:
	tree(const Tkey& key, const Tval& val)
		: m_root(new node<Tkey, Tval>(key, val))
	{;}

	explicit tree(node<Tkey, Tval>* root)
		: m_root(root)
	{;}

	void free(node<Tkey, Tval>* root)
	{
		if (root != nullptr)
		{
			free(root->left);
			free(root->right);

			auto* parent = root->parent;

			parent->select(parent->what_side(root)) = nullptr;

			delete root;
		}
	}

	inline void print() const
	{
		print(m_root);
	}

	void print(node<Tkey, Tval>* root, const size_t level = 0) const
	{
		if (root != nullptr)
		{
			std::cout << "level " << level << ": " << root->key << std::endl;

			print(root->left, level + 1);
			print(root->right, level + 1);
		}
	}

	void recalc_rebalance(node<Tkey, Tval>* leaf, const int deep = 2)
	{
		if (leaf != nullptr)
		{
			// считаем, что дл€ всех нижних уровней все сбалансировано
			const bool need_recalc_parent = leaf->recalc_weight();
			const auto [lweight, rweight] = leaf->get_balances();

			// Ћевое вращение
			if (rweight - lweight > 1)
			{
				const auto [lrweight, rrweight] = leaf->right->get_balances();

				auto* parent = leaf->parent;

				auto* a = leaf;
				auto* b = a->right;
				auto* c = b->left;

				// ћалое левое вращение
				if (lrweight <= rrweight)
				{
					a->connect(binary::side::right, c);
					b->connect(binary::side::left, a);

					if (parent)
					{
						parent->connect(parent->what_side(leaf), b);
					}
					else
					{
						m_root = b;
						m_root->parent = nullptr;
					}
				}
				// Ѕольшое левое вращение
				else
				{
					a->connect(binary::side::right, c->left);
					b->connect(binary::side::left, c->right);

					c->connect(binary::side::left, a);
					c->connect(binary::side::right, b);

					if (parent)
					{
						parent->connect(parent->what_side(leaf), c);
					}
					else
					{
						m_root = c;
						m_root->parent = nullptr;
					}
				}

				a->recalc_weight();
				b->recalc_weight();
				c->recalc_weight();
			}

			// ѕравое вращение
			if (lweight - rweight > 1)
			{
				const auto [llweight, rlweight] = leaf->left->get_balances();

				auto* parent = leaf->parent;

				auto* a = leaf;
				auto* b = a->left;
				auto* c = b->right;

				// ћалое правое вращение
				if (rlweight <= llweight)
				{
					a->connect(binary::side::left, c);
					b->connect(binary::side::right, a);

					if (parent)
					{
						parent->connect(parent->what_side(leaf), b);
					}
					else
					{
						m_root = b;
						m_root->parent = nullptr;
					}
				}
				// Ѕольшое правое вращение
				else
				{
					b->connect(binary::side::right, c->left);
					a->connect(binary::side::left, c->right);

					c->connect(binary::side::left, b);
					c->connect(binary::side::right, a);

					if (parent)
					{
						parent->connect(parent->what_side(leaf), c);
					}
					else
					{
						m_root = c;
						m_root->parent = nullptr;
					}
				}

				a->recalc_weight();
				b->recalc_weight();
				c->recalc_weight();
			}

			if (deep > 0)
			{
				return recalc_rebalance(leaf->parent, deep + static_cast<int>(need_recalc_parent) - 1);
			}
		}
	}

	std::pair<node<Tkey, Tval>*, binary::side> explore(node<Tkey, Tval>* root, const Tkey& key)
	{
		while (root)
		{
			if (root->key == key)
			{
				return std::make_pair(root, binary::side::parent);
			}
			else if (key < root->key)
			{
				if (root->left)
				{
					root = root->left;
					continue;
				}

				return std::make_pair(root, binary::side::left);
			}
			else
			{
				if (root->right)
				{
					root = root->right;
					continue;
				}

				return std::make_pair(root, binary::side::right);
			}
		}
		
		throw "shit happend";

		return std::make_pair(nullptr, binary::side::parent);
	}

	inline std::pair<node<Tkey, Tval>*, binary::side> explore(const Tkey& key)
	{
		return explore(m_root, key);
	}

	node<Tkey, Tval>* insert(node<Tkey, Tval>* root, node<Tkey, Tval>* child)
	{
		auto [parent, side] = explore(root, child->key);

		bool need_recalc_weight = parent->connect_child(side, child);

		if (need_recalc_weight)
		{
			recalc_rebalance(child);
		}

		switch (side)
		{
			// такой ключ уже вставлен в дерево!
			case binary::side::parent:
			{
				// но если вставл€ем ноду есть еще потомки, 
				if (child->is_node())
				{
					// мен€ем значение
					parent->val = child->val;

					// и нужно как-то распределить потомков этой ноды!
					throw "shit happend";
				}
				else
				{
					// значит нужно просто помен€ть значение
					parent->val = child->val;
				}

				return parent;
			}

			case binary::side::left:
			{
				return child;
			}

			case binary::side::right:
			{
				return child;
			}
		}

		return nullptr;
	}

	inline node<Tkey, Tval>* insert(node<Tkey, Tval>* item)
	{
		return insert(m_root, item);
	}

	inline void emplace(const Tkey& key, const Tval& val)
	{
		insert(new node<Tkey, Tval>(key, val));
	}

	node<Tkey, Tval>* bound(node<Tkey, Tval>* root, const binary::side side, const Tkey& key) const
	{
		while (root && root->select(side, root->key < key))
		{
			root = root->select(side, root->key < key);

			if ((root->key > key) && root->is_leaf())
			{
				return root->parent;
			}
			else if (root->key == key)
			{
				return root;
			}
		}

		return root;
	}

	inline node<Tkey, Tval>* lower_bound(node<Tkey, Tval>* root, const Tkey& key) const
	{
		return bound(root, binary::side::left, key);
	}

	inline node<Tkey, Tval>* lower_bound(const Tkey& key) const
	{
		return bound(m_root, binary::side::left, key);
	}

	inline node<Tkey, Tval>* upper_bound(node<Tkey, Tval>* root, const Tkey& key) const
	{
		return bound(root, binary::side::right, key);
	}

	inline node<Tkey, Tval>* upper_bound(const Tkey& key) const
	{
		return bound(m_root, binary::side::right, key);
	}

	node<Tkey, Tval>* lift_down(node<Tkey, Tval>* root, const binary::side side, size_t deep) const
	{
		while (root && root->select(side) && (deep > 0))
		{
			root = root->select(side);
			--deep;
		}

		return root;
	}

	node<Tkey, Tval>* find_max(node<Tkey, Tval>* root, size_t deep = std::numeric_limits<size_t>::max()) const
	{
		return lift_down(root, binary::side::right, deep);
	}

	node<Tkey, Tval>* find_min(const node<Tkey, Tval>* root, size_t deep = std::numeric_limits<size_t>::max()) const
	{
		return lift_down(root, binary::side::left, deep);
	}

	void erase(node<Tkey, Tval>* search_root, const Tkey& key)
	{
		auto [root, side] = explore(search_root, key);

		if (side == binary::side::parent)
		{
			if (root->is_node())
			{
				// найдем максимум в левом поддереве и помен€ем его с удал€емой вершиной, после чего вызовем ребалансировку
				auto* max = find_max(root->left);

				root->swap(max);
				std::swap(root, max);
			}
			
			auto* parent = root->parent;

			free(root);

			recalc_rebalance(parent);
		}
	}

	inline void erase(const Tkey& key)
	{
		erase(m_root, key);
	}
};



}


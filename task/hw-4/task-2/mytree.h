#include <limits>
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

	constexpr binary::side sides[3] = { binary::side::left, binary::side::right, binary::side::parent };

template<class Tkey, class Tval>
struct node
{
	Tkey key;
	Tval val;

	union
	{
		node<Tkey, Tval>* pointers[3];

		struct
		{
			node<Tkey, Tval>* left;
			node<Tkey, Tval>* right;
			node<Tkey, Tval>* parent;
		};
	};

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

	node<Tkey, Tval>* m_max{ nullptr };
	node<Tkey, Tval>* m_min{ nullptr };

	void free(node<Tkey, Tval>* root)
	{
		if (root != nullptr)
		{
			free(root->left);
			free(root->right);

			auto* parent = root->parent;

			if (parent)
			{
				parent->select(parent->what_side(root)) = nullptr;
			}

			delete root;
		}
	}

public:
	tree()
		: m_root(nullptr)
	{;}

	tree(const Tkey& key, const Tval& val)
		: m_root(new node<Tkey, Tval>(key, val))
	{;}
	
	~tree()
	{
		free(m_root);
	}

private:
	explicit tree(node<Tkey, Tval>* root)
		: m_root(root)
	{;}

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

				if (c)
				{
					c->recalc_weight();
				}
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

				if (c)
				{
					c->recalc_weight();
				}
			}

			if (deep > 0)
			{
				return recalc_rebalance(leaf->parent, deep + static_cast<int>(need_recalc_parent) - 1);
			}
		}
	}

	std::pair<node<Tkey, Tval>*, binary::side> private_explore(node<Tkey, Tval>* root, const Tkey& key)
	{
		auto index = static_cast<uint8_t>(root->key < key) | (static_cast<uint8_t>(root->key == key) << 1U);
		auto* next = root->pointers[index];

		while (next && root->key != key)
		{
			root = next;

			index = static_cast<uint8_t>(root->key < key) + (static_cast<uint8_t>(root->key == key) << 1U);

			next = root->pointers[index];
		}

		return std::make_pair(root, sides[index]);
		
		//throw "shit happend";

		//return std::make_pair(nullptr, binary::side::parent);
	}

	inline std::pair<node<Tkey, Tval>*, binary::side> private_explore(const Tkey& key)
	{
		return private_explore(m_root, key);
	}

	node<Tkey, Tval>* private_insert_hint(node<Tkey, Tval>* root, node<Tkey, Tval>* parent, binary::side side, node<Tkey, Tval>* child)
	{
		const bool need_recalc_weight = parent->connect_child(side, child);

		if (need_recalc_weight)
		{
			recalc_rebalance(child);
		}

		if (child)
		{
			m_max = (!m_max || (m_max->val < child->val)) ? child : m_max;
			m_min = (!m_min || (child->val < m_min->val)) ? child : m_min;
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

	inline node<Tkey, Tval>* private_insert(node<Tkey, Tval>* root, node<Tkey, Tval>* child)
	{
		auto [parent, side] = private_explore(root, child->key);

		return private_insert_hint(root, parent, side, child);
	}

	node<Tkey, Tval>* private_insert_hint(node<Tkey, Tval>* root, node<Tkey, Tval>* parent, node<Tkey, Tval>* child)
	{
		binary::side side = binary::side::parent;

		if (!parent->left)
		{
			return private_insert_hint(root, parent, binary::side::left, child);
		}
		
		if (!parent->right)
		{
			return private_insert_hint(root, parent, binary::side::right, child);
		}

		return private_insert(root, child);
	}

	inline node<Tkey, Tval>* private_insert(node<Tkey, Tval>* item)
	{
		return private_insert(m_root, item);
	}

	inline node<Tkey, Tval>* private_emplace(const Tkey& key, const Tval& val)
	{
		if (!m_root)
		{
			return m_root = new node<Tkey, Tval>(key, val);
		}
		else
		{
			return private_insert(new node<Tkey, Tval>(key, val));
		}
	}

	inline node<Tkey, Tval>* private_emplace_hint(node<Tkey, Tval>* parent, const Tkey& key, const Tval& val)
	{
		if (parent == nullptr)
		{
			return private_insert(m_root, new node<Tkey, Tval>(key, val));
		}

		return private_insert_hint(m_root, parent, new node<Tkey, Tval>(key, val));
	}

	node<Tkey, Tval>* private_bound(node<Tkey, Tval>* root, const binary::side side, const Tkey& key) const
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

	inline node<Tkey, Tval>* private_lower_bound(node<Tkey, Tval>* root, const Tkey& key) const
	{
		return private_bound(root, binary::side::left, key);
	}

	inline node<Tkey, Tval>* private_lower_bound(const Tkey& key) const
	{
		return private_bound(m_root, binary::side::left, key);
	}

	inline node<Tkey, Tval>* private_upper_bound(node<Tkey, Tval>* root, const Tkey& key) const
	{
		return private_bound(root, binary::side::right, key);
	}

	inline node<Tkey, Tval>* private_upper_bound(const Tkey& key) const
	{
		return private_bound(m_root, binary::side::right, key);
	}

	static node<Tkey, Tval>* private_lift_down(node<Tkey, Tval>* root, const binary::side side, size_t deep)
	{
		while (root && root->select(side) && (deep > 0))
		{
			root = root->select(side);
			--deep;
		}

		return root;
	}

	node<Tkey, Tval>* private_find_max(node<Tkey, Tval>* root, size_t deep = std::numeric_limits<size_t>::max())
	{
		return m_max ? m_max : m_max = private_lift_down(root, binary::side::right, deep);
	}

	node<Tkey, Tval>* private_find_min(node<Tkey, Tval>* root, size_t deep = std::numeric_limits<size_t>::max())
	{
		return m_min ? m_min : m_min = private_lift_down(root, binary::side::left, deep);
	}

	void private_erase(node<Tkey, Tval>* search_root, const Tkey& key)
	{
		auto [root, side] = private_explore(search_root, key);

		if (side == binary::side::parent)
		{
			if (root->is_node())
			{
				// найдем максимум в левом поддереве и помен€ем его с удал€емой вершиной, после чего вызовем ребалансировку
				auto* max = private_lift_down(root->left, binary::side::right, std::numeric_limits<size_t>::max()); // private_find_max(root->left);

				// на левой ветке все пусто
				if (!max)
				{
					max = private_lift_down(root->right, binary::side::left, std::numeric_limits<size_t>::max()); //private_find_min(root->right);
				}

				root->swap(max);
				std::swap(root, max);
			}
			
			auto* parent = root->parent;

			free(root);

			recalc_rebalance(parent);
		}
	}

	inline void private_erase(const Tkey& key)
	{
		private_erase(m_root, key);
	}

	template <class Iter>
	class NodeTreeIterator
	{
		friend class tree<Tkey, Tval>;
	public:
		using iterator_type		= Iter;
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type		= iterator_type;
		using reference			= iterator_type&;
		using pointer			= iterator_type*;
		using difference_type	= ptrdiff_t;

		iterator_type* p_root;
		iterator_type* p_previous;
	private:
		NodeTreeIterator(Iter* root, Iter* previous = nullptr)
			: p_root(root), p_previous(previous) // , p_next(nullptr)
		{;}
	public:
		NodeTreeIterator(const NodeTreeIterator& other)
			: p_root(other.p_root), p_previous(other.p_previous) // , p_next(other.p_next)
		{;}

		inline bool operator!=(const NodeTreeIterator& other) const
		{
			return p_root != other.p_root;
		}

		inline bool operator==(const NodeTreeIterator& other) const
		{
			return p_root == other.p_root;
		}

		inline typename reference operator*() const
		{
			return *p_root;
		}

		NodeTreeIterator& operator++()
		{
			if (p_root)
			{
				p_previous = p_root;

				if (p_root->right)
				{
					p_root = private_lift_down(p_root->right, binary::side::left, std::numeric_limits<size_t>::max());
				}
				else
				{
					const Tkey key = p_root->key;

					while (p_root && (p_root->key <= key))
					{						
						p_root = p_root->parent ? p_root->parent : nullptr; // end()
					}
				}
			}

			return *this;
		}

		NodeTreeIterator& operator--()
		{
			if (p_root)
			{
				//p_next = p_root;

				if (p_root->left)
				{
					p_root = private_lift_down(p_root->left, binary::side::right, std::numeric_limits<size_t>::max());
				}
				else
				{
					const Tkey key = p_root->key;

					while (p_root && (p_root->key >= key))
					{
						p_root = p_root->parent ? p_root->parent : nullptr; // end()
					}
				}

				return *this;
			}
			else if (p_previous)
			{
				p_root = p_previous;
				p_previous = nullptr;
			}

			return *this;
		}
	};

	using iterator = typename NodeTreeIterator<node<Tkey, Tval>>;

public:
	inline iterator begin()
	{
		return iterator(this->private_find_min(m_root));
	}

	inline iterator end()
	{
		return iterator(nullptr, this->private_find_max(m_root));
	}

	inline iterator lower_bound(const Tkey& key)
	{
		//auto r1 = private_lower_bound(key);
		auto r2 = private_explore(key);

		if (r2.second == side::right)
		{
			return ++iterator(r2.first);
		}
		else
		{
			return iterator(r2.first);
		}
	}

	inline void erase(iterator wh) noexcept
	{
		this->private_erase(wh.p_root->key);
	}

	inline iterator emplace_hint(iterator wh, const Tkey& key, const Tval& val)
	{
		return iterator(this->private_emplace_hint(wh.p_root, key, val));
	}

	inline iterator emplace(const Tkey& key, const Tval& val)
	{
		return iterator(this->private_emplace(key, val));
	}
};



}


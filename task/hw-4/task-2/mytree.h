#include <limits>
#include <utility>
#include <cstddef>
#include <iostream>

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

private:
	explicit tree(node<Tkey, Tval>* root)
		: m_root(root)
	{;}

	~tree()
	{
		free(m_root);
	}

	//inline void print() const
	//{
	//	print(m_root);
	//}

	//void print(node<Tkey, Tval>* root, const size_t level = 0) const
	//{
	//	if (root != nullptr)
	//	{
	//		std::cout << "level " << level << ": " << root->key << std::endl;

	//		print(root->left, level + 1);
	//		print(root->right, level + 1);
	//	}
	//}

	void recalc_rebalance(node<Tkey, Tval>* leaf, const int deep = 2)
	{
		if (leaf != nullptr)
		{
			// �������, ��� ��� ���� ������ ������� ��� ��������������
			const bool need_recalc_parent = leaf->recalc_weight();
			const auto [lweight, rweight] = leaf->get_balances();

			// ����� ��������
			if (rweight - lweight > 1)
			{
				const auto [lrweight, rrweight] = leaf->right->get_balances();

				auto* parent = leaf->parent;

				auto* a = leaf;
				auto* b = a->right;
				auto* c = b->left;

				// ����� ����� ��������
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
				// ������� ����� ��������
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

			// ������ ��������
			if (lweight - rweight > 1)
			{
				const auto [llweight, rlweight] = leaf->left->get_balances();

				auto* parent = leaf->parent;

				auto* a = leaf;
				auto* b = a->left;
				auto* c = b->right;

				// ����� ������ ��������
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
				// ������� ������ ��������
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

	std::pair<node<Tkey, Tval>*, binary::side> private_explore(node<Tkey, Tval>* root, const Tkey& key)
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

		switch (side)
		{
			// ����� ���� ��� �������� � ������!
			case binary::side::parent:
			{
				// �� ���� ��������� ���� ���� ��� �������, 
				if (child->is_node())
				{
					// ������ ��������
					parent->val = child->val;

					// � ����� ���-�� ������������ �������� ���� ����!
					throw "shit happend";
				}
				else
				{
					// ������ ����� ������ �������� ��������
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

	inline node<Tkey, Tval>* private_private_insert(node<Tkey, Tval>* root, node<Tkey, Tval>* child)
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

		return private_private_insert(root, child);
	}

	inline node<Tkey, Tval>* private_private_insert(node<Tkey, Tval>* item)
	{
		return private_private_insert(m_root, item);
	}

	inline node<Tkey, Tval>* private_emplace(const Tkey& key, const Tval& val)
	{
		if (!m_root)
		{
			return m_root = new node<Tkey, Tval>(key, val);
		}
		else
		{
			return private_private_insert(new node<Tkey, Tval>(key, val));
		}
	}

	inline node<Tkey, Tval>* private_emplace_hint(node<Tkey, Tval>* parent, const Tkey& key, const Tval& val)
	{
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

	static node<Tkey, Tval>* private_find_max(node<Tkey, Tval>* root, size_t deep = std::numeric_limits<size_t>::max())
	{
		return private_lift_down(root, binary::side::right, deep);
	}

	static node<Tkey, Tval>* private_find_min(node<Tkey, Tval>* root, size_t deep = std::numeric_limits<size_t>::max())
	{
		return private_lift_down(root, binary::side::left, deep);
	}

	void private_erase(node<Tkey, Tval>* search_root, const Tkey& key)
	{
		auto [root, side] = private_explore(search_root, key);

		if (side == binary::side::parent)
		{
			if (root->is_node())
			{
				// ������ �������� � ����� ��������� � �������� ��� � ��������� ��������, ����� ���� ������� ��������������
				auto* max = private_find_max(root->left);

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
		//iterator_type* p_next;
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
					p_root = private_find_min(p_root->right);
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
					p_root = private_find_max(p_root->left);
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
	inline iterator begin() const
	{
		return iterator(private_find_min(m_root));
	}

	inline iterator end() const
	{
		return iterator(nullptr, private_find_max(m_root));
	}

	inline iterator lower_bound(const Tkey& key)
	{
		return iterator(private_lower_bound(key));
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


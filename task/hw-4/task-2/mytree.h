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

	bool connect(const binary::side side, node<Tkey, Tval>* item)
	{
		switch (side)
		{
			case binary::side::left:
			{
				this->left = item;
				break;
			}

			case binary::side::right:
			{
				this->right = item;
				break;
			}

			case binary::side::parent:
			{
				return false;
			}
		}

		if (item)
		{
			item->parent = this;
		}

		return true;
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

	//inline bool is_need_rebalance() const
	//{
	//	const auto [lweight, rweight] = get_balances();

	//	return std::abs(lweight - rweight) > 1;
	//}

	bool recalc_weight()
	{
		const size_t weight = get_weight(this->left, 1) + get_weight(this->right, 1);

		if (weight != m_weight)
		{
			m_weight = weight;

			return true;
		}
		else
		{
			// m_weight = weight;
			return false;
		}
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

			//const bool need_rebalance = std::abs(lweight - rweight) > 1;

			// Ћевое вращение
			if (rweight - lweight > 1)
			{
				const auto [lrweight, rrweight] = leaf->right->get_balances();

				auto* parent = leaf->parent;
				binary::side side = binary::side::parent;

				if (parent)
				{
					side = (parent->left == leaf) ? binary::side::left : binary::side::right;
				}

				// ћалое левое вращение
				if (lrweight <= rrweight)
				{
					auto* a = leaf;
					auto* b = a->right;
					auto* c = b->left;

					a->connect(binary::side::right, c);
					b->connect(binary::side::left, a);

					if (parent)
					{
						parent->connect(side, b);
					}
					else
					{
						m_root = b;
						m_root->parent = nullptr;
					}

					a->recalc_weight();
					b->recalc_weight();
				}
				// Ѕольшое левое вращение
				else
				{
					auto* a = leaf;
					auto* b = a->right;
					auto* c = b->left;

					a->connect(binary::side::right, c->left);
					b->connect(binary::side::left, c->right);

					c->connect(binary::side::left, a);
					c->connect(binary::side::right, b);

					if (parent)
					{
						parent->connect(side, c);
					}
					else
					{
						m_root = c;
						m_root->parent = nullptr;
					}

					a->recalc_weight();
					b->recalc_weight();
					c->recalc_weight();
				}
			}

			// ѕравое вращение
			if (lweight - rweight > 1)
			{
				const auto [llweight, rlweight] = leaf->left->get_balances();

				auto* parent = leaf->parent;
				binary::side side = binary::side::parent;

				if (parent)
				{
					side = (parent->left == leaf) ? binary::side::left : binary::side::right;
				}

				// ћалое правое вращение
				if (rlweight <= llweight)
				{
					auto* a = leaf;
					auto* b = a->left;
					auto* c = b->right;

					a->connect(binary::side::left, c);
					b->connect(binary::side::right, a);

					if (parent)
					{
						parent->connect(side, b);
					}
					else
					{
						m_root = b;
						m_root->parent = nullptr;
					}

					a->recalc_weight();
					b->recalc_weight();
				}
				// Ѕольшое правое вращение
				else
				{
					auto* a = leaf;
					auto* b = a->left;
					auto* c = b->right;

					b->connect(binary::side::right, c->left);
					a->connect(binary::side::left, c->right);

					c->connect(binary::side::left, b);
					c->connect(binary::side::right, a);

					if (parent)
					{
						parent->connect(side, c);
					}
					else
					{
						m_root = c;
						m_root->parent = nullptr;
					}

					a->recalc_weight();
					b->recalc_weight();
					c->recalc_weight();
				}
			}

			if (deep > 0)
			{
				return recalc_rebalance(leaf->parent, deep + static_cast<int>(need_recalc_parent) - 1);
			}
		}
	}

	std::pair<node<Tkey, Tval>*, binary::side> explore(node<Tkey, Tval>* root, const Tkey& key)
	{
		root = root;

		while (true)
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
	}

	inline std::pair<node<Tkey, Tval>*, binary::side> explore(const Tkey& key)
	{
		return explore(m_root, key);
	}

	node<Tkey, Tval>* insert(node<Tkey, Tval>* root, node<Tkey, Tval>* child)
	{
		auto [parent, side] = explore(root, child->key);

		bool need_recalc_weight = parent->connect(side, child);

		if (need_recalc_weight)
		{
			recalc_rebalance(child);
		}

		switch (side)
		{
			case binary::side::parent:
			{
				parent->val = child->val;

				// все ноды child должны быть распределены как-то
				throw "wtf";

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

	void erase(node<Tkey, Tval>* root, const Tkey& key)
	{
		auto [parent, side] = explore(root, key);

		if (side == binary::side::parent)
		{
			if (parent->left || parent->right)
			{
				// нужно отсоединить найденную ноду и перезагрузить все нижние ноды в дерево
				throw "wtf";
			}
			else
			{
				free(parent);
			}
		}
	}

	inline void erase(const Tkey& key)
	{
		erase(m_root, key);
	}
};



}


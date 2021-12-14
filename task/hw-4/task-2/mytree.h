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
		if (item != nullptr)
		{
			switch (side)
			{
				case binary::side::left:
				{
					this->left = item;
					item->parent = this;
					return true;
				}

				case binary::side::right:
				{
					this->right = item;
					item->parent = this;
					return true;
				}
			}
		}

		return false;
	}

	inline static size_t get_weight(const node<Tkey, Tval>* root)
	{
		return root ? root->get_weight() : 0;
	}

	inline size_t get_weight() const
	{
		return m_weight;
	}

	inline bool is_need_rebalance() const
	{
		const int lweight = static_cast<int>(get_weight(left));
		const int rweight = static_cast<int>(get_weight(right));

		return std::abs(lweight - rweight) > 1;
	}

	bool recalc_weight()
	{
		const size_t weight = get_weight(this->left) + get_weight(this->right);

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

	void recalc_rebalance(node<Tkey, Tval>* leaf)
	{
		if (leaf != nullptr)
		{
			// считаем, что для всех нижних уровней все сбалансировано
			const bool need_recalc_parent = leaf->recalc_weight();
			const bool need_rebalance = leaf->is_need_rebalance();

			if (need_rebalance)
			{

			}

			if (need_recalc_parent)
			{
				return recalc_rebalance(leaf->parent);
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


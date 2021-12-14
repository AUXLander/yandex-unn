#include "W4T3.hpp"
#include <iterator>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <utility>
#include <stack>
#include <limits>

template<class T>
struct Node
{
    T data;
    Node<T>* left;
    Node<T>* right;

    explicit Node(const T& data)
        : data(data), left(nullptr), right(nullptr)
    {;}
};

template<class T>
class TreeView
{
    std::ostream& output;
    int idx;

    Node<T>* m_root{ nullptr };

    void tree_free(Node<T>* tree)
    {
        if (tree != nullptr)
        {
            tree_free(tree->left);
            tree_free(tree->right);

            delete tree;
        }
    }

    Node<T>* next_preorder(std::vector<T>& preorder, int val = std::numeric_limits<int>::max())
    {
        if (idx >= static_cast<int>(preorder.size()) || preorder[idx] >= val)
        {
            return nullptr;
        }

        Node<T>* node = new Node<T>(preorder[idx++]);

        node->left = next_preorder(preorder, node->data);
        node->right = next_preorder(preorder, val);

        return node;
    }

public:

    enum class order
    {
        in,
        in_new,

        post,
        pre_new
    };

    TreeView(std::ostream& output, std::vector<T>& preorder)
        :output(output), idx(0)
    {
        m_root = next_preorder(preorder);
    }

    ~TreeView()
    {
        tree_free(m_root);
    }

    void process(order action)
    {
        return process(action, m_root);
    }

    void process(order action, Node<T>* root)
    {
        if (root != nullptr)
        {
            switch (action)
            {
                case order::post:
                {
                    process(order::post, root->left);
                    process(order::post, root->right);

                    output << root->data << " ";

                    return;
                }

                case order::in:
                {
                    process(order::in, root->left);
                    output << root->data << " ";
                    process(order::in, root->right);

                    return;
                }

                case order::pre_new:
                {
                    std::stack<Node<T>*> stack;

                    int temp = 1;

                    Node<T>* node = root;

                    while (temp == 1)
                    {
                        while (root->left != nullptr)
                        {
                            stack.push(root);
                            root = root->left;
                        }

                        while ((root->right == nullptr) || (root->right == node))
                        {
                            output << root->data << " ";

                            node = root;

                            if (!stack.empty())
                            {
                                root = stack.top();
                                stack.pop();
                            }
                            else
                            {
                                return;
                            }
                        }

                        stack.push(root);
                        root = root->right;
                    }

                    return;
                }

                case order::in_new:
                {
                    std::stack<Node<T>*> stack;

                    stack.push(root);

                    while (!stack.empty())
                    {
                        if (root)
                        {
                            root = root->left;
                        }
                        else
                        {
                            root = stack.top();

                            stack.pop();

                            output << root->data << " ";

                            root = root->right;
                        }

                        if (root)
                        {
                            stack.push(root);
                        }
                    }

                    return;
                }
            }
        }
    }
};

void W4T3::main(std::istream& input, std::ostream& output)
{
    int n_size;
    input >> n_size;

    std::vector<int> preorder(n_size);

    for (auto& element : preorder)
    {
        input >> element;
    }

    TreeView<int> view(output, preorder);

    view.process(TreeView<int>::order::pre_new);

    output << '\n';

    view.process(TreeView<int>::order::in);
}

void W4T3::test(Test* const reference)
{
	if (reference != nullptr)
	{
		reference->open(*this).input("7\n4 2 1 3 6 5 7\n").expect("1 3 2 5 7 6 4 \n1 2 3 4 5 6 7 ");
		reference->open(*this).input("6\n5 3 2 3 5 6\n").expect("2 3 3 6 5 5 \n2 3 3 5 5 6 ");
	}
}
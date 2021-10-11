#include <iostream>
#include <stack>

static bool selectRule(const char ch)
{
    switch (ch)
    {
    case '(': return true;
    case ')': return false;

    case '{': return true;
    case '}': return false;

    case '[': return true;
    case ']': return false;
    }

    return false;
}

static char selectCloseRule(const char ch)
{
    switch (ch)
    {
    case '(': return ')';
    case '{': return '}';
    case '[': return ']';
    }
    return '\0';
}

inline int parse(std::stack<char>& stack, size_t& index, const char cnext)
{
    const bool isRuleExist = (cnext == '(') || (cnext == '[') || (cnext == '{' || (cnext == '}') || (cnext == ']') || (cnext == ')'));

    if (isRuleExist == false)
    {
        return index;
    }

    if (selectRule(cnext) == true) // rule::open
    {
        stack.push(cnext);
        index++;

        return -1;
    }

    if (stack.empty() == true)
    {
        return index;
    }

    if (selectCloseRule(stack.top()) != cnext)
    {
        return index;
    }

    stack.pop();
    index++;

    return -1;
}

int main(int, char* [])
{
    size_t index = 0;
    std::stack<char> stack;

    char cnext;
    int status = -2;

    while ((std::cin.peek() != '\n') && (std::cin >> cnext) && (status < 0))
    {
        status = parse(stack, index, cnext);
    }

    if (status < 0)
    {
        if (stack.empty())
        {
            if (status == -1)
            {
                std::cout << "CORRECT" << '\n';
            }
            else
            {
                std::cout << 0 << '\n';
            }
        }
        else
        {
            std::cout << index << '\n';
        }
    }
    else
    {
        std::cout << index << '\n';
    }

    return 0;
}

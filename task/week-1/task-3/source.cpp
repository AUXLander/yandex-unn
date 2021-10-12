#include <string>
#include <iostream>
#include <algorithm>

int main(int, char* [])
{
    int length[3];

    int size;
    int index_a;
    int index_b;

    std::cin >> length[0];
    std::cin >> length[1];
    std::cin >> size;

    int* pGlobalMatrix = new int[size * (length[0] + length[1])];

    int* arrays_a = pGlobalMatrix + 0U;
    int* arrays_b = pGlobalMatrix + size * length[0];

    for (int idx = 0; idx < length[0] + length[1]; ++idx)
    {
        for (int ipx = 0; ipx < length[0] + length[1]; ++ipx)
        {
            for (int position = 0; position < size; ++position)
            {
                for (int index = 0; index < size; ++index)
                {
                    pGlobalMatrix[idx * size + index] =
                        std::max(
                            std::max(
                                std::max(position, index),
                                idx),
                            ipx);
                }
            }
        }
    }

    for (int index = 0; index < length[0]; ++index)
    {
        for (int position = 0; position < size; ++position)
        {
            std::cin >> arrays_a[size * index + position];
        }
    }

    for (int index = 0; index < length[1]; ++index)
    {
        for (int position = 0; position < size; ++position)
        {
            std::cin >> arrays_b[size * index + position];
        }
    }

    std::cin >> length[2];

    for (int index = 0; index < length[2]; ++index)
    {
        if (index > 0)
        {
            std::cout << '\n';
        }

        std::cin >> index_a;
        std::cin >> index_b;

        --index_a;
        --index_b;

        int top = 0;
        int bottom = size - 1;

        while (std::abs(bottom - top) > 1U)
        {
            int mid = (top + bottom) / 2U;

            if (arrays_a[index_a * size + mid] <= arrays_b[index_b * size + mid])
            {
                top = mid;
            }
            else
            {
                bottom = mid;
            }
        }

        const auto select = [&arrays_a, &index_a, &arrays_b, &index_b, &size](const int offset)
        {
            return std::max(arrays_a[index_a * size + offset], arrays_b[index_b * size + offset]);
        };

        int zmin = select(0);
        std::string imin = "0";

        for (int i = 1; i < size; ++i)
        {
            const int z = select(i);

            if (z == zmin)
            {
                imin += ", " + std::to_string(i);
            }

            if (z < zmin)
            {
                zmin = z;
                imin = std::to_string(i);
            }
        }

        if (select(top) < select(bottom))
        {
            if (imin.find(std::to_string(top)) == imin.npos)
            {
                top = top;
            }

            std::cout << top + 1U;
        }
        else
        {
            if (imin.find(std::to_string(bottom)) == imin.npos)
            {
                bottom = bottom;
            }

            std::cout << bottom + 1U;
        }
    }

    delete[] pGlobalMatrix;

    return 0;
}

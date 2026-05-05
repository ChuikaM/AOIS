#include <helper.hpp>
#include <iostream>
#include <format>

void printSourceVectors(const std::vector<std::vector<float>>& data, const std::string& title)
{
    int i = 1;
    for(const auto row : data)
    {
        printVector(row, std::format("{}{}", title, i++));
        std::cout << std::endl;
    }
}
void printVector(const std::vector<float> &data, const std::string& title)
{
    std::cout << std::format("{} = ", title);
    std::cout << "[ ";
    std::copy(data.begin(), data.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "]";
}

void printVectorHighlighted(const std::vector<float>& data, const std::string& title, int highlightIndex)
{
    std::cout << std::format("{} ({}) = ", title, highlightIndex);

    std::cout << "[ ";
    for (size_t i = 0; i < data.size(); i++) {
        int val = static_cast<int>(data[i]);
        if (highlightIndex > 0 && i + 1 == static_cast<size_t>(highlightIndex))
            std::cout << "(" << val << ")";
        else
            std::cout << val;

        if (i + 1 < data.size())
            std::cout << " ";
    }
    std::cout << " ]";
}

void printBidirectionalVectors(const std::vector<std::vector<float>> &X, const std::vector<std::vector<float>> &Y)
{
    const int SIZE = X.size();
    for (size_t i = 0; i < SIZE; i++)
    {
        printVector(X[i], std::format("x{}", i+1));
        std::cout << " ";
        printVector(Y[i], std::format("y{}", i+1));
        std::cout << std::endl;
    }
}

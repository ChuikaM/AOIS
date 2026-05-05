#pragma once
#include <vector>

void printSourceVectors(const std::vector<std::vector<float>>& data, const std::string& title = "y");
void printVector(const std::vector<float> &data, const std::string& title);

void printVectorHighlighted(const std::vector<float>& data, const std::string& title, int highlightIndex);

void printBidirectionalVectors(const std::vector<std::vector<float>> &X, const std::vector<std::vector<float>> &Y);
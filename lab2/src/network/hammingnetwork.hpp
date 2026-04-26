#pragma once
#include <inetwork.hpp>
#include <vector>

class HammingNetwork : public INetwork {
public:
    explicit HammingNetwork();

    void train() override;

private:
    std::vector<std::vector<int>> m_w;


};
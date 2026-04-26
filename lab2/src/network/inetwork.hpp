#pragma once

class INetwork {
public:
    virtual ~INetwork() = default;

    virtual void train() = 0;
    

};
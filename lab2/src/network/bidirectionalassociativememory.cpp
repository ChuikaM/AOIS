#include <bidirectionalassociativememory.hpp>

BidirectionalAssociativeMemory::BidirectionalAssociativeMemory()
{
}

void BidirectionalAssociativeMemory::train(const Matrix<float>& trainData)
{
    m_w.resize(trainData.column(), trainData.row());
    
}

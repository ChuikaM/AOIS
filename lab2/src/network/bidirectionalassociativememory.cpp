#include <bidirectionalassociativememory.hpp>

template <typename T> bool F(T val) {
    return val > T(0);
}

void BidirectionalAssociativeMemory::train(const Matrix<float>& trainData)
{
   m_w.resize(trainData.column(), trainData.row());
}

void BidirectionalAssociativeMemory::train(const Matrix<float> &X, const Matrix<float> &Y)
{
   m_w.resize(Y.column(), X.row());
   m_w = X.transpose() * Y;
}

Report BidirectionalAssociativeMemory::sync(const Matrix<float> &noiseData, const Matrix<float> &originalData, InputVariable variable)
{
   switch (variable)
   {
   case InputVariable::X: return computeY(noiseData, originalData);
   case InputVariable::Y: return computeX(noiseData, originalData);
   }
}

Report BidirectionalAssociativeMemory::computeX(const Matrix<float> &noiseY, const Matrix<float> &originalY)
{
   auto singMatrix = [](std::vector<float> &row){
      for(int i = 0; i < row.size(); i++)
      {
         row[i] = F(row[i]);
      }
   };

   auto S = noiseY * m_w;
   auto Y = S.at_column(0);
   singMatrix(Y);

   return {};
}

Report BidirectionalAssociativeMemory::computeY(const Matrix<float> &noiseX, const Matrix<float> &originalX)
{
   auto singMatrix = [](std::vector<float> &row){
      for(int i = 0; i < row.size(); i++)
      {
         row[i] = F(row[i]);
      }
   };

   auto S = noiseX * m_w;
   auto X = S.at_column(0);
   singMatrix(X);

   return {};
}

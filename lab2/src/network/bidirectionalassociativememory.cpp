#include <bidirectionalassociativememory.hpp>
#include <algorithm>
#include <vector>

inline float toBipolar(float val) { return (val >= 0.5f) ? 1.0f : -1.0f; }

void BidirectionalAssociativeMemory::train(const Matrix<float>& /*trainData*/)
{}

void BidirectionalAssociativeMemory::train(const Matrix<float> &X, const Matrix<float> &Y)
{
   m_w = X.transpose() * Y;
}

void BidirectionalAssociativeMemory::sync(const Matrix<float> &noiseData, const Matrix<float> &originalData, InputVariable variable)
{
   const int MAX_STAGES = 10;
   bool forward = (variable == InputVariable::X);
   const Matrix<float>& W = forward ? m_w : m_w.transpose();

   std::vector<float> state = (noiseData.row() == 1) ? noiseData.at_row(0) : noiseData.at_column(0);
   std::vector<float> target = (originalData.row() == 1) ? originalData.at_row(0) : originalData.at_column(0);

   for (auto& v : state) v = toBipolar(v);
   for (auto& v : target) v = toBipolar(v);

   for (int stage = 1; stage <= MAX_STAGES; ++stage) {
      std::vector<float> prev_state = state;

      Matrix<float> inMat(1, state.size());
      for (size_t i = 0; i < state.size(); ++i) inMat.at(0, i) = state[i];

      Matrix<float> resMat = inMat * W;
      std::vector<float> next_state = resMat.at_row(0);

      for (size_t i = 0; i < next_state.size(); ++i) {
         if (next_state[i] > 0.0f) next_state[i] = 1.0f;
         else if (next_state[i] < 0.0f) next_state[i] = -1.0f;
         else next_state[i] = prev_state[i];
      }

      state = next_state;
      // report.outputs.push_back(state);
      // report.stages = stage;

      if (state == prev_state) {
         // report.results.push_back(true);
         // report.iterations = stage;
         break;
      }
      //report.results.push_back(false);
   }
   //report.iterations = report.stages;
}

BidirectionalReport BidirectionalAssociativeMemory::generateReport() const
{
   BidirectionalReport report;

   return report;
}

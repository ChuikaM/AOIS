import numpy as np

class HammingNetwork:
    def __init__(self, n, m, epsilon=0.3):
        self.n = n
        self.m = m
        self.epsilon = epsilon
        self.patterns = []

    def train(self, patterns):
        self.patterns = [np.array(p) for p in patterns]

    def _layer1(self, input_vec):
        scores = []
        for p in self.patterns:
            sim = np.sum(p * input_vec)
            scores.append(0.5 * (sim + self.n))
        return np.array(scores)

    def predict(self, noisy):
        z = self._layer1(noisy)
        history_z = [z.copy().tolist()]
        
        for _ in range(20):
            if np.sum(z > 0) == 1:
                break

            sum_z = np.sum(z)
            s = z - self.epsilon * (sum_z - z)
          
            z = np.array([si if si > 0 else 0.0 for si in s])
            history_z.append(z.copy().tolist())
            
        winner_idx = np.argmax(z)
        return self.patterns[winner_idx].tolist(), history_z
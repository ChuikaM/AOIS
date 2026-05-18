import numpy as np

class HammingNetwork:
    def __init__(self, n, m, epsilon=0.3):
        self.n = n
        self.m = m
        self.epsilon = epsilon
        self.patterns = []
        self.input_size = n
        self.output_size = m
        self.source_vectors = None

    def train(self, patterns):
        self.patterns = [np.array(p) for p in patterns]
        self.source_vectors = patterns.copy()

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

    def recognize(self, noisy_vector, save_history=False):
        result, _ = self.predict(noisy_vector)
        class Result:
            def __init__(self, converged, winner_index):
                self.converged = converged
                self.winner_index = winner_index
        winner_index = -1
        for idx, orig in enumerate(self.source_vectors):
            if result == orig:
                winner_index = idx
                break
        converged = (winner_index != -1)
        return Result(converged, winner_index)
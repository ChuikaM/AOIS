import numpy as np

class HopfieldNetwork:
    def __init__(self, n):
        self.n = n
        self.W = np.zeros((n, n))
        self.source_vectors = None

    def train(self, patterns):
        self.source_vectors = patterns.copy()
        self.W = np.zeros((self.n, self.n))
        for p in patterns:
            bipolar = 2 * np.array(p) - 1
            self.W += np.outer(bipolar, bipolar)
        np.fill_diagonal(self.W, 0)

    def _activation(self, s, current_val):
        if s > 0: return 1
        if s < 0: return 0
        return current_val

    def predict_async(self, noisy, max_cycles=10):
        y = np.array(noisy, copy=True)
        stages = []
        for cycle in range(max_cycles):
            stage_updates = []
            y_start_cycle = y.copy()
            for idx in range(self.n):
                s = np.dot(y, self.W[:, idx])
                y[idx] = self._activation(s, y[idx])
                stage_updates.append((idx, y.copy()))
            stages.append(stage_updates)
            if np.array_equal(y, y_start_cycle):
                break
        return y.tolist(), stages

    def predict_sync(self, noisy, max_steps=10):
        y = np.array(noisy, copy=True)
        history = [y.copy().tolist()]
        for _ in range(max_steps):
            s = y @ self.W
            y_new = np.array([self._activation(si, y[i]) for i, si in enumerate(s)])
            if np.array_equal(y, y_new):
                break
            y = y_new
            history.append(y.copy().tolist())
        return y.tolist(), history
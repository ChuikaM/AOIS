import numpy as np

class BAMNetwork:
    def __init__(self, n, m):
        self.n = n
        self.m = m
        self.W = np.zeros((n, m))

    def train(self, pairs):
        for x, y in pairs:
            x_b = 2 * np.array(x) - 1
            y_b = 2 * np.array(y) - 1
            self.W += np.outer(x_b, y_b)

    def _bipolar_to_binary(self, vec):
        return ((vec + 1) // 2).astype(int).tolist()

    def _activation(self, s, current_state):
        res = np.where(s > 0, 1, np.where(s < 0, -1, current_state))
        return res

    def recall_from_x(self, x_input, max_steps=10):
        x = 2 * np.array(x_input) - 1
        y = np.zeros(self.m, dtype=int)
        hist_x = [x_input]
        hist_y = []
        
        for _ in range(max_steps):
            s_y = x @ self.W
            y = self._activation(s_y, y)
            
            s_x = y @ self.W.T
            x = self._activation(s_x, x)
            
            hist_y.append(self._bipolar_to_binary(y))
            hist_x.append(self._bipolar_to_binary(x))
            
            if np.array_equal(x, 2*np.array(hist_x[-2])-1): # Упрощённая проверка стабильности
                break
        return self._bipolar_to_binary(x), self._bipolar_to_binary(y)

    def recall_from_y(self, y_input, max_steps=10):
        y = 2 * np.array(y_input) - 1
        x = np.zeros(self.n, dtype=int)
        hist_y = [y_input]
        hist_x = []
        
        for _ in range(max_steps):
            s_x = y @ self.W.T
            x = self._activation(s_x, x)
            
            s_y = x @ self.W
            y = self._activation(s_y, y)
            
            hist_x.append(self._bipolar_to_binary(x))
            hist_y.append(self._bipolar_to_binary(y))
            
            if np.array_equal(y, 2*np.array(hist_y[-2])-1):
                break
        return self._bipolar_to_binary(x), self._bipolar_to_binary(y)
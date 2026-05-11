import math
import random

a = 4
b = 8
d = 0.4
input_count = 3

learning_rate = 0.0662
min_error = 0.0001
max_epochs = 1000
random.seed(1)

def generate_data(a, b, d, num_points=45):
    period = 2 * math.pi / b
    step = period / (num_points - 1)
    x_values = []
    y_values = []
    for i in range(num_points):
        x = i * step
        y = a * math.sin(b * x) + d
        x_values.append(x)
        y_values.append(y)
    return x_values, y_values

def split_data(y_values, train_size=30):
    return y_values[:train_size], y_values[train_size:]

def create_train_samples(train_data, input_count):
    samples = []
    for i in range(len(train_data) - input_count):
        inputs = train_data[i:i + input_count]
        target = train_data[i + input_count]
        samples.append((inputs, target))
    return samples

def initialize_weights(input_count):
    weights = [random.uniform(-0.5, 0.5) for _ in range(input_count)]
    threshold = random.uniform(-0.5, 0.5)
    return weights, threshold

def train_epoch(train_samples, weights, threshold, learning_rate):
    train_loss = 0.0
    for inputs, target in train_samples:
        output = sum(w * inp for w, inp in zip(weights, inputs)) - threshold
        error = output - target
        train_loss += 0.5 * error * error

        for i in range(len(weights)):
            weights[i] -= learning_rate * error * inputs[i]
        threshold += learning_rate * error
    return train_loss, weights, threshold

def test_model(weights, threshold, train_data, test_data, input_count):
    test_loss = 0.0
    test_rows = []
    current_inputs = train_data[-input_count:].copy()

    for real_value in test_data:
        output = sum(w * inp for w, inp in zip(weights, current_inputs)) - threshold
        error = output - real_value
        test_loss += 0.5 * error * error
        test_rows.append((current_inputs.copy(), output, real_value))

        current_inputs.pop(0)
        current_inputs.append(output)
    return test_loss, test_rows

def print_data(x_values, y_values, train_data, test_data, train_samples, input_count):
    print("Этап 1: подготовка данных")
    for i in range(len(x_values)):
        print(f"x{i + 1} = {x_values[i]:.6f}; y{i + 1} = {y_values[i]:.6f}")

    print("\nЭтап 2: разделение данных на обучающую и тестовую выборки")
    print("\nОбучающая выборка:")
    for i in range(len(train_data)):
        print(f"x{i + 1} = {x_values[i]:.6f}; y{i + 1} = {train_data[i]:.6f}")
    print("\nТестовая выборка:")
    for i in range(len(test_data)):
        idx = len(train_data) + i
        print(f"x{idx + 1} = {x_values[idx]:.6f}; y{idx + 1} = {test_data[i]:.6f}")

    print("\nЭтап 3: подготовка обучающих и тестовых данных для ИНС")
    print("\nОбучающие наборы:")
    for i, (inputs, _) in enumerate(train_samples):
        left = ", ".join([f"y{i + j + 1}" for j in range(input_count)])
        right = f"y{i + input_count + 1}"
        print(f"{left} -> {right}")

    print("\nТестовые данные:")
    left = ", ".join([f"y{len(train_data) - input_count + j + 1}" for j in range(input_count)])
    print(f"{left} -> y'{len(train_data) + 1}")

def print_results(best_epoch, best_test_loss, best_rows, best_weights, best_threshold):
    print("\nЭтап 5: вывод результатов настроенной модели")
    print(f"Лучшая эпоха: {best_epoch}")
    print(f"Минимальная ошибка тестирования: {best_test_loss:.8f}")

    print("\nРезультаты прогнозирования:")
    for i, (inputs, predicted, real_value) in enumerate(best_rows):
        input_text = ", ".join([f"{val:.6f}" for val in inputs])
        print(f"{input_text} -> y'{31 + i} = {predicted:.6f} (y{31 + i} = {real_value:.6f})")

    print("\nИтоговые параметры модели:")
    for i, w in enumerate(best_weights):
        print(f"w{i + 1} = {w:.6f}")
    print(f"T = {best_threshold:.6f}")

def main():
    x_values, y_values = generate_data(a, b, d)
    train_data, test_data = split_data(y_values)
    train_samples = create_train_samples(train_data, input_count)

    print_data(x_values, y_values, train_data, test_data, train_samples, input_count)

    weights, threshold = initialize_weights(input_count)

    print("\nЭтап 4: обучение и тестирование модели")
    print(f"Шаг обучения = {learning_rate}")

    best_epoch = 0
    best_test_loss = 10**9
    best_rows = []
    best_weights = weights.copy()
    best_threshold = threshold

    for epoch in range(1, max_epochs + 1):
        train_loss, weights, threshold = train_epoch(train_samples, weights, threshold, learning_rate)

        test_loss, test_rows = test_model(weights, threshold, train_data, test_data, input_count)

        if test_loss < best_test_loss:
            best_test_loss = test_loss
            best_epoch = epoch
            best_rows = test_rows.copy()
            best_weights = weights.copy()
            best_threshold = threshold

        print(f"\nЭпоха №{epoch}")
        print(f"Ошибка обучения: {train_loss:.8f}\t Ошибка тестирования: {test_loss:.8f}")

        if test_loss <= min_error:
            print(f"Ошибка тестирования < {min_error} -> обучение остановлено")
            break
        else:
            print(f"Ошибка тестирования > {min_error} -> обучение продолжается")

    print_results(best_epoch, best_test_loss, best_rows, best_weights, best_threshold)

if __name__ == "__main__":
    main()
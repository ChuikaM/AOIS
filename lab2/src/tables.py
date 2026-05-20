def flip_binary_bits(vec, positions):
    noisy = vec.copy()
    for pos in positions:
        if pos < len(noisy):
            noisy[pos] = 1 - noisy[pos]
    return noisy


def build_full_noise_order(positions):
    return list(positions)


def recognition_result_for_mode(network, vector, noise_positions, mode="async"):
    noisy = flip_binary_bits(vector, noise_positions)
    if mode == "async":
        result, _ = network.predict_async(noisy, max_cycles=10)
    else:
        result, _ = network.predict_sync(noisy)
    return result == vector


def print_dominance_table(network, source_vectors, noisy_positions):
    print("Таблица доминирования:")
    print()
    print(f"{'N':>3} | {'HA':>9} | {'HS':>9}")
    print("-" * 31)

    vector_size = len(source_vectors[0])
    full_noise_orders = []

    for positions in noisy_positions:
        full_order = build_full_noise_order(positions)
        full_noise_orders.append(full_order)

    for noise_level in range(1, vector_size + 1):
        async_correct = 0
        sync_correct = 0
        total_vectors = len(source_vectors)

        for vector, full_order in zip(source_vectors, full_noise_orders):
            current_noise_positions = full_order[:noise_level]

            async_result = recognition_result_for_mode(
                network, vector, current_noise_positions, mode="async"
            )
            sync_result = recognition_result_for_mode(
                network, vector, current_noise_positions, mode="sync"
            )

            if async_result:
                async_correct += 1
            if sync_result:
                sync_correct += 1

        async_percent = async_correct / total_vectors * 100
        sync_percent = sync_correct / total_vectors * 100

        print(f"{noise_level:>3} | {async_percent:>8.2f}% | {sync_percent:>8.2f}%")

    print()


def print_hamming_accuracy_table(network):
    print("3. Таблица распознавания сети Хэмминга:")
    print()
    print(f"{'N':<6}{'Hm':<10}")
    print("-" * 16)

    for noisy_bits_count in range(1, network.input_size + 1):
        correct_recognitions = 0
        noisy_positions = list(range(noisy_bits_count))

        for vector_index, original_vector in enumerate(network.source_vectors):
            noisy_vector = flip_binary_bits(original_vector, noisy_positions)
            result = network.recognize(noisy_vector, save_history=False)
            if result.converged and result.winner_index == vector_index:
                correct_recognitions += 1

        accuracy_percent = correct_recognitions / network.output_size * 100
        print(f"{noisy_bits_count:<6}{accuracy_percent:.2f}%")

    print()


def print_dominance_table_simple(dominance_x, dominance_y):
    print("Таблица доминирования:")
    print("-" * 61)
    print(f"{'Число искажённых битов':<28}{'BAM X':<16}{'BAM Y':<16}")
    print("-" * 61)

    max_rows = max(len(dominance_x), len(dominance_y))

    for index in range(max_rows):
        bits = index + 1
        x_value = f"{dominance_x[index]:.2f}%" if index < len(dominance_x) else "-"
        y_value = f"{dominance_y[index]:.2f}%" if index < len(dominance_y) else "-"
        print(f"{bits:<28}{x_value:<16}{y_value:<16}")

    print("-" * 61)
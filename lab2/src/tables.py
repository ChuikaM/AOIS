import numpy as np
from rich.console import Console
from rich.table import Table
from rich.text import Text

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
    indices = [i for i, (a, b) in enumerate(zip(result, vector)) if a == b]
    threshold = 0.5
    return len(indices) / len(vector) > threshold

def fmt_orig(vec):
    return Text(" ".join(map(str, vec)), style="dim cyan")

def fmt_rec(original, recovered):
    txt = Text()
    for i in range(len(recovered)):
        bit = str(recovered[i])
        style = "bold red on yellow" if recovered[i] != original[i] else "green"
        txt.append(bit, style=style)
        if i < len(recovered) - 1:
            txt.append(" ")
    return txt

def print_dominance_table(network, source_vectors, noisy_positions):
    if not source_vectors:
        print("Таблица доминирования: нет данных для отображения\n")
        return

    console = Console()
    total_vectors = len(source_vectors)
    vector_size = len(source_vectors[0])
    demo_idx = 0

    table = Table(title="Таблица доминирования (Hopfield)", show_lines=True)
    table.add_column("N", justify="center", style="bold cyan", width=4)
    table.add_column("Async %", justify="center", style="bold green", width=10)
    table.add_column("Оригинал", justify="left", style="dim", width=45)
    table.add_column("Восстановлен (Async)", justify="left", width=45)
    table.add_column("Sync %", justify="center", style="bold green", width=8)
    table.add_column("Оригинал", justify="left", style="dim", width=45)
    table.add_column("Восстановлен (Sync)", justify="left", width=45)

    full_noise_orders = [build_full_noise_order(p) for p in noisy_positions]

    for noise_level in range(1, vector_size + 1):
        async_correct, sync_correct = 0, 0

        for vector, full_order in zip(source_vectors, full_noise_orders):
            current_noise = full_order[:noise_level]
            noisy_vec = flip_binary_bits(vector, current_noise)

            res_async, _ = network.predict_async(noisy_vec, max_cycles=10)
            res_sync, _ = network.predict_sync(noisy_vec)

            if res_async == vector: async_correct += 1
            if res_sync == vector: sync_correct += 1

        async_pct = (async_correct / total_vectors) * 100
        sync_pct = (sync_correct / total_vectors) * 100

        demo_vec = source_vectors[demo_idx]
        demo_noise = full_noise_orders[demo_idx][:noise_level]
        demo_noisy = flip_binary_bits(demo_vec, demo_noise)

        rec_async, _ = network.predict_async(demo_noisy, max_cycles=10)
        rec_sync, _ = network.predict_sync(demo_noisy)

        table.add_row(
            str(noise_level),
            f"{async_pct:.1f}%",
            fmt_orig(demo_vec),
            fmt_rec(demo_vec, rec_async),
            f"{sync_pct:.1f}%",
            fmt_orig(demo_vec),
            fmt_rec(demo_vec, rec_sync)
        )

    console.print(table)
    console.print(f"[dim]Всего векторов: {total_vectors} | Размер: {vector_size} бит[/dim]\n")


def print_hamming_accuracy_table(network):
    if not network.source_vectors:
        print("Таблица распознавания сети Хэмминга: нет данных для отображения\n")
        return

    console = Console()
    total_vectors = len(network.source_vectors)
    vector_size = network.input_size
    demo_idx = 0

    table = Table(title="Таблица распознавания сети Хэмминга", show_lines=True)
    table.add_column("N", justify="center", style="bold cyan", width=4)
    table.add_column("Точность %", justify="center", style="bold green", width=10)
    table.add_column("Оригинал", justify="left", style="dim", width=45)
    table.add_column("Восстановлен", justify="left", width=45)

    for noisy_bits_count in range(1, vector_size + 1):
        correct_recognitions = 0
        noisy_positions = list(range(noisy_bits_count))

        for vector_index, original_vector in enumerate(network.source_vectors):
            noisy_vector = flip_binary_bits(original_vector, noisy_positions)
            result_vec, _ = network.predict(noisy_vector)
            if result_vec == original_vector:
                correct_recognitions += 1

        accuracy_pct = (correct_recognitions / total_vectors) * 100

        demo_orig = network.source_vectors[demo_idx]
        demo_noisy = flip_binary_bits(demo_orig, noisy_positions)
        demo_recovered, _ = network.predict(demo_noisy)

        table.add_row(
            str(noisy_bits_count),
            f"{accuracy_pct:.1f}%",
            fmt_orig(demo_orig),
            fmt_rec(demo_orig, demo_recovered)
        )

    console.print(table)
    console.print(f"[dim]Всего векторов: {total_vectors} | Размер: {vector_size} бит[/dim]\n")


def print_dominance_table_simple(bam_net, source_x, source_y, max_bits=None):
    if not source_x or not source_y:
        print("Таблица доминирования BAM: нет данных для отображения\n")
        return

    console = Console()
    total_vectors = len(source_x)
    limit = max_bits or len(source_x[0])

    table = Table(title="Таблица доминирования BAM", show_lines=True)
    table.add_column("N", justify="center", style="bold cyan", width=4)
    table.add_column("X→Y %", justify="center", style="bold green", width=8)
    table.add_column("Оригинал (Y)", justify="left", style="dim", width=30)
    table.add_column("Восстановлен (Y)", justify="left", width=30)
    table.add_column("Y→X %", justify="center", style="bold green", width=8)
    table.add_column("Оригинал (X)", justify="left", style="dim", width=30)
    table.add_column("Восстановлен (X)", justify="left", width=30)

    for n in range(1, limit + 1):
        x_ok, y_ok = 0, 0
        x_results = []
        y_results = []

        for idx in range(total_vectors):
            nx = source_x[idx].copy()
            for k in range(n):
                if k < len(nx): nx[k] = 1 - nx[k]
            _, ry = bam_net.recall_from_x(nx)
            x_correct = (ry == source_y[idx])
            if x_correct: x_ok += 1
            x_results.append((source_y[idx], ry, x_correct))

            ny = source_y[idx].copy()
            for k in range(n):
                if k < len(ny): ny[k] = 1 - ny[k]
            rx, _ = bam_net.recall_from_y(ny)
            y_correct = (rx == source_x[idx])
            if y_correct: y_ok += 1
            y_results.append((source_x[idx], rx, y_correct))

        x_pct = (x_ok / total_vectors) * 100
        y_pct = (y_ok / total_vectors) * 100

        demo_x_idx = next((i for i, (_, _, correct) in enumerate(x_results) if not correct), 0)
        demo_y_idx = next((i for i, (_, _, correct) in enumerate(y_results) if not correct), 0)

        orig_y, rec_y, _ = x_results[demo_x_idx]
        orig_x, rec_x, _ = y_results[demo_y_idx]

        table.add_row(
            str(n), f"{x_pct:.1f}%", fmt_orig(orig_y), fmt_rec(orig_y, rec_y),
            f"{y_pct:.1f}%", fmt_orig(orig_x), fmt_rec(orig_x, rec_x)
        )

    console.print(table)
    console.print(f"[dim]Всего векторов: {total_vectors} | Размер: {limit} бит[/dim]\n")
import numpy as np
from rich.console import Console
from rich.table import Table
from rich.text import Text
from rich.bar import Bar
import random

from rich.text import Text

def fmt_orig(vec):
    return Text(" ".join(map(str, vec)), style="dim cyan")


def fmt_rec(original, recovered):
    txt = Text()
    for i in range(len(recovered)):
        bit = str(recovered[i])
       
        style = "bold red on yellow" if recovered[i] != original[i] else "green"
        txt.append(bit, style=style)
        if i < len(recovered) - 1:
            txt.append(" ")  # пробел между битами
    return txt


def create_success_bar(success_rate, width=30):
    green_blocks = int(width * success_rate / 100)
    red_blocks = width - green_blocks
    if red_blocks == 0:
        return Text("█" * width, style="green")
    return Text("█" * green_blocks, style="green") + Text("░" * red_blocks, style="red")


def flip_binary_bits_random(vec, noise_level, rng=None):
    if rng is None:
        rng = np.random.default_rng()
    noisy = vec.copy()
    n_bits = min(noise_level, len(vec))
    positions = rng.choice(len(vec), size=n_bits, replace=False)
    for pos in positions:
        noisy[pos] = 1 - noisy[pos]
    return noisy



def flip_binary_bits_random(vec, noise_level, rng=None):
    if rng is None:
        rng = np.random.default_rng()
    noisy = vec.copy()
    positions = rng.choice(len(vec), size=min(noise_level, len(vec)), replace=False)
    for pos in positions:
        noisy[pos] = 1 - noisy[pos]
    return noisy

def create_success_bar(success_rate, width=30):
    green_blocks = int(width * success_rate / 100)
    red_blocks = width - green_blocks
    bar_text = "█" * green_blocks + "░" * red_blocks
    return Text(bar_text, style="green") if red_blocks == 0 else Text(
        "█" * green_blocks, style="green"
    ) + Text("░" * red_blocks, style="red")

def print_statistical_table(
    network, 
    source_vectors, 
    noise_levels, 
    trials_per_level=50, 
    mode="async",
    network_name="Network"
):
    if not source_vectors:
        print(f"Таблица {network_name}: нет данных для отображения\n")
        return

    console = Console()
    total_vectors = len(source_vectors)
    vector_size = len(source_vectors[0])
    rng = np.random.default_rng(42)  # для воспроизводимости

    table = Table(title=f"Статистика восстановления — {network_name}", show_lines=True)
    table.add_column("Шум", justify="center", style="bold cyan", width=6)
    table.add_column("Успех %", justify="center", style="bold green", width=10)
    table.add_column("Визуализация", justify="left", width=35)

    for noise_level in noise_levels:
        successful_trials = 0
        
        for vector in source_vectors:
            for _ in range(trials_per_level):
                noisy_vec = flip_binary_bits_random(vector, noise_level, rng)
                
                if mode == "async":
                    result, _ = network.predict_async(noisy_vec, max_cycles=10)
                elif mode == "sync":
                    result, _ = network.predict_sync(noisy_vec)
                else:
                    result, _ = network.predict(noisy_vec)
                
                if result == vector:
                    successful_trials += 1
        
        success_rate = (successful_trials / (trials_per_level * len(source_vectors))) * 100
        
        bar = create_success_bar(success_rate, width=30)
        table.add_row(
            str(noise_level),
            f"{success_rate:.1f}%",
            bar
        )

    console.print(table)
    console.print(f"[dim]Статистика: {trials_per_level} испытаний × {total_vectors}  тестов на уровень шума[/dim]\n")


def print_dominance_table_statistical(network, source_vectors, max_noise=20, trials=50):
    print_statistical_table(
        network, source_vectors, range(1, max_noise+1), 
        trials_per_level=trials, mode="async", network_name="Hopfield (Async)"
    )


def print_dominance_table_sync_statistical(network, source_vectors, max_noise=20, trials=50):
    print_statistical_table(
        network, source_vectors, range(1, max_noise+1), 
        trials_per_level=trials, mode="sync", network_name="Hopfield (Sync)"
    )


def print_hamming_accuracy_table_statistical(network, max_noise=20, trials=50):
    if not network.source_vectors:
        print("Таблица Хэмминга: нет данных для отображения\n")
        return
    print_statistical_table(
        network, network.source_vectors, range(1, max_noise+1), 
        trials_per_level=trials, mode="default", network_name="Hamming Network"
    )


def print_bam_table_statistical_x_to_y(bam_net, source_x, source_y, max_bits=13, trials=50):
    if not source_x or not source_y:
        print("Таблица BAM: нет данных для отображения\n")
        return

    console = Console()
    total_vectors = len(source_x)
    rng = np.random.default_rng(42)

    table = Table(title="Статистика восстановления BAM (X→Y)", show_lines=True)
    table.add_column("Шум", justify="center", style="bold cyan", width=6)
    table.add_column("Успех %", justify="center", style="bold green", width=10)
    table.add_column("Визуализация", justify="left", width=35)

    for noise_level in range(1, max_bits + 1):
        total_trials = 0
        successful_trials = 0
        
        for x_vec, y_true in zip(source_x, source_y):
            for _ in range(trials):
                noisy_x = flip_binary_bits_random(x_vec, noise_level, rng)
                _, y_rec = bam_net.recall_from_x(noisy_x)
                total_trials += 1
                if y_rec == y_true:
                    successful_trials += 1
        
        success_rate = (successful_trials / total_trials * 100) if total_trials > 0 else 0
        bar = create_success_bar(success_rate, width=30)
        
        table.add_row(str(noise_level), f"{success_rate:.1f}%", bar)

    console.print(table)
    console.print(f"[dim]BAM статистика: {trials} испытаний × {total_vectors} пар = {total_trials} тестов/уровень[/dim]\n")


def print_bam_table_statistical_y_to_x(bam_net, source_x, source_y, max_bits=None, trials=50):
    """Статистическая таблица для BAM (направление Y→X)"""
    if not source_x or not source_y:
        print("Таблица BAM (Y→X): нет данных для отображения\n")
        return

    console = Console()
    total_vectors = len(source_x)
    rng = np.random.default_rng(42)

    # Если max_bits не указан, берём длину Y-вектора (шум накладывается на Y)
    limit = max_bits if max_bits is not None else len(source_y[0])

    table = Table(title="Статистика восстановления BAM (Y→X)", show_lines=True)
    table.add_column("Шум", justify="center", style="bold cyan", width=6)
    table.add_column("Успех %", justify="center", style="bold green", width=10)
    table.add_column("Визуализация", justify="left", width=35)

    for noise_level in range(1, limit + 1):
        total_trials = 0
        successful_trials = 0
        
        for x_true, y_vec in zip(source_x, source_y):
            for _ in range(trials):
                noisy_y = flip_binary_bits_random(y_vec, noise_level, rng)
                x_rec, _ = bam_net.recall_from_y(noisy_y)
                total_trials += 1
                if x_rec == x_true:
                    successful_trials += 1
        
        success_rate = (successful_trials / total_trials * 100) if total_trials > 0 else 0
        bar = create_success_bar(success_rate, width=30)
        
        table.add_row(str(noise_level), f"{success_rate:.1f}%", bar)

    console.print(table)
    console.print(f"[dim]BAM статистика (Y→X): {trials} испытаний × {total_vectors} пар = {total_trials} тестов/уровень[/dim]\n")
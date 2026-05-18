import random
import math
import os

# ==========================================
# МОДЕЛЬ ПАМЯТИ И КЭША (ВАРИАНТ 4)
# RAM: 64 блока по 4 байта (всего 256 байт)
# Кэш: 16 линий по 4 байта (всего 64 байта)
# Тип: Прямое отображение (Direct Mapped)
# Стратегия замещения: FIFO
# ==========================================

class RAM:
    def __init__(self, num_blocks, block_size):
        self.num_blocks = num_blocks
        self.block_size = block_size
        # Инициализация нулями
        self.data = [[0] * block_size for _ in range(num_blocks)]

    def read_block(self, block_addr):
        if 0 <= block_addr < self.num_blocks:
            return self.data[block_addr].copy()
        raise IndexError("Адрес блока RAM вне диапазона")

    def write_block(self, block_addr, data):
        if 0 <= block_addr < self.num_blocks:
            self.data[block_addr] = data.copy()
        else:
            raise IndexError("Адрес блока RAM вне диапазона")

    def read_byte(self, byte_addr):
        block = byte_addr // self.block_size
        offset = byte_addr % self.block_size
        return self.data[block][offset]

    def write_byte(self, byte_addr, value):
        block = byte_addr // self.block_size
        offset = byte_addr % self.block_size
        self.data[block][offset] = value


class CacheLine:
    def __init__(self):
        self.valid = False
        self.tag = -1
        self.data = []
        self.fifo_age = 0  # Для отслеживания "возраста" данных (FIFO)


class DirectMappedCache:
    def __init__(self, ram_ref, num_lines, block_size):
        self.ram = ram_ref
        self.num_lines = num_lines
        self.block_size = block_size
        self.lines = [CacheLine() for _ in range(num_lines)]
        
        self.hits = 0
        self.misses = 0
        self.fifo_counter = 0
        self.usage_history = []  # Для вычисления среднего использования

        # Параметры адресации
        self.offset_bits = int(math.log2(block_size))          # 2 бита (4 байта)
        self.index_bits = int(math.log2(num_lines))            # 4 бита (16 линий)
        self.tag_bits = 8 - self.offset_bits - self.index_bits # 2 бита (адресация 0-255)

    def _decode_address(self, addr):
        offset = addr & ((1 << self.offset_bits) - 1)
        index = (addr >> self.offset_bits) & ((1 << self.index_bits) - 1)
        tag = addr >> (self.offset_bits + self.index_bits)
        return tag, index, offset

    def _log_usage(self):
        current_usage = sum(1 for line in self.lines if line.valid)
        self.usage_history.append(current_usage)

    def read(self, addr, debug=False):
        tag, index, offset = self._decode_address(addr)
        line = self.lines[index]
        hit = line.valid and line.tag == tag

        if debug:
            print(f"1. Input address: {addr}")
            print(f"2. Binary: {addr:08b}")
            print(f"3. Threshold (Offset): {offset} -> {offset:0{self.offset_bits}b}")
            print(f"4. Index: {index} -> {index:0{self.index_bits}b}")
            print(f"5. Tag: {tag} -> {tag:0{self.tag_bits}b}")
            print("6. Поиск в кэше...", end=" ")

        if hit:
            self.hits += 1
            if debug: print("КЭШ-ПОПАДАНИЕ. Возвращается результат.")
            value = line.data[offset]
        else:
            self.misses += 1
            if debug: print("КЭШ-ПРОМАХ. Обращение к RAM для загрузки блока.")
            # Загрузка блока из RAM
            block_data = self.ram.read_block(addr // self.block_size)
            # FIFO-замещение: перезаписываем строку, обновляем возраст
            self.fifo_counter += 1
            line.valid = True
            line.tag = tag
            line.data = block_data
            line.fifo_age = self.fifo_counter
            value = line.data[offset]

        self._log_usage()
        return value

    def write(self, addr, value, debug=False):
        tag, index, offset = self._decode_address(addr)
        line = self.lines[index]
        hit = line.valid and line.tag == tag

        if hit:
            self.hits += 1
            line.data[offset] = value
            self.ram.write_byte(addr, value)  # Write-Through
            if debug: print("КЭШ-ПОПАДАНИЕ (Запись).")
        else:
            self.misses += 1
            if debug: print("КЭШ-ПРОМАХ (Запись). Загрузка блока из RAM, запись, обновление RAM.")
            # Write-Allocate
            block_data = self.ram.read_block(addr // self.block_size)
            block_data[offset] = value
            self.fifo_counter += 1
            line.valid = True
            line.tag = tag
            line.data = block_data
            line.fifo_age = self.fifo_counter
            self.ram.write_byte(addr, value)

        self._log_usage()


class CacheLabSimulator:
    def __init__(self):
        # Вариант 4: RAM 64x4, Кэш 16x4
        self.ram = RAM(num_blocks=64, block_size=4)
        self.cache = DirectMappedCache(self.ram, num_lines=16, block_size=4)
        self._init_demo_data()

    def _init_demo_data(self):
        # Заполняем RAM последовательными числами для наглядности
        val = 0
        for i in range(64):
            self.ram.write_block(i, [val + j for j in range(4)])
            val += 4

    def load_file(self, filepath):
        if not os.path.exists(filepath):
            print(f"Файл {filepath} не найден. Создается демо-файл.")
            with open("demo_file.txt", "w", encoding="utf-8") as f:
                f.write("Кэш-память позволяет ускорить доступ к часто используемым данным в системе.")
            filepath = "demo_file.txt"

        with open(filepath, "r", encoding="utf-8") as f:
            text = f.read(256)  # Ограничиваем размером RAM (256 байт)

        # Очищаем RAM и записываем байты файла
        for i in range(64):
            self.ram.write_block(i, [0]*4)
            
        byte_idx = 0
        for char in text:
            if byte_idx >= 256: break
            self.ram.write_byte(byte_idx, ord(char))
            byte_idx += 1
        print(f"Файл '{filepath}' загружен в RAM ({len(text)} символов).")

    def run_linear_access(self):
        print("\n=== ЛИНЕЙНЫЙ ДОСТУП (Чтение последовательно) ===")
        for addr in range(256):
            self.cache.read(addr)
        self._print_stats()

    def run_random_access(self):
        print("\n=== СЛУЧАЙНЫЙ ДОСТУП (Хаотичный опрос) ===")
        num_requests = 50
        addresses = random.sample(range(256), min(num_requests, 256))
        for addr in addresses:
            self.cache.read(addr)
        self._print_stats()

    def run_local_access(self):
        print("\n=== ЛОКАЛЬНЫЙ ДОСТУП (Группа адресов) ===")
        base = 100
        window = 16  # Группа из 16 адресов
        num_requests = 60
        for _ in range(num_requests):
            # Имитация локальности: 80% запросов в окно, 20% случайные
            if random.random() < 0.8:
                addr = base + random.randint(0, window-1)
            else:
                addr = random.randint(0, 255)
            self.cache.read(addr)
        self._print_stats()

    def manual_input(self):
        print("\n=== РУЧНОЙ ВВОД АДРЕСА ===")
        try:
            addr = int(input("Введите адрес (0-255): "))
            if not (0 <= addr <= 255): raise ValueError
        except ValueError:
            print("Некорректный ввод. Требуется число от 0 до 255.")
            return

        print("-" * 40)
        self.cache.read(addr, debug=True)
        print("-" * 40)
        self._print_stats()

    def print_contents(self):
        print("\n=== СОДЕРЖИМОЕ RAM (первые 8 блоков) ===")
        for i in range(8):
            print(f"RAM[{i}]: {self.ram.read_block(i)}")
            
        print("\n=== СОДЕРЖИМОЕ КЭША (все 16 линий) ===")
        for i, line in enumerate(self.cache.lines):
            status = "VALID" if line.valid else "EMPTY"
            tag_str = f"Tag:{line.tag}" if line.valid else "Tag:-"
            fifo_str = f"FIFO:{line.fifo_age}" if line.valid else "FIFO:-"
            print(f"Cache[{i:2d}] | {status:5s} | {tag_str:5s} | {fifo_str:6s} | Data: {line.data if line.valid else '[  ]'}")

    def show_usage_stats(self):
        valid_lines = sum(1 for line in self.cache.lines if line.valid)
        avg_usage = sum(self.cache.usage_history) / len(self.cache.usage_history) if self.cache.usage_history else 0
        print(f"\n--- Статистика кэш-памяти ---")
        print(f"Текущий объем используемой кэш-памяти: {valid_lines} / {self.cache.num_lines} линий ({valid_lines*4} / {self.cache.num_lines*4} байт)")
        print(f"Среднее значение используемой кэш-памяти: {avg_usage:.2f} линий за время моделирования")

    def _print_stats(self):
        total = self.cache.hits + self.cache.misses
        hit_rate = (self.cache.hits / total * 100) if total > 0 else 0
        miss_rate = (self.cache.misses / total * 100) if total > 0 else 0
        print(f"📊 Всего обращений: {total} | Попадания: {self.cache.hits} ({hit_rate:.1f}%) | Промахи: {self.cache.misses} ({miss_rate:.1f}%)")
        self.show_usage_stats()

    def run(self):
        while True:
            print("\n" + "="*50)
            print("ЛАБОРАТОРНАЯ РАБОТА №4 | ВАРИАНТ 4: Direct Mapped Cache")
            print("="*50)
            print("1. Загрузить файл в RAM (линейное считывание)")
            print("2. Тест случайного доступа к адресам")
            print("3. Тест локального доступа к группе адресов")
            print("4. Ручной ввод адреса + Debug-лог")
            print("5. Вывод содержимого RAM и Кэша")
            print("6. Показать статистику и среднее использование")
            print("7. Сбросить кэш и статистику")
            print("0. Выход")
            
            choice = input("\nВыберите действие: ").strip()
            
            if choice == '1': self.load_file(input("Путь к файлу (Enter для демо): ") or "demo_file.txt")
            elif choice == '2': self.run_random_access()
            elif choice == '3': self.run_local_access()
            elif choice == '4': self.manual_input()
            elif choice == '5': self.print_contents()
            elif choice == '6': self.show_usage_stats()
            elif choice == '7': 
                self.cache = DirectMappedCache(self.ram, num_lines=16, block_size=4)
                print("Кэш и статистика сброшены.")
            elif choice == '0': break
            else: print("Неверный выбор. Попробуйте снова.")

if __name__ == "__main__":
    simulator = CacheLabSimulator()
    simulator.run()
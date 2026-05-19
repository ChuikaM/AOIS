from typing import Tuple, List
from ram import RAM

class DirectMappedCache:
    def __init__(self, lines: int, block_size: int):
        self.lines = lines
        self.block_size = block_size
        self.valid = [False] * lines
        self.tags = [0] * lines
        self.data = [[0] * block_size for _ in range(lines)]
        self.hits = 0
        self.misses = 0

    def reset_stats(self):
        self.hits = 0
        self.misses = 0

    def clear(self):
        self.valid = [False] * self.lines
        self.tags = [0] * self.lines
        self.data = [[0] * self.block_size for _ in range(self.lines)]

    def _get_block_addr(self, addr: int) -> int:
        return addr // self.block_size

    def _get_offset(self, addr: int) -> int:
        return addr % self.block_size

    def _get_index(self, block_addr: int) -> int:
        return block_addr % self.lines

    def _get_tag(self, block_addr: int) -> int:
        return block_addr // self.lines

    def _to_char(self, b: int) -> str:
        return chr(b) if 32 <= b <= 126 else '.'

    def read(self, addr: int, ram: RAM) -> Tuple[int, bool]:
        block_addr = self._get_block_addr(addr)
        offset = self._get_offset(addr)
        idx = self._get_index(block_addr)
        tag = self._get_tag(block_addr)

        hit = self.valid[idx] and self.tags[idx] == tag
        if hit:
            self.hits += 1
            value = self.data[idx][offset]
            return value, True
        else:
            self.misses += 1
            block_data = ram.read_block(block_addr)
            self.valid[idx] = True
            self.tags[idx] = tag
            self.data[idx] = block_data[:]
            return self.data[idx][offset], False

    def write(self, addr: int, value: int, ram: RAM):
        block_addr = self._get_block_addr(addr)
        offset = self._get_offset(addr)
        idx = self._get_index(block_addr)
        tag = self._get_tag(block_addr)

        ram.write_byte(addr, value)

        hit = self.valid[idx] and self.tags[idx] == tag
        if hit:
            self.data[idx][offset] = value & 0xFF
        else:
            block_data = ram.read_block(block_addr)
            self.valid[idx] = True
            self.tags[idx] = tag
            self.data[idx] = block_data[:]
            self.data[idx][offset] = value & 0xFF

    def get_used_lines(self) -> int:
        return sum(1 for v in self.valid if v)

    def get_usage_ratio(self) -> float:
        return self.get_used_lines() / self.lines

    def hit_ratio(self) -> float:
        total = self.hits + self.misses
        return self.hits / total if total > 0 else 0.0

    def display(self):
        print("\nКэш (прямое отображение, 16 строк по 4 байта):")
        print(f"{'Idx':<4} {'Valid':<5} {'Tag':<5} {'Data (hex)':<20} {'Data (char)':<10}")
        for i in range(self.lines):
            if self.valid[i]:
                data_hex = ' '.join(f'{b:02x}' for b in self.data[i])
                data_char = ''.join(self._to_char(b) for b in self.data[i])
                print(f"{i:<4} {'Да':<5} {self.tags[i]:<5} {data_hex:<20} {data_char:<10}")
            else:
                print(f"{i:<4} {'Нет':<5} {'-':<5} {'-':<20} {'-':<10}")

    def debug_lookup(self, addr: int, ram: RAM):
        block_addr = self._get_block_addr(addr)
        offset = self._get_offset(addr)
        idx = self._get_index(block_addr)
        tag = self._get_tag(block_addr)
        binary = f"{addr:08b}"
        print(f"Входной адрес: {addr}")
        print(f"Двоичный: {binary}")
        print(f"Смещение (offset): {offset:02b} ({offset})")
        print(f"Индекс (index): {idx:04b} ({idx})")
        print(f"Тег (tag): {tag:02b} ({tag})")

        if self.valid[idx] and self.tags[idx] == tag:
            print("Результат: КЭШ-ПОПАДАНИЕ")
            print(f"Значение из кэша: {self.data[idx][offset]} (символ: {self._to_char(self.data[idx][offset])})")
        else:
            print("Результат: КЭШ-ПРОМАХ")
            block_data = ram.read_block(block_addr)
            val = block_data[offset]
            print(f"Значение из RAM: {val} (символ: {self._to_char(val)})")
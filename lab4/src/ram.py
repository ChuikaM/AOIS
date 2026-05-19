import os
from typing import List

class RAM:
    def __init__(self, rows: int, block_size: int):
        self.block_size = block_size
        self.rows = rows
        self.size = rows * block_size
        self.memory = bytearray(self.size)
        self.data_size = 0

    def read_byte(self, addr: int) -> int:
        if addr < 0 or addr >= self.size:
            raise ValueError("Адрес вне диапазона RAM")
        return self.memory[addr]

    def write_byte(self, addr: int, value: int):
        if addr < 0 or addr >= self.size:
            raise ValueError("Адрес вне диапазона RAM")
        self.memory[addr] = value & 0xFF

    def read_block(self, block_addr: int) -> List[int]:
        start = block_addr * self.block_size
        return [self.memory[start + i] for i in range(self.block_size)]

    def write_block(self, block_addr: int, data: List[int]):
        start = block_addr * self.block_size
        for i in range(self.block_size):
            self.memory[start + i] = data[i] & 0xFF

    def load_from_file(self, filename: str):
        if not os.path.exists(filename):
            raise FileNotFoundError(f"Файл {filename} не найден")
        with open(filename, 'rb') as f:
            data = f.read()
        self.data_size = min(len(data), self.size)
        self.memory[:self.data_size] = data[:self.data_size]

    def _to_char(self, b: int) -> str:
        return chr(b) if 32 <= b <= 126 else '.'

    def display(self, rows_per_line: int = 8):
        print("\nСодержимое RAM (байты, символы):")
        for i in range(0, self.size, self.block_size):
            block = self.memory[i:i+self.block_size]
            hex_str = ' '.join(f'{b:02x}' for b in block)
            char_str = ''.join(self._to_char(b) for b in block)
            print(f"Блок {i//self.block_size:2d}: {hex_str}  |{char_str}|")
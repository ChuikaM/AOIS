import random
from ram import RAM
from cache import DirectMappedCache

class AccessPatterns:
    @staticmethod
    def linear_read(ram: RAM, cache: DirectMappedCache):
        print("\n--- Линейное считывание ---")
        for addr in range(ram.data_size):
            cache.read(addr, ram)

    @staticmethod
    def random_read(ram: RAM, cache: DirectMappedCache, num_accesses: int):
        print("\n--- Случайное считывание ---")
        for _ in range(num_accesses):
            addr = random.randint(0, ram.size - 1)
            cache.read(addr, ram)

    @staticmethod
    def local_read(ram: RAM, cache: DirectMappedCache, region_start: int, region_size: int, num_accesses: int):
        print("\n--- Локальное считывание ---")
        for _ in range(num_accesses):
            addr = random.randint(region_start, region_start + region_size - 1)
            addr = min(addr, ram.size - 1)
            cache.read(addr, ram)
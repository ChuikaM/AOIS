import os
from ram import RAM
from cache import DirectMappedCache
from access_patterns import AccessPatterns

def main():
    RAM_ROWS = 64
    BLOCK_SIZE = 4
    CACHE_LINES = 16

    ram = RAM(RAM_ROWS, BLOCK_SIZE)
    cache = DirectMappedCache(CACHE_LINES, BLOCK_SIZE)

    test_file = "test_data.txt"
    if not os.path.exists(test_file):
        with open(test_file, 'w') as f:
            f.write("A" * 256)

    while True:
        print("\n--- МЕНЮ ---")
        print("1. Загрузить данные из файла в RAM")
        print("2. Линейное считывание (весь RAM)")
        print("3. Случайное считывание (200 обращений)")
        print("4. Локальное считывание (200 обращений, область 0-31)")
        print("5. Показать содержимое RAM")
        print("6. Показать содержимое кэша")
        print("7. Ручной поиск адреса (debug-лог)")
        print("8. Показать статистику (попадания/промахи)")
        print("9. Сбросить статистику кэша")
        print("10. Очистить кэш (сбросить все строки)")
        print("0. Выход")

        choice = input("Выберите действие: ")

        if choice == '1':
            try:
                ram.load_from_file(test_file)
                print(f"Файл '{test_file}' загружен в RAM (размер {ram.size} байт)")
            except Exception as e:
                print(f"Ошибка: {e}")
        elif choice == '2':
            cache.reset_stats()
            AccessPatterns.linear_read(ram, cache)
            print(f"Попадания: {cache.hits}, Промахи: {cache.misses}, Доля попаданий: {cache.hit_ratio():.2%}")
            print(f"Используемых строк кэша: {cache.get_used_lines()} из {CACHE_LINES}")
        elif choice == '3':
            cache.reset_stats()
            AccessPatterns.random_read(ram, cache, 200)
            print(f"Попадания: {cache.hits}, Промахи: {cache.misses}, Доля попаданий: {cache.hit_ratio():.2%}")
            print(f"Используемых строк кэша: {cache.get_used_lines()} из {CACHE_LINES}")
        elif choice == '4':
            cache.reset_stats()
            AccessPatterns.local_read(ram, cache, 0, 32, 200)
            print(f"Попадания: {cache.hits}, Промахи: {cache.misses}, Доля попаданий: {cache.hit_ratio():.2%}")
            print(f"Используемых строк кэша: {cache.get_used_lines()} из {CACHE_LINES}")
        elif choice == '5':
            ram.display()
        elif choice == '6':
            cache.display()
        elif choice == '7':
            try:
                addr = int(input("Введите адрес (0-255): "))
                if 0 <= addr < ram.size:
                    cache.debug_lookup(addr, ram)
                else:
                    print("Адрес вне диапазона")
            except ValueError:
                print("Некорректный ввод")
        elif choice == '8':
            print(f"Статистика кэша: попадания = {cache.hits}, промахи = {cache.misses}, доля = {cache.hit_ratio():.2%}")
            print(f"Используемая память кэша: {cache.get_used_lines() * BLOCK_SIZE} байт из {CACHE_LINES * BLOCK_SIZE} (доля строк: {cache.get_usage_ratio():.2%})")
        elif choice == '9':
            cache.reset_stats()
            print("Статистика сброшена")
        elif choice == '10':
            cache.clear()
            print("Кэш очищен (все строки сброшены)")
        elif choice == '0':
            print("Выход из программы")
            break
        else:
            print("Неверный выбор, повторите")

if __name__ == "__main__":
    main()
import sys
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) != 2:
        print("Использование: python plot.py <файл_данных>")
        sys.exit(1)

    filename = sys.argv[1]
    indices = []
    values = []

    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split()
            if len(parts) != 2:
                continue
            try:
                idx = int(parts[0])
                val = int(parts[1])
                indices.append(idx)
                values.append(val)
            except ValueError:
                continue

    plt.plot(indices, values, marker='o', linestyle='-')
    plt.xlabel('Добавлено элементов')
    plt.ylabel('Кол-во коллизий')
    plt.title('График коллизий')
    plt.grid(True)
    plt.show()

if __name__ == '__main__':
    main()
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

def draw_figure(coordinates, scale, connection_types, filename='figure.png'):
    fig, ax = plt.subplots()
    ax.set_aspect('equal')
    ax.set_axis_off()
    fig.subplots_adjust(left=0, right=1, bottom=0, top=1)
    plt.gca().set_xlim([0, scale])
    plt.gca().set_ylim([0, scale])

    # Создаем список сегментов для фигуры
    segments = []

    for i in range(len(coordinates)):
        x0, y0 = coordinates[i]
        x1, y1 = coordinates[(i + 1) % len(coordinates)]
        midpoint = ((x0 + x1) / 2, (y0 + y1) / 2)
        radius = np.hypot(x1 - x0, y1 - y0) / 2
        angle = np.arctan2(y1 - y0, x1 - x0)

        if connection_types[i] == 'line':
            segments.append(((x0, y0), (x1, y1)))
        elif connection_types[i] in ['convex', 'concave']:
            # Вычисляем центр окружности
            if connection_types[i] == 'convex':
                center = (midpoint[0] - radius * np.sin(angle), midpoint[1] + radius * np.cos(angle))
            else:  # 'concave'
                center = (midpoint[0] + radius * np.sin(angle), midpoint[1] - radius * np.cos(angle))

            # Вычисляем начальный и конечный углы дуги
            start_angle = np.degrees(np.arctan2(y0 - center[1], x0 - center[0]))
            end_angle = np.degrees(np.arctan2(y1 - center[1], x1 - center[0]))

            # Добавляем дугу в список сегментов
            segments.append((center, radius, angle, start_angle, end_angle))

    # Рисуем фигуру
    for segment in segments:
        if len(segment) == 2:  # Линейное соединение
            plt.plot([segment[0][0], segment[1][0]], [segment[0][1], segment[1][1]], color='white')
        else:  # Дуга
            arc = patches.Arc(segment[0], segment[1] * 2, segment[1] * 2, angle=np.degrees(segment[2]), theta1=segment[3], theta2=segment[4], color='white')
            ax.add_patch(arc)

    plt.savefig(filename, bbox_inches='tight', pad_inches=0, dpi=300, facecolor='black')
    plt.close()

# Пример использования:
coords = [(1, 1), (2, 3), (3, 1)]  # Замените на ваши координаты
scale = 5  # Замените на ваш масштаб
conn_types = ['line', 'convex', 'concave']  # Замените на типы соединений
draw_figure(coords, scale, conn_types)

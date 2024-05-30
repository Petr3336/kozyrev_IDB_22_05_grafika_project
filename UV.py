import numpy as np
import matplotlib.pyplot as plt
import ast

def read_rectangles_from_file(filename):
    """
    Чтение прямоугольников из файла.
    Формат файла: каждая строка содержит пару кортежей с координатами.
    Пример:
    [(20, 1971), (479, 2028)]
    [(499, 1971), (957, 2028)]
    """
    rectangles = []
    with open(filename, 'r') as file:
        for line in file:
            rectangle = ast.literal_eval(line.strip())
            rectangles.append(rectangle)
    return rectangles

def draw_rectangles(image_size, rectangles):
    # Создаем изображение с черным фоном
    image = np.zeros((image_size, image_size))

    # Зарисовываем прямоугольники белым цветом
    for rect in rectangles:
        top_left = rect[0]
        bottom_right = rect[1]
        image[top_left[1]:bottom_right[1], top_left[0]:bottom_right[0]] = 1

    return image

def main():
    image_size = 2048
    rectangles = read_rectangles_from_file('coordinates.txt')  # Замените на имя вашего файла

    image = draw_rectangles(image_size, rectangles)

    # Отображаем изображение
    plt.imshow(image, cmap='gray')
    plt.axis('off')  # Скрываем оси
    plt.show()

    # Сохраняем изображение в файл
    plt.imsave('output_image.png', image, cmap='gray')

if __name__ == '__main__':
    main()

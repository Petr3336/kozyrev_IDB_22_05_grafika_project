import tkinter as tk
from tkinter import messagebox

class Rectangle:
    def __init__(self, canvas, x=0, y=0, width=100, height=100):
        self.canvas = canvas
        self.rect = self.canvas.create_rectangle(x, y, x + width, y + height, outline='white', fill='white')
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.aspect_ratio = width / height

    def update(self, x, y, width, height):
        self.canvas.coords(self.rect, x, y, x + width, y + height)
        self.x = x
        self.y = y
        self.width = width
        self.height = height

class UVUnwrapper:
    def __init__(self, root, canvas_width, canvas_height):
        self.root = root
        self.canvas_width = canvas_width
        self.canvas_height = canvas_height
        self.rectangles = []
        self.selected_rectangle = None
        self.start_x = None
        self.start_y = None
        self.resizing = False

        self.root.bind('<Control-1>', self.start_resize)

        self.canvas = tk.Canvas(root, width=canvas_width, height=canvas_height, bg="black")
        self.canvas.pack(side=tk.LEFT)

        self.canvas.bind("<Button-1>", self.select_rectangle)
        self.canvas.bind("<B1-Motion>", self.move_or_resize_rectangle)
        self.canvas.bind("<ButtonRelease-1>", self.deselect_rectangle)

        self.menu_frame = tk.Frame(root, width=200, bg="white")
        self.menu_frame.pack(side=tk.RIGHT, fill=tk.Y)

        self.entry_x = tk.Entry(self.menu_frame)
        self.entry_y = tk.Entry(self.menu_frame)
        self.entry_width = tk.Entry(self.menu_frame)
        self.entry_height = tk.Entry(self.menu_frame)

        self.entry_x.pack()
        self.entry_y.pack()
        self.entry_width.pack()
        self.entry_height.pack()

        tk.Label(self.menu_frame, text="X:").pack()
        tk.Label(self.menu_frame, text="Y:").pack()
        tk.Label(self.menu_frame, text="Width:").pack()
        tk.Label(self.menu_frame, text="Height:").pack()

        self.create_button = tk.Button(self.menu_frame, text="Create Rectangle", command=self.create_new_rectangle)
        self.create_button.pack()

        self.rectangles_listbox = tk.Listbox(self.menu_frame)
        self.rectangles_listbox.pack()

        self.delete_button = tk.Button(self.menu_frame, text="Delete Rectangle", command=self.delete_selected_rectangle)
        self.delete_button.pack()

        self.show_coordinates_button = tk.Button(self.menu_frame, text="Show Coordinates", command=self.show_coordinates)
        self.show_coordinates_button.pack()

    def select_rectangle(self, event):
        self.start_x = event.x
        self.start_y = event.y
        for rect in self.rectangles:
            x1, y1, x2, y2 = self.canvas.coords(rect.rect)
            if x1 < event.x < x2 and y1 < event.y < y2:
                self.selected_rectangle = rect
                break

    def move_or_resize_rectangle(self, event):
        if self.selected_rectangle:
            dx = event.x - self.start_x
            dy = event.y - self.start_y
            if self.resizing:  # Check if Ctrl key is held down
                # Resize with aspect ratio
                delta = dx+dy
                new_width = self.selected_rectangle.width + delta
                new_height = new_width / self.selected_rectangle.aspect_ratio
                self.selected_rectangle.update(self.selected_rectangle.x, self.selected_rectangle.y, new_width, new_height)
            else:
                # Move rectangle
                new_x = self.selected_rectangle.x + dx
                new_y = self.selected_rectangle.y + dy
                self.selected_rectangle.update(new_x, new_y, self.selected_rectangle.width, self.selected_rectangle.height)
            self.start_x = event.x
            self.start_y = event.y

    def start_resize(self, event):
        self.resizing = True
        
    def deselect_rectangle(self, event):
        self.selected_rectangle = None
        self.resizing = False

    def create_new_rectangle(self):
        x = int(self.entry_x.get() or 0)
        y = int(self.entry_y.get() or 0)
        width = int(self.entry_width.get() or 100)
        height = int(self.entry_height.get() or 100)
        rect = Rectangle(self.canvas, x, y, width, height)
        self.rectangles.append(rect)
        self.update_rectangles_listbox()

    def update_rectangles_listbox(self):
        self.rectangles_listbox.delete(0, tk.END)
        for rect in self.rectangles:
            x1, y1, x2, y2 = self.canvas.coords(rect.rect)
            self.rectangles_listbox.insert(tk.END, f"({x1}, {y1}) - ({x2}, {y2})")

    def delete_selected_rectangle(self):
        selected_index = self.rectangles_listbox.curselection()
        if selected_index:
            selected_index = selected_index[0]
            rect = self.rectangles.pop(selected_index)
            self.canvas.delete(rect.rect)
            self.update_rectangles_listbox()

    def show_coordinates(self):
        selected_index = self.rectangles_listbox.curselection()
        if selected_index:
            selected_index = selected_index[0]
            rect = self.rectangles[selected_index]
            x1, y1, x2, y2 = self.canvas.coords(rect.rect)
            absolute_coords = f"Абсолютные: ({x1}, {y1}) - ({x2}, {y2})"
            relative_coords = f"Относительные: ({x1/self.canvas_width}, {y1/self.canvas_height}) - ({x2/self.canvas_width}, {y2/self.canvas_height})"
            messagebox.showinfo("Координаты прямоугольника", f"{absolute_coords}\n{relative_coords}")

if __name__ == "__main__":
    root = tk.Tk()
    root.title("UV Unwrapper")
    uv_unwrapper = UVUnwrapper(root, canvas_width=512, canvas_height=512)
    root.mainloop()

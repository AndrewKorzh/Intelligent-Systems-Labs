import tkinter as tk

# Размеры сетки и отступы
grid_size = 15
cell_size = 40
offset = 20

# Массив значений
grid = [
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
]

root = tk.Tk()
root.title("Круги на пересечениях")

canvas = tk.Canvas(
    root,
    width=grid_size * cell_size + 2 * offset,
    height=grid_size * cell_size + 2 * offset,
)
canvas.pack()


def draw_grid():
    for i in range(grid_size):
        canvas.create_line(
            offset + i * cell_size,
            offset,
            offset + i * cell_size,
            offset + (grid_size - 1) * cell_size,
        )
        canvas.create_line(
            offset,
            offset + i * cell_size,
            offset + (grid_size - 1) * cell_size,
            offset + i * cell_size,
        )

    for i in range(grid_size):
        for j in range(grid_size):
            x = offset + j * cell_size
            y = offset + i * cell_size
            if grid[i][j] == 1:
                canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="white")
            elif grid[i][j] == 2:
                canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="black")


def on_click_delete(event):
    x = (event.x - offset // 2) // cell_size
    y = (event.y - offset // 2) // cell_size

    if 0 <= x < grid_size and 0 <= y < grid_size:
        grid[y][x] = 0
        canvas.delete("all")
        draw_grid()


player = 0


def on_click_add(event, player=1):
    x = (event.x - offset // 2) // cell_size
    y = (event.y - offset // 2) // cell_size

    if 0 <= x < grid_size and 0 <= y < grid_size:
        grid[y][x] = 1
        canvas.delete("all")
        draw_grid()


canvas.bind("<Button-1>", on_click_add)
canvas.bind("<Button-3>", on_click_delete)

draw_grid()
root.mainloop()

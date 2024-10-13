# Добавить штуку для блокировки интерфейса
# Возможные варианты - бот-бот (добавление адаптивной задержки), человек-бот, человек-человек
# Проверка на изменение (хотя нет - бред, от бота ждём результата от человека нажатия)
# Можно удаление точки вообще заблокировать
# Лучше обновлять всё окно срау?
import tkinter as tk
import subprocess
import threading
import time


class CustomStack:
    def __init__(self):
        self.stack = []
        self.element_set = set()

    def push(self, item):
        self.stack.append(item)
        self.element_set.add(item)

    def pop(self):
        if self.stack and len(self.stack) > 0:
            item = self.stack.pop()
            self.element_set.remove(item)
            return item
        return None

    def remove(self, item):
        if item in self.element_set:
            self.stack.remove(item)
            self.element_set.remove(item)
            return True
        return False

    def contains(self, item):
        return item in self.element_set

    def __str__(self):
        return str(self.stack)


class BotManager:

    def __init__(self, grid_size=15, cell_size=40, mode="p-p"):
        self.root = None
        self.grid_size = grid_size
        self.cell_size = cell_size
        self.offset = cell_size // 2
        self.grid = [[0 for _ in range(self.grid_size)] for _ in range(self.grid_size)]
        self.player_label = None

        self.canvas = None
        self.mode = mode
        self.player = 1
        self.bot1_path = "proj.exe"
        self.wait_for_bot = 5

        self.steps = CustomStack()
        return

    def add_root(self, root: tk.Tk):
        self.root = root
        self.canvas = tk.Canvas(
            self.root,
            width=self.grid_size * self.cell_size + 2 * self.offset,
            height=self.grid_size * self.cell_size + 2 * self.offset,
        )
        self.canvas.pack()
        self.player_label = tk.Label(self.root, text=f"PLAYER {self.player} TURN")
        self.player_label.pack(padx=10, pady=10)

        back_button = tk.Button(self.root, text="Back", command=self.on_back)
        back_button.pack(side=tk.LEFT, padx=10, pady=10)

    def show_field(self):
        if self.canvas and self.root:
            self.draw_grid()
            self.canvas.bind("<Button-1>", self.on_click_add)
            # self.canvas.bind("<Button-3>", self.on_click_delete)
            self.canvas.bind("<Button-3>", self.handle_bot_p_c)

            self.root.mainloop()
        return

    def draw_grid(self):
        for i in range(self.grid_size):
            self.canvas.create_line(
                self.offset + i * self.cell_size,
                self.offset,
                self.offset + i * self.cell_size,
                self.offset + (self.grid_size - 1) * self.cell_size,
            )
            self.canvas.create_line(
                self.offset,
                self.offset + i * self.cell_size,
                self.offset + (self.grid_size - 1) * self.cell_size,
                self.offset + i * self.cell_size,
            )

        for i in range(self.grid_size):
            for j in range(self.grid_size):
                x = self.offset + j * self.cell_size
                y = self.offset + i * self.cell_size
                if self.grid[i][j] == 1:
                    self.canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="white")
                elif self.grid[i][j] == 2:
                    self.canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="black")

    def on_back(self):
        (x, y) = self.steps.pop()
        self.player = 1 + self.player % 2
        self.player_label.config(text=f"PLAYER {self.player} TURN")
        self.grid[y][x] = 0
        self.canvas.delete("all")
        self.draw_grid()
        print(self.steps)

    def on_click_delete(self, event):
        x = (event.x - self.offset // 2) // self.cell_size
        y = (event.y - self.offset // 2) // self.cell_size

        if 0 <= x < self.grid_size and 0 <= y < self.grid_size:
            self.grid[y][x] = 0
            self.steps.remove((x, y))
            self.canvas.delete("all")
            self.draw_grid()

    def on_click_add(self, event):
        x = (event.x - self.offset // 2) // self.cell_size
        y = (event.y - self.offset // 2) // self.cell_size

        if 0 <= x < self.grid_size and 0 <= y < self.grid_size:
            if self.grid[y][x] == 0:
                self.grid[y][x] = self.player
                self.steps.push((x, y))
                if self.mode == "p-p":
                    self.player = 1 + self.player % 2
                self.canvas.delete("all")
                self.draw_grid()
                self.player_label.config(text=f"PLAYER {self.player} TURN")

    def communicate_with_exe(self, process, step: str):
        try:
            start_time = time.time()

            process.stdin.write(step.encode())
            print(f"Sent: {step}")
            process.stdin.flush()

            response = process.stdout.readline().decode().strip()
            elapsed_time = time.time() - start_time
            if elapsed_time > self.wait_for_bot:
                print(f"Timeout! Process took too long: {elapsed_time} seconds.")
                process.terminate()
            else:
                print(f"Response from exe: {response}")

        except Exception as e:
            print(f"Error during communication: {e}")
        finally:
            if process.poll() is None:
                process.terminate()
                print("Process terminated.")

    def handle_bot_p_c(
        self,
        event,
    ):

        step = "3\n"
        process = subprocess.Popen(
            [self.bot1_path],  # Путь к exe
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        thread = threading.Thread(
            target=self.communicate_with_exe, args=(process, step)
        )
        thread.start()
        thread.join(timeout=self.wait_for_bot)

        if thread.is_alive():
            print("Process exceeded time limit. Killing it now...")
            process.terminate()


if __name__ == "__main__":
    bm = BotManager(mode="p-p")

    root = tk.Tk()
    root.title("Gom")

    bm.add_root(root=root)
    bm.show_field()

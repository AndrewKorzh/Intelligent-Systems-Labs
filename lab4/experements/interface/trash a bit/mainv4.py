import tkinter as tk
import subprocess
import time
import atexit


class BotManager:

    def __init__(
        self,
        root: tk.Tk,
        grid_size=15,
        cell_size=40,
        bot1_path="bot1.exe",
        bot2_path="bot2.exe",
        first_step_player=1,
    ):
        # Добавит кто ходит первый - player 1 или 2, если
        self.root = root
        self.grid_size = grid_size
        self.cell_size = cell_size
        self.offset = cell_size // 2
        self.first_step_player = first_step_player

        self.grid = [[0 for _ in range(self.grid_size)] for _ in range(self.grid_size)]
        self.steps = []
        self.canvas = None
        self.player_label = None

        self.bot1_path = bot1_path
        self.bot2_path = bot2_path
        self.player_turn = 1
        self.wait_for_bot = 5

        self.bot_1_process = None
        self.bot_2_process = None

        self.mode = "p-to-p"

        self.canvas = tk.Canvas(
            self.root,
            width=self.grid_size * self.cell_size + 2 * self.offset,
            height=self.grid_size * self.cell_size + 2 * self.offset,
        )
        self.canvas.pack()
        self.player_label = tk.Label(self.root, text=f"PLAYER {self.player_turn} TURN")
        self.player_label.pack(padx=10, pady=10)

        self.back_button = tk.Button(self.root, text="Back", command=self.on_back)
        self.back_button.pack(side=tk.LEFT, padx=10, pady=10)

        atexit.register(self.terminate_bots)

        return

    def terminate_bots(self):
        if self.bot_1_process:
            self.bot_1_process.terminate()
            self.bot_1_process = None

    def on_back(self):
        if self.steps and len(self.steps) > 0:
            (x, y) = self.steps.pop()
            self.player_turn = 1 + self.player_turn % 2
            self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
            self.grid[y][x] = 0
            self.canvas.delete("all")
            self.draw_grid()

            self.canvas.update()
            print(self.steps)

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
                y = (
                    self.offset + (self.grid_size - 1 - i) * self.cell_size
                )  # Обратный порядок
                if self.grid[i][j] == 1:
                    self.canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="white")
                elif self.grid[i][j] == 2:
                    self.canvas.create_oval(x - 5, y - 5, x + 5, y + 5, fill="black")

    def on_click_add(self, event):
        self.canvas.unbind("<Button-1>")
        x = (event.x - self.offset // 2) // self.cell_size
        y = (event.y - self.offset // 2) // self.cell_size

        if 0 <= x < self.grid_size and 0 <= y < self.grid_size:
            if self.grid[self.grid_size - 1 - y][x] == 0:  # Обратный порядок
                self.grid[self.grid_size - 1 - y][x] = self.player_turn
                self.steps.append(
                    (x, self.grid_size - 1 - y)
                )  # Храним как (столбец, строка)
                self.player_turn = 1 + self.player_turn % 2
                self.canvas.delete("all")
                self.draw_grid()
                self.canvas.update()
                cw = self.check_winner()
                if cw != 0:
                    print(f"PLAYER {cw} is WINNER!")
                    self.reset()
                    return
                self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
                if self.mode == "p-to-b":
                    self.bot_step_p_to_b(
                        (self.grid_size - 1 - y, x)
                    )  # Обратный порядок
        self.canvas.bind("<Button-1>", self.on_click_add)

    def reset(self):
        self.grid = [[0 for _ in range(self.grid_size)] for _ in range(self.grid_size)]
        self.steps.clear()
        self.canvas.delete("all")
        self.draw_grid()
        self.canvas.bind("<Button-1>", self.on_click_add)
        self.player_turn = 1
        self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
        if self.bot_1_process:
            self.bot_1_process.terminate()
            self.bot_1_process = subprocess.Popen(
                [self.bot1_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )

        if self.bot_2_process:
            self.bot_2_process.terminate()
            self.bot_2_process = None
        return

    def check_winner(self):
        rows = len(self.grid)
        cols = len(self.grid[0])
        target_length = 5

        def check_direction(x, y, dx, dy, player):
            count = 0
            for i in range(target_length):
                new_x = x + dx * i
                new_y = y + dy * i
                if (
                    0 <= new_x < rows
                    and 0 <= new_y < cols
                    and self.grid[new_x][new_y] == player
                ):
                    count += 1
                else:
                    break
            return count == target_length

        for x in range(rows):
            for y in range(cols):
                player = self.grid[x][y]
                if player == 0:
                    continue

                if (
                    check_direction(x, y, 0, 1, player)
                    or check_direction(x, y, 1, 0, player)
                    or check_direction(x, y, 1, 1, player)
                    or check_direction(x, y, 1, -1, player)
                ):
                    return player

        return 0

    def encode_values(self, number1, number2):
        letter = chr(ord("a") + number1)
        result = f"{letter}{number2+1}"
        return result

    def decode_values(self, encoded_string):
        encoded_string = encoded_string.strip()

        letter = encoded_string[0]

        y = int(encoded_string[1:]) - 1
        x = ord(letter) - ord("a")

        return y, x

    def bot_step_p_to_b(self, step):
        (y, x) = step
        step_str = f"{self.encode_values(x, y)}\n"
        print(step_str)
        self.bot_1_process.stdin.write(step_str.encode())
        self.bot_1_process.stdin.flush()
        response = self.bot_1_process.stdout.readline().decode().strip()
        print(f"from bot {response}")
        (yn, xn) = self.decode_values(response)
        self.grid[yn][xn] = self.player_turn
        self.steps.append((xn, yn))
        cw = self.check_winner()
        if cw != 0:
            print(f"PLAYER {cw} is a WINNER!")
            self.reset()
            return
        self.player_turn = 1 + self.player_turn % 2
        self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
        self.canvas.delete("all")
        self.draw_grid()

        return

    def start_p_to_p(self):
        self.draw_grid()
        self.canvas.bind("<Button-1>", self.on_click_add)
        self.root.mainloop()
        return

    def start_p_to_b(self):
        self.draw_grid()
        self.canvas.bind("<Button-1>", self.on_click_add)

        self.mode = "p-to-b"

        self.bot_1_process = subprocess.Popen(
            [self.bot1_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        self.root.mainloop()
        return


# Можно сделать разные функции в словаре, типа + endpoint
# Конечно можно в целом и разные классы для разных классов с наследованием


if __name__ == "__main__":
    root = tk.Tk()
    root.title("Gom gom")
    bm = BotManager(
        root=root,
        bot1_path="random_step.exe",
    )
    # bm.start_p_to_p()
    bm.start_p_to_b()

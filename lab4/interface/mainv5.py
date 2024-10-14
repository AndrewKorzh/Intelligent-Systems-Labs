import abc
import tkinter as tk
import subprocess
import time
import atexit
import math
import threading


class Gomocu(abc.ABC):
    def __init__(
        self,
        root: tk.Tk,
        grid_size=15,
        cell_size=40,
        bot1_path=None,
        bot2_path=None,
        first_step=1,  # 1,  # 0 - bot 1 - human
    ):
        self.root = root
        self.root.title("Gomocu")
        self.grid_size = grid_size
        self.cell_size = cell_size
        self.offset = cell_size // 2
        self.first_step = int(first_step)

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

        self.canvas = tk.Canvas(
            self.root,
            width=self.grid_size * self.cell_size + 2 * self.offset,
            height=self.grid_size * self.cell_size + 2 * self.offset,
        )
        self.canvas.pack()
        self.player_label = tk.Label(self.root, text=f"PLAYER {self.player_turn} TURN")
        self.player_label.pack(padx=10, pady=10)

        self.back_button = None

        atexit.register(self.terminate_bots)
        return

    def terminate_bots(self):
        if self.bot_1_process:
            self.bot_1_process.terminate()
            self.bot_1_process = None

        if self.bot_2_process:
            self.bot_2_process.terminate()
            self.bot_2_process = None

    def check_winner(self):
        if len(self.steps) >= 100:
            return -1
        rows = len(self.grid)
        cols = len(self.grid[0])
        target_length = 10

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

    def reset(self):
        self.player_turn = 1
        self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
        if self.bot_1_process:
            self.bot_1_process.terminate()
            self.bot_1_process = None

        if self.bot_2_process:
            self.bot_2_process.terminate()
            self.bot_2_process = None

        self.grid = [[0 for _ in range(self.grid_size)] for _ in range(self.grid_size)]
        self.steps = []
        self.start_game()

        return

    @abc.abstractmethod
    def start_game(self):
        pass


class GomocuPToP(Gomocu):
    def start_game(self):
        if not self.back_button:
            self.back_button = tk.Button(self.root, text="Back", command=self.on_back)
            self.back_button.pack(side=tk.LEFT, padx=10, pady=10)

        self.canvas.delete("all")
        self.draw_grid()
        self.canvas.bind("<Button-1>", self.on_click_add)
        self.root.mainloop()

    def on_back(self):
        if self.steps and len(self.steps) > 0:
            (x, y) = self.steps.pop()
            self.player_turn = 1 + self.player_turn % 2
            self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
            self.grid[y][x] = 0
            self.canvas.delete("all")
            self.draw_grid()
            self.canvas.update()

    def on_click_add(self, event):
        x = (event.x - self.offset // 2) // self.cell_size
        y = (event.y - self.offset // 2) // self.cell_size

        if 0 <= x < self.grid_size and 0 <= y < self.grid_size:
            if self.grid[self.grid_size - 1 - y][x] == 0:
                self.grid[self.grid_size - 1 - y][x] = self.player_turn
                self.steps.append((x, self.grid_size - 1 - y))
                self.player_turn = 1 + self.player_turn % 2
                self.canvas.delete("all")
                self.draw_grid()
                self.canvas.update()
                cw = self.check_winner()
                self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
                if cw != 0:
                    print(f"PLAYER {cw} is WINNER!")
                    self.reset()
                    return


class GomocuPToB(Gomocu):
    def start_game(self):
        if not self.bot1_path:
            print("Не указан путь к боту")
            return
        if not self.back_button:
            self.back_button = tk.Button(self.root, text="Back", command=self.on_back)
            self.back_button.pack(side=tk.LEFT, padx=10, pady=10)

        param = f"{self.first_step}"

        self.bot_1_process = subprocess.Popen(
            [self.bot1_path, param],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        if self.first_step == 1:

            self.canvas.delete("all")
            self.draw_grid()
            self.canvas.bind("<Button-1>", self.p_click)
            self.root.mainloop()

        elif self.first_step == 0:

            response = self.bot_1_process.stdout.readline().decode().strip()

            (yn, xn) = self.decode_values(response)
            self.grid[yn][xn] = self.player_turn
            self.steps.append((xn, yn))

            self.player_turn = 1 + self.player_turn % 2
            self.player_label.config(text=f"PLAYER {self.player_turn} TURN")

            self.canvas.delete("all")
            self.draw_grid()
            self.canvas.bind("<Button-1>", self.p_click)
            self.root.mainloop()
        else:
            print("unknown player")
            return

        self.canvas.delete("all")
        self.draw_grid()

    def p_click(self, event):
        self.canvas.unbind("<Button-1>")
        x = (event.x - self.offset // 2) // self.cell_size
        y = (event.y - self.offset // 2) // self.cell_size

        if 0 <= x < self.grid_size and 0 <= y < self.grid_size:
            if self.grid[self.grid_size - 1 - y][x] == 0:
                self.grid[self.grid_size - 1 - y][x] = self.player_turn
                self.steps.append((x, self.grid_size - 1 - y))
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

                self.bot_step((self.grid_size - 1 - y, x))
        self.canvas.bind("<Button-1>", self.p_click)

    def on_back(self):
        if self.steps and len(self.steps) > 0:
            (x, y) = self.steps.pop()
            self.grid[y][x] = 0

            u1 = "u1\n"
            self.bot_1_process.stdin.write(u1.encode())
            self.bot_1_process.stdin.flush()
            response = self.bot_1_process.stdout.readline().decode().strip()
            print(f"from bot {response}")

            self.player_turn = 1 + self.player_turn % 2
            self.player_label.config(text=f"PLAYER {self.player_turn} TURN")

            self.canvas.delete("all")
            self.draw_grid()
            self.canvas.update()

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

    def bot_step(self, step):
        (y, x) = step
        step_str = f"{self.encode_values(x, y)}\n"
        print(step_str)
        self.bot_1_process.stdin.write(step_str.encode())
        self.bot_1_process.stdin.flush()
        response = self.bot_1_process.stdout.readline().decode().strip()
        # print(f"from bot {response}")
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


class GomocuBtoB(Gomocu):
    def __init__(
        self,
        root: tk.Tk,
        grid_size=15,
        cell_size=40,
        bot1_path=None,
        bot2_path=None,
        first_step=1,
        wait_time=100,
    ):
        super().__init__(root, grid_size, cell_size, bot1_path, bot2_path, first_step)
        self.round = 0
        self.wait_time = wait_time

    def start_game(self):

        if not self.bot1_path or not self.bot2_path:
            print("Не указан путь к боту")
            return

        first_step = self.round % 2
        second_step = abs(first_step - 1)

        bot_1_process = subprocess.Popen(
            [self.bot1_path, f"{first_step}"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        bot_2_process = subprocess.Popen(
            [self.bot1_path, f"{second_step}"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        bots = {
            first_step: bot_1_process,
            second_step: bot_2_process,
        }

        step = 0

        response = bots[step].stdout.readline().decode().strip()

        (yn, xn) = self.decode_values(response)
        self.grid[yn][xn] = self.player_turn
        self.steps.append(response)

        while True:
            self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
            self.player_turn = 1 + self.player_turn % 2
            step += 1
            bots[step % 2].stdin.write(f"{response}\n".encode())
            bots[step % 2].stdin.flush()
            response = bots[step % 2].stdout.readline().decode().strip()
            self.steps.append(response)
            (yn, xn) = self.decode_values(response)
            self.grid[yn][xn] = self.player_turn
            cw = self.check_winner()

            self.canvas.delete("all")
            self.draw_grid()
            self.canvas.update()

            if cw != 0:
                print(f"PLAYER {cw} is a WINNER!")
                self.reset()
                return  # тут должен быть игрок либо ничья
            if cw == -1:
                print("Ничья")
                self.reset()
                return  # ничья

        return

    def decode_values(self, encoded_string):
        encoded_string = encoded_string.strip()

        letter = encoded_string[0]

        y = int(encoded_string[1:]) - 1
        x = ord(letter) - ord("a")

        return y, x


if __name__ == "__main__":
    root = tk.Tk()

    game = GomocuBtoB(
        root=root,
        first_step="1",
        bot1_path="random_step.exe",
        bot2_path="random_step2.exe",
    )
    game.start_game()

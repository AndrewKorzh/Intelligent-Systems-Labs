import abc
import tkinter as tk
import subprocess
import atexit
import signal
import threading
import logging
from typing import Optional, List, Tuple, Dict
from contextlib import contextmanager

# Configure logging
logging.basicConfig(
    level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s"
)


@contextmanager
def managed_subprocess(command: List[str]):
    """
    Context manager to handle subprocess lifecycle.
    """
    process = subprocess.Popen(
        command,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    try:
        yield process
    finally:
        process.terminate()
        process.wait()


class Gomocu(abc.ABC):
    def __init__(
        self,
        root: tk.Tk,
        cell_size: int = 40,
        bot1_path: Optional[str] = None,
        bot2_path: Optional[str] = None,
        first_step: int = 1,
        grid_size: int = 15,
        win_length: int = 5,
    ):
        self.root = root
        self.root.title("Gomocu")
        self.grid_size = grid_size
        self.cell_size = cell_size
        self.offset = cell_size // 2
        self.first_step = int(first_step)

        self.grid = [[0 for _ in range(self.grid_size)] for _ in range(self.grid_size)]
        self.steps: List[str] = []
        self.canvas = None
        self.player_label = None

        self.bot1_path = bot1_path
        self.bot2_path = bot2_path
        self.player_turn = self.first_step
        self.wait_for_bot = 5

        self.bot_1_process: Optional[subprocess.Popen] = None
        self.bot_2_process: Optional[subprocess.Popen] = None

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
        self.setup_signal_handlers()

    def setup_signal_handlers(self):
        signal.signal(signal.SIGINT, self.signal_handler)
        signal.signal(signal.SIGTERM, self.signal_handler)

    def signal_handler(self, signum, frame):
        logging.info(f"Received signal {signum}. Terminating bots...")
        self.terminate_bots()
        self.root.destroy()
        exit(0)

    def terminate_bots(self):
        if self.bot_1_process and self.bot_1_process.poll() is None:
            logging.info("Terminating Bot 1 process.")
            self.bot_1_process.terminate()
            self.bot_1_process = None

        if self.bot_2_process and self.bot_2_process.poll() is None:
            logging.info("Terminating Bot 2 process.")
            self.bot_2_process.terminate()
            self.bot_2_process = None

    def check_winner(self) -> int:
        if len(self.steps) >= self.grid_size**2:
            return -1  # Draw

        rows = len(self.grid)
        cols = len(self.grid[0])
        target_length = 5  # This could be self.win_length

        def check_direction(x: int, y: int, dx: int, dy: int, player: int) -> bool:
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

        return 0  # No winner yet

    def draw_grid(self):
        self.canvas.delete("grid")
        for i in range(self.grid_size):
            self.canvas.create_line(
                self.offset + i * self.cell_size,
                self.offset,
                self.offset + i * self.cell_size,
                self.offset + (self.grid_size - 1) * self.cell_size,
                tags="grid",
            )
            self.canvas.create_line(
                self.offset,
                self.offset + i * self.cell_size,
                self.offset + (self.grid_size - 1) * self.cell_size,
                self.offset + i * self.cell_size,
                tags="grid",
            )

        for i in range(self.grid_size):
            for j in range(self.grid_size):
                x = self.offset + j * self.cell_size
                y = self.offset + (self.grid_size - 1 - i) * self.cell_size
                if self.grid[i][j] == 1:
                    self.canvas.create_oval(
                        x - 5, y - 5, x + 5, y + 5, fill="white", tags="stones"
                    )
                elif self.grid[i][j] == 2:
                    self.canvas.create_oval(
                        x - 5, y - 5, x + 5, y + 5, fill="black", tags="stones"
                    )

    def update_gui(self):
        """
        Updates the GUI by redrawing the grid and refreshing the canvas.
        """
        self.draw_grid()
        self.canvas.update()

    def reset(self):
        logging.info("Resetting the game.")
        self.player_turn = self.first_step
        self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
        self.terminate_bots()

        self.grid = [[0 for _ in range(self.grid_size)] for _ in range(self.grid_size)]
        self.steps.clear()
        self.start_game()

    @abc.abstractmethod
    def start_game(self):
        pass

    def decode_values(self, encoded_string: str) -> Tuple[int, int]:
        """
        Decodes the bot's response into grid coordinates.

        Args:
            encoded_string (str): The encoded move string.

        Returns:
            Tuple[int, int]: The (y, x) coordinates on the grid.

        Raises:
            ValueError: If the encoded string is invalid.
        """
        try:
            encoded_string = encoded_string.strip().lower()
            letter = encoded_string[0]
            x = ord(letter) - ord("a")
            y = int(encoded_string[1:]) - 1

            if not (0 <= x < self.grid_size and 0 <= y < self.grid_size):
                raise ValueError("Decoded coordinates are out of bounds.")

            return y, x
        except (IndexError, ValueError) as e:
            raise ValueError(f"Invalid encoded string '{encoded_string}': {e}")


class GomocuBotB(Gomocu):
    def __init__(
        self,
        root: tk.Tk,
        cell_size: int = 40,
        bot1_path: Optional[str] = None,
        bot2_path: Optional[str] = None,
        first_step: int = 1,
        wait_time: int = 100,
        round_amount: int = 6,
        grid_size: int = 15,
        win_length: int = 5,
    ):
        super().__init__(
            root, cell_size, bot1_path, bot2_path, first_step, grid_size, win_length
        )
        self.round = 0
        self.round_amount = round_amount
        self.wait_time = wait_time
        self.score: Dict[str, int] = {bot1_path: 0, bot2_path: 0}
        self.process_lock = threading.Lock()

    def start_game(self):
        if not self.bot1_path or not self.bot2_path:
            logging.error("Bot paths are not specified.")
            return

        if self.round >= self.round_amount:
            logging.info("Final Scores:")
            for bot, score in self.score.items():
                logging.info(f"{bot}: {score}")
            return

        if not self.process_lock.acquire(blocking=False):
            logging.warning("A game is already in progress.")
            return

        try:
            with managed_subprocess([self.bot1_path, "0"]) as bot1, managed_subprocess(
                [self.bot2_path, "1"]
            ) as bot2:

                self.bot_1_process = bot1
                self.bot_2_process = bot2

                bots = [bot1, bot2]
                bot_paths = [self.bot1_path, self.bot2_path]
                step = 0

                # Initial move
                response = bots[step].stdout.readline().strip()
                y, x = self.decode_values(response)
                self.grid[y][x] = self.player_turn
                self.steps.append(response)
                self.update_gui()

                while True:
                    self.player_label.config(text=f"PLAYER {self.player_turn} TURN")
                    self.player_turn = 1 + self.player_turn % 2
                    step += 1
                    current_bot = bots[step % 2]
                    current_bot.stdin.write(f"{response}\n")
                    current_bot.stdin.flush()
                    response = current_bot.stdout.readline().strip()

                    if not response:
                        logging.error(f"{bot_paths[step % 2]} failed to respond.")
                        break

                    self.steps.append(response)
                    y, x = self.decode_values(response)
                    self.grid[y][x] = self.player_turn

                    self.update_gui()
                    winner = self.check_winner()

                    if winner == -1:
                        self.round += 1
                        logging.info("Draw")
                        self.score["Draws"] = self.score.get("Draws", 0) + 1
                        self.reset()
                        break
                    elif winner != 0:
                        self.round += 1
                        logging.info(f"{bot_paths[winner - 1]} wins round {self.round}")
                        self.score[bot_paths[winner - 1]] += 1
                        self.reset()
                        break

        except Exception as e:
            logging.error(f"An error occurred during the game: {e}")
            self.reset()
        finally:
            self.process_lock.release()


if __name__ == "__main__":

    root = tk.Tk()

    game = GomocuBotB(
        root=root,
        first_step="1",
        cell_size=20,
        bot1_path="random_step.exe",
        bot2_path="random_step2.exe",
    )
    game.start_game()

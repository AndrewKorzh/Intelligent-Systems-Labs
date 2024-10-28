import subprocess
import threading
import time


def communicate_with_exe(process):

    s = "aaaaa"
    message = f"{s}\n"
    process.stdin.write(message.encode())
    print(f"Sent: {message}")
    process.stdin.flush()
    response = process.stdout.readline().decode().strip()

    print(f"Response from exe: {response}")
    process.kill()


def run():
    # Запускаем exe файл
    process = subprocess.Popen(
        ["proj.exe"],  # Путь к exe
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    # Запускаем поток для общения с exe
    thread = threading.Thread(target=communicate_with_exe, args=(process,))
    thread.start()

    # Ожидание завершения процесса
    process.wait()


if __name__ == "__main__":
    run()

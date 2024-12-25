from telegram import Update
from telegram.ext import (
    ApplicationBuilder,
    CommandHandler,
    MessageHandler,
    filters,
    ContextTypes,
)
from telegram import Update
from PIL import Image
import os

import numpy as np
from network import NeuralNetwork
import aiml

kernel = aiml.Kernel()

# Загружаем AIML файлы
kernel.learn("bot.aiml")

nn = NeuralNetwork("network.json")


def compute_pixel_vector(image_path: str) -> list:
    with Image.open(image_path) as img:
        img_bw = img.convert("L")
        pixel_array = np.array(img_bw)
        vertical_sums = np.sum(pixel_array == 0, axis=1)[::-1]
        horizontal_sums = np.sum(pixel_array == 0, axis=0)[::-1]
        result_vector = list(vertical_sums) + list(horizontal_sums)

    return result_vector


async def start(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text("Привет!")


async def handle_text(update: Update, context: ContextTypes.DEFAULT_TYPE):
    user_message = update.message.text
    response = kernel.respond(user_message)
    await update.message.reply_text(response)


def process_image(input_path: str, output_path: str, size: tuple = (200, 200)):
    with Image.open(input_path) as img:
        width, height = img.size
        min_side = min(width, height)
        left = (width - min_side) // 2
        top = (height - min_side) // 2
        right = (width + min_side) // 2
        bottom = (height + min_side) // 2
        img_cropped = img.crop((left, top, right, bottom))
        img_bw = img_cropped.convert("L").point(
            lambda x: 255 if x > 200 else 0, mode="1"
        )
        img_resized = img_bw.resize(size)
        img_resized.save(output_path)


figures = {
    "0": "Треугольник",
    "1": "Прямоугольник",
    "2": "Круг",
    "3": "Синусоида",
}


def get_figure_name(arr):
    max_index = arr.index(max(arr))
    return figures.get(f"{max_index}", "Unknown")


def get_probs(arr):
    output = []
    for i in range(len(arr)):
        output.append(f"{figures[f'{i}']}: {round(arr[i], 2)}")

    return output


async def handle_photo(update: Update, context: ContextTypes.DEFAULT_TYPE):
    photo = update.message.photo[-1]
    file = await photo.get_file()

    input_path = "temp_photo.jpg"
    output_path = "processed_image.jpg"
    await file.download_to_drive(input_path)
    process_image(input_path, output_path)

    arr = compute_pixel_vector(output_path)

    res = nn.compute(arr)
    print(res)
    await context.bot.send_photo(
        chat_id=update.effective_chat.id, photo=open(output_path, "rb")
    )

    await context.bot.send_message(
        chat_id=update.effective_chat.id,
        text=f"{"\n".join(get_probs(res))};\nЭто {get_figure_name(res)}!",
    )

    os.remove(input_path)
    os.remove(output_path)


def generate_random_array(size=400, low=0, high=200):
    return np.random.randint(low, high + 1, size)


def main():
    app = (
        ApplicationBuilder()
        .token("7955555615:AAHlJZOoDutcPvPOOxlQWl76YtGLvxsDylk")
        .build()
    )

    app.add_handler(CommandHandler("start", start))
    app.add_handler(MessageHandler(filters.PHOTO, handle_photo))
    app.add_handler(MessageHandler(filters.TEXT, handle_text))
    app.run_polling()


if __name__ == "__main__":
    main()

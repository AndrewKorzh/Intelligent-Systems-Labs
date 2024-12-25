import aiml

# Создаем объект ядра AIML
kernel = aiml.Kernel()

# Загружаем AIML файлы
kernel.learn("bot.aiml")  # Загрузить AIML файл с шаблонами


# Функция для общения с ботом
def chat():
    print("Type 'BYE' to exit the chat.")
    while True:
        user_input = input("You: ")
        if user_input.upper() == "BYE":
            print("Bot: Goodbye!")
            break
        response = kernel.respond(user_input)
        print(f"Bot: {response}")


# Запуск чата
if __name__ == "__main__":
    chat()

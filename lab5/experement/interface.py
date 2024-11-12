import tkinter as tk
from tkinter import ttk


# Функция для вывода выбранных элементов в текстовое поле
def display_selected():
    left_selected = [left_items[i] for i, var in enumerate(left_vars) if var.get()]
    right_selected = [right_items[i] for i, var in enumerate(right_vars) if var.get()]

    output_text.delete(1.0, tk.END)  # Очистить текстовое поле
    output_text.insert(tk.END, f"Слева выбрано: {', '.join(left_selected)}\n")
    output_text.insert(tk.END, f"Справа выбрано: {', '.join(right_selected)}\n")


# Создаем основное окно
root = tk.Tk()
root.title("Приложение с двумя списками и выводом")
root.geometry("900x600")

# Данные для выбора
left_items = ["Элемент A" + str(i) for i in range(1, 15)]
right_items = ["Элемент B" + str(i) for i in range(1, 5)]

# Переменные для флажков
left_vars = [tk.BooleanVar() for _ in left_items]
right_vars = [tk.BooleanVar() for _ in right_items]


# Функция для создания прокручиваемого списка с флажками
def create_scrollable_checklist(parent, items, vars):
    canvas = tk.Canvas(parent)
    scrollbar = tk.Scrollbar(parent, orient="vertical", command=canvas.yview)
    scrollable_frame = tk.Frame(canvas)

    scrollable_frame.bind(
        "<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
    )

    canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
    canvas.configure(yscrollcommand=scrollbar.set)

    for i, item in enumerate(items):
        tk.Checkbutton(scrollable_frame, text=item, variable=vars[i]).pack(anchor="w")

    canvas.grid(row=0, column=0, sticky="nsew")
    scrollbar.grid(row=0, column=1, sticky="ns")


# Левый блок с флажками
left_frame = tk.LabelFrame(root, text="Левый список")
left_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
create_scrollable_checklist(left_frame, left_items, left_vars)

# Правый блок с флажками
right_frame = tk.LabelFrame(root, text="Правый список")
right_frame.grid(row=0, column=2, padx=10, pady=10, sticky="nsew")
create_scrollable_checklist(right_frame, right_items, right_vars)

# Прокручиваемое текстовое поле для вывода информации
output_frame = tk.Frame(root)
output_frame.grid(row=1, column=0, columnspan=3, padx=10, pady=10, sticky="nsew")

output_text = tk.Text(output_frame, height=10, wrap="word")
output_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

scrollbar = tk.Scrollbar(output_frame, command=output_text.yview)
scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

output_text.config(yscrollcommand=scrollbar.set)

# Кнопка для отображения выбора
display_button = tk.Button(root, text="Отобразить выбор", command=display_selected)
display_button.grid(row=2, column=0, columnspan=3, pady=10)

# Настройка растягивания для адаптации интерфейса
root.grid_rowconfigure(1, weight=1)
root.grid_columnconfigure(1, weight=1)

root.mainloop()

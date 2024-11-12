import json
from collections import defaultdict


class ProductionSystem:
    def __init__(self, facts, rules):
        self.facts = set(facts)  # множество фактов
        self.rules = rules  # список правил
        self.explanations = defaultdict(list)  # хранит объяснения для каждого факта

    def add_fact(self, fact):
        self.facts.add(fact)

    def forward_inference(self):
        added = True
        while added:
            added = False
            for conditions, result in self.rules:
                # Проверяем, есть ли все условия в известных фактах
                if (
                    all(condition in self.facts for condition in conditions)
                    and result not in self.facts
                ):
                    self.facts.add(result)  # Добавляем новый факт
                    explanation = f"Получено по правилу: {conditions} -> {result}"
                    self.explanations[result].append(explanation)
                    added = True
                    print(
                        f"Прямой вывод: добавлен факт {result} по правилу {conditions} -> {result}"
                    )

    def explain(self, fact):
        if fact in self.explanations:
            for explanation in self.explanations[fact]:
                print(f"Объяснение для {fact}: {explanation}")
        else:
            print(f"Нет объяснения для факта {fact}.")


# Загрузка фактов и правил из JSON по пути к файлу
def load_data_from_json(file_path):
    try:
        with open(file_path, "r", encoding="utf-8") as file:
            data = json.load(file)
            facts = data["facts"]
            rules = [(rule["conditions"], rule["result"]) for rule in data["rules"]]
            return facts, rules
    except FileNotFoundError:
        print(f"Файл {file_path} не найден.")
        return [], []
    except json.JSONDecodeError:
        print(f"Ошибка при чтении JSON из файла {file_path}.")
        return [], []


# Путь к вашему JSON-файлу
json_file_path = "illnes.json"  # Укажите имя вашего файла

# Загрузка фактов и правил
facts, rules = load_data_from_json(json_file_path)

# Инициализация системы
ps = ProductionSystem(facts, rules)

# Пример работы системы
print("\nПрямой вывод:")
ps.forward_inference()

# Объяснение результата для C
print(f"\nОбъяснения для C:")
ps.explain("C")

ps.explain("B")

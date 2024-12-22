import tkinter as tk
import json
import copy
import hashlib
import random


# Todo
# Отображение - из имени в f +
# Получени всех посылок и того, что доказать
# Применение алгоритма - он возвращает массив строк
# Добавить выбор конкретного алгоритма
# Певцы - русское, больше связей
# Сделать чтобы нормальная сортировк была - по встречаемости в df (Другая прога)
# Добавить выбрать всё
# Сделать вывод понятным поэтапно
# добавить очистку


class Fact:
    def __init__(self, id: str, text, number):
        self.id = id
        self.text = text
        self.number = number

    def __str__(self):
        return f"Fact(id={self.id}, text='{self.text}', number={self.number})"


class Rule:
    def __init__(self, id: str, premises: list[str], conclusion, confidence=None):
        self.id = id
        self.premises = premises
        self.conclusion = conclusion
        if not confidence:
            self.confidence = random.uniform(0.98, 1.0)
        else:
            self.confidence = confidence

    def str_rule_and_conf(self):
        return f"{self.id} [{(self.confidence):.2f}]"

    def __str__(self):
        premises_str = ", ".join([str(p) for p in self.premises])
        return f"{premises_str} -- {(self.confidence)} --> {self.conclusion}"


class Rules:
    def __init__(self):
        self.rules = {}

    def add(self, rule: Rule):
        self.rules[rule.id] = rule

    def get_fact_by_rule(self, id: str):
        return self.rules[id].conclusion

    def get_premises_by_rule(self, id: str):
        return self.rules[id].premises

    def get_rule_by_id(self, id: str):
        return self.rules[id]

    def get_rules_by_fact(self, id):
        return

    def str_rule_and_conf_by_id(self, id):
        return self.rules[id].str_rule_and_conf()

    def load(self, file_path):
        with open(file_path, "r", encoding="utf-8") as file:
            data = json.load(file)
            for rule in data["rules"]:
                id = rule["id"]
                premises = rule["premises"]
                conclusion = rule["conclusion"]
                self.add(Rule(id, premises, conclusion))


class Facts:
    def __init__(self):
        self.facts = {}

    def add(self, fact: Fact):
        self.facts[fact.id] = fact

    def get_text_by_id(self, id: str):
        if id in self.facts:
            return self.facts[id].text
        return None

    def load(self, file_path):
        with open(file_path, "r", encoding="utf-8") as file:
            data = json.load(file)
            for fact in data["facts"]:
                id = fact["id"]
                text = fact["text"]
                self.add(Fact(id, text=text, number=0))


class Node:
    def __init__(
        self, facts: set[str], rule: str, parent_index: int, confidence_dict=None
    ):
        self.facts = facts
        self.rule = rule
        self.parent_index = parent_index
        if confidence_dict:
            self.confidence_dict = confidence_dict
        else:
            self.confidence_dict = {}
            for fact in self.facts:
                self.confidence_dict[fact] = random.uniform(0.9, 1.0)  # 1.0

    def __str__(self):
        facts_str = ", ".join(
            [f"{str(f)}[{self.confidence_dict[f]}]" for f in self.facts]
        )
        return f"Node(facts=[{facts_str}], rule={self.rule})"

    def rules_that_gives_new_facts_forward(self, rules: Rules) -> dict:
        rules_for_new_nodes_id = {}
        for r in rules.rules.values():
            # print(r)
            inferable = all(f in self.facts for f in r.premises)
            if inferable and r.conclusion not in self.facts:

                confid = 1
                for fact in r.premises:
                    confid *= self.confidence_dict[fact]
                confid *= r.confidence
                rules_for_new_nodes_id[r.id] = confid

        return rules_for_new_nodes_id


def generate_hash(strings):
    sorted_strings = sorted(strings)
    hasher = hashlib.sha256()
    for string in sorted_strings:
        hasher.update(string.encode("utf-8"))  # Кодируем строку в байты
    return hasher.hexdigest()


class ProductionSystem:
    def __init__(self, path_rules_and_facts) -> None:
        self.facts = Facts()
        self.facts.load(path_rules_and_facts)
        self.rules = Rules()
        self.rules.load(path_rules_and_facts)

    def find_path_to_target_facts_forward(
        self, start_facts: set[str], target_facts: set[str]
    ):
        hashes_set = set()
        if target_facts.issubset(start_facts):
            return [Node(facts=start_facts, rule=None, parent_index=-1)]
        node = Node(facts=start_facts, rule=None, parent_index=-1)
        nodes = [node]
        index = 0
        found = False
        while index < len(nodes) and not found:
            print(f"{index} - {nodes[index]}")
            # Тут доьавить штуку такую - надо получить правило и сразу уверенность - получаем что-то типа списка кортежей

            new_rules_dict = nodes[index].rules_that_gives_new_facts_forward(
                rules=self.rules
            )

            for rule, confidence in new_rules_dict.items():
                facts = copy.deepcopy(nodes[index].facts)

                new_fact = self.rules.get_fact_by_rule(rule)

                if new_fact not in facts:
                    confidence_dict = copy.deepcopy(nodes[index].confidence_dict)
                    confidence_dict[new_fact] = confidence
                    facts.add(new_fact)
                    current_hash = generate_hash(facts)
                    if current_hash not in hashes_set:
                        nodes.append(
                            Node(
                                facts=facts,
                                rule=rule,
                                parent_index=index,
                                confidence_dict=confidence_dict,
                            )
                        )
                        hashes_set.add(current_hash)
                    if target_facts.issubset(facts):
                        found = True
                        break
            index += 1

        if found:
            path = []
            node = nodes[-1]

            while node.parent_index != -1:
                path.append(node)
                node = nodes[node.parent_index]

            path.append(node)
            path.reverse()

            return path
        else:
            return None

    def path_str(self, path: list[Node]):
        output = []
        output.append("\n")
        if path:
            for step in path:
                if step.rule:
                    output.append(f"- {step.rule} -> {step.facts}")
                else:
                    output.append(f"{step.facts}")

            output.append("\n")
            for step in path:
                if step.rule:
                    premises_ids = self.rules.get_premises_by_rule(step.rule)
                    premises = [
                        f"{self.facts.get_text_by_id(id)} [{step.confidence_dict[id]:.2f}]"
                        for id in premises_ids
                    ]
                    conclusion_id = self.rules.get_fact_by_rule(step.rule)
                    conclusion = f"{self.facts.get_text_by_id(conclusion_id)} [{step.confidence_dict[conclusion_id]:.2f}]"

                    output.append(  # self.rules
                        f"{" + ".join(premises)} - ({self.rules.str_rule_and_conf_by_id(step.rule)}) -> {conclusion}\n"
                    )

        else:
            output.append("Невыводимо")

        return " ".join(output)


class ProductionSystemInterface:
    def __init__(
        self, root: tk.Tk, rules_dict: dict, production_system: ProductionSystem
    ) -> None:
        self.root = root
        self.root.geometry("900x800")
        self.root.title("ProductionSystem")

        self.production_system = production_system

        self.rules_dict = rules_dict

        # Тут уже должен быть список аксиом и тд
        self.left_items = None  # []  # "Элемент A" + str(i) for i in range(1, 2)
        self.right_items = None  # []  # "Элемент B" + str(i) for i in range(1, 2)

        self.fact_to_id = None

        self.add_facts_to_fields()
        self.create_fact_to_id()

        self.left_vars = [tk.BooleanVar() for _ in self.left_items]
        self.right_vars = [tk.BooleanVar() for _ in self.right_items]

        self.left_frame = tk.LabelFrame(root, text="Выбор аксиом")
        self.left_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
        self.create_scrollable_checklist(
            self.left_frame, self.left_items, self.left_vars
        )

        right_frame = tk.LabelFrame(root, text="Доказать")
        right_frame.grid(row=0, column=2, padx=10, pady=10, sticky="nsew")
        self.create_scrollable_checklist(right_frame, self.right_items, self.right_vars)

        self.select_all_button = tk.Button(
            self.root, text="Выбрать всё", command=self.select_all_left
        )
        self.select_all_button.grid(row=1, column=0, pady=10)

        self.output_frame = tk.Frame(root)
        self.output_frame.grid(
            row=2, column=0, columnspan=3, padx=10, pady=10, sticky="nsew"
        )

        self.output_text = tk.Text(self.output_frame, height=20, wrap="word")
        self.output_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        scrollbar = tk.Scrollbar(self.output_frame, command=self.output_text.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.output_text.config(yscrollcommand=scrollbar.set)

        self.prove_button = tk.Button(
            self.root, text="Доказать", command=self.prove_selected
        )
        self.prove_button.grid(row=3, column=0, columnspan=3, pady=10)

        self.root.grid_rowconfigure(1, weight=1)
        self.root.grid_columnconfigure(1, weight=1)

        # self.select_all_left()

        self.output_text.bind("<Button-3>", self.copy_to_clipboard)

    def copy_to_clipboard(self, event):
        """Копирует текст из окна вывода в буфер обмена"""
        text = self.output_text.get(1.0, tk.END)  # Получаем весь текст из виджета
        self.root.clipboard_clear()  # Очищаем буфер обмена
        self.root.clipboard_append(text)  # Добавляем текст в буфер обмена
        self.root.update()  # Обновляем буфер обмена

    def select_all_left(self):
        """Выбирает все аксиомы в левой части (ставит галочки на все)"""
        for var in self.left_vars:
            var.set(True)

    def add_facts_to_fields(self):
        facts = self.rules_dict["facts"]
        facts_texts = [ell["text"] for ell in facts]

        self.left_items = facts_texts
        self.right_items = facts_texts
        return

    def create_fact_to_id(self):
        self.fact_to_id = {}
        facts = self.rules_dict["facts"]
        for ell in facts:
            self.fact_to_id[ell["text"]] = ell["id"]

    def create_scrollable_checklist(self, parent, items, vars):
        canvas = tk.Canvas(parent)
        scrollbar = tk.Scrollbar(parent, orient="vertical", command=canvas.yview)
        scrollable_frame = tk.Frame(canvas)

        scrollable_frame.bind(
            "<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )

        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        for i, item in enumerate(items):
            tk.Checkbutton(scrollable_frame, text=item, variable=vars[i]).pack(
                anchor="w"
            )

        canvas.grid(row=0, column=0, sticky="nsew")
        scrollbar.grid(row=0, column=1, sticky="ns")

    def prove_selected(self):
        left_selected = [
            self.left_items[i] for i, var in enumerate(self.left_vars) if var.get()
        ]
        right_selected = [
            self.right_items[i] for i, var in enumerate(self.right_vars) if var.get()
        ]

        # Тут уже обращение к модели - её передавть в конструкторе
        self.output_text.delete(1.0, tk.END)
        self.output_text.insert(
            tk.END,
            f"Аксиомы: {', '.join([f"{self.fact_to_id[ell]} - {ell}" for ell in left_selected])}\n",
        )
        self.output_text.insert(
            tk.END,
            f"Вывести: {', '.join([f"{self.fact_to_id[ell]} - {ell}" for ell in right_selected])}\n",
        )
        lef_set = set([self.fact_to_id[ell] for ell in left_selected])
        right_set = set([self.fact_to_id[ell] for ell in right_selected])

        path = self.production_system.find_path_to_target_facts_forward(
            start_facts=lef_set, target_facts=right_set
        )

        path_str = self.production_system.path_str(path)

        self.output_text.insert(
            tk.END,
            f"Доказательство: {path_str}\n",
        )

    def start(self):
        self.root.mainloop()


file_path = ".//rules_and_facts//music-20.json"

with open(file_path, "r", encoding="utf-8") as file:
    rules_dict = json.load(file)


ps = ProductionSystem(file_path)
# path = ps.find_path_to_target_facts_forward({"f1", "f2"}, {"f3", "f4"})


root = tk.Tk()
psi = ProductionSystemInterface(root, rules_dict, production_system=ps)
psi.start()


# медленный пример - эволюция + древняя -> элтон джон (556000 узло)
# эволюция + древняя -> settlers - хорошо - большой пример

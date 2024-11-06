import copy
import json


class Fact:
    def __init__(self, id: str, text, number):
        self.id = id
        self.text = text
        self.number = number

    def __str__(self):
        return f"Fact(id={self.id}, text='{self.text}', number={self.number})"


class Rule:
    def __init__(self, id: str, premises: list[str], conclusion):
        self.id = id
        self.premises = premises
        self.conclusion = conclusion

    def __str__(self):
        premises_str = ", ".join([str(p) for p in self.premises])
        return f"{premises_str} -> {self.conclusion}"


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

    def load(self, file_path):
        try:
            with open(file_path, "r", encoding="utf-8") as file:
                data = json.load(file)
                for rule in data["rules"]:
                    id = rule["id"]
                    premises = rule["premises"]
                    conclusion = rule["conclusion"]
                    self.add(Rule(id, premises, conclusion))
        except FileNotFoundError:
            print(f"Файл {file_path} не найден.")
        except json.JSONDecodeError:
            print(f"Ошибка при чтении JSON из файла {file_path}.")


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
        try:
            with open(file_path, "r", encoding="utf-8") as file:
                data = json.load(file)
                for fact in data["facts"]:
                    id = fact["id"]
                    text = fact["text"]
                    self.add(Fact(id, text=text, number=0))
        except FileNotFoundError:
            print(f"Файл {file_path} не найден.")
        except json.JSONDecodeError:
            print(f"Ошибка при чтении JSON из файла {file_path}.")


class Node:
    def __init__(self, facts: set[str], rule: str, parent_index: int):
        self.facts = facts
        self.rule = rule
        self.parent_index = parent_index

    def __str__(self):
        facts_str = ", ".join([str(f) for f in self.facts])
        return f"Node(facts=[{facts_str}], rule={self.rule})"

    def rules_that_gives_new_facts_forward(self, rules: Rules):
        rules_for_new_nodes_id = []
        for r in rules.rules.values():
            inferable = all(f in self.facts for f in r.premises)
            if inferable and r.conclusion not in self.facts:
                rules_for_new_nodes_id.append(r.id)

        return rules_for_new_nodes_id

    def rules_that_gives_new_facts_backward(self, rules: Rules):
        rules_for_new_nodes_id = []
        for r in rules.rules.values():
            if r.conclusion in self.facts:
                rules_for_new_nodes_id.append(r.id)

        return rules_for_new_nodes_id


def find_path_to_target_facts_forward(
    rules: Rules, start_facts: list[str], target_facts: set[str]
):
    node = Node(facts=set(start_facts), rule=None, parent_index=-1)
    nodes = [node]
    index = 0
    found = False

    # Main search loop
    while index < len(nodes) and not found:
        new_rules = nodes[index].rules_that_gives_new_facts_forward(rules=rules)
        for rule in new_rules:
            facts = copy.deepcopy(nodes[index].facts)
            new_fact = rules.get_fact_by_rule(rule)

            if new_fact not in facts:
                facts.add(new_fact)
                nodes.append(Node(facts=facts, rule=rule, parent_index=index))

                # Check if all target facts are found
                if target_facts.issubset(facts):
                    found = True
                    break
        index += 1

    # Trace back the path if found
    if found:
        path = []
        node = nodes[-1]

        while node.parent_index != -1:
            path.append(node)
            node = nodes[node.parent_index]

        path.append(node)  # Add the initial node facts
        path.reverse()

        return path
    else:
        return None


def find_path_to_target_facts_backward(
    rules: Rules, start_facts: list[str], target_facts: set[str]
):
    node = Node(facts=target_facts, rule=None, parent_index=-1)
    nodes = [node]
    index = 0
    found = False
    start_facts = set(start_facts)

    while index < len(nodes) and not found:
        new_rules = nodes[index].rules_that_gives_new_facts_backward(rules=rules)
        # print(new_rules)
        for rule in new_rules:
            facts = copy.deepcopy(nodes[index].facts)
            premises = rules.get_premises_by_rule(rule)
            # print(premises)

            facts_are_collected = all(f in facts for f in premises)
            if not facts_are_collected:
                for premise in premises:
                    facts.add(premise)

                nodes.append(Node(facts=facts, rule=rule, parent_index=index))

                if start_facts.issubset(facts):
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

        return path
    else:
        return None


def print_path(path):
    if path:
        print("Path to reach target facts:")
        for step in path:
            if step.rule:
                print(f"- {step.rule} -> {step.facts}", end=" ")
            else:
                print(f"{step.facts}", end=" ")
        print()
    else:
        print("Target facts could not be reached.")


def print_rules(path, rules: Rules):
    if path:
        for step in path:
            if step.rule:
                print(rules.get_rule_by_id(step.rule))
    else:
        print("Target facts could not be reached.")


def print_by_text(path, rules: Rules, facts: Facts, start_facts, target_facts):
    start_facts_str = " и ".join([facts.get_text_by_id(f) for f in start_facts])
    target_facts_str = " и ".join([facts.get_text_by_id(f) for f in target_facts])
    print(f"\n{start_facts_str} -> {target_facts_str}\n")

    if path:
        for step in path:
            if step.rule:
                rule_obj = rules.get_rule_by_id(step.rule)
                facts_str = " и ".join(
                    [facts.get_text_by_id(p) for p in rule_obj.premises]
                )
                print(
                    f"Если {facts_str} - > {facts.get_text_by_id(rule_obj.conclusion)}"
                )

    else:
        print("Target facts could not be reached.")


file_path = "illnes.json"

facts = Facts()
facts.load(file_path)

rules = Rules()
rules.load(file_path=file_path)


start_facts = ["f2", "f6"]
target_facts = {"f3"}

path = find_path_to_target_facts_forward(rules, start_facts, target_facts)

print_path(path=path)
print_rules(path=path, rules=rules)
print_by_text(
    path=path,
    rules=rules,
    facts=facts,
    start_facts=start_facts,
    target_facts=target_facts,
)


path = find_path_to_target_facts_backward(rules, start_facts, target_facts)

print_rules(path=path, rules=rules)
print_by_text(
    path=path,
    rules=rules,
    facts=facts,
    start_facts=start_facts,
    target_facts=target_facts,
)
path.reverse()
print_path(path=path)

# # print_path(path=path)
# # print_rules(path=path, rules=rules)
# print_by_text(
#     path=path,
#     rules=rules,
#     facts=facts,
#     start_facts=start_facts,
#     target_facts=target_facts,
# )

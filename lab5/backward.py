import tkinter as tk
import json
from typing import Dict
import copy
import hashlib
from enum import Enum


class Status(Enum):
    UNKNOWN = "unknown"
    BLOCKED = "blocked"
    PROVED = "proved"


class Fact:
    def __init__(self, id: str, text, number):
        self.id = id
        self.text = text
        self.number = number

    def __str__(self):
        return f"Fact(id={self.id}, text='{self.text}', number={self.number})"


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
        self.rules: Dict[str, Rule] = {}
        self.rules_backward: Dict[str, Rule] = {}

    def add(self, rule: Rule):
        self.rules[rule.id] = rule
        self.rules_backward[rule.conclusion] = rule

    def get_fact_by_rule(self, id: str):
        return self.rules[id].conclusion

    def get_premises_by_rule(self, id: str):
        return self.rules[id].premises

    def get_rule_by_id(self, id: str):
        return self.rules[id]

    def load(self, file_path):
        with open(file_path, "r", encoding="utf-8") as file:
            data = json.load(file)
            for rule in data["rules"]:
                id = rule["id"]
                premises = rule["premises"]
                conclusion = rule["conclusion"]
                self.add(Rule(id, premises, conclusion))


class NodeBackward:
    def __init__(
        self,
        # Статус факта, индекс ребёнка, rule_id
        facts: dict[str:[Status, int, str]],
        # parent_indx, fact_id
        parent_info: list[int, str],
    ) -> None:
        self.facts = facts
        self.parent_info = parent_info

    def check_status(self) -> Status:
        status_set = set()
        for key, value in self.facts.items():
            status_set.add(value[0])
            # print(f"{key} - {value[0]}")

        if Status.UNKNOWN in status_set:
            return Status.UNKNOWN

        if Status.BLOCKED in status_set:
            return Status.BLOCKED

        return Status.PROVED

    def __str__(self) -> str:
        str_output = []
        str_output.append("Node { \n  Facts:")

        for key, value in self.facts.items():
            str_output.append(f"        {key} - {value}")

        str_output.append(" ParentInfo")
        str_output.append(f"        {self.parent_info}")
        str_output.append("}")
        return "\n".join(str_output)


class ProductionSystem:
    def __init__(self, path_rules_and_facts) -> None:
        self.facts = Facts()
        self.facts.load(path_rules_and_facts)
        self.rules = Rules()
        self.rules.load(path_rules_and_facts)

    def backward(
        self,
        start_facts: set[str],
        target_facts: set[str],
    ):
        target_facts_list = list(target_facts)
        first_facts_dict = {}
        for fact_id in target_facts_list:
            first_facts_dict[fact_id] = [Status.UNKNOWN, -1, ""]
        node = NodeBackward(first_facts_dict, [-1, ""])

        # node.facts[list(node.facts.keys())[0]][0] = Status.UNKNOWN - для тестов
        nodes = [node]

        index = 0
        found = False

        while index < len(nodes) and not found:
            facts = list(nodes[index].facts.keys())
            for fact in facts:
                if fact in start_facts:
                    nodes[index].facts[fact][0] = Status.PROVED

            status = nodes[index].check_status()
            if status == Status.PROVED:
                if nodes[index].parent_info[0] == -1:
                    found = True
                    break

            index += 1

        for n in nodes:
            print(n)
        if found == True:
            print("Found!")
            return nodes

        print("Not found!")
        return None


file_path = "rules.json"


ps = ProductionSystem(file_path)

ps.backward({"f1", "f2", "f3", "f4", "f5"}, {"f3", "f4", "f5"})


# rules_backwardm = ps.rules.rules_backward

# for key, value in rules_backwardm.items():
#     print(f"{key}: {value.premises}")

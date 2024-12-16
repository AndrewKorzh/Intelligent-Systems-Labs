from dataclasses import dataclass, field
from typing import Dict, List
from enum import Enum
import tkinter as tk
import json
from typing import Dict
import copy
import hashlib
from enum import Enum


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
        # Тупанул - должен быть список правил
        self.rules_backward: Dict[str, list[Rule]] = {}

    def add(self, rule: Rule):
        self.rules[rule.id] = rule
        if rule.conclusion in self.rules_backward:
            self.rules_backward[rule.conclusion].append(rule)
        else:
            self.rules_backward[rule.conclusion] = [rule]

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


class Status(Enum):
    UNKNOWN = "unknown"
    BLOCKED = "blocked"
    PROVED = "proved"


@dataclass
class FactInfo:
    state: Status
    child_id: int


@dataclass
class NodeBackward:
    facts: Dict[str, FactInfo]
    parent_index: int
    parent_fact_id: str
    parent_rule: str

    def check_status(self) -> Status:
        status_set = {fact_info.state for fact_info in self.facts.values()}

        if Status.UNKNOWN in status_set:
            return Status.UNKNOWN
        if Status.BLOCKED in status_set:
            return Status.BLOCKED
        return Status.PROVED

    def block(self):
        for val in self.facts.values():
            val.state = Status.BLOCKED

    def __str__(self) -> str:
        facts_str = "\n".join(
            [
                f"        {fact_id}: {fact_info}"
                for fact_id, fact_info in self.facts.items()
            ]
        )
        parent_str = f"ParentIndex: {self.parent_index}, ParentRule: {self.parent_rule}"
        return f"Node {{\n  Facts:\n{facts_str}\n  {parent_str}\n}}"


class ProductionSystem:
    def __init__(self, path_rules_and_facts) -> None:
        self.facts = Facts()
        self.facts.load(path_rules_and_facts)
        self.rules = Rules()
        self.rules.load(path_rules_and_facts)

    def collect_facts_to_root(self, nodes: list[NodeBackward], index) -> set:
        facts_set = set()
        while True:
            if index == -1:
                break
            for fact in nodes[index].facts.keys():
                facts_set.add(fact)
            index = nodes[index].parent_index

        print("facts_set: ", facts_set)
        return facts_set

    def backward(
        self,
        start_facts: set[str],
        target_facts: set[str],
    ) -> List[NodeBackward]:

        target_facts_list = list(target_facts)
        first_facts_dict = {
            fact_id: FactInfo(Status.UNKNOWN, -1) for fact_id in target_facts_list
        }

        nodes = [
            NodeBackward(
                facts=first_facts_dict,
                parent_index=-1,
                parent_fact_id="",
                parent_rule="",
            )
        ]
        current_index = 0
        proved = False

        count = 0

        while current_index < len(nodes):
            count += 1
            # if count >= 100:
            #     return
            print(current_index)
            current_node = nodes[current_index]
            # Проверка фактов на начальные
            for fact_id in current_node.facts.keys():
                if fact_id in start_facts:
                    current_node.facts[fact_id].state = Status.PROVED

            current_node_status = current_node.check_status()
            if current_node_status == Status.PROVED:
                if current_node.parent_index == -1:
                    proved = True
                    break
                parent_fact = current_node.parent_fact_id
                nodes[current_node.parent_index].facts[
                    parent_fact
                ].state = Status.PROVED
                current_index = current_node.parent_index
                continue

            if current_node_status == Status.BLOCKED:
                print("\n\nblocked\n\n")
                if current_node.parent_index == -1:
                    break
                parent_fact = current_node.parent_fact_id
                nodes[current_node.parent_index].facts[
                    parent_fact
                ].state = Status.BLOCKED
                current_index = current_node.parent_index
                continue

            # Если статус UNKNOWN, поиск новых фактов
            fact_id_and_rule_id: list[(str, str)] = []
            for fact_id in current_node.facts.keys():
                if current_node.facts[fact_id].state == Status.UNKNOWN:
                    if fact_id in self.rules.rules_backward:
                        for rule in self.rules.rules_backward[fact_id]:
                            new_facts = set(rule.premises)
                            collected = self.collect_facts_to_root(nodes, current_index)
                            if not new_facts.issubset(collected):
                                print("append")
                                fact_id_and_rule_id.append((fact_id, rule.id))

            if len(fact_id_and_rule_id) == 0:
                current_node.block()

            for fact_id, rule_id in fact_id_and_rule_id:
                premises = self.rules.get_premises_by_rule(rule_id)
                new_fact_dict = {p: FactInfo(Status.UNKNOWN, -1) for p in premises}
                nodes.append(
                    NodeBackward(
                        facts=new_fact_dict,
                        parent_index=current_index,
                        parent_fact_id=fact_id,
                        parent_rule=rule_id,
                    )
                )

            current_index += 1

        print("Proved" if proved else "Not Proved")
        return nodes


file_path = "music-50.json"

ps = ProductionSystem(file_path)

nodes = ps.backward(
    start_facts={"f1", "f2"},
    target_facts={"f39"},
)


for node in nodes:
    print(node)

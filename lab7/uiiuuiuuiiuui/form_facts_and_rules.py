import pandas as pd
import json
from collections import OrderedDict


# print(df.values)


# как-то отсортировать
def unic_values(df):
    unic_val = OrderedDict()
    values = df.values
    for row in values:
        for ell in row:
            if pd.notna(ell):
                unic_val[ell] = None
    return list(unic_val.keys())


def form_facts_dict(values):
    facts = {}
    for i, ell in enumerate(values, start=1):
        if ell:
            facts[ell] = f"f{i}"
    return facts


def form_facts_list(values):
    facts = []
    for i, ell in enumerate(values, start=1):
        if ell:
            facts.append({"id": f"f{i}", "text": ell})
    return facts


def form_rules(df, facts_dict):
    rules = []
    values = df.values
    for i, row in enumerate(values, start=1):
        if not row[0] in facts_dict:
            continue
        conclution = facts_dict[row[0]]
        premises = []
        for j in range(1, len(row)):
            if row[j] in facts_dict:
                premises.append(facts_dict[row[j]])

        rules.append({"id": f"r{i}", "premises": premises, "conclusion": conclution})

    return rules


def excel_format_to_normal(
    path_excel, path_save, delimiter="\t", encoding="windows 1251"
):
    df = pd.read_csv(path_excel, delimiter=delimiter, encoding=encoding)
    df.to_csv(path_save, encoding="utf-8", index=False)


def form_facts_and_rules_dict(df):
    unic_val = unic_values(df)
    facts_dict = form_facts_dict(unic_val)

    facts = form_facts_list(unic_val)
    rules = form_rules(df, facts_dict)

    for fact in facts:
        print(fact)

    print(f"facts: {len(facts)}")
    print(f"rules: {len(rules)}")

    result = {"facts": facts, "rules": rules}
    return result


path_to_csv = ".//source_csv//music-20.csv"
path_to_formated_csv = "rules.csv"
path_to_json = ".//rules_and_facts//music-20.json"

excel_format_to_normal(path_to_csv, path_to_formated_csv)

df = pd.read_csv(path_to_formated_csv, delimiter=";")
# print(df)
result = form_facts_and_rules_dict(df)

with open(path_to_json, "w", encoding="utf-8") as f:
    json.dump(result, f, indent=4, ensure_ascii=False)

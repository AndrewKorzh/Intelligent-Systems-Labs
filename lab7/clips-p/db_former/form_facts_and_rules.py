import pandas as pd
import json
from collections import OrderedDict


def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False


def unic_values(df):
    unic_val = OrderedDict()
    values = df.values
    for row in values:
        for ell in row:
            if pd.notna(ell) and not is_number(ell):
                unic_val[ell] = None
    return list(unic_val.keys())


def form_facts_list(values):
    facts = []
    for i, ell in enumerate(values):
        if ell:
            facts.append(f"F{i}: {ell}")
    return facts


def form_clips_rules(df, facts_dict):
    rules = []
    values = df.values
    for row in values:
        if row[0] not in facts_dict:
            continue
        conclusion = facts_dict[row[0]]
        premises = [
            facts_dict[row[j]] for j in range(2, len(row)) if row[j] in facts_dict
        ]
        if premises:
            rules.append(f"{','.join(premises)}={conclusion};{row[1]};")
    return rules


def form_facts_and_rules_clips(df):
    unic_val = unic_values(df)
    facts_dict = {ell: f"F{i}" for i, ell in enumerate(unic_val)}

    initial_facts = form_facts_list(unic_val)
    target_facts = "\n"
    rules = form_clips_rules(df, facts_dict)

    return initial_facts, target_facts, rules


def excel_format_to_normal(
    path_excel, path_save, delimiter="\t", encoding="windows 1251"
):
    df = pd.read_csv(path_excel, delimiter=delimiter, encoding=encoding)
    df.to_csv(path_save, encoding="utf-8", index=False)


path_to_csv = "./source_csv/music-20+conf.csv"
path_to_formated_csv = "rules.csv"
path_to_output_txt = "music-20.txt"

# Преобразование формата исходного CSV
excel_format_to_normal(path_to_csv, path_to_formated_csv)

# Чтение и преобразование данных
df = pd.read_csv(path_to_formated_csv, delimiter=";")
initial_facts, target_facts, rules = form_facts_and_rules_clips(df)

# Сохранение результатов
with open(path_to_output_txt, "w", encoding="utf-8") as f:
    f.write("\n".join(initial_facts))
    f.write("\n\n")
    f.write("\n".join(target_facts))
    f.write("\n\n")
    f.write("\n".join(rules))

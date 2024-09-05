import pandas as pd
import re

full_data = pd.read_csv("full_data.csv")
pcol = full_data['publisher'].dropna(axis=0)
pcol = pcol.astype({'publisher': 'string'})
pcol.reset_index()
print(pcol)
publishers = set()
list_checker = re.compile(r"'(.*?)'")
for line in full_data["publisher"]:
    if line and isinstance(line, str):
        s = list_checker.findall(line)
        if s:
            for e in line:
                publishers.add(e)
        else:
            publishers.add(line)
score_agg = dict.fromkeys(publishers, [])

for line in full_data["publisher"]:
    if line and isinstance(line, str):
        s = list_checker.findall(line)
        if s:
            for e in line:
                publishers.add(e)
        else:
            publishers.add(line)
for p in publishers:
    revs = full_data[full_data['publisher'] == p]
    for i,r in revs.iterrows():
        try:
            score_agg[p].append(float(r['score']))
        except KeyError:
            print(f'key {p} not found')
    print(f'to {p} appended {score_agg[p]}')
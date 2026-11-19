#!/usr/bin/env python3

import random
import csv

SEED = 202611
random.seed(SEED)

DIAS = ["Seg", "Ter", "Qua", "Qui", "Sex"]
HORAS = ["14h", "15h", "16h"]
HORARIOS = [f"{d}-{h}" for d in DIAS for h in HORAS]
DISCIPLINAS = ["Matemática", "Português", "Física", "Química", "Ciências", "Inglês"]

ETAPAS_EF = ["6º ano EF", "7º ano EF", "8º ano EF", "9º ano EF"]
ETAPAS_EM = ["1ª série EM", "2ª série EM", "3ª série EM"]

PESOS_EF = {"Matemática": 0.42, "Português": 0.28, "Ciências": 0.18, "Inglês": 0.12, "Física": 0.00, "Química": 0.00}
PESOS_EM = {"Matemática": 0.36, "Português": 0.20, "Física": 0.20, "Química": 0.16, "Inglês": 0.08, "Ciências": 0.00}

def sample_without_replacement_weighted(items, weights, k):
    chosen = []
    pool = [(it, w) for it, w in zip(items, weights) if w > 0]
    for _ in range(min(k, len(pool))):
        total = sum(w for _, w in pool)
        r = random.uniform(0, total)
        acc = 0.0
        for idx, (it, w) in enumerate(pool):
            acc += w
            if r <= acc:
                chosen.append(it)
                pool.pop(idx)
                break
    return chosen

def main():
    N_VOL, N_ALUNOS = 30, 55
    alunos = []
    for i in range(1, N_ALUNOS + 1):
        is_ef = (i <= 30)
        etapa = random.choice(ETAPAS_EF) if is_ef else random.choice(ETAPAS_EM)
        nivel = "Ensino Fundamental" if is_ef else "Ensino Médio"
        n_disc = 1 if random.random() < 0.55 else 2
        pesos = PESOS_EF if is_ef else PESOS_EM
        discs = sample_without_replacement_weighted(DISCIPLINAS, [pesos[d] for d in DISCIPLINAS], n_disc)
        n_hor = random.choices([1, 2, 3], weights=[0.25, 0.50, 0.25])[0]
        hors = sorted(random.sample(HORARIOS, n_hor), key=lambda x: HORARIOS.index(x))
        alunos.append({
            "id": N_VOL + (i - 1), "codigo": f"A{i:02d}", "nivel": nivel,
            "etapa": etapa, "disciplinas": discs, "horarios": hors,
        })

    PERFIS_VOL = [
        ("Computação/Engenharias", 0.50, {"Matemática": 0.40, "Física": 0.25, "Inglês": 0.20, "Química": 0.08, "Ciências": 0.05, "Português": 0.02}),
        ("Ciências/Licenciaturas", 0.25, {"Ciências": 0.30, "Química": 0.28, "Matemática": 0.20, "Física": 0.14, "Português": 0.05, "Inglês": 0.03}),
        ("Letras/Educação",        0.25, {"Português": 0.55, "Inglês": 0.25, "Ciências": 0.10, "Matemática": 0.10, "Física": 0.00, "Química": 0.00}),
    ]

    voluntarios = []
    for i in range(1, N_VOL + 1):
        r = random.random()
        acc = 0.0
        perfil_nome, perfil_pesos = PERFIS_VOL[0][0], PERFIS_VOL[0][2]
        for nome, prob, p in PERFIS_VOL:
            acc += prob
            if r <= acc:
                perfil_nome, perfil_pesos = nome, p
                break
        n_disc = random.choices([1, 2, 3], weights=[0.30, 0.50, 0.20])[0]
        discs = sample_without_replacement_weighted(DISCIPLINAS, [perfil_pesos[d] for d in DISCIPLINAS], n_disc)
        n_hor = random.choices([2, 3, 4, 5], weights=[0.20, 0.35, 0.30, 0.15])[0]
        hors = sorted(random.sample(HORARIOS, n_hor), key=lambda x: HORARIOS.index(x))
        cap = min(n_hor, random.choices([1, 2, 3], weights=[0.30, 0.50, 0.20])[0])
        voluntarios.append({
            "id": i - 1, "codigo": f"V{i:02d}", "perfil": perfil_nome,
            "disciplinas": discs, "horarios": hors, "capacidade": cap,
        })

    arestas = []
    for v in voluntarios:
        set_dv, set_hv = set(v["disciplinas"]), set(v["horarios"])
        for a in alunos:
            disc_comum = len(set_dv & set(a["disciplinas"]))
            hor_comum = len(set_hv & set(a["horarios"]))
            if disc_comum >= 1 and hor_comum >= 1:
                arestas.append((v["id"], a["id"], 2 * disc_comum + hor_comum))

    with open("dados_voluntarios.csv", "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["id", "codigo", "perfil", "capacidade_semanal", "disciplinas", "horarios"])
        for v in voluntarios:
            w.writerow([v["id"], v["codigo"], v["perfil"], v["capacidade"], "; ".join(v["disciplinas"]), "; ".join(v["horarios"])])

    with open("dados_alunos.csv", "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["id", "codigo", "nivel", "etapa", "disciplinas", "horarios"])
        for a in alunos:
            w.writerow([a["id"], a["codigo"], a["nivel"], a["etapa"], "; ".join(a["disciplinas"]), "; ".join(a["horarios"])])

    with open("grafo.txt", "w", encoding="utf-8") as f:
        f.write("2\n")
        f.write(f"{N_VOL + N_ALUNOS}\n")
        for v in voluntarios:
            f.write(f'{v["id"]} "{v["codigo"]}"\n')
        for a in alunos:
            f.write(f'{a["id"]} "{a["codigo"]}"\n')
        f.write(f"{len(arestas)}\n")
        for u, v_id, peso in arestas:
            f.write(f"{u} {v_id} {peso}\n")

    print(f"Dataset gerado: {N_VOL + N_ALUNOS} vertices, {len(arestas)} arestas.")

if __name__ == "__main__":
    main()

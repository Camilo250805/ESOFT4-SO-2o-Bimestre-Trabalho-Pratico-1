"""Questao 2 - Simulacao de memoria com particoes fixas e First-Fit.

Execucao: python questao2.py
As unidades de memoria sao abstratas, conforme o enunciado.
"""

# As particoes permanecem na ordem original e nao sao divididas ou unidas.
particoes = [
    {"tamanho": tamanho, "processo": None, "ocupado": 0}
    for tamanho in (100, 150, 200, 250, 300)
]


def fragmentacao_interna_total():
    """Soma apenas as sobras das particoes atualmente ocupadas."""
    return sum(
        p["tamanho"] - p["ocupado"]
        for p in particoes
        if p["processo"] is not None
    )


def alocar_processo(nome, tamanho):
    """Aloca na primeira particao livre suficiente; retorna sucesso/falha."""
    if not isinstance(nome, str) or not nome.strip():
        print("Erro: o nome do processo deve ser um texto nao vazio.")
        return False
    nome = nome.strip()
    if isinstance(tamanho, bool) or not isinstance(tamanho, int) or tamanho <= 0:
        print("Erro: o tamanho deve ser um numero inteiro positivo.")
        return False
    if any(p["processo"] == nome for p in particoes):
        print(f"Erro: o processo {nome} ja esta alocado.")
        return False

    # First-Fit: percorre desde a primeira particao e para ao encontrar vaga.
    for indice, particao in enumerate(particoes, start=1):
        if particao["processo"] is None and tamanho <= particao["tamanho"]:
            particao["processo"] = nome
            particao["ocupado"] = tamanho
            sobra = particao["tamanho"] - tamanho
            print(f"{nome} ({tamanho} unidades) alocado na particao {indice} "
                  f"({particao['tamanho']} unidades).")
            print(f"Fragmentacao interna desta alocacao: {sobra} unidades.")
            print(f"Fragmentacao interna total: "
                  f"{fragmentacao_interna_total()} unidades.")
            return True

    # Nao basta haver memoria livre total: uma unica particao deve comportar
    # o processo, pois este simulador usa particoes fixas e indivisiveis.
    print(f"Falha ao alocar {nome} ({tamanho} unidades): "
          "nenhuma particao livre comporta o processo.")
    return False


def liberar_processo(nome):
    """Libera a particao do processo e remove sua fragmentacao interna."""
    if not isinstance(nome, str) or not nome.strip():
        print("Erro: o nome do processo deve ser um texto nao vazio.")
        return False
    nome = nome.strip()
    for indice, particao in enumerate(particoes, start=1):
        if particao["processo"] == nome:
            particao["processo"] = None
            particao["ocupado"] = 0
            print(f"{nome} liberado da particao {indice}.")
            return True
    print(f"Erro: o processo {nome} nao esta alocado.")
    return False


def exibir_memoria():
    """Mostra ocupacao, particoes livres e desperdicio interno atual."""
    print("\nESTADO DA MEMORIA (1000 unidades)")
    print(f"{'Particao':>8} | {'Tamanho':>7} | {'Processo':>8} | "
          f"{'Em uso':>6} | {'Frag. interna':>13}")
    print("-" * 57)
    for indice, p in enumerate(particoes, start=1):
        # Uma particao livre nao gera fragmentacao interna.
        sobra = p["tamanho"] - p["ocupado"] if p["processo"] is not None else 0
        processo = p["processo"] if p["processo"] is not None else "LIVRE"
        print(f"{indice:>8} | {p['tamanho']:>7} | {processo:>8} | "
              f"{p['ocupado']:>6} | {sobra:>13}")
    livres = sum(p["tamanho"] for p in particoes if p["processo"] is None)
    usados = sum(p["ocupado"] for p in particoes)
    print(f"Memoria usada pelos processos: {usados} unidades.")
    print(f"Memoria em particoes livres: {livres} unidades.")
    print(f"Fragmentacao interna total: {fragmentacao_interna_total()} unidades.\n")


def main():
    """Executa exatamente a sequencia de operacoes solicitada no PDF."""
    exibir_memoria()
    alocar_processo("P1", 90)
    exibir_memoria()
    alocar_processo("P2", 140)
    exibir_memoria()
    alocar_processo("P3", 180)
    exibir_memoria()
    liberar_processo("P2")
    exibir_memoria()
    alocar_processo("P4", 100)
    exibir_memoria()
    alocar_processo("P5", 350)
    exibir_memoria()


if __name__ == "__main__":
    main()

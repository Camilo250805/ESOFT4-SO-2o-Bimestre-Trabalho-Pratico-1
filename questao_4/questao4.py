"""Questao 4 - Contagem de referencias e coleta de ciclos em CPython.

Executar: python questao4.py
Requer CPython 3.4 ou posterior, com GIL habilitado, para observar a
destruicao imediata por contagem de referencias apresentada aqui.
"""

import gc
import sys
import weakref


class Objeto:
    """Objeto com um nome e uma lista grande para simular uso de memoria."""

    def __init__(self, nome):
        self.nome = nome
        # A lista reserva 100.000 posicoes; nao sao 100.000 inteiros distintos.
        self.dados = [0] * 100_000
        self.referencia = None
        print(f"CRIADO: {self.nome} | {len(self.dados)} posicoes na lista")

    def __del__(self):
        # Nao ressuscita o objeto nem cria novas referencias a ele.
        print(f"DESTRUIDO: {self.nome}")


def exibir_estatisticas(titulo):
    """Mostra os contadores acumulados informados por gc.get_stats()."""
    print(f"\n{titulo}")
    for indice, estatisticas in enumerate(gc.get_stats()):
        print(f"Indice {indice}: {estatisticas}")
    # A organizacao das geracoes pode variar conforme a versao do CPython.
    print("Os indices e contadores seguem a versao do Python em execucao.")


def cenario_1():
    print("\nCENARIO 1 - CONTAGEM DE REFERENCIAS, SEM CICLOS")
    objeto = Objeto("A")
    observador = weakref.ref(objeto)  # Nao mantem o objeto vivo.
    print("getrefcount(A), com uma variavel:", sys.getrefcount(objeto))
    apelido = objeto
    print("getrefcount(A), com duas variaveis:", sys.getrefcount(objeto))
    print("getrefcount inclui uma referencia temporaria da propria chamada.")

    del objeto
    print("Depois de del objeto, A continua vivo:", observador() is not None)
    print("Removendo a ultima referencia forte (apelido):")
    del apelido
    # No CPython tradicional, a contagem chega a zero e __del__ e chamado
    # imediatamente, mesmo com o coletor automatico de ciclos desativado.
    print("A foi destruido:", observador() is None)


def cenario_2():
    print("\nCENARIO 2 - OBJETO COM AUTORREFERENCIA")
    objeto = Objeto("B")
    objeto.referencia = objeto
    observador = weakref.ref(objeto)
    print("getrefcount(B), incluindo autorreferencia:", sys.getrefcount(objeto))

    del objeto
    print("Depois de del objeto, B continua vivo:", observador() is not None)
    print("A autorreferencia impede que a contagem chegue a zero.")
    coletados = gc.collect()  # Coleta explicita de todas as geracoes.
    print("Objetos inalcancaveis reportados por gc.collect():", coletados)
    print("B foi coletado:", observador() is None)


def cenario_3():
    print("\nCENARIO 3 - CICLO ENTRE DOIS OBJETOS")
    primeiro = Objeto("C")
    segundo = Objeto("D")
    primeiro.referencia = segundo
    segundo.referencia = primeiro
    observador_c = weakref.ref(primeiro)
    observador_d = weakref.ref(segundo)
    print("getrefcount(C):", sys.getrefcount(primeiro))
    print("getrefcount(D):", sys.getrefcount(segundo))

    del primeiro
    del segundo
    print("Sem variaveis externas, C e D continuam vivos:",
          observador_c() is not None and observador_d() is not None)
    coletados = gc.collect()
    print("Objetos inalcancaveis reportados por gc.collect():", coletados)
    print("C e D foram coletados:",
          observador_c() is None and observador_d() is None)


def main():
    # A destruicao imediata por contagem de referencias e uma caracteristica
    # do CPython tradicional, nao uma garantia de toda implementacao Python.
    if sys.implementation.name != "cpython":
        print("Execute no CPython para reproduzir esta demonstracao.")
        return
    gil_habilitado = getattr(sys, "_is_gil_enabled", None)
    if gil_habilitado is not None and not gil_habilitado():
        print("Execute no CPython com GIL habilitado para reproduzir "
              "a demonstracao de destruicao imediata.")
        return

    print("Implementacao:", sys.implementation.name)
    print("Versao:", sys.version.split()[0])
    print("Tamanho da lista de cada objeto: 100.000 posicoes.")
    estado_anterior = gc.isenabled()
    print("Coleta automatica de ciclos inicialmente ativa:", estado_anterior)

    # Desativa apenas a coleta AUTOMATICA de ciclos para controlar o momento
    # das demonstracoes. A contagem de referencias continua funcionando, e
    # chamadas explicitas a gc.collect() continuam permitidas.
    gc.disable()
    try:
        gc.collect()  # Limpa ciclos anteriores antes de registrar a base.
        exibir_estatisticas("ESTATISTICAS INICIAIS - gc.get_stats()")
        cenario_1()
        cenario_2()
        cenario_3()
        exibir_estatisticas("ESTATISTICAS FINAIS - gc.get_stats()")
        print("\nANALISE DOS RESULTADOS")
        print("Cenario 1: a contagem de referencias libera o objeto sem ciclos.")
        print("Cenarios 2 e 3: o coletor identifica ciclos inalcancaveis.")
        print("gc.collect() pode contar tambem listas e outros objetos do ciclo;")
        print("seu retorno nao corresponde apenas as instancias de Objeto.")
        print("Destruir objetos nao garante queda imediata da memoria do processo:")
        print("o alocador pode reter memoria para reutilizacao.")
    finally:
        # Restaura a configuracao original, inclusive se ocorrer uma excecao.
        if estado_anterior:
            gc.enable()
        else:
            gc.disable()
        print("\nColeta automatica de ciclos ativa ao finalizar:", gc.isenabled())


if __name__ == "__main__":
    main()

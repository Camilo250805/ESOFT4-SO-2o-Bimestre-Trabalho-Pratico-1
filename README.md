# Gerenciamento de Memoria em Sistemas Operacionais

Codigos das cinco questoes e do bonus, organizados por atividade.

| Pasta | Tema | Linguagem |
|---|---|---|
| [questao_1](questao_1) | Alocacao estatica e dinamica | C |
| [questao_2](questao_2) | Fragmentacao de memoria | Python |
| [questao_3](questao_3) | Substituicao de paginas FIFO | Java |
| [questao_4](questao_4) | Garbage Collection em Python | Python |
| [questao_5](questao_5) | Comparacao de desempenho entre pilha e heap | C |
| [questao_bonus](questao_bonus) | LRU e comparacao com FIFO | Java |

## Como usar

Abra a pasta da questao no VS Code e siga seu README. Requisitos: GCC para C, CPython 3 com GIL para Python e JDK para Java. As extensoes do editor nao substituem essas ferramentas.

## Verificacao

Todos os seis programas foram executados no Windows. Os programas C foram compilados com Zig/Clang 19.1.7 com avisos tratados como erros; os programas Java foram compilados com javac -Xlint:all. Python: CPython 3.12.14. Cada pasta inclui a saida real da execucao em resultado_execucao.txt.

Esses testes verificam as execucoes realizadas; enderecos, tempos e contadores podem variar. Os arquivos de resultados sao registros de texto, nao capturas de tela.

## Entrega

Este repositorio contem os codigos, instrucoes e registros dos testes. O PDF das respostas teoricas e as capturas reais das execucoes devem ser incluidos na entrega conforme o enunciado.

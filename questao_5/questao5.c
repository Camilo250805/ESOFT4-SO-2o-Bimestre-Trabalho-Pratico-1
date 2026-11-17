/* Questao 5 - Comparacao de alocacao local e dinamica em C.
 * Compilar: gcc -std=c11 -O2 -Wall -Wextra -Wpedantic questao5.c -o questao5
 * Windows: .\questao5.exe   |   Linux/macOS: ./questao5
 * Sao feitas 7 rodadas, cada uma com 1.000.000 de chamadas por teste.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define OPERACOES 1000000
#define REPETICOES 7

/* Evita incorporar as funcoes ao laco nos compiladores indicados.
 * Em outros compiladores, a variavel volatile ainda exige os acessos.
 */
#if defined(_MSC_VER)
#define NAO_INLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define NAO_INLINE __attribute__((noinline))
#else
#define NAO_INLINE
#endif

/* Os ponteiros volatile fazem as chamadas reais ao alocador passarem por
 * leituras observaveis, dificultando eliminar o par malloc/free no teste.
 * O custo dessa chamada indireta faz parte da medicao do teste dinamico.
 */
static void *(* volatile alocar)(size_t) = malloc;
static void (* volatile desalocar)(void *) = free;

typedef struct {
    double segundos;
    uint64_t soma;
} Resultado;

static double agora(void)
{
#ifdef _WIN32
    /* No Windows, clock() pode ser grosseiro demais para o teste local.
     * O contador de alta resolucao mede tempo decorrido em segundos.
     */
    LARGE_INTEGER frequencia;
    LARGE_INTEGER contador;
    if (!QueryPerformanceFrequency(&frequencia)
            || !QueryPerformanceCounter(&contador)) {
        fprintf(stderr, "Erro: contador de alta resolucao indisponivel.\n");
        exit(EXIT_FAILURE);
    }
    return (double)contador.QuadPart / (double)frequencia.QuadPart;
#else
    clock_t instante = clock();
    if (instante == (clock_t)-1) {
        fprintf(stderr, "Erro: clock() indisponivel.\n");
        exit(EXIT_FAILURE);
    }
    return (double)instante / CLOCKS_PER_SEC;
#endif
}

static NAO_INLINE int operacao_pilha(int valor)
{
    /* Uma variavel local de armazenamento automatico por chamada.
     * volatile exige escrita e leitura; seu tempo de vida termina ao sair.
     * Nao ha uma funcao free para variaveis locais.
     */
    volatile int local = valor;
    return local;
}

static NAO_INLINE int operacao_heap(int valor)
{
    int *bloco = alocar(sizeof *bloco);
    if (bloco == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memoria durante o teste.\n");
        exit(EXIT_FAILURE);
    }
    /* Mesma escrita e leitura de int volatile usadas no teste local. */
    volatile int *acesso = bloco;
    *acesso = valor;
    int resultado = *acesso;
    desalocar(bloco);
    return resultado;
}

static Resultado medir(int usar_heap)
{
    uint64_t soma = 0;
    double inicio = agora();
    /* A escolha fica fora do laco para nao adicionar uma condicao por item. */
    if (usar_heap) {
        for (int i = 0; i < OPERACOES; ++i) {
            soma += (uint64_t)operacao_heap(i);
        }
    } else {
        for (int i = 0; i < OPERACOES; ++i) {
            soma += (uint64_t)operacao_pilha(i);
        }
    }
    double fim = agora();
    if (fim < inicio) {
        fprintf(stderr, "Erro: leitura invalida ou retorno do relogio.\n");
        exit(EXIT_FAILURE);
    }
    Resultado resultado = {fim - inicio, soma};
    return resultado;
}

int main(void)
{
    double total_pilha = 0.0;
    double total_heap = 0.0;
    uint64_t esperado = (uint64_t)OPERACOES * (OPERACOES - 1) / 2;

    /* Aquecimento fora da medicao para reduzir efeitos da primeira chamada. */
    uint64_t aquecimento = 0;
    for (int i = 0; i < 10000; ++i) {
        aquecimento += (uint64_t)operacao_pilha(i);
        aquecimento += (uint64_t)operacao_heap(i);
    }

    printf("COMPARACAO: PILHA E HEAP\n");
    printf("Operacoes por teste: %d | Repeticoes: %d\n", OPERACOES, REPETICOES);
    printf("Aquecimento (soma): %" PRIu64 "\n", aquecimento);
#ifdef _WIN32
    printf("Relogio: QueryPerformanceCounter (tempo decorrido).\n\n");
#else
    printf("Relogio: clock(); resolucao nominal: %.9f s\n\n", 1.0 / CLOCKS_PER_SEC);
#endif
    printf("Rodada | Pilha (s)   | Heap (s)\n");

    for (int rodada = 0; rodada < REPETICOES; ++rodada) {
        Resultado pilha;
        Resultado heap;
        /* Alterna a ordem para reduzir o favorecimento de um dos testes. */
        if (rodada % 2 == 0) {
            pilha = medir(0);
            heap = medir(1);
        } else {
            heap = medir(1);
            pilha = medir(0);
        }
        /* Confere o trabalho realizado e utiliza os resultados calculados. */
        if (pilha.soma != esperado || heap.soma != esperado) {
            fprintf(stderr, "Erro: soma de verificacao incorreta.\n");
            return EXIT_FAILURE;
        }
        total_pilha += pilha.segundos;
        total_heap += heap.segundos;
        printf("%6d | %11.6f | %11.6f\n", rodada + 1,
               pilha.segundos, heap.segundos);
        if (pilha.segundos == 0.0 || heap.segundos == 0.0) {
            printf("Aviso: uma medicao ficou abaixo da resolucao do relogio.\n");
        }
    }

    double media_pilha = total_pilha / REPETICOES;
    double media_heap = total_heap / REPETICOES;
    printf("\nMedia pilha: %.6f s\n", media_pilha);
    printf("Media heap:  %.6f s\n", media_heap);
    printf("Soma verificada em cada teste: %" PRIu64 "\n", esperado);

    /* Percentual relativo ao tempo da pilha: nao confundir aumento de
     * tempo com percentual de reducao de tempo, que teria outro denominador.
     */
    if (media_pilha > 0.0) {
        double percentual = (media_heap - media_pilha) / media_pilha * 100.0;
        printf("Variacao do tempo do heap em relacao a pilha: %+.2f%%\n",
               percentual);
        if (percentual >= 0.0) {
            printf("Nesta execucao, o heap levou %.2f%% mais tempo.\n", percentual);
        } else {
            printf("Nesta execucao, o heap levou %.2f%% menos tempo.\n", -percentual);
        }
    } else {
        printf("Percentual indisponivel: media da pilha igual a zero.\n");
    }

    /* INTERPRETACAO:
     * Espera-se menor tempo no teste local: a pilha normalmente reutiliza
     * espaco do quadro da funcao, enquanto malloc/free precisam administrar
     * blocos dinamicos. Nao existe obrigacao de o heap ser sempre mais lento.
     * Medimos os caminhos completos, incluindo chamadas, acessos volatile,
     * verificacao de malloc e soma, e nao apenas uma instrucao de alocacao.
     * Cada bloco dinamico e liberado imediatamente; o alocador pode reutilizar
     * o mesmo endereco. Nao sao um milhao de blocos simultaneamente vivos.
     * Hardware, compilador, otimizacoes, cache, biblioteca de alocacao,
     * resolucao do relogio e outras tarefas podem alterar os resultados.
     * C define armazenamento automatico, mas nao exige uma pilha fisica.
     */
    printf("\nAnalise: o heap inclui o gerenciamento de blocos por malloc/free.\n");
    printf("Os testes incluem chamadas de funcao e acessos a memoria.\n");
    printf("Tempos variam com compilador, hardware, cache e tarefas concorrentes.\n");
    printf("Execute novamente para observar essa variacao.\n");
    return EXIT_SUCCESS;
}

/* Questao 1 - Alocacao de tamanho fixo e alocacao dinamica.
 * Compilar: gcc -std=c11 -Wall -Wextra -Wpedantic questao1.c -o questao1
 * Executar no Windows (PowerShell): .\questao1.exe
 * Executar no Linux/macOS: ./questao1
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    /* Array de tamanho fixo, chamado de "estatico" no enunciado.
     * Por ser local e nao usar static, tem armazenamento automatico.
     * Em implementacoes usuais, esse armazenamento fica na pilha.
     */
    int array_fixo[5] = {1, 2, 3, 4, 5};

    /* Reserva espaco para 10 inteiros na memoria dinamica (heap).
     * sizeof *array_dinamico utiliza o tamanho do tipo apontado.
     */
    int *array_dinamico = malloc(10 * sizeof *array_dinamico);

    /* O ponteiro deve ser verificado antes de acessar a memoria. */
    if (array_dinamico == NULL) {
        fprintf(stderr, "Erro: nao foi possivel alocar memoria para 10 inteiros.\n");
        return EXIT_FAILURE;
    }

    /* Preenche o array dinamico com os valores de 10 a 19. */
    for (int i = 0; i < 10; ++i) {
        array_dinamico[i] = 10 + i;
    }

    /* %p recebe void * e apresenta o endereco de cada elemento. */
    printf("ARRAY DE TAMANHO FIXO - 5 INTEIROS\n");
    printf("Endereco inicial: %p\n", (void *)array_fixo);
    for (int i = 0; i < 5; ++i) {
        printf("Indice %d | Valor: %2d | Endereco: %p\n",
               i, array_fixo[i], (void *)&array_fixo[i]);
    }

    printf("\nARRAY DINAMICO - 10 INTEIROS\n");
    printf("Endereco inicial: %p\n", (void *)array_dinamico);
    for (int i = 0; i < 10; ++i) {
        printf("Indice %d | Valor: %2d | Endereco: %p\n",
               i, array_dinamico[i], (void *)&array_dinamico[i]);
    }

    /* Nao se pode subtrair diretamente ponteiros de arrays distintos.
     * Convertemos os enderecos para uintptr_t e calculamos a distancia
     * numerica absoluta. uintptr_t esta disponivel nos compiladores
     * usuais de Windows/Linux com enderecamento linear em bytes.
     * A interpretacao numerica dos ponteiros depende da implementacao.
     */
    uintptr_t endereco_fixo = (uintptr_t)(void *)array_fixo;
    uintptr_t endereco_dinamico = (uintptr_t)(void *)array_dinamico;
    uintptr_t diferenca = (endereco_fixo >= endereco_dinamico)
        ? endereco_fixo - endereco_dinamico
        : endereco_dinamico - endereco_fixo;

    printf("\nDiferenca absoluta entre os enderecos iniciais: %" PRIuPTR
           " bytes (em sistemas com enderecamento linear em bytes).\n",
           diferenca);
    printf("Os enderecos e a diferenca podem variar a cada execucao.\n");
    printf("Usualmente, o array local fica na pilha e o dinamico no heap.\n");
    printf("A linguagem C nao exige essa organizacao da memoria.\n");

    /* Libera a memoria obtida com malloc. Ela nao sera mais acessada. */
    free(array_dinamico);
    array_dinamico = NULL;
    printf("\nMemoria dinamica liberada com sucesso.\n");

    return EXIT_SUCCESS;
}

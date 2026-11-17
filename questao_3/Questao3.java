/* Questao 3 - Substituicao de paginas FIFO.
 * Compilar: javac Questao3.java
 * Executar: java Questao3
 * Informe as paginas separadas por espacos ou virgulas.
 */
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Locale;
import java.util.Queue;
import java.util.Scanner;

public class Questao3 {
    public static void main(String[] args) {
        try (Scanner entrada = new Scanner(System.in)) {
            int quantidadeFrames = lerFrames(entrada);
            List<Integer> referencias = lerReferencias(entrada);
            simularFIFO(quantidadeFrames, referencias);
        } catch (IllegalStateException e) {
            System.err.println("Erro: " + e.getMessage());
            System.exit(1);
        }
    }

    private static String lerLinha(Scanner entrada) {
        if (!entrada.hasNextLine()) {
            throw new IllegalStateException("entrada encerrada antes de informar todos os dados.");
        }
        return entrada.nextLine().trim();
    }

    private static int lerFrames(Scanner entrada) {
        while (true) {
            System.out.print("Numero de frames (inteiro positivo): ");
            try {
                int quantidade = Integer.parseInt(lerLinha(entrada));
                if (quantidade > 0) {
                    return quantidade;
                }
            } catch (NumberFormatException e) {
                // Texto invalido ou numero fora do intervalo de int.
            }
            System.out.println("Entrada invalida. Digite um inteiro maior que zero.");
        }
    }

    private static List<Integer> lerReferencias(Scanner entrada) {
        while (true) {
            System.out.print("Paginas (inteiros nao negativos, separados por espacos ou virgulas): ");
            String linha = lerLinha(entrada);
            List<Integer> referencias = new ArrayList<>();
            try {
                if (linha.isEmpty()) {
                    throw new NumberFormatException();
                }
                // Aceita espacos, virgulas e espacos ao redor de virgulas.
                for (String item : linha.split("\\s*,\\s*|\\s+", -1)) {
                    int pagina = Integer.parseInt(item);
                    if (pagina < 0) {
                        throw new NumberFormatException();
                    }
                    referencias.add(pagina);
                }
                return referencias;
            } catch (NumberFormatException e) {
                System.out.println("Entrada invalida. Informe ao menos uma pagina, "
                        + "usando somente inteiros nao negativos.");
            }
        }
    }

    private static void simularFIFO(int quantidadeFrames, List<Integer> referencias) {
        // A fila representa os frames ocupados em ordem de chegada.
        // A primeira pagina da fila e sempre a proxima candidata a sair.
        Queue<Integer> frames = new LinkedList<>();
        int faltas = 0;
        int acertos = 0;

        System.out.println("\nSIMULACAO FIFO - " + quantidadeFrames + " frames");
        System.out.println("Frames exibidos da pagina mais antiga para a mais recente.");
        for (int pagina : referencias) {
            String resultado;
            if (frames.contains(pagina)) {
                ++acertos;
                // Um acerto nao altera a ordem de chegada no FIFO.
                resultado = "Page Hit";
            } else {
                ++faltas;
                if (frames.size() == quantidadeFrames) {
                    int removida = frames.remove();
                    resultado = "Page Fault (substituiu " + removida + ")";
                } else {
                    resultado = "Page Fault (ocupou frame livre)";
                }
                frames.add(pagina);
            }
            System.out.printf("Referencia: %d | Frames: %s | %s | Total Faults: %d%n",
                    pagina, frames, resultado, faltas);
        }

        // Usa divisao em ponto flutuante para preservar a parte decimal.
        double taxa = (double) faltas / referencias.size();
        System.out.println("\nESTATISTICAS FINAIS");
        System.out.println("Total de referencias: " + referencias.size());
        System.out.println("Total de faltas de pagina: " + faltas);
        System.out.println("Total de acertos: " + acertos);
        System.out.printf(Locale.ROOT, "Taxa de faltas: %d/%d = %.4f (%.2f%%)%n",
                faltas, referencias.size(), taxa, taxa * 100.0);
    }
}

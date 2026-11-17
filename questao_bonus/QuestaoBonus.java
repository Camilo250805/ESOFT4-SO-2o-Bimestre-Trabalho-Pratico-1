/* Bonus - LRU e comparacao com FIFO da Questao 3.
 * Compilar: javac QuestaoBonus.java
 * Executar: java QuestaoBonus
 * Paginas: inteiros nao negativos separados por espacos ou virgulas.
 */
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Queue;
import java.util.Scanner;

public class QuestaoBonus {
    public static void main(String[] args) {
        try (Scanner entrada = new Scanner(System.in)) {
            int capacidade = lerFrames(entrada);
            List<Integer> referencias = lerReferencias(entrada);
            int faltasFIFO = simularFIFO(capacidade, referencias);
            int faltasLRU = simularLRU(capacidade, referencias);
            comparar(faltasFIFO, faltasLRU, referencias.size());
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
                // Solicita novamente caso a entrada nao represente um int.
            }
            System.out.println("Entrada invalida. Digite um inteiro maior que zero.");
        }
    }

    private static List<Integer> lerReferencias(Scanner entrada) {
        while (true) {
            System.out.print("Paginas (separadas por espacos ou virgulas): ");
            String linha = lerLinha(entrada);
            List<Integer> referencias = new ArrayList<>();
            try {
                if (linha.isEmpty()) {
                    throw new NumberFormatException();
                }
                for (String item : linha.split("\\s*,\\s*|\\s+", -1)) {
                    int pagina = Integer.parseInt(item);
                    if (pagina < 0) {
                        throw new NumberFormatException();
                    }
                    referencias.add(pagina);
                }
                return referencias;
            } catch (NumberFormatException e) {
                System.out.println("Entrada invalida. Informe ao menos uma pagina "
                        + "usando inteiros nao negativos.");
            }
        }
    }

    private static int simularFIFO(int capacidade, List<Integer> referencias) {
        // Mesma regra da Questao 3: acertos nao alteram a ordem de chegada.
        Queue<Integer> frames = new ArrayDeque<>();
        int faltas = 0;
        System.out.println("\nFIFO - ordem: mais antiga -> mais recente na chegada");
        for (int pagina : referencias) {
            String resultado = "Page Hit";
            if (!frames.contains(pagina)) {
                ++faltas;
                if (frames.size() == capacidade) {
                    resultado = "Page Fault (substituiu " + frames.remove() + ")";
                } else {
                    resultado = "Page Fault (frame livre)";
                }
                frames.add(pagina);
            }
            System.out.printf("Referencia: %d | Frames: %s | %s | Total Faults: %d%n",
                    pagina, frames, resultado, faltas);
        }
        return faltas;
    }

    private static int simularLRU(int capacidade, List<Integer> referencias) {
        /* Mapa em ordem de acesso (accessOrder=true): a primeira chave e
         * a pagina menos recentemente usada. Cada valor registra o instante
         * logico da ultima referencia, sem depender do relogio da maquina.
         * get() atualiza a ordem inclusive em um acerto; containsKey() nao.
         * Busca/atualizacao tem custo medio O(1); a primeira chave permite
         * selecionar a vitima sem procurar o menor timestamp em todo o mapa.
         */
        Map<Integer, Integer> frames = new LinkedHashMap<>(16, 0.75f, true);
        int faltas = 0;
        System.out.println("\nLRU - ordem: menos recente -> mais recente no uso");
        System.out.println("Mapa: pagina=instante da ultima referencia (inicia em 1).");
        for (int indice = 0; indice < referencias.size(); ++indice) {
            int pagina = referencias.get(indice);
            int instante = indice + 1;
            String resultado;
            if (frames.get(pagina) != null) {
                resultado = "Page Hit";
            } else {
                ++faltas;
                if (frames.size() == capacidade) {
                    int removida = frames.keySet().iterator().next();
                    frames.remove(removida);
                    resultado = "Page Fault (substituiu " + removida + ")";
                } else {
                    resultado = "Page Fault (frame livre)";
                }
            }
            // Toda referencia atualiza o instante e a recencia da pagina.
            frames.put(pagina, instante);
            System.out.printf("Referencia: %d | Frames: %s | %s | Total Faults: %d%n",
                    pagina, frames.keySet(), resultado, faltas);
            System.out.println("Ultimo uso: " + frames);
        }
        return faltas;
    }

    private static void comparar(int fifo, int lru, int total) {
        double taxaFIFO = 100.0 * fifo / total;
        double taxaLRU = 100.0 * lru / total;
        System.out.println("\nCOMPARACAO - MESMA ENTRADA PARA OS DOIS ALGORITMOS");
        System.out.println("Referencias: " + total);
        System.out.printf(Locale.ROOT, "FIFO | Faltas: %d | Acertos: %d | Taxa: %.2f%%%n",
                fifo, total - fifo, taxaFIFO);
        System.out.printf(Locale.ROOT, "LRU  | Faltas: %d | Acertos: %d | Taxa: %.2f%%%n",
                lru, total - lru, taxaLRU);
        if (lru < fifo) {
            System.out.println("Nesta entrada, LRU teve " + (fifo - lru)
                    + " falta(s) a menos que FIFO.");
        } else if (lru > fifo) {
            System.out.println("Nesta entrada, LRU teve " + (lru - fifo)
                    + " falta(s) a mais que FIFO.");
        } else {
            System.out.println("Nesta entrada, os algoritmos tiveram o mesmo numero de faltas.");
        }
        System.out.printf(Locale.ROOT,
                "Diferenca entre taxas (FIFO - LRU): %+.2f pontos percentuais.%n",
                taxaFIFO - taxaLRU);
        System.out.println("\nANALISE CRITICA");
        System.out.println("FIFO considera a chegada; LRU considera a referencia mais recente.");
        System.out.println("LRU pode aproveitar a localidade temporal, mantendo paginas usadas recentemente.");
        System.out.println("LRU exige atualizar a recencia a cada referencia, inclusive nos acertos.");
        System.out.println("FIFO possui controle simples, mas pode remover uma pagina usada recentemente.");
        System.out.println("LRU nao garante menos faltas que FIFO em toda sequencia.");
        System.out.println("Esta comparacao mede faltas de pagina, nao tempo de execucao nem custo real de E/S.");
    }
}

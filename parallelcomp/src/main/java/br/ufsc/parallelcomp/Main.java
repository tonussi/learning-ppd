package br.ufsc.parallelcomp;

import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

public class Main {

    private final static Logger LOGGER = Logger.getLogger(Logger.GLOBAL_LOGGER_NAME);

    static List<Thread> bubbleThreads = new LinkedList();
    static List<Thread> quickThreads = new LinkedList();

    static List<OrdenadorBubbleSort> ordenadoresBubble = new LinkedList();
    static List<OrdenadorQuickSort> ordenadoresQuick = new LinkedList();

    public static void main(String[] args) throws InterruptedException {
        int tasksQuantity = 40, n = 40, processingPackage = 0, numberOfCores = 0;
        numberOfCores = Runtime.getRuntime().availableProcessors();

//        if (m % 2 == 0) {
//            if (cores % 2 == 0) {
//                pack = (int) m / cores;
//            } else {
//            }
//        } else {
//            if (cores % 2 == 0) {
//                pack = (int) m / cores;
//            } else {
//            }
//        }
        processingPackage = (tasksQuantity + numberOfCores - 1) / numberOfCores;
        if (processingPackage <= 0) {
            numberOfCores = tasksQuantity;
        }

        Matrix mat1 = new Matrix(tasksQuantity, n);
        Matrix mat2 = new Matrix(tasksQuantity, n);

        launchOrdenadoresBubble(mat1, tasksQuantity, processingPackage, numberOfCores);

        joinOrdenadoresBubble();

        showTimesOrdenadoresBubble();

        launchOrdenadoresQuick(mat2, tasksQuantity, processingPackage, numberOfCores);

        joinOrdenadoresQuick();

        showTimesOrdenadoresQuick();
    }

    private static void launchOrdenadoresBubble(Matrix mat, int m, int processingPackSize, int cores) {
        for (int i = 0; i < cores; i++) {
            int startIndex = i * processingPackSize;
            int endIndex = Math.min(startIndex + processingPackSize, m);

            OrdenadorBubbleSort ordenadorBubbleSort = new OrdenadorBubbleSort(mat, startIndex, endIndex);
            ordenadoresBubble.add(ordenadorBubbleSort);

            Thread t = new Thread(ordenadorBubbleSort);
            bubbleThreads.add(t);
            t.start();
        }
    }

    private static void launchOrdenadoresQuick(Matrix mat, int m, int processingPackSize, int cores) {
        for (int i = 0; i < cores; i++) {
            int startIndex = i * processingPackSize;
            int endIndex = Math.min(startIndex + processingPackSize, m);

            OrdenadorQuickSort ordenadorQuickSort = new OrdenadorQuickSort(mat, startIndex, endIndex);
            ordenadoresQuick.add(ordenadorQuickSort);

            Thread t = new Thread(ordenadorQuickSort);
            quickThreads.add(t);
            t.start();
        }
    }

    private static void joinOrdenadoresBubble() throws InterruptedException {
        for (int i = 0; i < bubbleThreads.size(); i++) {
            bubbleThreads.get(i).join();
        }
    }

    private static void joinOrdenadoresQuick() throws InterruptedException {
        for (int i = 0; i < quickThreads.size(); i++) {
            quickThreads.get(i).join();
        }
    }

    private static void showTimesOrdenadoresBubble() {
        double sum = 0.0;
        for (int i = 0; i < ordenadoresBubble.size(); i++) {
            for (Map.Entry<Integer, Long> entry : ordenadoresBubble.get(i).getDurations().entrySet()) {
                double seconds = (double) entry.getValue() / 1_000_000_000.0;
//                System.out.printf("Ordenador Bubble %d : Exec Duration (seconds): %.10f\n", entry.getKey(), seconds);
                sum += seconds;
            }
        }

        System.out.printf("\nSum Of All Ordenadores Bubble : Exec Duration (seconds): %.10f\n", sum);
    }

    private static void showTimesOrdenadoresQuick() {
        double sum = 0.0;

        for (int i = 0; i < ordenadoresQuick.size(); i++) {
            for (Map.Entry<Integer, Long> entry : ordenadoresQuick.get(i).getDurations().entrySet()) {
                double seconds = (double) entry.getValue() / 1_000_000_000.0;
//                System.out.printf("Ordenador Quick %d : Exec Duration (seconds): %.10f\n", entry.getKey(), seconds);
                sum += seconds;
            }
        }

        System.out.printf("\nSum Of All Ordenadores Quick: Exec Duration (seconds): %.10f\n", sum);
    }
}

package br.ufsc.parallelcomp;

import java.util.HashMap;
import java.util.Map;

public class OrdenadorBubbleSort implements Runnable {

    Matrix mat;
    Integer start;
    Integer end;
    Map<Integer, Long> durations;

    public OrdenadorBubbleSort(Matrix mat, Integer startIndex, Integer endIndex) {
        this.mat = mat;
        this.durations = new HashMap<>();
        this.start = startIndex;
        this.end = endIndex;
    }

    @Override
    public void run() {
        int position = 0;
        for (int i = start; i < end; i++) {
            long startTime = System.nanoTime();
            int[] aux = mat.getMat(i);
            BenchSortAlgo.bubbleSort(aux);
            long endTime = System.nanoTime();
            durations.put(i, endTime - startTime);
        }
    }

    Map<Integer, Long> getDurations() {
        return durations;
    }

}

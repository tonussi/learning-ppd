package br.ufsc.parallelcomp;

public class Matrix {

    Integer m;
    Integer n;
    int[][] mat;

    public Matrix(Integer m, Integer n) {
        this.m = m;
        this.n = n;
        this.mat = new int[m][n];
        fillMatrix();
    }

    public static int getRandomIntegerBetweenRange(int min, int max) {
        return (int) (Math.random() * ((max - min) + 1)) + min;
    }

    public void fillMatrix() {
        for (int i = 0; i < mat.length; i++) {
            for (int j = 0; j < mat[i].length; j++) {
                mat[i][j] = getRandomIntegerBetweenRange(0, 100);
            }
        }
    }

    public void printMatrix() {
        for (int i = 0; i < mat.length; i++) {
            for (int j = 0; j < mat[i].length; j++) {
                System.out.print(mat[i][j] + " ");
            }
            System.out.println();
        }
    }

    int[] getMat(Integer position) {
        return mat[position];
    }
}

package ds.misc;

import java.util.List;
import java.util.stream.IntStream;


public class PermutationCalculator {
    private final int[][] distances;
    private final int[][] connectivity;

    public PermutationCalculator(int[][] distances, int[][] connectivity) {
        this.distances = distances;
        this.connectivity = connectivity;
    }

    public int[] calculateBestPermutationForShare(int share) {
        int error = Integer.MAX_VALUE;
        int[] permutation = IntStream.range(0, connectivity.length - 1).boxed().mapToInt(i -> i).toArray();
        List<int[]> permutations = Utils.permute(permutation.clone());
        for (int i = 0; i < permutations.size(); i++) {
            int[] current = sharePermutation(permutations.get(i), share);
            int bias = calculatePermutationScore(current);
            if (bias < error) {
                error = bias;
                permutation = current;
            }
        }
        return permutation;
    }

    private int[] sharePermutation(int[] permutation, int share) {
        int[] shared = new int[permutation.length + 1];
        for (int i = 0; i < shared.length; i++) {
            if (i == 0) shared[i] = share;
            else if (permutation[i - 1] < share) shared[i] = permutation[i - 1];
            else shared[i] = permutation[i - 1] + 1;
        }
        return shared;
    }

    public int calculatePermutationScore(int[] permutation) {
        int[][] permutated = new int[connectivity.length][connectivity.length];
        for (int i = 0; i < permutation.length; i++)
            for (int j = 0; j < permutation.length; j++)
                permutated[i][j] = connectivity[permutation[i]][permutation[j]];
        int error = 0;
        for (int i = 0; i < distances.length; i++)
            for (int j = 0; j < distances[i].length; j++)
                error += permutated[i][j] == -1 ? 0 : distances[i][j];
        return error;
    }
}

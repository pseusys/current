package ds.misc;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;


public class Utils {
    public static List<int[]> permute(int[] nums) {
        List<int[]> result = new ArrayList<>();
        Utils.permute_internal(0, nums, result);
        return result;
    }

    private static void permute_internal(int i, int[] nums, List<int[]> result) {
        if (i == nums.length - 1) result.add(nums.clone());
        else for (int j = i, l = nums.length; j < l; j++) {
            int temp = nums[j];
            nums[j] = nums[i];
            nums[i] = temp;
            permute_internal(i + 1, nums, result);
            temp = nums[j];
            nums[j] = nums[i];
            nums[i] = temp;
        }
    }

    public static int[][] readMatrixFromResource(String resource) throws IOException {
        System.out.println("Reading resource configuration file: " + resource);
        InputStream resStream = Utils.class.getClassLoader().getResourceAsStream(resource);
        BufferedReader reader = new BufferedReader(new InputStreamReader(resStream));

        String line;
        ArrayList<ArrayList<Integer>> lines = new ArrayList<>();
        while ((line = reader.readLine()) != null) {
            ArrayList<Integer> values = new ArrayList<>();
            for (String value: line.split(";")) values.add(Integer.parseInt(value));
            lines.add(values);
        }
        
        int[][] result = new int[lines.size()][];
        for (int i = 0; i < lines.size(); i++)
            result[i] = lines.get(i).stream().mapToInt(e -> e).toArray();
        return result;
    }
}

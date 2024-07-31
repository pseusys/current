package ds.launchers;

import java.util.stream.IntStream;

import ds.base.Launcher;
import ds.node.NodeBuilder;


public class SimpleLauncher extends Launcher {
    public static void main(String[] args) throws InterruptedException {
        initialize(args);

        for (int i = 0; i < num; i++)
            NodeBuilder.create(i, matrix[i])
                .defineVirtual(IntStream.range(0, num).toArray(), mapping[i])
                .afterInitialization(initializationCallback)
                .onVirtualMessage(messageCallback)
                .build();
    }
}

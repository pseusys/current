package ds.launchers;

import ds.base.Launcher;
import ds.node.NodeBuilder;


public class IndividualLauncher extends Launcher {
    public static void main(String[] args) throws InterruptedException {
        initialize(args);

        int id = Integer.parseInt(System.getenv().getOrDefault("PHYSICAL_ID", "-1"));
        if (id == -1) throw new RuntimeException("Node id not defined ('PHYSICAL_ID' environmental variable)!");

        NodeBuilder.create(id, matrix[id])
            .computeVirtual(mapping)
            .afterInitialization(initializationCallback)
            .onVirtualMessage(messageCallback)
            .build();
    }
}

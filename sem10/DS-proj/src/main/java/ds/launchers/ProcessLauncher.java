package ds.launchers;

import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.ForkJoinTask;
import java.util.function.Consumer;

import ds.base.Launcher;
import ds.misc.TripleConsumer;
import ds.node.Node;
import ds.node.NodeBuilder;


public class ProcessLauncher extends Launcher {
    private static class NodeTask extends ForkJoinTask<Void> {
        private final int id;
        private final Consumer<Node> initializationCallback;
        private final TripleConsumer<String, Integer, Node> messageCallback;

        NodeTask(int physID, Consumer<Node> initializationCallback, TripleConsumer<String, Integer, Node> messageCallback) {
            this.id = physID;
            this.initializationCallback = initializationCallback;
            this.messageCallback = messageCallback;
        }

        @Override
        protected boolean exec() {
            NodeBuilder.create(id, matrix[id])
                .computeVirtual(mapping)
                .afterInitialization(initializationCallback)
                .onVirtualMessage(messageCallback)
                .build();
            return true;
        }

        @Override
        public Void getRawResult() {
            return null;
        }

        @Override
        protected void setRawResult(Void arg) {}
    }


    public static void main(String[] args) throws InterruptedException {
        initialize(args);
        ForkJoinPool commonPool = ForkJoinPool.commonPool();
        for (int i = 0; i < num; i++)
            commonPool.invoke(new NodeTask(i, initializationCallback, messageCallback));
    }
}

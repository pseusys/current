package ds.misc;


@FunctionalInterface
public interface TripleConsumer<A, B, C> {
    void apply(A a, B b, C c);
}

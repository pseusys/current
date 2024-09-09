package ds.base;


public interface VirtualNode {
    public int getVirtualID();

    public String virtualRepresentation();

    public void sendTextVirtual(String message, int recipient);

    public void sendMessageVirtual(BaseMessage message, int recipient);

    public void forwardMessageVirtual(BaseMessage message, int recipient);

    public void broadcastMessageVirtual(BaseMessage message);
}

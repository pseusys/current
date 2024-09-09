package ds.base;


public interface PhysicalNode {
    public int getPhysicalID();

    public String physicalRepresentation();

    public int[] physicalDistances();

    public void sendMessagePhysical(BaseMessage message, int neighbor);

    public void broadcastMessagePhysical(BaseMessage message);

    public void die(Integer cause);
}

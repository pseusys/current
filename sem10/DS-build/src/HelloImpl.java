import java.rmi.*;

public  class HelloImpl implements Hello {
    private final String message;

    public HelloImpl(String s) {
        message = s ;
    }

    public String sayHello() throws RemoteException {
        return message ;
    }
}

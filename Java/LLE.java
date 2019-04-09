import java.util.*;

// Linked List Extension: list constructors in functional style

public class LLE<A> extends LinkedList<A> {
    
    public static <A> LinkedList<A> empty () {
        return (new LinkedList<A>());
    }
    public static <A> LinkedList<A> add_elem (A elem, LinkedList<A> list) {
        list.addFirst(elem);
        return (list);
    }
}

import java.util.*;

class IntV extends Value {
    /* Fields */
    int iv;

    /* Constructors */
    public IntV (int i) {
	iv = i;
    }

    int get_int () {
        return iv;
    }
    void set_int (int i) {
        iv = i;
    }

    void print_value() {
        System.out.print(iv);
    }
}

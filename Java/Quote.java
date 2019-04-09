import java.util.*;

class Quote extends Instr {
    Value v;

    
    Value get_value() {
        return v;
    }

    void set_value(Value nv) {
        v = nv;
    }

    /* Constructors */
    public Quote (Value vl) {
        v = vl;
    }
    
    void exec_instr(Config cf) {
        cf.set_value(v);
        cf.get_code().pop();
    }
}

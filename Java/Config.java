import java.util.*;

class Config extends Object {
    Value v;
    LinkedList<Instr> c;
    LinkedList<StackElem> s;

    Value get_value() {
        return v;
    }
    LinkedList<Instr> get_code() {
        return c;
    }
    LinkedList<StackElem> get_stack() {
        return s;
    }

    void set_value(Value nv) {
        v = nv;
    }
    void set_code(LinkedList<Instr> nc) {
        c = nc;
    }
    void get_stack(LinkedList<StackElem> ns) {
        s = ns;
    }

    /* Constructors */
    public Config (Value vl, LinkedList<Instr> cd, LinkedList<StackElem> se) {
        v = vl;
        c = cd;
        s = se;
    }


    // one-step execution 
    boolean exec_step() {
        // to be implemented
        return true;
    }

    // run to completion
    void exec() {
        // to be implemented
    }

    // run for n steps
    void step(int n) {
        // to be implemented
    }
    
}


class A {
    x : Int <- 10;
    getX() : Int { x };
    setX(val : Int) : Int { x <- val };
};

class Main inherits IO {
    main() : Object {
        let a : A <- new A in {
            out_int(a.getX());
            out_string("\n");
            a.setX(99);
            out_int(a.getX());
            out_string("\n");
        }
    };
};

class Main inherits IO {
    main() : Object {
        let counter : Int <- 3 in {
            while 0 < counter loop {
                out_int(counter);
                out_string("\n");
                counter <- counter - 1;
            } pool;
            out_string("done\n");
        }
    };
};

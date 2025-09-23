class TreeNode {
  key  : Int;
  left : TreeNode;
  right: TreeNode;

  init(k : Int) : SELF_TYPE {
    {
      key   <- k;
      left  <- left;
      right <- right;
      self;
    }
  };

  insert(k : Int) : SELF_TYPE {
    {
      if k < key then
        if isvoid left then
          left <- (new TreeNode).init(k)
        else
          left.insert(k)
        fi
      else if key < k then
        if isvoid right then
          right <- (new TreeNode).init(k)
        else
          right.insert(k)
        fi
      else
        self
      fi fi;
      self;
    }
  };

  search(k : Int) : Bool {
    if k = key then
      true
    else if k < key then
      if isvoid left then false else left.search(k) fi
    else
      if isvoid right then false else right.search(k) fi
    fi fi
  };

  printInOrder(io : IO) : Object {
    {
      if isvoid left then 0 else left.printInOrder(io) fi;
      io.out_int(key).out_string(" ");
      if isvoid right then 0 else right.printInOrder(io) fi;
      0;
    }
  };
};

class BinaryTree {
  root : TreeNode;

  insert(k : Int) : SELF_TYPE {
    {
      if isvoid root then
        root <- (new TreeNode).init(k)
      else
        root.insert(k)
      fi;
      self;
    }
  };

  search(k : Int) : Bool {
    if isvoid root then false else root.search(k) fi
  };

  printInOrder() : Object {
    {
      let io : IO <- new IO in
        if isvoid root then
          io.out_string("<empty>\n")
        else
          { root.printInOrder(io); io.out_string("\n"); }
        fi;
      0;
    }
  };
};

class Main {
  main() : Object {
    {
      let t : BinaryTree <- new BinaryTree in {
        t.insert(5).insert(3).insert(7).insert(1).insert(4).insert(6).insert(9);
        (new IO).out_string("InOrder: ");
        t.printInOrder();
        (new IO).out_string("Search 4 -> ")
                 .out_string(if t.search(4) then "true\n" else "false\n" fi);
        (new IO).out_string("Search 8 -> ")
                 .out_string(if t.search(8) then "true\n" else "false\n" fi);
      };
      0;
    }
  };
};

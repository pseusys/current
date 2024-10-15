let rec f x =
   let rec g y = x + y in g
in print_int ((f 0) 0)
let x = 
let y = 1 + 2 in y
in let rec succ x = x + 1 
in let rec double x = 2 + x 
in print_int (succ (double x))
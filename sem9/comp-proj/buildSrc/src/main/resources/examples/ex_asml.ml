let x = 1 in
let rec add_x y = y + x in
let rec apply_to_zero u = u 0 in
print_int (apply_to_zero add_x)
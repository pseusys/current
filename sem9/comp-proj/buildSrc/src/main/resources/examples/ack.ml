let rec ack x y =
  if x <= 0 then 1 + 2 else
  ack (x - 1) (ack x (y - 1)) in
print_int (ack 3 10)

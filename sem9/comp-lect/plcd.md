# PLCD some lab session

## Exercise 77

Source code:

```c
main () {
    int x, y, z;
    x = 5;
    y = 1;
    z = x + y;
}
```

Stack offset table:

| Variable | Stack offset |
| :---: | :---: |
| x | 4 |
| y | 8 |
| z | 12 |

Generated code:

```asm
prologue(12)

# x := 5
ADD R1 R0 5
ST R1 [FP-4]

# y := 1
ADD R1 R0 1
ST R1 [FP-8]

# z := x + y
LD R1 [FP-4]
LD R2 [FP-8]
ADD R3 R1 R2
ST R3 [FP-12]

epilogue()
```

## Exercise 78

Source code:

```c
var x, y: int;
while (not x = y) do
    if (x > y) then
        x := x - y;
    else
        y := y - x;
    fi
od
```

Stack offset table:

| Variable | Stack offset |
| :---: | :---: |
| x | 4 |
| y | 8 |

Generated code:

```asm
main:
    prologue(12)

loop:
    LD R1 [FP-4]
    LD R2 [FP-8]
    CMP R1 R2  # Set the conditional flag
    BEQ end1  # if condition holds: PC <- end1, else: PC <- PC+4

    LD R1 [FP-4]
    LD R2 [FP-8]
    BLE else

    LD R1 [FP-4]
    LD R2 [FP-8]
    SUB R3 R1 R2
    ST R3 [FP-4]
    BA end2

else:
    LD R1 [FP-4]
    LD R2 [FP-8]
    SUB R3 R2 R1
    ST R3 [FP-8]

end2:
    BA loop

end1:
    epilogue()


epilogue()
```

## Extra (!!) exercise

```c
begin
    var x: int
    x:= 5
    begin
        var y, z: int
        z:= z* x + 1
        if (z > 10) then
            y := x
        else
            y := 0
        fi
        begin
            var z: int
            z := 2 * x + y
        end
        while (z >= 0) do
            y := y + 1
        od
        begin
            var y: int
            y := x + z
        end
    end
end
```

```asm
prologue(4)

ADD R1 R0 5
ST R1 [FP-4]

prologue(8)

LD R1 [FP-8]
LD R2 [FP]
LD R3 [R2-4]
ADD R1 R1 R3
ST R1 [FP-8]

else:
endif:
loop:
endloop:
```

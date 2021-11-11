import json
from functools import reduce


def hex_arr(lst):
    return f"[{' '.join(hex(item) for item in lst)}]"


def hex_mat(mat):
    return "\n" + '\n'.join(f"[ {' '.join(hex(item) for item in mat[4 * j:4 * j + 4])} ]" for j in range(0, 4))


def prepare_data_from_string(string):
    string = ((' ' * (16 - len(string))) if len(string) < 16 else '') + string
    print(f"Preparing string: '{string}'")
    bts = bytearray(string, 'ascii')[:16]
    print(" ".join(string), " ".join(hex(i) for i in bts), " ".join(format(i, '08b') for i in bts), sep='\n')
    return [int(i) for i in bts]


def rotate_bytes(bts, num):
    return bts[num:] + bts[:num]


def substitute(bts):
    return list(map(lambda b: tables["sb"][16 * (b // 16) + (b % 16)], bts))


def xor(bts, kys):
    return [bt ^ k for bt, k in zip(bts, kys)]


def gmul(a, b):
    if b == 1:
        return a
    if b not in (2, 3, 9, 11, 13, 14):
        raise Exception(f"GF multiplication is defined for 2 and 3 only (not {b})!")
    return tables[f"mul-{b}"][a]


def gpow(a, b):
    if a == 1:
        return 1
    if a not in (2, 3, 9, 11, 13, 14):
        raise Exception(f"GF powering is defined for 2 and 3 only (not {a})!")
    return reduce(lambda x, y: gmul(x, y), [a] * b, 1)


def round_key(num, prev_bytes):
    print(f"\nGenerating round key for round {num}, using {hex_arr(prev_bytes)} as previous key bytes...")
    rotated = rotate_bytes(prev_bytes[-4:], 1)
    print(f"Rotated: {hex_arr(rotated)}")
    substituted = substitute(rotated)
    print(f"Substituted: {hex_arr(substituted)}")
    rconst = [gpow(2, num), 0, 0, 0]
    xored = xor(substituted, rconst)
    print(f"XORed with round constant ({hex_arr(rconst)}): {hex_arr(xored)}")
    w = xored
    for i in range(-4, 1):
        w += xor(w[-4:], prev_bytes[4 * i:4 * i + 4])
    print(f"{num}th round key: {hex_arr(w[4:])}")
    return w[4:]


def compare(check, real):
    return "correct" if check == real else "wrong"


def unsubstitute(bts):
    return list(map(lambda b: tables["sb-1"][16 * (b // 16) + (b % 16)], bts))


# Визуализация алгоритма AES
if __name__ == '__main__':
    with open("AES_tables.json") as AES_tables:
        tables = json.load(AES_tables, object_hook=lambda x: dict(zip(x, map(lambda val: [int(v, 16) for v in val], x.values()))))

    print("Input text: first and last names, 128bit\n")
    plain_text = prepare_data_from_string("Name_Surname")
    print("\n\nKey: uni number and fathers name, 128bit\n")
    key = prepare_data_from_string("123456_LastName")

    # Шаг 1: расширение ключа
    round_keys = [None] * 10
    print()
    round_keys[0] = round_key(0, key)
    for i in range(1, 10):
        round_keys[i] = round_key(i, round_keys[i-1])

    # Шаг 2: добавление ключа
    matrix = xor(plain_text, key)
    print(f"\n\nMatrix XORed with initial key: {hex_mat(matrix)}\n")

    # Шаг 3: раундовые преобразования
    for i in range(0, 10):
        print(f"\n\nRound: {i}")

        matrix = substitute(matrix)
        print(f"\nBytes substituted: {hex_mat(matrix)}")

        for j in range(0, 4):
            matrix[j::4] = rotate_bytes(matrix[j::4], j)
        print(f"\nRows shifted: {hex_mat(matrix)}")

        if i < 9:
            f = [0x02, 0x03, 0x01, 0x01]
            fixed = f + rotate_bytes(f, -1) + rotate_bytes(f, -2) + rotate_bytes(f, -3)
            multiplied = [None] * len(matrix)
            for j in range(0, len(matrix)):
                multiplied[j] = reduce(lambda x, y: x ^ y, [gmul(m, f) for m, f in zip(matrix[j - (j % 4):j - (j % 4) + 4], fixed[4 * (j % 4):4 * (j % 4) + 4])])
            matrix = multiplied
            print(f"\nColumns mixed: {hex_mat(matrix)}")

        matrix = xor(matrix, round_keys[i])
        print(f"\nKey added: {hex_mat(matrix)}")

    # Шаг 3: результат работы алгоритма
    print(f"\n\nResult: {hex_mat(matrix)}")

    # Шаг 7: проверка
    print(f"\n\n\nDecrypting {matrix}...")
    for i in range(9, -1, -1):
        print(f"\n\nRound: {9 - i}")

        matrix = xor(matrix, round_keys[i])
        print(f"\nKey added: {hex_mat(matrix)}")

        if i < 9:
            f = [0x0e, 0x0b, 0x0d, 0x09]
            fixed = f + rotate_bytes(f, -1) + rotate_bytes(f, -2) + rotate_bytes(f, -3)
            multiplied = [None] * len(matrix)
            for j in range(0, len(matrix)):
                multiplied[j] = reduce(lambda x, y: x ^ y, [gmul(m, f) for m, f in zip(matrix[j - (j % 4):j - (j % 4) + 4], fixed[4 * (j % 4):4 * (j % 4) + 4])])
            matrix = multiplied
            print(f"\nColumns mixed: {hex_mat(matrix)}")

        for j in range(0, 4):
            matrix[4 - j::4] = rotate_bytes(matrix[4 - j::4], j)
        print(f"\nRows shifted: {hex_mat(matrix)}")

        matrix = unsubstitute(matrix)
        print(f"\nBytes substituted: {hex_mat(matrix)}")

    matrix = xor(matrix, key)
    print(f"\n\nMatrix XORed with initial key: {hex_mat(matrix)}\n")

    print(f"\nDecoded string value: '{bytearray(matrix).decode('ascii')}'")

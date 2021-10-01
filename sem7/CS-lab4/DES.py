import json
from functools import reduce


def get_bit(number, bit):
    return 1 & (number >> bit)


def rearrange_bits_by_matrix(number, matrix):
    return reduce(lambda a, i: (a << 1) | get_bit(number, i - 1), matrix, 0)


def split(number, mid_bit, parts):
    return reduce(lambda a, i: a + [(number >> (i * mid_bit)) & ((1 << mid_bit) - 1)], reversed(range(0, parts)), [])


def merge(numbers, mid_bit):
    return reduce(lambda a, i: (a << mid_bit) | (i & ((1 << mid_bit) - 1)), numbers, 0)


def cyclic_shift(number, bits, limit):
    return ((number << bits) | ((number << bits) >> limit)) & ((1 << limit) - 1)


def prepare_data_from_string(string, length):
    print(f"Preparing string: '{string}'")
    bts = bytearray(string, 'ascii')[:length // 8]
    print(" ".join(string), " ".join(str(i) for i in bts), " ".join(format(i, '08b') for i in bts), sep='\n')
    output = reduce(lambda a, i: (a << 8) + i, bts, 0)
    print(format(output, f'0{length}b'), output, "", sep='\n')
    return output


def f(value, key, tables, log):
    if log: print("Expanding and XORing with key R part:\n", format(value, '032b'), sep='\n')
    expanded_value = rearrange_bits_by_matrix(value, tables["e"])
    if log: print("\nexpanded to\n", format(expanded_value, '048b'), sep='\n')
    xor = expanded_value ^ key
    b_parts = split(xor, 6, 8)
    if log: print("\nXORed to\n", format(xor, '048b'), " ".join(format(i, '06b') for i in b_parts), "", sep='\n')
    nums = [None] * 8
    if log: print("Applying S boxes:\n", " ".join(format(i, '06b') for i in b_parts), sep='\n')
    for index, part in enumerate(b_parts):
        row = (get_bit(part, 5) << 1) | (get_bit(part, 0))
        col = reduce(lambda a, b: (a << 1) | b, [get_bit(part, i) for i in reversed(range(1, 5))], 0)
        nums[index] = tables["s"][index][row * 16 + col]
    if log: print("\nchanged to\n", " ".join(format(i, '04b') for i in nums), sep='\n')
    pre_result = merge(nums, 4)
    if log: print("\nconcatenated to\n", format(pre_result, '032b'), "", sep='\n')
    if log: print("R permutation:\n", format(pre_result, '032b'), sep='\n')
    result = rearrange_bits_by_matrix(pre_result, tables["p"])
    if log: print("\nchanged to\n", format(result, '032b'), sep='\n')
    return result


def compare(check, real):
    return "correct" if check == real else "wrong"


# Визуализация алгоритма DES
if __name__ == '__main__':
    with open("DES_tables.json") as DES_tables:
        tables = json.load(DES_tables)

    print("Input text: first and last names, 64bit")
    plain_text = prepare_data_from_string("Name_Surname", 64)
    print("Key: uni number and fathers name first letter, 56bit")
    key = prepare_data_from_string("123456L", 56)

    # Шаг 1: начальная перестановка данных
    print("Input text initial permutation:\n", format(plain_text, '064b'), sep='\n')
    prepared_text = rearrange_bits_by_matrix(plain_text, tables['ip'])
    print("\nis changed to\n", format(prepared_text, '064b'), sep='\n')

    # Шаг 2: начальная перестановка ключа
    print("Key permuted choice:\n", format(key, '056b'), sep='\n')
    prepared_key = rearrange_bits_by_matrix(key, tables["pc1"])
    print("\nis changed to\n", format(prepared_key, '056b'), sep='\n')

    # Шаг 3: разделение и циклический сдвиг ключа
    print("Key separation:\n", format(prepared_key, '056b'), sep='\n')
    key_l, key_r = split(prepared_key, 28, 2)
    print("\nis split to\n", f"{format(key_l, '028b')} {format(key_r, '028b')}", "\n", sep='\n')

    shifted_keys = {}
    print("Key shift (there are only two shift options, by 1 bit and by 2 bits):\n")
    for i in set(tables["sls"]):
        print(f"{format(key_l, '028b')} {format(key_r, '028b')}", f"\nis shifted by {i} to\n", sep='\n')
        sh_key_l = cyclic_shift(key_l, i, 28)
        sh_key_r = cyclic_shift(key_r, i, 28)
        print(f"{format(sh_key_l, '028b')} {format(sh_key_r, '028b')}", "", sep='\n')
        shifted_keys[i] = (sh_key_l, sh_key_r)

    # Шаг 4: генерация раундовых ключей
    print("Key generation:\n")
    keys = [None] * 16

    for i in range(0, 16):
        print(f"Generating key for round {i}...")
        pre_key = merge([shifted_keys[tables["sls"][i]][0], shifted_keys[tables["sls"][i]][1]], 28)
        print("\nPermuted choice:\n", format(pre_key, '056b'), sep='\n')
        keys[i] = rearrange_bits_by_matrix(pre_key, tables["pc2"])
        print("\nis changed to\n", format(keys[i], '048b'), "\n", sep='\n')

    # Шаг 5: раунды шифрования
    print("DES main body:\n")
    L, R = split(prepared_text, 32, 2)

    for i in range(0, 16):
        print(f"Round {i}...\n", f"L: {format(L, '032b')}, R: {format(R, '032b')}", "", sep='\n')
        old_R = R
        R = f(R, keys[i], tables, True) ^ L
        L = old_R
        print("", f"L: {format(L, '032b')}, R: {format(R, '032b')}", "\n", sep='\n')

    print("Final swap:\n")
    L, R = R, L
    print(f"L: {format(L, '032b')}, R: {format(R, '032b')}")
    output_text = merge([L, R], 32)
    print("", format(output_text, '064b'), sep='\n')

    # Шаг 6: финальная перестановка данных
    print("Input text final permutation:\n", format(output_text, '064b'), sep='\n')
    cipher_text = rearrange_bits_by_matrix(output_text, tables['ip-1'])
    print("\nis changed to\n", format(cipher_text, '064b'), "", sep='\n')
    print("Result:\n", format(cipher_text, '064b'), cipher_text, sep='\n')

    # Шаг 7: проверка
    print("DES result:", format(cipher_text, '064b'))
    ch_output_text = rearrange_bits_by_matrix(cipher_text, tables['ip'])
    print("Output text:", format(ch_output_text, '064b'), compare(ch_output_text, output_text), end='\n\n')

    ch_L, ch_R = split(ch_output_text, 32, 2)
    print(f"L: {format(ch_L, '032b')}, R: {format(ch_R, '032b')}", end='\n\n')
    ch_L, ch_R = ch_R, ch_L

    for i in range(0, 16):
        old_L = ch_L
        ch_L = f(ch_L, keys[15 - i], tables, False) ^ ch_R
        ch_R = old_L
        print(f"Round {i}: L = {format(ch_L, '032b')}, R = {format(ch_R, '032b')}")

    ch_prepared_text = merge([ch_L, ch_R], 32)
    print(f"\nPrepared text: {format(ch_prepared_text, '064b')}", compare(ch_prepared_text, prepared_text), end='\n\n')

    ch_plain_text = rearrange_bits_by_matrix(ch_prepared_text, tables['ip-1'])
    print("Plain text:", format(ch_plain_text, '064b'), compare(ch_plain_text, plain_text), end='\n\n')

    ch_letters = bytearray(split(ch_plain_text, 8, 8))
    print(f"Decoded string value: '{ch_letters.decode('ascii')}'")

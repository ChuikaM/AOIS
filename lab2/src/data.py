VECTORS_TABLE = {
    1:  [0,1,0,0,1,1,0,1,0,0,0,0,1,0,1,0,1,0,0,0],
    2:  [0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0],
    3:  [1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1],
    4:  [1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0],
    5:  [1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0],
    6:  [1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0],
    7:  [0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1],
    8:  [1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1],
    9:  [0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0],
    10: [0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0],
    11: [0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0],
    12: [1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1]
}

def flip_bits(vec, k):
    noisy = vec.copy()
    for i in range(min(k, len(vec))):
        noisy[i] = 1 - noisy[i]
    return noisy

def fmt(v): 
    return "[" + " ".join(map(str, v)) + "]"

def test_max_bits(patterns, recall_func):
    results = []
    for p in patterns:
        max_k = 0
        for k in range(1, len(p) + 1):
            if recall_func(flip_bits(p, k)) == p:
                max_k = k
            else:
                break
        results.append(max_k)
    return results

def test_max_bits_bam(patterns_in, patterns_out, recall_func):
    results = []
    for p_in, p_out in zip(patterns_in, patterns_out):
        max_k = 0
        for k in range(1, len(p_in) + 1):
            noisy = flip_bits(p_in, k)
            if recall_func(noisy) == p_out:
                max_k = k
            else:
                break
        results.append(max_k)
    return results
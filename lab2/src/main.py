import numpy as np
from hopfield import HopfieldNetwork
from hamming import HammingNetwork
from bam import BAMNetwork
from data import VECTORS_TABLE, flip_bits, fmt, test_max_bits, test_max_bits_bam
from tables import (
    print_dominance_table,
    print_hamming_accuracy_table,
    print_dominance_table_simple
)

if __name__ == "__main__":
    np.random.seed(42)
    N, M = 13, 13
    IDS = [3, 8, 4, 9]
    
    full_vecs = [VECTORS_TABLE[i][:20] for i in IDS]
    hop_patterns = full_vecs.copy()
    ham_patterns = full_vecs.copy()
    bam_x = [v[:N] for v in full_vecs]
    bam_y = [v[-M:] for v in full_vecs]

    print("=== Сеть Хопфилда ===")
    print("1. Source vectors:")
    for i, p in enumerate(hop_patterns):
        print(f"   y{i+1}={fmt(p)}")

    hop = HopfieldNetwork(20)
    hop.train(hop_patterns)
    
    for i, p in enumerate(hop_patterns):
        print(f"\n2. Async example for y{i+1}:")
        print(f"   y_original={fmt(p)}")
        _, stages = hop.predict_async(p, max_cycles=10)
        for stage_idx, updates in enumerate(stages, 1):
            print(f"Stage {stage_idx}:")
            for idx, state in updates:
                parts = [f"({v})" if k == idx else str(v) for k, v in enumerate(state)]
                print(f"   y_model ({idx+1}) = [{ ' '.join(parts) }]")
            final_state = updates[-1][1]
            if np.array_equal(final_state, p):
                print(f"   y_stage_{stage_idx} == y_original -> relaxation, correct")
                break
            else:
                if stage_idx == 10:
                    print(f"   y_stage_{stage_idx} != y_original -> no relaxation (max 10 tries)")
                else:
                    print(f"   y_stage_{stage_idx} != y_original")

        print(f"\n3. Sync example for y{i+1}:")
        print(f"   y_original={fmt(p)}")
        _, history_s = hop.predict_sync(p)
        for step_idx, state in enumerate(history_s[1:], 1):
            print(f"   Stage {step_idx}: y_model({step_idx})={fmt(state)}")
        match_s = (history_s[-1] == p)
        print(f"   y_stage_{len(history_s)-1} == y_original -> {'relaxation, correct' if match_s else 'relaxation, incorrect'}")

    print("\n4. Maximum number of recognised noisy bits:")
    max_async = test_max_bits(hop_patterns, lambda x: hop.predict_async(x, max_cycles=10)[0])
    max_sync = test_max_bits(hop_patterns, lambda x: hop.predict_sync(x)[0])
    print(f"   Async: {', '.join(f'y_{i+1}= {m}' for i, m in enumerate(max_async))}")
    print(f"   Sync:  {', '.join(f'y_{i+1}= {m}' for i, m in enumerate(max_sync))}")

    noisy_positions_list = [list(range(20)) for _ in hop_patterns]
    print_dominance_table(hop, hop_patterns, noisy_positions_list)

    print("\n=== Сеть Хэмминга ===")
    print("1. Source vectors:")
    for i, p in enumerate(ham_patterns):
        print(f"   y{i+1}={fmt(p)}")

    ham = HammingNetwork(20, len(ham_patterns), epsilon=0.3)
    ham.train(ham_patterns)
    
    print("2. Recognition test:")
    for i, p in enumerate(ham_patterns):
        res, hz = ham.predict(p)
        print(f"   y_original={fmt(p)}")
        for idx, z_state in enumerate(hz, 1):
            z_str = "[" + " ".join(f"{v:.2f}" if isinstance(v, float) else str(v) for v in z_state) + "]"
            tag = "# after activate" if idx < len(hz) else "# after activate, only 1 neuron > 0"
            print(f"   winner({idx})={z_str} {tag}")
        print(f"   y_model({len(hz)})={fmt(res)}")
        print(f"   y_model({len(hz)})== y_original-> {'correct' if res==p else 'incorrect'}\n")

    print("3. Maximum number of recognised noisy bits:")
    max_ham = test_max_bits(ham_patterns, lambda x: ham.predict(x)[0])
    print("\n".join(f"   y_{i+1}= {m}" for i, m in enumerate(max_ham)))
    
    print_hamming_accuracy_table(ham)

    print("\n=== Двунаправленная ассоциативная память (BAM) ===")
    print("1. Source vectors:")
    for i in range(len(bam_x)):
        print(f"   x{i+1}={fmt(bam_x[i])}; y{i+1}={fmt(bam_y[i])}")

    bam = BAMNetwork(N, M)
    bam.train(list(zip(bam_x, bam_y)))
    
    for i in range(len(bam_x)):
        print(f"\n2. Example for x{i+1}:")
        print(f"   x_original={fmt(bam_x[i])}")
        rx, ry = bam.recall_from_x(bam_x[i])
        print(f"   Stage 1: y_model(1)={fmt(ry)}")
        print(f"   x_model(1)={fmt(rx)}")
        print(f"   x_model(1)== x_original-> {'relaxation, correct' if rx==bam_x[i] else 'incorrect'}")

        print(f"\n3. Example for y{i+1}:")
        print(f"   y_original={fmt(bam_y[i])}")
        ry2, rx2 = bam.recall_from_y(bam_y[i])
        print(f"   Stage 1: x_model(1)={fmt(rx2)}")
        print(f"   y_model(1)={fmt(ry2)}")
        print(f"   y_model(1)== y_original-> {'relaxation, correct' if ry2==bam_y[i] else 'incorrect'}")

    print("\n4. Maximum number of recognised noisy bits:")
    max_bam_x_to_y = test_max_bits_bam(bam_x, bam_y, lambda x: bam.recall_from_x(x)[1])
    max_bam_y_to_x = test_max_bits_bam(bam_y, bam_x, lambda y: bam.recall_from_y(y)[0])
    
    print("   Y direction (X->Y recovery):")
    print("\n".join(f"   y_{i+1}= {m}" for i, m in enumerate(max_bam_x_to_y)))
    print("   X direction (Y->X recovery):")
    print("\n".join(f"   x_{i+1}= {m}" for i, m in enumerate(max_bam_y_to_x)))
    
    print_dominance_table_simple(max_bam_x_to_y, max_bam_y_to_x)
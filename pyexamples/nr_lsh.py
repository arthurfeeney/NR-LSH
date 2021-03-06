#
# This defines the 'real' package to include.
# includes better helper stuff than the bindings.
#

import numpy as np
import nr_binding


class float32(np.float32):
    pass


class float64(np.float64):
    pass


_valid_float32 = [float32, np.float32, 'float32']
_valid_float64 = [float64, np.float64, 'float64']


def simplelsh(bits, dim):
    return nr_binding.SimpleLSH(bits, dim)


def same_bits(n1, n2, bits):
    return nr_binding.same_bits(n1, n2, bits)


def sizes_from_probs(n_to_insert, p1, p2):
    return nr_binding.sizes_from_probs(n_to_insert, p1, p2)


def lsh_table(num_tables, bits, dim, num_buckets, dtype=float32):
    if any([dtype is f for f in _valid_float32]):
        if num_tables == 1:
            return nr_binding.LSHProbeFloat(bits, dim, num_buckets)
        else:
            return nr_binding.LSHMultiTableProbeFloat(num_tables, bits, dim,
                                                      num_buckets)
    elif any([dtype is f for f in _valid_float64]):
        return nr_binding.LSHProbeDouble(bits, dim, num_buckets)
    else:
        raise ValueError('multiprobe', 'invalid dtype', 'dtype=' + str(dtype))


def multiprobe(num_tables,
               num_partitions,
               bits,
               dim,
               num_buckets,
               dtype=float32):
    if any([dtype is f for f in _valid_float32]):
        return nr_binding.MultiProbeFloat(num_tables, num_partitions, bits,
                                          dim, num_buckets)
    elif any([dtype is f for f in _valid_float64]):
        return nr_binding.MultiProbeDouble(num_tables, num_partitions, bits,
                                           dim, num_buckets)
    else:
        raise ValueError('multiprobe', 'invalid dtype', 'dtype=' + str(dtype))

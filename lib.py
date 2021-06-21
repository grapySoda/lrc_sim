header = """fio version 2 iolog
%s add
%s open
"""
trailer = """
%s close"""

# def gen_trace(trace, device, offsets, size=4096):
def gen_trace(trace, device, offsets, size=3145728):
    f = open('%s.trace' % (trace,), 'w')
    f.write('\n'.join(["%d, %d, %d, %d, %d" % (0, 0, 0, size, o) for o in offsets]))
    f.close()

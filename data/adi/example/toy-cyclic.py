__author__ = 'arenduchintala'
import fst


def make_segmenter(phrases, sym_f):
    individual_tokens = set([])
    segmenter = fst.Transducer(sym_f, sym_f)
    segmenter[0].final = True
    s = 0
    e = 1
    for fp in phrases:
        if len(fp) > 1:
            fc = '_'.join(fp)
            for idx, fw in enumerate(fp):
                individual_tokens.add(fw)
                if idx == 0:
                    segmenter.add_arc(0, e, fw, fst.EPSILON, 0.0)
                else:
                    segmenter.add_arc(s, e, fw, fst.EPSILON, 0.0)
                s = e
                e += 1
            segmenter.add_arc(s, 0, fst.EPSILON, fc, -1.0)
        else:
            fw = fp[0]
            individual_tokens.add(fw)
    # adding individual tokens
    for idt in individual_tokens:
        # make self loops with single tokens
        segmenter.add_arc(0, 0, idt, idt, -1.0)
    return segmenter


sym = fst.SymbolTable()
for r in open('example/toy.sym', 'r').readlines():
    [s, wt] = r.split()
    sym[s]
sym.write('example/toy.bin')
phrases_f = [tuple(l.split('|||')[0].split()) for l in open('example/tm-toy', 'r').readlines()]
phrases_f = set(phrases_f)
seg = make_segmenter(phrases_f, sym)
seg.write('example/seg-toy.fst', sym, sym)
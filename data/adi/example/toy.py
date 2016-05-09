__author__ = 'arenduchintala'
import fst
import os
import pdb


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


def make_acyclic_segmenter(phrases, sym_f):
    individual_tokens = set([])
    segmenter = None  # fst.Transducer(sym_f, sym_f)
    #segmenter[0].final = True
    finals = []
    for fp, fc in phrases:
        ofpl = len(fp)
        fp = fp + [fst.EPSILON] * len(fc)
        fc = [fst.EPSILON] * ofpl + fc
        subseg = fst.LogTransducer(sym_f, sym_f)
        for idx, fw in enumerate(zip(fp, fc)):
            subseg.add_arc(idx, idx + 1, fw[0], fw[1], 0.0)
            fin = idx + 1
        subseg[fin].final = True
        if segmenter is None:
            segmenter = subseg
        else:
            segmenter = segmenter.union(subseg)
            segmenter.remove_epsilon()
            # segmenter.determinize()
            # segmenter.minimize()i
    ns = 0
    for s in segmenter.states:
        if s.final:
            finals.append(ns)
            print finals
            s.final = False
        ns += 1
    #pdb.set_trace()
    segmenter.add_state()
    for of in finals:
        segmenter.add_arc(of, ns , 0, 0, 0.0)
    segmenter[ns].final = True
    print ns, finals
    return segmenter


sym = fst.SymbolTable()
for r in open('toy.sym', 'r').readlines():
    [s, wt] = r.split()
    sym[s]
sym.write('toy.bin')
phrases_f = [(l.split('|||')[0].split(), l.split('|||')[1].split())
             for l in open('tm-toy', 'r').readlines()]
seg = make_acyclic_segmenter(phrases_f, sym)
seg.write('seg-toy.fst', sym, sym)
os.system(
    'fstrmepsilon seg-toy.fst | fstdeterminize | fstminimize |fstrmepsilon >seg-min.fst')

import os
import sys

from distutils.core import setup, Extension
# from setuptools import setup, Extension
 
setup(name = "konlp",
        version = "1.0",
        
        py_modules=["konlp.detector.detector", "konlp.ngram.ngram", "konlp.pos.index", "konlp.wordcount.wordcount"],
        ext_modules = [Extension("konlp.c.codescan", extra_compile_args=["-w"], sources=["konlp/detector/pydetector.c"]),
        Extension("konlp.c.wordcount", extra_compile_args=["-w"], sources=["konlp/wordcount/pywordcount.c", "konlp/wordcount/wordcount.c", "konlp/wordcount/wordcount-main.c", "konlp/wordcount/wordcount-sort.c"]),
	Extension("konlp.c.index", extra_compile_args=["-Wall", "-w"], extra_link_args=["-L.", "-lindex"], sources=["konlp/pos/pyindex.c"])]
)


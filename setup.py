
from distutils.core import setup, Extension
 
setup(name = "konlp",
version = "1.0",
        py_modules=["konlp.detector.detector", "konlp.ngram.ngram"],
        ext_modules = [Extension("konlp.c.codescan", ["konlp/c_src/detector.c"]),
        Extension("konlp.wordcount", ["konlp/wordcount/wordcount.c", "konlp/wordcount/wordcount_.c", "konlp/wordcount/wordcount-main.c", "konlp/wordcount/wordcount-sort.c"])]
)
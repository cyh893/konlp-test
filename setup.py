
from distutils.core import setup, Extension
 
setup(name = "konlp",
version = "1.0",
        py_modules=["konlp.detector.detector"],
        ext_modules = [Extension("konlp.c.codescan", ["konlp/c_src/detector.c"]),
        ]
)
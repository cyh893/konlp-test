from distutils.core import setup, Extension
 
setup(name = "konlp",
        py_module=["konlp.detector.detector"],
        ext_modules = [Extension("konltk.c.codescan", ["konlp/c_src/detector.c"]),
        ]
)
import konlp.c.index
import os

class Index:
    def __init__(self, path = ""):
        self.dic_init()
        self.index_list = []
        self.input = ""

    def dic_init(self, path = ""):
        if path == "":
            kdic_path = os.environ['KLT_DIC']
        else:
            kdic_path = path
        
        rc = konlp.c.index.init(kdic_path)
    
    def set_data(self, data = ""):
        self.input == data

    def index(self, data = ""):
        self.set_data(data)

        self.index_list = konlp.c.index.index(data)

        return self.index_list
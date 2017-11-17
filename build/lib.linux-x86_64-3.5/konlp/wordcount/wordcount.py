import konlp.c.wordcount
import os

class WordCount:
    def __init__(self):#, input = ""):
        # self.set_input(input)
        self.result_list = []

    # def set_input(self, input = ""):
    #     self.input = input

    def wordcount(self, input = ""):
        # if input != "":
        #     self.input = input
        self.input = input

        self.result_list = konlp.c.wordcount.wordcount(self.input, 1)

        return self.result_list
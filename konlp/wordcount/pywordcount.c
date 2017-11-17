#include "../c_src/header.h"
#include "wordcount.h"


#define TEMP    "wrodcount_temp"
#define OUTPUT  "wordcount_output"

#define TY_TEXT 1
#define TY_FILE 2

int checkInputType(char *input){
    int file_name_len = 100;
    int type = TY_TEXT;
    int i = 0;
    FILE *input_file;

    while(file_name_len--){
        if(input[i++] != ' ') break;
    }

    if(file_name_len){
        input_file = fopen(input, "r");
        if(input_file == NULL) type = TY_TEXT;
        else    type = TY_FILE;
    }
    return type;
}


static PyObject* wordcount(PyObject* self, PyObject*  args){
    int is_add = 0, n_uniqwords, n_words;
    char* input, *output_file = OUTPUT;
    PyObject *dlist = PyDict_New();
    PyObject *result_list = PyList_New(0);
    int i;
    FILE* temp_output_file;
    WORDCNT_STR Text={0}, *wp=&Text;


    if(!PyArg_ParseTuple(args, "si", &input, &is_add))
        return NULL;

    if(checkInputType(input) == TY_TEXT){
        FILE *temp_intput_file = fopen(TEMP, "w");
        fprintf(temp_intput_file, "%s", input);
        fclose(temp_intput_file);
        input = TEMP;
    }
    
    if(is_add)
        delete_accumulated_data();
    
    n_uniqwords = word_count(input, wp, 0/*word unit*/, 0/*dont show dot*/);
    /*n_words = put_wordlist_sidx(OUTPUT, wp, 0);	// sorting index를 이용하여 출력 -- put_wordlist()와 동일한 기능임!
    temp_output_file = fopen(OUTPUT, "r");
    while(1){
        char msg[1000];    
        int num = -1;
        fscanf(temp_output_file, "%d %s", &num, msg);
        if(feof(temp_output_file))  break;
    
        // PyList_Append(dlist, Py_BuildValue("(is)", num, msg));
        PyObject *key = Py_BuildValue("s", msg), *value = Py_BuildValue("i", num);
        PyDict_SetItem(dlist, key, value);
    }*/
    for(i = 0; i < wp->n_uniqwords; i++){
        int temp = wp->sidx[i];
        PyObject *obj = PyTuple_New(2);
        PyTuple_SET_ITEM(obj, 0, PY_STRING(wp->words[temp]));
        PyTuple_SET_ITEM(obj, 1, PY_INT(wp->count[temp]));
        PyList_Append(result_list, obj);
    }    

    return result_list;
}

//methods list
static struct PyMethodDef methods[] ={
    {"wordcount", wordcount, METH_VARARGS},

    {NULL, NULL}
};

//main
MOD_INIT(wordcount){
    PyObject* m;

    MOD_DEF(m, "konlp.c.wordcount", "wordcount c code", methods);

    
    if(m == NULL)
        return MOD_ERROR_VAL;
    
    return MOD_SUCCESS_VAL(m);
}
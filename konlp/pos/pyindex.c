#include <Python.h>
#include <string.h>
#include "index_header/ham-ndx.h"
#include "../c_src/header.h"

extern int get_sent_or_linse();

void error_message(mode)	/* cannot load dictionary files */
HAM_RUNMODE mode;
{
	fprintf(stderr, "ERROR: cannot load dictionary files in <%s>. Error code %d in <header/err-code.h>\n",
		mode.dicpath, mode.err_code);
	fprintf(stderr, "Directory path for dictionaries should be correctly positioned. Or\n");
	fprintf(stderr, "you should modify <src/index.c> and <%s>.\n", INI_FILENAME);
	fprintf(stderr, "1. Check or modify the value <DicDirPath> in <%s>.\n", INI_FILENAME);
	fprintf(stderr, "2. Check or modify <src/index.c> --> function open_HAM() --> argument <%s>\n", INI_FILENAME);
}

/*	Output test for 'get_stems()'.
		*(term[i]-2) --- word id. at a sentence: 0~255
		*(term[i]-1) --- term type(defined in 'header/keyword.h')
		term[i]      --- term string
*/
void put_stems(sent, term, n, mode, result_list)
unsigned char *sent;	/* input sentence */
unsigned char *term[];	/* list of keywords */
int n;	/* number of keywords at 'keywords[]' */
HAM_RUNMODE *mode;	/* HAM running mode */
PyObject *result_list;
{
	int i;
	PyObject *obj = PyTuple_New(2);
	PyObject *indexobj = PyTuple_New(n);
	PyTuple_SET_ITEM(obj, 0, PY_STRING(sent));

	for (i = 0; i < n; i++) {
		PyTuple_SET_ITEM(indexobj, i, PY_STRING(term[i]));
		// PyList_Append(result_list, PY_STRING(term[i]));
		
	}
	PyTuple_SET_ITEM(obj, 1, indexobj);
	PyList_Append(result_list, obj);

}


/* 'KeyMem', 'HamOut' is needed ONLY for THREAD-SAFE running */
unsigned char KeyMem[MAXKEYBYTES];	/* TEMPORARY MEMORY: keyword string */
HAM_MORES HamOut;	/* TEMPORARY MEMORY: morph. analysis result */


int check_file(char *input, int file_max_len){
    int file_name_len = file_max_len;
    int type = 0;//0 is text;
    int i = 0;
    FILE *input_file;

    while(file_name_len--){
        if(input[i++] != ' ') break;
    }
    if(file_name_len){
		
        input_file = fopen(input, "r");
        if(input_file == NULL ) type = 0;
        else                    type = 1;
    }

    return type;
}


PyObject *version(PyObject *self, PyObject *args){
    PyObject *result = NULL;
	const char *rs = "KMU KLT2 INDEX";
	int32_t ret = 0;//version(rs);
	if (ret != 0)
		rs = "error, failed to execute version().";

	const char *out = rs;
	result = Py_BuildValue ("s", out);

	return result;
}

HAM_RUNMODE mode;


PyObject *init (PyObject *self, PyObject *args)
{
	char *sdir = 0x00;
	/* python->c 함수 인자값을 파싱하여 얻어냄 */
	if (PyArg_ParseTuple (args, "s", &sdir))
	{
        char tmppath[1024];
        char kltpath[1024];
	strcpy(tmppath, sdir);
	//
	strcpy(kltpath, sdir);
        if(tmppath[strlen(tmppath) - 1] != '/'){
			strcat(tmppath, "/");
			strcat(kltpath, "/");
		}
        strcat(tmppath, "hdic/");
	strcat(kltpath, "hdic/KLT2000.ini");	
	    
	//printf("\n---------\n%s\n%s\n-------\n", kltpath, tmppath);

    int rc = open_HAM_index_dicpath(&mode, 0x00, kltpath, tmppath, "ham-usr.dic", "ham-cnn.dic", "stopword.dic");
	
	if (rc != 0) {
		printf ("[ERR] failed to init ham module from '%s'. (error code = %d)\n", sdir, rc);
		return Py_BuildValue ("i", -101);
	}
	
		/* c->pyhton 리턴값을 생성함 */
		return Py_BuildValue ("i", 0);
	}

	return 0x00;
}

PyObject *_index(PyObject *self, PyObject *args){
    FILE *fpin;
	char *otpstr = NULL;

	char *input;
    unsigned char sent[SENTSIZE] = "";	/* input word or sentence */
	int n;	/* number of extracted terms */
    unsigned char *term[MAXKEYWORDS];	/* extracted terms */
    PyObject *result_list = PyList_New(0);

    // const char *sent = 0x00;
    if (!PyArg_ParseTuple (args, "s", &input))
        return NULL;

    if(check_file(input, 100)){
		fpin = fopen(input, "r");
		//printf("1\n");
		//PyList_Append(result_list, PY_STRING("qwe"));
        while (fscanf(fpin, "%s", sent)) {	if (feof(fpin)) break;
            sent[SENTSIZE-1] = '\0';
    	
            n = get_stems(sent, term, &mode);	/* It is NOT thread-safe!!! */
    	
            put_stems(sent, term, n, &mode, result_list);
        }

        if (fpin != stdin) fclose(fpin);
    }else{
		char *ptr = strtok(input, " ");
		// unsigned char *ptr = malloc(sizeof(unsigned char) * SENTSIZE);
		sprintf(sent, "%s", ptr);
		
		while(ptr != NULL){			
			sent[SENTSIZE-1] = '\0';
			
			n = get_stems(sent, term, &mode);	/* It is NOT thread-safe!!! */
		
			put_stems(sent, term, n, &mode, result_list);

			ptr = strtok(NULL, " ");
			sprintf(sent, "%s", ptr);
		}
    }
	
    // close_HAM_index(&mode);
	
	return result_list;
}

static PyMethodDef methods[] ={
	{"version", version, METH_VARARGS},
	{"init", init, METH_VARARGS},
	{"index", _index, METH_VARARGS},

	{NULL, NULL}
};

MOD_INIT(index){
	PyObject *m;
	MOD_DEF(m, "konlp.c.index", "", methods);
}

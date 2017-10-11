#include "header.h"

typedef enum { NONE, EUCKR, UTF8, UTF16BE, UTF16LE } CODETYPE;

char *text;
int len;

int range_euc[][4] = {{0xb0, 0xc8, 0xa1, 0xfe},
{0xca, 0xfd, 0xa1, 0xfe},
{0xa1, 0xa1, 0xa1, 0xfe},
{0xa2, 0xa2, 0xa1, 0xe7},
{0xa3, 0xa4, 0xa1, 0xfe},
{0xa5, 0xa5, 0xa1, 0xaa},
{0xa5, 0xa5, 0xb0, 0xb9},
{0xa5, 0xa5, 0xc1, 0xcf},
{0xa5, 0xa5, 0xd0, 0xd8},
{0xa5, 0xa5, 0xe1, 0xef},
{0xa5, 0xa5, 0xf0, 0xf8},
{0xa6, 0xa6, 0xa1, 0xe4},
{0xa7, 0xa7, 0xa1, 0xef},
{0xa8, 0xa8, 0xa1, 0xa5},
{0xa8, 0xa8, 0xa6, 0xa6},
{0xa8, 0xa8, 0xa8, 0xaf},
{0xa8, 0xa8, 0xb1, 0xfe},
{0xa9, 0xa9, 0xa1, 0xfe},
{0xaa, 0xaa, 0xa1, 0xf3},
{0xab, 0xab, 0xa1, 0xf6},
{0xac, 0xac, 0xa1, 0xc1},
{0xac, 0xac, 0xd1, 0xf1}};

int range_cp949[][4] = {{0x81, 0xa0, 0x41, 0x5a},
{0x81, 0xa0, 0x61, 0x7a},
{0x81, 0xa0, 0x81, 0xfe},
{0xa1, 0xc5, 0x41, 0x5a},
{0xa1, 0xc5, 0x61, 0x7a},
{0xa1, 0xc5, 0x81, 0xa0},
{0xc6, 0xc6, 0x41, 0x52}};

int range_unicode[][2] = {{0xac00, 0xd7ac},
{0x3131, 0x318e},
{0x1100, 0x11ff},
{0x2000, 0x2bff},
{0x0080, 0x00ff},
{0x2e80, 0x2eff},
{0x2500, 0x25ff},
{0x3000, 0x303f},
{0x3130, 0x318f},
{0x31c0, 0x31ef},
{0x3200, 0x4dbf},
{0x4e00, 0x9fbf},
{0xf900, 0xfaff},
{0xfe30, 0xfe4f},
{0xff00, 0xffef}};

int euckr(int idx){
    int t1, t2;
    int r_len = 22;//sizeof(range_euc) / sizeof(range_euc[0]);
    int i;
    if(len - 2 < idx){
        return 0;
    }

    t1 = text[idx] & 0xff;
    t2 = text[idx + 1] & 0xff;

    for(i = 0; i < r_len; i++){
        if((range_euc[i][0] <= t1 && t1 <= range_euc[i][1]) && 
        (range_euc[i][2] <= t2 && t2 <= range_euc[i][3])){
            return 1;
        }
    }

    return 0;
}

int cp949(int idx){
    int t1, t2;
    int r_len = 7;//sizeof(range_cp949) / sizeof(range_cp949[0]);
    int i;
    if(len - 2 < idx){
        return 0;
    }

    t1 = text[idx] & 0xff;
    t2 = text[idx + 1] & 0xff;

    if(euckr(idx)){
        return 1;
    }

    for(i = 0; i < r_len; i++){
        if((range_cp949[i][0] <= t1 && t1 <= range_cp949[i][1]) && 
        (range_cp949[i][2] <= t2 && t2 <= range_cp949[i][3])){
            return 1;
        }
    }

    return 0;
}

int utf8(int idx){
    int t1, t2, t3;
    int r_len = sizeof(range_unicode) / sizeof(range_unicode[0]);
    int i;
    int uni;
    if(len - 3 < idx){
        return 0;
    }

    t1 = text[idx] & 0xff;
    t2 = text[idx + 1] & 0xff;
    t3 = text[idx + 2] & 0xff;

    uni = ((t1 & 0x0f) << 12) + ((t2 & 0x3f) << 6) + (t3 & 0x3f);

    for(i = 0; i < r_len; i++){
        if(range_unicode[i][0] <= uni && uni <= range_unicode[i][1])
            return 1;
    }
    return 0;
}

int utf16be(int idx){
    int t1, t2;
    int r_len = sizeof(range_unicode) / sizeof(range_unicode[0]);
    int i;
    int uni;
    if(len - 2 < idx){
        return 0;
    }

    t1 = text[idx] & 0xff;
    t2 = text[idx + 1] & 0xff;

    uni = (t1 << 8) + t2;

    for(i = 0; i < r_len; i++){
        if(range_unicode[i][0] <= uni && uni <= range_unicode[i][1])
            return 1;
    }
    return 0;
}
 
int utf16le(int idx){
    int t1, t2;
    int r_len = sizeof(range_unicode) / sizeof(range_unicode[0]);
    int i;
    int uni;
    if(len - 2 < idx){
        return 0;
    }

    t2 = text[idx] & 0xff;
    t1 = text[idx + 1] & 0xff;

    uni = (t1 << 8) + t2;

    for(i = 0; i < r_len; i++){
        if(range_unicode[i][0] <= uni && uni <= range_unicode[i][1])
            return 1;
    }
    return 0;
}

int _detect(int max){
    int cnt[5] = {0};
    int not_cnt[5] = {0};

    int i;

    for(i = 0; i < len - 1; i++){
        if (text[i] & 0x80) {
			if (cp949(i))
				cnt[EUCKR]++;
 			else    not_cnt[EUCKR]++;	// KSC5601 negative
			i++;
        }
        
        if (cnt[EUCKR] > max)   break;
    }

    for(i = 0; i < len - 2; i++){
        if (text[i] & 0x80){
            if (utf8(i)){
                cnt[UTF8]++;
                i++;
            }
            else    not_cnt[UTF8]++;
            i++;
        }

        if (cnt[UTF8] > max)   break;
    }

    for(i = 0; i < len - 1; i++){
        if (i == 0 &&	// check if BOM: 0xFEFF
			(text[0] & 0x00FF) == 0x00FE &&
			(text[1] & 0x00FF) == 0x00FF) {
			i++;
			continue;
        }
        
        if(/*(text[i] & 0x80) && */ utf16be(i)){
            cnt[UTF16BE]++;
            i++;
        }else if(text[i] == 0 && (text[i + 1] & 0x80) == 0){
            cnt[UTF16BE]++;
            i++;
        }else{
            not_cnt[UTF16BE] = 99;
            break;
        }

        if (cnt[UTF16BE] > max)   break;
    }

    for(i = 0; i < len - 1; i++){
        if (i == 0 &&	
			(text[0] & 0xFF) == 0xFF &&
			(text[1] & 0xFF) == 0xFE) {
			i++;
			continue;
        }
        
        if(/*(text[i + 1] & 0x80) && */ utf16le(i)){
            cnt[UTF16LE]++;
            i++;
        }else if(text[i + 1] == 0 && (text[i] & 0x80) == 0){
            cnt[UTF16LE]++;
            i++;
        }else{
            not_cnt[UTF16LE] = 99;
            break;
        }

        if (cnt[UTF16LE] > max)   break;
    }

    // for(i = 1; i < 5; i++){
    //     printf("%d %d\n", cnt[i], not_cnt[i]);    
    // }

    for(i = 1;i < 5; i++){
        if(cnt[i] && !not_cnt[i])   return i;
    }
	return NONE;
}

static PyObject* detect(PyObject* self, PyObject *args){
    int max;
    int result = 0;
    Py_ssize_t count;


    if(!PyArg_ParseTuple(args,"s#ii", &text, &count, &len, &max)){
        return NULL;
    }
    // printf("%s %d %d\n", text, len, max);

    result = _detect(max);


    return Py_BuildValue("i", result);
}

static struct PyMethodDef methods[] =
{
    {"detect", detect, METH_VARARGS},
    
    {NULL, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "codescan",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    methods
};

MOD_INIT(codescan){
    PyObject* m;

    MOD_DEF(m, "konlp.c.codescan", "detector c code", methods);

    if (m == NULL)
        return MOD_ERROR_VAL;

    return MOD_SUCCESS_VAL(m);
}
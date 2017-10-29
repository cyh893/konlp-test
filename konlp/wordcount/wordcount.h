#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WORD_FILE_NAME	"wordlist.txt"
#define LINK_FILE_NAME	"wordlist.dat"

typedef long	ULONG;	// unsigned �� �ϸ� �ȵ�!!!
typedef unsigned char	UCHAR;

typedef struct WordCount {
	ULONG n_words;		// text�� �� ���� ����
	ULONG n_uniqwords;	// �ߺ��� ������ ���� ����
	ULONG n_filesize;	// �Է� �ؽ�Ʈ ������ byte-size

	UCHAR *wordlist;	// null ���ڷ� ���е� list of words
	UCHAR *textFile;	// �Է� ���� ��ü�� ���⿡ load �� �Ŀ� �� ���� ���� null���ڷ� setting

	UCHAR **words;		// text�� �ִ� ���� ��Ʈ���� ���� ������
	ULONG *count;		// cnt[i] --> word count for word[i]
	ULONG root, *llink, *rlink;	// binary tree

	ULONG *sidx;		// index of sorting order for word[i] --> put_wordlist_2() ����� ���� �ʿ���!
} WORDCNT_STR, *P_WORDCNT_STR;

int is_file_exist(char *fname);

int load_words_and_links(char *fname_word, char *fname_link, WORDCNT_STR *wp);
int save_words_and_links(char *fname_word, char *fname_link, WORDCNT_STR *wp);
int word_count(char *infile, WORDCNT_STR *wp, int flag_lineinput, int flag_dots);
void free_memory(WORDCNT_STR *wp);

void put_wordlist(char *fname, WORDCNT_STR *wp, int flag);
int put_wordlist_sidx(char *fname, WORDCNT_STR *wp, int flag);

void sort_by_freq(ULONG sidx[], ULONG count[], ULONG n);
void sort_by_rev_string(WORDCNT_STR *wp, ULONG n);

void sort_by_freq2(ULONG sidx[], ULONG count[], ULONG n);
void sort_by_word2(UCHAR *words[], ULONG sidx[], ULONG n);
void merge(ULONG mid1, ULONG mid2, ULONG *sidx1, ULONG *sidx2, ULONG count[], ULONG sidx[]);
void merge2(ULONG mid1, ULONG mid2, ULONG *sidx1, ULONG *sidx2, UCHAR *words[], ULONG sidx[]);

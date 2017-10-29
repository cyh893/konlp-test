/*
	Word count program for unlimited size of input file.

	(c) Kang Seung-Shik, April 15, 2006

	Modified at 2014/09/12
		- memory_allocation() -- error�� �� ��Ҹ��� ���
		- dot ���÷��̸� 10������ 1����...(���� 1����)
*/
#include "wordcount.h"

/*=========================================================================================*/
static int N_bytesize=0;
static int n_sort_index=0;	// 'sidx[]' ������ ���� ����
/*=========================================================================================*/

int myisspace(UCHAR c) { return (c == ' ' || c == '\t' || c == 0x0d || c == 0x0a || c == '\'' || c == '\"'); }
int myispunct(UCHAR c) { return (c == '.' || c == ',' || c == '?' || c == '!' || c == '\'' || c == '\"'); }
int filesize(FILE *fp) { int n; fseek(fp, 0L, 2); n = ftell(fp); fseek(fp, 0L, 0); return n; }

// ���� 'fname'�� �̹� �����ϴ� �������� �˻�
int is_file_exist(char *fname)
{
	FILE *fp;

	fp = fopen(fname, "r");
	if (fp) {
		fclose(fp);
		return 1;
	} else return 0;
}

/*
	�Է� ������ memory�� load�Ͽ� �����͸� return
*/
UCHAR *load_text(char *filename)
{
	int n;
	UCHAR *p;

	FILE *fp=fopen(filename, "rb");
	if (!fp) return NULL;	// file open failed

	fseek(fp, 0L, 2);
	n = ftell(fp);	/* n: byte size of file 'fp' */

	p = (UCHAR *) malloc(n+100);	/* memory allocation */
	if (p == NULL) return NULL;

	memset(p, 0, n+100);
	fseek(fp, 0L, 0);
	fread((char *) p, sizeof(char), (int)n, fp);	/* read 'fp' to 'p' */
	*(p+n) = '\0';

	N_bytesize = n;
	fclose(fp);
	return (UCHAR *) p;
}

void free_memory(WORDCNT_STR *wp)
{
	if (wp->wordlist) free(wp->wordlist);
	if (wp->textFile) free(wp->textFile);

	if (wp->words) free(wp->words);
	if (wp->count) free(wp->count);

	if (wp->llink) free(wp->llink);
	if (wp->rlink) free(wp->rlink);

	if (wp->sidx) free(wp->sidx);
}

int memory_allocation(WORDCNT_STR *wp, ULONG n)
{
	wp->words = (UCHAR **) calloc(n, sizeof(UCHAR *));
	if (!wp->words) fprintf(stderr, "Error: memory allocation of wp->words\n");
	wp->count = (ULONG *) calloc(n, sizeof(ULONG));
	if (!wp->count) fprintf(stderr, "Error: memory allocation of wp->count\n");

	wp->llink = (ULONG *) calloc(n, sizeof(ULONG));
	if (!wp->llink) fprintf(stderr, "Error: memory allocation of wp->llink\n");
	wp->rlink = (ULONG *) calloc(n, sizeof(ULONG));
	if (!wp->rlink) fprintf(stderr, "Error: memory allocation of wp->rlink\n");

	wp->sidx = (ULONG *) calloc(n, sizeof(ULONG));	// Sorting Index 'sidx' memory allocation
	if (!wp->sidx) fprintf(stderr, "Error: memory allocation of wp->sidx\n");

	if (!wp->textFile || !wp->count || !wp->words ||
		!wp->llink || !wp->rlink || !wp->sidx) {
//		fprintf(stderr, "Error: memory allocation.");
//		free_memory(wp);
		return 0;
	} else return 1;
}

int flag_ignore_empty_line = 0;
/*
	�� ������ �ν��Ͽ� ���� ���� '\0'�� setting�ϰ�, ���� ������ count�Ͽ� return
*/
ULONG set_delimiter(UCHAR *text, int flag_lineinput, int flag_dots)
{
	ULONG n=0;
	UCHAR *p=text;

	while (*p && p-text < N_bytesize) {
		if (flag_lineinput) {	/* ���� ������ ó�� --> not ���� ���� */
			if (flag_ignore_empty_line) {	// 2013/07/02 -- ���½� ����: line count�� �� ���� ���ε� ����ϵ���...
				while (*p && (*p == 0x0D || *p == 0x0A)) p++;	// skip leading CR-LF
			}
//			while (*p && !(*p == 0x0D || *p == 0x0A)) p++;
			while (*p && !(*p == 0x0A)) p++;

			if (*p == 0) {	// ���ϳ��� LF ���� ���� ������ ���
				if (*(p-1)) n++;
				return n;
			}

			*p++ = '\0'; n++;
			if (flag_dots && (n % 100000 == 0)) fputc('.', stderr);	// ���� ��Ȳ ǥ��
		} else {
			while (*p && myisspace(*p)) p++;	// skip leading blanks
			while (*p && !myisspace(*p)) p++;
			while (*p && p>text && myispunct(*(p-1))) *p-- = ' '; // ignore punctuation marks

			if (p>text && !myisspace(*(p-1))) {
				*p++ = '\0'; n++;
				if (flag_dots && (n % 100000 == 0)) fputc('.', stderr);	// ���� ��Ȳ ǥ��
			} else {
				*p = ' ';
			}
		}
	}

	return n;
}

/*
	return value: root node
*/
int insert_word_or_count(ULONG nth, UCHAR *aWord, WORDCNT_STR *wp)
{
	int i=wp->root, n=wp->n_uniqwords, res;

	if (*aWord == '\0') return wp->root;
	if (wp->root == -1) wp->root = nth;

	while (i >= 0) {
		res = strcmp(aWord, wp->words[i]);
		if (!res) {		// �̹� �����ϴ� �� --> count�� ����
			wp->count[i]++;
			return wp->root;
		} else if (res < 0) {
			if (wp->llink[i] == -1) {
				wp->llink[i] = n;
				break;
			} else i = wp->llink[i];
		} else {
			if (wp->rlink[i] == -1) {
				wp->rlink[i] = n;
				break;
			} else i = wp->rlink[i];
		}
	}

	wp->words[n] = aWord;
	wp->count[n] = 1;
	wp->llink[n] = wp->rlink[n] = -1;	// llink, rlink �ʱ�ȭ
	wp->n_uniqwords = n+1;

	return wp->root;	// skewed tree�� ���� ó���� �ʿ���.
}

/*
	text�� �ִ� �� �����鿡 ���� word count

	return value: uniq word ����
*/
int sort_and_count(WORDCNT_STR *wp, int flag_lineinput, int flag_dots)
{
	int i;
	UCHAR *p=wp->textFile;
	
	// fprintf(stderr, "\nSorting and counting the frequencies.");
	printf("\nSorting and counting the frequencies.");

	printf("%lu\n", wp->n_words);
	for (i=0; i < /*wp->n_words*/ 7054; i++) {
		printf("%d ", i);
		if (flag_lineinput) {
			if (!flag_ignore_empty_line)	// 2013/07/02 -- ���½� ����: line count�� �� ���� ���ε� ����ϵ���...
				;
			else while (*p == 0x0d || *p == 0x0a) p++;
		} else {
			while (*p && myisspace(*p)) p++;
		}

		if (*p == 0xA1 && *(p+1) == 0xA1) p += 2;	// 2����Ʈ ���� ����


		wp->root = insert_word_or_count(i, p, wp);
		p += strlen((char*)p) + 1;

		if (flag_dots && (i % 10000 == 0)) printf(".");//fputc('.', stderr);		// ���� ��Ȳ ǥ��
	}
	// fputc('\n', stderr);
	printf("\n");

	return wp->n_uniqwords;
}

/*
	Tree traverse & set sorting index for each word
*/
void set_sorting_index(int node, WORDCNT_STR *wp)
{
	if (node == -1) return;

	set_sorting_index(wp->llink[node], wp);
	wp->sidx[n_sort_index++] = node;
	set_sorting_index(wp->rlink[node], wp);
}

/*
	word list�� count[](�󵵼�), llink[], rlink[] �� ���Ͽ� ����
*/
int save_words_and_links(char *fname_word, char *fname_link, WORDCNT_STR *wp)
{
	FILE *fp;
	ULONG i, n=wp->n_uniqwords;

	if (is_file_exist(fname_word)) {
		printf("Overwrite <%s> ? ", fname_word);
		if (getchar() == 'y') getchar();
		else return 0;
	}
	fp = fopen(fname_word, "wb");

	for (i=0; i < wp->n_uniqwords; i++)
		fprintf(fp, "%s\n", wp->words[i]);
	fclose(fp);	// �ܾ���� ���Ͽ� ���� �Ϸ�

	if (is_file_exist(fname_link)) {
		printf("Overwrite <%s> ? ", fname_link);
		if (getchar() == 'y') getchar();
		else return 0;
	}
	fp = fopen(fname_link, "wb");

	fwrite(wp->count, sizeof(ULONG), n, fp);	// �󵵼� ����
	fwrite(wp->llink, sizeof(ULONG), n, fp);	// llink ����
	fwrite(wp->rlink, sizeof(ULONG), n, fp);	// rlink ����
	fclose(fp);	// count[](�󵵼�), llink[], rlink[] �� ���Ͽ� ���� �Ϸ�

	fprintf(stderr, "\tFiles <%s> and <%s> were created.\n", fname_word, fname_link);
	return 1;
}

/*
	���� ���Ͽ��� ���� �� �󵵰��� ����� �о��.

		- save_words_and_link()�� ���� ����� ����� �о��.

	<����> dynamic memory�� �Ҵ�Ǿ����Ƿ� free ���Ѿ� �� --> �Լ� free_memory()
*/
int load_words_and_links(char *fname_word, char *fname_link, WORDCNT_STR *wp)
{
	FILE *fp;
	ULONG i, n;
	UCHAR *p;
	int flag_lineinput=1;	// ����� wordlist�� �� line�� 1�����̹Ƿ�(line count�� ��쿡)
	int flag_dots=0;	// �����Ȳ�� ... ���� ǥ��

	wp->root = 0;
	wp->wordlist = load_text(fname_word);
	if (wp->wordlist == NULL) return 0;
	wp->n_filesize = N_bytesize;	// input file byte-size

	wp->n_uniqwords = set_delimiter(wp->wordlist, flag_lineinput, flag_dots);
	fprintf(stderr, "\n\t--> %ld data(words or lines) are found in <%s>.\n", wp->n_uniqwords, fname_word);

	fp = fopen(fname_link, "rb"); if (!fp) return 0;
	n = filesize(fp) / sizeof(ULONG) / 3;	// table 3�� -- count[], llink[], rlink[]
	if (n != wp->n_uniqwords)
		fprintf(stderr, "Mismatch -- %ld words and %ld links\n", wp->n_words, n);

	// �Է� ������ �������� �߰��Ͽ� memory �Ҵ�
	if (!memory_allocation(wp, n + wp->n_words)) { fclose(fp); return 0; }
	fread(wp->count, sizeof(ULONG), n, fp);
	fread(wp->llink, sizeof(ULONG), n, fp);
	fread(wp->rlink, sizeof(ULONG), n, fp);
	fclose(fp);

	for (i=0, p=wp->wordlist; i < wp->n_uniqwords; i++) {
		wp->words[i] = p;
		p += strlen((char *)p) + 1;
	}

	n_sort_index = 0;
	set_sorting_index(wp->root, wp);	// Sorting Index 'sidx[]' �� setting
//	put_wordlist("TEMP.TXT", 0, wp);	// ���� ���� ���� Ȯ�� -- �׽�Ʈ ��

	return 1;
}

/*
	word count -- incremental version
	���� �� ��� ����� �Է� ���Ͽ� ���� �󵵸� ����Ͽ� �߰��� ��.

		- 'infile'�� word �Ǵ� line�� ���� �󵵼� ���
		- flag_lineinput -- 0: word unit, 1: line unit

  <����> dynamic memory�� �Ҵ�Ǿ����Ƿ� free ���Ѿ� �� --> �Լ� free_memory()
*/
int word_count(char *infile, WORDCNT_STR *wp, int flag_lineinput, int flag_dots)
{
	int n;

	if (flag_dots) fprintf(stderr, "\nLoading and setting delimiters for <%s>.", infile);
	wp->textFile = load_text(infile);
	if (wp->textFile == NULL) return 0;
	wp->n_filesize = N_bytesize;	// input file byte-size

	if (flag_dots) fprintf(stderr, "\nDisplaying dots for every 100,000 data.", infile);
	n = wp->n_words = set_delimiter(wp->textFile, flag_lineinput, flag_dots);
	fprintf(stderr, "\n\t--> %d data(words or lines) are found in <%s>.\n", n, infile);

	if (load_words_and_links(WORD_FILE_NAME, LINK_FILE_NAME, wp))
		;	// ���� word count ������ �о����
	else {
		wp->root = -1;	// root node �ʱ�ȭ
		wp->n_uniqwords = 0;
		// to do
		// if (!memory_allocation(wp, n)) return 0;
	}

	wp->n_uniqwords = sort_and_count(wp, flag_lineinput, flag_dots);	// �Է� ���Ͽ� ���� word count

	n_sort_index = 0;
	set_sorting_index(wp->root, wp);	// Sorting Index 'sidx[]' �� setting

	return wp->n_uniqwords;
}

/*
	���� �� ��� ����� �ݿ����� ���� old version.
	��, load_words_and_links()�� ������� �ʰ�, ���� �Է� ���Ͽ� ���ؼ��� �� ���.
*/
int word_count_OLD(char *infile, WORDCNT_STR *wp, int flag_lineinput)
{
	int n, flag_dots=1;

	fprintf(stderr, "\nLoading and setting delimiters for <%s>.", infile);
	wp->wordlist = load_text(infile);
	if (wp->textFile == NULL) return 0;

	wp->n_uniqwords = 0;
	n = wp->n_words = set_delimiter(wp->textFile, flag_lineinput, flag_dots);
	fprintf(stderr, "\n\t--> %d data(words or lines) are found in <%s>.\n", n, infile);

	if (!memory_allocation(wp, n)) return 0;
	wp->n_uniqwords = sort_and_count(wp, flag_lineinput, flag_dots);

	n_sort_index = 0;
	set_sorting_index(wp->root, wp);	// Sorting Index 'sidx[]' �� setting

	return wp->n_uniqwords;
}

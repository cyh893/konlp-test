/*
	�ܾ�/���� count ���α׷�

	- ��뷮 �ؽ�Ʈ ���Ͽ� ���� word/line count ���
	- ���� ���� �ؽ�Ʈ ������ coutn ����� �����Ͽ� �� �� count ���

	- 2012/04/18 ��������
		1) <��, �ܾ�> ���� �����ڸ� tab ���ڷ� ����(���� blank)
			���� ���Ͽ� ������ �� �� �κа� �ܾ�/���� �κ��� �����ǵ��� ��.
		2) �� word/line ������ count�Ͽ� ����ϴ� �κ� �߰�
				put_wordlist_sidx()�� return������ �����ϵ��� ��.
	- 2016/11/15 ��������: ������ minfreq �̻��� �͵鸸 ����ϴ� ��� �߰�
*/
#include <ctype.h>
#include "wordcount.h"

int minfreq;	// minimum frequency for output

/*
	convert 'value' to 'n' of 2^n
*/
int conv_int_to_2n(int value)
{
	int i, n=0;

	for (i=0; i < 32; i++) {
		if (value >> i) n = i;
	}

	return n;	// 2�� n��
}

/*
	���� N�� 1����Ʈ �����ڵ�� encode

	- 1 ~ 48  --> '0' + value (���� �״�� ����)
	- 49 �̻� --> '\\' + log N (2^n �� n���� ���ڷ� mapping)
*/
char fencode(int value)
{
	int i, n=0;

	if (value <= 48) return '0'+value;

	for (i=0; i < 32; i++)
		if (value >> i) n = i;
	return '\\'+n;	// '\\'=92(0x5C) -- 5->'a', 6->'b', ...
}

/*
	1����Ʈ �����ڵ带 ���� N���� decode
*/
int fdecode(char ch)
{
	int i = ch-'0', n;

	if (i <= 48) return i;

	i -= 44;
	n = (0x01 << i);
	return (n + (n << 1)) / 2;
}

void put_tree(FILE *fp, int flag, WORDCNT_STR *wp, int node)
{
	ULONG i=node, j;
	char ch;

	if (i == -1) return;

	put_tree(fp, flag, wp, wp->llink[i]);

	if (wp->count[i] < minfreq) return;	// ���󵵾� ��� ����

	switch (flag) {
	case 0:	// �󵵼� ���
		fprintf(fp, "%6ld %s\n", wp->count[i], wp->words[i]);
		break;
	case 1:	// �� ���� -- word list only
		fprintf(fp, "%s\n", wp->words[i]);
		break;
	case 2:	// �� ��ſ� word�� �󵵼���ŭ ��� -- sorting ȿ��
		for (j=0; j < wp->count[i]; j++)
			fprintf(fp, "%s\n", wp->words[i]);
		break;
	case 3:	// 1����Ʈ �󵵼� -- 0~255
		ch = (wp->count[i] > 255) ? 255 : (char) wp->count[i];
		fprintf(fp, "%c%s\n", ch, wp->words[i]);
		break;
	case 4:	// 1����Ʈ �󵵼� -- 2^n �ڵ尪
		ch = '0' + conv_int_to_2n(wp->count[i]);
		fprintf(fp, "%c%s\n", ch, wp->words[i]);
		break;
	case 5:	// 1����Ʈ �󵵼� -- 1~48: ��+'0', 49�̻�: 2^n �ڵ尪
		ch = '0' + fencode(wp->count[i]);
		fprintf(fp, "%c%s\n", ch, wp->words[i]);
		break;
	default:
		fprintf(fp, "%6ld %s\n", wp->count[i], wp->words[i]);
		break;
	}

	put_tree(fp, flag, wp, wp->rlink[i]);
}

/*
flag
	case 0:	// �󵵼� ���
	case 1:	// �� ���� -- word list only
	case 2:	// �� ��ſ� word�� �󵵼���ŭ ��� -- sorting ȿ��
	case 3:	// 1����Ʈ �󵵼� -- 0~255
*/
void put_wordlist(char *fname, WORDCNT_STR *wp, int flag)
{
	FILE *fp;

	if (is_file_exist(fname)) {
		printf("Overwrite <%s> ? ", fname);
		if (getchar() == 'y') getchar();
		else return;
	}
	fp = fopen(fname, "w");

	put_tree(fp, flag, wp, wp->root);

	fclose(fp);
}

// �� ���� ���� CR ���ڰ� ���� ������ CR ���� ���� -- 2013/07/02 ���½�
void remove_CR(char *s)
{
	int n=strlen(s);

	if (*(s+n-1) == 0x0D) *(s+n-1) = 0;
}

void remove_CR_char(WORDCNT_STR *wp)
{
	int i, k, n=wp->n_uniqwords;

	for (k=0; k < n; k++) {
		i = wp->sidx[k];
		remove_CR(wp->words[i]);
	}
}

/*
	Set sort index after sorting and counting the frequencies.

		n : number of unique words
*/
int put_wordlist_sidx(char *fname, WORDCNT_STR *wp, int flag)
{
	FILE *fp;
	ULONG i, j, k, n=wp->n_uniqwords;
	int total=0;

	if (is_file_exist(fname)) {
		printf("Overwrite <%s> ? ", fname);
		if (getchar() == 'y') getchar();
		else return 0;
	}
	fp = fopen(fname, "w");

	for (k=0; k < n; k++) {
//		fprintf(fp, "%8ld %s\n", wp->sidx[i], wp->words[i]);
//		fprintf(fp, "%8ld %s\n", wp->count[wp->sidx[i]], wp->words[wp->sidx[i]]);

		i = wp->sidx[k];
		total += wp->count[i];
		if (wp->count[i] < minfreq) continue;	// ���󵵾� ��� ����

		switch (flag) {
		case 0:	// �󵵼� ���
			fprintf(fp, "%6ld\t%s\n", wp->count[i], wp->words[i]);
			break;
		case 1:	// �� ���� -- word list only
			fprintf(fp, "%s\n", wp->words[i]);
			break;
		case 2:	// �� ��ſ� word�� �󵵼���ŭ ��� -- sorting ȿ��
			for (j=0; j < wp->count[i]; j++)
				fprintf(fp, "%s\n", wp->words[i]);
			break;
		case 3:	// 1����Ʈ �󵵼� -- 0~255
			fprintf(fp, "%c%s\n", (wp->count[i] > 255 ? 255 : wp->count[i]), wp->words[i]);
			break;
		case 4:	// 1����Ʈ �󵵼� -- 2^n �ڵ尪
			fprintf(fp, "%c%s\n", '0'+conv_int_to_2n(wp->count[i]+1), wp->words[i]);
			break;
		case 5:	// 1����Ʈ �󵵼� -- 1~48: ��+'0', 49�̻�: 2^n �ڵ尪
			fprintf(fp, "%c%s\n", '0'+fencode(wp->count[i]+1), wp->words[i]);
			break;
		case 6:
			fprintf(fp, "%6ld\t%s\n", wp->count[i], wp->words[i]);
			break;
		case 7:	// ������ ���
		default:
			fprintf(fp, "%6ld\t%6ld\t%s\n", wp->count[i], total, wp->words[i]);
			break;
		}
	}

	fclose(fp);
	return total;	// �� �ܾ� ����
}

/*
	word count ����� ���Ϸ� ���

	-0:	�󵵼� ���
	-1:	�� ���� -- word list only(sorting �� �ߺ� ����)
	-2:	�� ��ſ� word�� �󵵼���ŭ ��� -- sorting ȿ��(�ߺ� ���� ����)
	-3:	1����Ʈ �󵵼��� ��� -- 0~255
*/
void usage()
{
	fprintf(stderr, "$ wordcount.exe [-add] [-l] [-0/1/2/3/4] <input-file> <output-file>\n"); 
	fprintf(stderr, "\t-new: ������ ������ �ڷ� ���� --> default\n");
	fprintf(stderr, "\t-add: ������ ������ �ڷ� �������� �ʰ� ��� ������Ŵ\n");
	fprintf(stderr, "\t-rev: ������ �� reverse string ������\n");
	fprintf(stderr, "\t-dot: �ð��� ���� �ɸ��� �۾��� �����Ȳ ǥ�� ����\n");
	fprintf(stderr, "\t-freq n: min. frequency for output\n");
	fprintf(stderr, "\t-l : line ���� ó�� --> not ���� ����\n\n");
	fprintf(stderr, "\t-0 : ������ ������ ���(�󵵼� ���)\n");
	fprintf(stderr, "\t-1 : ������ ������ ���(�󵵼� ���� �� �ߺ� ����)\n");
	fprintf(stderr, "\t-2 : �󵵼� ��ſ� word�� �󵵼���ŭ ��� -- sorting ȿ��(�ߺ� ���� ����)\n");
	fprintf(stderr, "\t-3 : �󵵼��� 1����Ʈ �ڵ�� ��� -- 0~255\n");
	fprintf(stderr, "\t-4 : �󵵼��� 1����Ʈ 2^n �ڵ尪���� ��� -- '1'-'9',...\n");
	fprintf(stderr, "\t-5 : �󵵼��� 1����Ʈ �ڵ�� ��� -- 1~48: ��+'0', 49�̻�: 2^n �ڵ尪\n");
	fprintf(stderr, "\t-6 : �� ������ ���\n");
	fprintf(stderr, "\t-7 : �� ������ ��� + ������ ���\n");
	fprintf(stderr, "$ wordcount for Bigdata version 2.0 (2016/12/23) by Kang, Seung-Shik\n");
}

// �̵�Ͼ� ���� ���� �����ڷ���� ��� ����
void delete_accumulated_data()
{
	remove(WORD_FILE_NAME);
	remove(LINK_FILE_NAME);
}

/*
int main(int argc, char *argv[])
{
	char *infile=argv[1], *outfile="out.txt";
	int i=1, flag_output=0;
	int n_uniqwords, n_words=0;
	int add_results = 0;

	int flag_lineinput=0;	// 0: word unit, 1: line unit
	int flag_revstring=0;	// 1: ������ �� reverse string ������
	int flag_dots=1;	// ��뷮 ������ sorting �� ���� ��Ȳ ǥ��
	WORDCNT_STR Text={0}, *wp=&Text, PreResult;

	while (i < argc && argv[i][0] == '-') {
		if (!strcmp(argv[i], "-add"))
			add_results = 1;	// ���� ���� �����Ϳ� �߰�
		else if (!strcmp(argv[i], "-new"))
			add_results = -1;	// ������ �ڷ� ���� ����
		else if (!strcmp(argv[i], "-rev"))
			flag_revstring = 1;	// ������ �� reverse string ������
		else if (!strcmp(argv[i], "-dot"))
			flag_dots = 0;	// ������ �� ���� ��Ȳ�� ...���� ǥ�� ����
		else if (!strcmp(argv[i], "-freq")) {
			if (i+1 < argc && isdigit(argv[i+1][0])) {
				minfreq = atoi(argv[i+1]);	// min. frequency
				i++;
			}
		}
		else if (argv[i][1] == 'l')
			flag_lineinput = 1;	// line ���� ó�� --> word ������ �ƴϰ�...
		else if (isdigit(argv[i][1]))
			flag_output = argv[i][1]-'0';

		i++;
	}
	if (argc > 1 && add_results < 0)	// ���� ���� ������ ������ ����
		delete_accumulated_data();	// ������ �ڷ� ���� ����
	else if (argc > 1 && add_results == 0) {	// ���� ���� ������ ����
		fprintf(stderr, "Remove all the accumulated data? ");
		if (getchar() == 'y') {
			delete_accumulated_data();	// ������ �ڷ� ���� ����
			fprintf(stderr, "Accumulated data in <wordlist.xxx> have been deleted.\n");
		}
		getchar();
	}

	if (i >= argc) { usage(); return 0; }

	infile = argv[i];
	if (i+1 < argc) outfile = argv[i+1];

	if ((n_uniqwords = word_count(infile, wp, flag_lineinput, flag_dots))) {
		fprintf(stderr, "Wait a minite -- writing the results to a file <%s>.\n", outfile);
//		put_wordlist(outfile, wp, flag_output);
//		if (flag_output == 5) sort_by_freq(wp->sidx, wp->count, n);
		if (flag_output == 5 || flag_output == 6 || flag_output == 7)	// �󵵼����� ����
			sort_by_freq2(wp->sidx, wp->count, n_uniqwords);
		else if (flag_revstring) sort_by_rev_string(wp, n_uniqwords);

		if (flag_lineinput) remove_CR_char(wp);
		n_words = put_wordlist_sidx(outfile, wp, flag_output);	// sorting index�� �̿��Ͽ� ��� -- put_wordlist()�� ������ �����!
		save_words_and_links(WORD_FILE_NAME, LINK_FILE_NAME, wp);	// word count ������ file dump -- �ߺ� words ���ŵ�

//		load_words_and_links(WORD_FILE_NAME, LINK_FILE_NAME, &PreResult, flag_lineinput);	// word count ������ file dump
//		free_memory(&PreResult);
	} else fprintf(stderr, "No such file <%s> OR memory allocation error.\n", infile);

	free_memory(&Text);
	fprintf(stderr, "\tTotal %d words/lines are found in <%s>.\n", n_words, infile);
	fprintf(stderr, "\tUniq. %d words/lines are found in <%s>.\n", n_uniqwords, infile);

	return 0;
}
*/
#include "wordcount.h"

/*
	word count 순으로 sorting
*/
void sort_by_freq(ULONG sidx[], ULONG count[], ULONG n)
{
	ULONG i, j, temp;

	for (i=0; i < n-1; i++) {
		for (j=i+1; j < n; j++) {
			if (count[sidx[i]] < count[sidx[j]]) {
				temp = sidx[i];
				sidx[i] = sidx[j];
				sidx[j] = temp;
			}
		}
	}
}

/*
	word count 순으로 sorting
	sort_type : merge
*/
void sort_by_freq2(ULONG sidx[], ULONG count[], ULONG n)
{
	int i, j;

	if(n > 1)
	{
		ULONG mid1 = n / 2, mid2 = n - mid1;
		ULONG *sidx1, *sidx2;

		sidx1 = (ULONG*) malloc(mid1 * sizeof(ULONG));
		sidx2 = (ULONG*) malloc(mid2 * sizeof(ULONG));

		for(i = 0; i < mid1; i++) {
			sidx1[i] = sidx[i];
		}
		for(j = 0; j < mid2; i++, j++) {
			sidx2[j] = sidx[i];
		}

		sort_by_freq2(sidx1, count, mid1);
		sort_by_freq2(sidx2, count, mid2);
		merge(mid1, mid2, sidx1, sidx2, count, sidx);

		free(sidx1);
		free(sidx2);
	}
}

/******************************
* merge sort를 위한 merge함수 *
******************************/
void merge(ULONG mid1, ULONG mid2, ULONG *sidx1, ULONG *sidx2, ULONG count[], ULONG sidx[])
{
	ULONG i, j , k;
	i = j = k = 0;

	while(i < mid1 && j < mid2)
	{
		if(count[sidx1[i]] > count[sidx2[j]])
			sidx[k] = sidx1[i++];
		else
			sidx[k] = sidx2[j++];
		k++;
	}

	if(i >= mid1)
		// copy V[j] through V[m] to S[k] through S[h+m];
		while(j < mid2)
			sidx[k++] = sidx2[j++];
	else
		// copy U[i] through U[h] to S[k] through S[h+m];
		while(i < mid1)
			sidx[k++] = sidx1[i++];
}

// 한글 문자열을 reverse --> 영문자가 섞여 있으면 오작동
UCHAR *reverse_Hangul(UCHAR *s)
{
	UCHAR c, *p, *q;

	for (p=s, q=s+strlen(s)-1; p < q; p++, q--) {
		c = *q;
		*q = *p;
		*p = c;
	}

	for (p=s; *p; p++) {
		if (*p & 0x80) {
			c = *(p+1);
			*(p+1) = *p;
			*p = c;
			p++;
		}
	}

	return s;
}

/*
	모든 문자열을 reverse
*/
void reverse_all_words(UCHAR *words[], ULONG n)
{
	ULONG i;

	for (i=0; i < n; i++) reverse_Hangul(words[i]);
}

/*
	word count 순으로 sorting
*/
void sort_by_word(UCHAR *words[], ULONG sidx[], ULONG n)
{
	ULONG i, j, temp;

	for (i=0; i < n-1; i++) {
		for (j=i+1; j < n; j++) {
			if (strcmp((char*)words[sidx[i]], (char*)words[sidx[j]]) > 0) {
				temp = sidx[i];
				sidx[i] = sidx[j];
				sidx[j] = temp;
			}
		}
	}
}

/*
	word count 순으로 sorting
	sort_type : merge
*/
void sort_by_word2(UCHAR *words[], ULONG sidx[], ULONG n)
{
	int i, j;

	if(n > 1)
	{
		ULONG mid1 = n / 2, mid2 = n - mid1;
		ULONG *sidx1, *sidx2;

		sidx1 = (ULONG*) malloc(mid1 * sizeof(ULONG));
		sidx2 = (ULONG*) malloc(mid2 * sizeof(ULONG));

		for(i = 0; i < mid1; i++) {
			sidx1[i] = sidx[i];
		}
		for(j = 0; j < mid2; i++, j++) {
			sidx2[j] = sidx[i];
		}

		sort_by_word2(sidx1, words, mid1);
		sort_by_word2(sidx2, words, mid2);
		merge2(mid1, mid2, sidx1, sidx2, words, sidx);

		free(sidx1);
		free(sidx2);
	}
}

/******************************
* merge sort를 위한 merge함수 *
******************************/
void merge2(ULONG mid1, ULONG mid2, ULONG *sidx1, ULONG *sidx2, UCHAR *words[], ULONG sidx[])
{
	ULONG i, j , k;
	i = j = k = 0;

	while(i < mid1 && j < mid2)
	{
		if (strcmp((char*)words[sidx1[i]], (char*)words[sidx2[j]]) < 0)
			sidx[k] = sidx1[i++];
		else
			sidx[k] = sidx2[j++];
		k++;
	}

	if(i >= mid1)
		// copy V[j] through V[m] to S[k] through S[h+m];
		while(j < mid2)
			sidx[k++] = sidx2[j++];
	else
		// copy U[i] through U[h] to S[k] through S[h+m];
		while(i < mid1)
			sidx[k++] = sidx1[i++];
}

void sort_by_rev_string(WORDCNT_STR *wp, ULONG n)
{
	reverse_all_words(wp->words, n);
	sort_by_word(wp->words, wp->sidx, n);
	reverse_all_words(wp->words, n);
}

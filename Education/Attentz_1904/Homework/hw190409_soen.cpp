#define _CRT_NONSTDC_NO_WARNINGS    // 비표준 함수를 사용했을 때 에러 방지
#define _CRT_SECURE_NO_WARNINGS// 비표준 함수를 사용했을 때 에러 방지
#pragma warning(disable: 4819)

#include <ctype.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>

void insertstr(char *str, const char *insert)

{

	int len;



	len = strlen(insert);

	memmove(str + len, str, strlen(str) + 1);

	memcpy(str, insert, len);

}



void deletestr(char *str, int count)

{

	memmove(str, str + count, strlen(str + count) + 1);

}



void strreplace(char *str, const char *a, const char *b)

{

	char *p;



	for (;;) {

		p = strstr(str, a);

		if (p == NULL) return;

		deletestr(p, strlen(a));

		insertstr(p, b);

	}

}

void stradd(char *str, int c)

{

	int len = strlen(str);

	str[len] = c;

	str[len + 1] = 0;

}




void my_puts(const char *str)

{

	while (*str) putch(*str++);

	putch('\n');

}



void my_puts2(const char *str)

{

	int i;



	for (i = 0; str[i]; i++) {

		putch(str[i]);

	}

	putch('\n');

}



char *my_strcpy(char *dest, const char *src)

{

	char *d = dest;

	while (*dest++ = *src++) { ; }

	return d;

}



size_t my_strlen(const char *str)

{

	const char *p;



	for (p = str; *p; p++) { ; }

	return p - str;

}



size_t my_strlen2(const char *str)

{

	int i;



	for (i = 0; str[i]; i++) { ; }

	return i;

}



char *my_strcat(char *dest, const char *src)

{

	my_strcpy(dest + my_strlen(dest), src);

	return dest;

}



char *my_strchr(const char *string, int c)

{

	while (*string) {

		if (*string == c) {

			return (char *)string;

		}

		string++;

	}

	return NULL;

}



char *my_strstr(const char *string, const char *strSearch)

{

	const char *s, *sub;



	for (; *string; string++) {

		for (sub = strSearch, s = string; *sub && *s && *s == *sub; sub++, s++) { ; }

		if (*sub == 0) return (char *)string;

	}

	return NULL;

}

char *stristr(const char *string, const char *strSearch)

{

	const char *s, *sub;



	for (;*string;string++) {

		for (sub = strSearch, s = string;*sub && *s;sub++, s++) {

			if (tolower(*s) != tolower(*sub)) break;

		}

		if (*sub == 0) return (char *)string;

	}

	return NULL;

}



char *stristr2(const char *string, const char *strSearch)

{

	char *scopy, *srchcopy, *p;



	scopy = (char *)malloc(strlen(string) + 1);

	strcpy(scopy, string);

	strlwr(scopy);

	srchcopy = (char *)malloc(strlen(strSearch) + 1);

	strcpy(srchcopy, strSearch);

	strlwr(srchcopy);



	p = strstr(scopy, srchcopy);

	free(scopy);

	free(srchcopy);



	if (p == NULL) {

		return NULL;

	}
	else {

		return (char *)string + (p - scopy);

	}

}



int main()

{
	{
		char dest[256];



		my_puts("Korea");

		my_puts2("한글도 잘 된다.");



		my_puts(my_strcpy(dest, "my string copy function test"));



		printf("문자열의 길이는 %d입니다.\n", my_strlen("1234"));

		printf("문자열의 길이는 %d입니다.\n", my_strlen2("123456789"));



		char str[128] = "abcd";

		my_puts(my_strcat(str, "efgh"));



		printf("o가 %s습니다.\n", my_strchr("notebook", 'o') == NULL ? "없" : "있");

		printf("z가 %s습니다.\n", my_strchr("notebook", 'z') == NULL ? "없" : "있");

		printf("under가 %s습니다.\n", my_strstr("misunderstand", "under") == NULL ? "없" : "있");

		printf("above가 %s습니다.\n", my_strstr("misunderstand", "above") == NULL ? "없" : "있");

	}

	{

		char str[128] = "";



		stradd(str, 'a');puts(str);

		stradd(str, 'b');puts(str);

		stradd(str, 'c');puts(str);

		stradd(str, 'd');puts(str);

	}

	{

		if (stristr2("madeINkorea", "inko")) {

			puts("찾는 문자열이 있습니다.");

		}
		else {

			puts("찾는 문자열이 없습니다.");

		}

	}

	{

		char str[128] = "welcome to korea";

		char str2[512] = "내가 그린 기린 그림은 암 기린을 그린 기린 그림이고 "

			"네가 그린 기린 그림은 숫 기린을 그린 기린 그림이다.";

		char *p;



		puts(str);

		p = strstr(str, "korea");

		insertstr(p, "beautiful ");

		puts(str);

		p = strstr(str, "to");

		deletestr(p, 3);

		puts(str);



		puts(str2);

		strreplace(str2, "기린", "오랑우탄");

		puts(str2);

	}
}
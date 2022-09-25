/**
 * \file main.c
 * \author Vinogradov V.V.
 * \date 25 Марта 2019
 * \brief Программа, выводящая все слова, которые можно набрать на
 * клавиатуре (QWERTY) двигаясь по соседним клавишам
 *
 * Написать программу, выводящую все слова, которые можно набрать на
 * клавиатуре (QWERTY) двигаясь по соседним клавишам. Соседними
 * считаются клавиши, имеющие пересечения с вертикальными и
 * горизонтальными линиями, проведёнными через рассматриваемую клавишу.
 * Например, для клавиши D это E, R, S, F, X, C (но не W), а для U это
 * Y, I, H, J (но не K). Слово начинается с любой из клавиш и далее
 * может состоять только из тех букв, которые находятся рядом, например,
 * "DESERT". Слова из словаря. Слова формируются движением по
 * клавиатуре. То есть, если начало идёт от D, то D граничит с E и это
 * корректный переход, E граничит с S, и так далее до перехода от R к T,
 * они тоже соседи, поэтому всё сходится.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <time.h>

#define MAX_NUM_NEIGHBORS	6
#define BUFF_WORD_LENGTH	100
#define FILE_DICTIONARY		"dictionary.txt"

#define RESET   "\033[0m"
#define GRN  	"\x1B[32m"
#define RED     "\033[1;31m"

/**
 * \brief Характеристики клавиши
 */
typedef struct {
	uint8_t		ch;									/**< Буква на клавише */
	uint8_t		ind_neighbors[MAX_NUM_NEIGHBORS];	/**< Индексы буквы в массиве */
} key;


char *strupr (char *s);
int check_qwerty_word (char *word);

key const key_table[] =
{
	{'-'},
	{'Q', {2, 11}},
	{'W', {1, 11, 12, 3}},
	{'E', {2, 12, 13, 4}},
	{'R', {3, 13, 14, 5}},
	{'T', {4, 14, 15, 6}},
	{'Y', {5, 15, 16, 7}},
	{'U', {6, 16, 17, 8}},
	{'I', {7, 17, 18, 9}},
	{'O', {8, 18, 19, 10}},
	{'P', {9, 19}},
	{'A', {1, 2, 12, 20}},
	{'S', {11, 2, 3, 13, 20, 21}},
	{'D', {12, 3, 4, 14, 21, 22}},
	{'F', {13, 4, 5, 15, 22, 23}},
	{'G', {14, 5, 6, 16, 23, 24}},
	{'H', {15, 6, 7, 17, 24, 25}},
	{'J', {16, 7, 8, 18, 25, 26}},
	{'K', {17, 8, 9, 19, 26}},
	{'L', {18, 9, 10}},
	{'Z', {11, 12, 21}},
	{'X', {20, 12, 13, 22}},
	{'C', {21, 13, 14, 23}},
	{'V', {22, 14, 15, 24}},
	{'B', {23, 15, 16, 25}},
	{'N', {24, 16, 17, 26}},
	{'M', {25, 17, 18}}
};

#define NUM_KEY_TABLE (sizeof (key_table) \
							/ sizeof (key_table[0]))

int main () {
	uint64_t ind_max_len_word, num_word_dictionary, count_searched_words = 0;
	uint8_t len, max_len;
	char **dictionary;
	FILE *f_dictionary;
	clock_t start, stop;

	f_dictionary = fopen (FILE_DICTIONARY, "r");

	fscanf (f_dictionary, "%"SCNu64, &num_word_dictionary);

	dictionary = (char**) malloc (num_word_dictionary * sizeof(char*));
	if (!dictionary) {
		perror ("malloc");
		exit (EXIT_FAILURE);
	}
	for (uint64_t i = 0; i < num_word_dictionary; i++) {
		dictionary[i] = (char*) malloc (BUFF_WORD_LENGTH * sizeof(char));
		if (!dictionary[i]) {
			perror ("malloc");
			exit (EXIT_FAILURE);
		}
	}
	max_len = 0;
	ind_max_len_word = 0;
	for (uint64_t i = 0; i < num_word_dictionary; i++) {
		fgets (dictionary[i], BUFF_WORD_LENGTH, f_dictionary);
		len = strlen(dictionary[i]);
		dictionary[i] = (char *) realloc (dictionary[i], strlen(dictionary[i]) + 1);
		dictionary[i][strcspn(dictionary[i], "\n")] = '\0';
		dictionary[i] = strupr(dictionary[i]);
		if (max_len < len) {
			max_len = len;
			ind_max_len_word = i;
		}
	}

	printf ("Number of words in the dictionary - %s%"PRIu64"%s\n", RED, num_word_dictionary, RESET);
	printf ("Max word length in the dictionary - %s%"PRIu8"%s\n", RED, (max_len - 1), RESET);
	printf ("Max word in the dictionary - %s%s%s\n", RED, dictionary[ind_max_len_word], RESET);

	start = clock();

	max_len = 0;
	ind_max_len_word = 0;
	for (uint64_t i = 0; i < num_word_dictionary; i++) {
		if (check_qwerty_word(dictionary[i])) {
			len = strlen(dictionary[i]);
			if (max_len < len) {
				max_len = len;
				ind_max_len_word = i;
			}
			printf ("%s\n", dictionary[i]);
			count_searched_words++;
		}
	}

	stop = clock();

	printf ("Number of searched word - %s%"PRIu64"%s\n", RED, count_searched_words, RESET);
	printf ("Max length of the searched word - %s%"PRIu8"%s\n", RED, max_len, RESET);
	printf ("Max length searched word - %s%s%s\n", RED, dictionary[ind_max_len_word], RESET);
	printf ("Time: %s%f%s s\t\n", RED, (double)(stop - start) / CLOCKS_PER_SEC, RESET);

	for (uint64_t i = 0; i < num_word_dictionary; i++) {
		free(dictionary[i]);
	}
	free(dictionary);

	fclose (f_dictionary);
	return 0;
}

/**
 * \brief Перевод строки в верхний регистр
 */
char *strupr (char *s) {
	if (s == NULL) {
		return s;
	}

	char *pc = s;
	while (*pc) {
		*pc = toupper(*pc);
		++pc;
	}

	return s;
}

/**
 * \brief Проверка слова, можно ли его набрать на клавиатуре (QWERTY) двигаясь по соседним клавишам
 */
int check_qwerty_word (char *word) {
	uint8_t len = strlen(word);
	uint8_t ind_current_letter = 0;
	for (uint8_t i = 0; i < NUM_KEY_TABLE; i++) {
		if (word[0] == key_table[i].ch) {
			ind_current_letter = i;
			break;
		}
	}

	for (uint8_t i = 1; i < len; i++) {
		uint8_t flag = 0;
		for (uint8_t j = 0; j < MAX_NUM_NEIGHBORS; j++) {
			uint8_t ind_j = key_table[ind_current_letter].ind_neighbors[j];
			if (ind_j != 0) {
				if (word[i] == key_table[ind_j].ch) {
					ind_current_letter = ind_j;
					flag = 1;
					break;
				}
			} else {
				break;
			}
		}
		if (flag == 0) {
			return 0;
		}
	}
	return 1;
}

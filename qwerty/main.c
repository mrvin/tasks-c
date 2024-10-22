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

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NUM_NEIGHBORS 6
#define BUFF_WORD_LENGTH 128
#define FILE_DICTIONARY "/usr/share/dict/words"

#define RESET "\033[0m"
#define GRN "\x1B[32m"
#define RED "\033[1;31m"

/**
 * \brief Характеристики клавиши
 */
typedef struct {
	char ch;							   /**< Буква на клавише */
	char ind_neighbors[MAX_NUM_NEIGHBORS]; /**< Индексы буквы в массиве */
} key;

char *strupr(char *s);
int check_qwerty_word(const char *const word, size_t len);

key const key_table[] = {
	{'-', {0}},
	{'a', {'q', 'w', 's', 'z'}},
	{'b', {'v', 'g', 'h', 'n'}},
	{'c', {'x', 'd', 'f', 'v'}},
	{'d', {'s', 'e', 'r', 'f', 'c', 'x'}},
	{'e', {'w', 's', 'd', 'r'}},
	{'f', {'d', 'r', 't', 'g', 'v', 'c'}},
	{'g', {'f', 't', 'y', 'h', 'b', 'v'}},
	{'h', {'g', 'y', 'u', 'j', 'n', 'b'}},
	{'i', {'u', 'j', 'k', 'o'}},
	{'j', {'h', 'u', 'i', 'k', 'm', 'n'}},
	{'k', {'j', 'i', 'o', 'l', 'm'}},
	{'l', {'k', 'o', 'p'}},
	{'m', {'n', 'j', 'k'}},
	{'n', {'b', 'h', 'j', 'm'}},
	{'o', {'i', 'k', 'l', 'p'}},
	{'p', {'o', 'l'}},
	{'q', {'a', 'w'}},
	{'r', {'e', 'd', 'f', 't'}},
	{'s', {'a', 'w', 'e', 'd', 'x', 'z'}},
	{'t', {'r', 'f', 'g', 'y'}},
	{'u', {'y', 'h', 'j', 'i'}},
	{'v', {'c', 'f', 'g', 'b'}},
	{'w', {'q', 'a', 's', 'e'}},
	{'x', {'z', 's', 'd', 'c'}},
	{'y', {'t', 'g', 'h', 'u'}},
	{'z', {'a', 's', 'x'}},
};

#define NUM_KEY_TABLE (sizeof(key_table) / sizeof(key_table[0]))

int main(int argc, char *argv[]) {
	uint64_t num_word_dictionary = 0;
	uint64_t count_searched_words = 0;
	size_t max_len_dictionary = 0;
	size_t max_len_searched = 0;
	char word[BUFF_WORD_LENGTH];
	char longest_word_dictionary[BUFF_WORD_LENGTH];
	char longest_word_searched[BUFF_WORD_LENGTH];

	char *file_name = FILE_DICTIONARY;
	if (argc == 2) {
		file_name = argv[1];
	}

	FILE *f_dictionary = fopen(file_name, "r");
	if (f_dictionary == NULL) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	clock_t start = clock();
	while (fgets(word, BUFF_WORD_LENGTH, f_dictionary) != NULL) {
		size_t len = strlen(word);
		word[len - 1] = '\0';
		if (!strcmp(word, "")) {
			continue;
		}
		num_word_dictionary++;
		strupr(word);

		if (max_len_dictionary < len) {
			max_len_dictionary = len;
			strcpy(longest_word_dictionary, word);
		}

		if (check_qwerty_word(word, len)) {
			if (max_len_searched < len) {
				max_len_searched = len;
				strcpy(longest_word_searched, word);
			}
			printf("%s\n", word);
			count_searched_words++;
		}
	}
	if (!feof(f_dictionary)) {
		perror("Error reading line");
	}
	clock_t stop = clock();

	printf("Number of words in the dictionary: %s%" PRIu64 "%s\n", RED, num_word_dictionary, RESET);
	if (num_word_dictionary != 0) {
		printf("Max word length in the dictionary: %s%" PRIu64 "%s\n", RED, (max_len_dictionary - 1), RESET);
		printf("Max word in the dictionary: %s%s%s\n", RED, longest_word_dictionary, RESET);
	}
	printf("Number of searched word: %s%" PRIu64 "%s\n", GRN, count_searched_words, RESET);
	if (count_searched_words != 0) {
		printf("Max length of the searched word: %s%" PRIu64 "%s\n", GRN, (max_len_searched - 1), RESET);
		printf("Max length searched word: %s%s%s\n", GRN, longest_word_searched, RESET);
	}
	printf("Time: %s%f%s s\t\n", RED, (double)(stop - start) / CLOCKS_PER_SEC, RESET);

	fclose(f_dictionary);

	return EXIT_SUCCESS;
}

/**
 * \brief Перевод строки в нижний регистр
 */
char *strupr(char *s) {
	if (s == NULL) {
		return s;
	}

	char *pc = s;
	while (*pc) {
		*pc = tolower(*pc);
		++pc;
	}

	return s;
}

/**
 * \brief Проверка слова, можно ли его набрать на клавиатуре (QWERTY) двигаясь по соседним клавишам
 */
int check_qwerty_word(const char *const word, size_t len) {
	for (size_t i = 0; i < (len - 2); i++) {
		bool is_qwerty = false;
		uint8_t ind = word[i] - ('a' - 1);
		for (uint8_t j = 0; j < MAX_NUM_NEIGHBORS; j++) {
			if (word[i + 1] == key_table[ind].ind_neighbors[j]) {
				is_qwerty = true;
				break;
			}
		}
		if (!is_qwerty) {
			return 0;
		}
	}

	return 1;
}

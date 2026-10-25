#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef LINUX
#include <windows.h>
#endif

#define WORDS_SRC "english_words"

#define USR_SRC "users_strings"

#define RAND_GEN_WORD_COUNT 1	/* Колличество слов из случайных символов */

#define RAND_GEN_WORD_LEN 10	/* Длина слов из случайных символов */

#define RAND_WORD_COUNT 3	/* Колличество случайных слов */

#define RAND_USRS_STRS_COUNT 1	/* Колличество случайных строк пользователя */

/* Используем, чтобы получить текущее время в секундах */
int timer_init()
{
	return (int)time(NULL);
}

/* Просто вычисляет количество секунд между timer_init и текущим моментом */
int timer_res(int from_timer_init)
{
	int current_time = (int)time(NULL);
	return current_time - from_timer_init;
}

/* Возвращает число из диапозона [min, max] */
int get_random_num(int min, int max)
{
	return min + rand() % (max - min + 1);
}

/* Возвращает слово из случайных символов определённой длины*/
char* get_random_chars_word(int word_len)
{
	if (word_len <= 0) return NULL;

	char* word = (char*)malloc(word_len + 1);
	if (word == NULL) {
		return NULL;
	}

	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int charset_size = sizeof(charset) - 1;

	for (int i = 0; i < word_len; i++) {
		word[i] = charset[rand() % charset_size];
	}

	word[word_len] = '\0';
	return word;
}

/* Открывает файл на чтение и считает в нём строки, после чего закрывает файл */
int strs_cnt_in_file(char* filename)
{
	FILE* myfile = fopen(filename, "r");
	if (myfile == NULL) {
		printf("Ошибка: не удалось открыть файл %s\n", filename);
		return -1;
	}

	int count = 0;
	char buffer[1024];

	while (fgets(buffer, sizeof(buffer), myfile) != NULL) {
		count++;
	}

	fclose(myfile);
	return count;
}

/* Возвращает строку под определённым номером из файла */
char* get_str_from_file(char* filename, int str_num)
{
	FILE* myfile = fopen(filename, "r");
	if (myfile == NULL) {
		printf("Ошибка: не удалось открыть файл %s\n", filename);
		return NULL;
	}

	char* result = NULL;
	char buffer[1024];
	int current_line = 0;

	while (fgets(buffer, sizeof(buffer), myfile) != NULL) {
		if (current_line == str_num) {
			// Убираем символ новой строки в конце
			buffer[strcspn(buffer, "\n")] = '\0';
			result = strdup(buffer);
			break;
		}
		current_line++;
	}

	fclose(myfile);
	return result;
}

/* Читает строку, которую ввёл пользователь, после чего возвращает её */
char* read_str_from_user()
{
	char buffer[256];
	if (fgets(buffer, sizeof(buffer), stdin)) {
		buffer[strcspn(buffer, "\r\n")] = '\0';
		return strdup(buffer);
	}
	return strdup("");
}

/* Считает количество ошибок между строками */
int mistakes_count(char* original_str, char* users_str)
{
	int mistakes = 0;
	int len_orig = (int)strlen(original_str);
	int len_user = (int)strlen(users_str);
	int min_len = (len_orig < len_user) ? len_orig : len_user;

	for (int i = 0; i < min_len; i++) {
		if (original_str[i] != users_str[i]) {
			mistakes++;
		}
	}
	if(len_orig > len_user){
		mistakes += len_orig - len_user;
	}
	return mistakes;
}

/* Режим слов из случайных символов, возвращает колличество ошибок */
int random_generated_words_mode(int words_cnt, int word_len)
{
	int mistakes = 0;
	int i;
	char* rand_gen_word;
	char* users_answer;

	for (i = 0; i != words_cnt; i++) {
		rand_gen_word = get_random_chars_word(word_len);
		printf("Повторите строку:\n%s\n\n", rand_gen_word);
		users_answer = read_str_from_user();
		mistakes += mistakes_count(rand_gen_word, users_answer);
		if(rand_gen_word != NULL){
			free(rand_gen_word);
			rand_gen_word = NULL;
		}
		if(users_answer != NULL){
			free(users_answer);
			users_answer = NULL;
		}
	}
	return mistakes;
}

/* Берём слова из файла под именем WORDS_SRC и возвращаем колличество ошибок */
int random_words_mode(int words_cnt)
{
	int mistakes = 0;
	int i;
	int src_size = strs_cnt_in_file(WORDS_SRC);
	char* rand_word;
	char* users_answer;

	for (i = 0; i != words_cnt; i++) {
		rand_word = get_str_from_file(WORDS_SRC,
			get_random_num(0, src_size - 1));
		printf("Повторите строку:\n%s\n\n", rand_word);
		users_answer = read_str_from_user();
		mistakes += mistakes_count(rand_word, users_answer);
		if(rand_word != NULL){
			free(rand_word);
			rand_word = NULL;
		}
		if(users_answer != NULL){
			free(users_answer);
			users_answer = NULL;
		}
	}
	return mistakes;
}

/* Берём строки из файла под именем USER_SRC и возвращаем колличество ошибок */
int random_users_strings_mode(int strings_cnt)
{
	int mistakes = 0;
	int i;
	int src_size = strs_cnt_in_file(USR_SRC);
	char* rand_word;
	char* users_answer;

	if (src_size == -1) {
		return -1;
	}
	if (src_size == 0) {
		fprintf(stderr, "Error: %s is empty\n", USR_SRC);
		return -1;
	}
	for (i = 0; i != strings_cnt; i++) {
		rand_word = get_str_from_file(USR_SRC,
			get_random_num(0, src_size - 1));
		printf("Повторите строку:\n%s\n\n", rand_word);
		users_answer = read_str_from_user();
		mistakes += mistakes_count(rand_word, users_answer);
		if(rand_word != NULL){
			free(rand_word);
			rand_word = NULL;
		}
		if(users_answer != NULL){
			free(users_answer);
			users_answer = NULL;
		}
	}
	return mistakes;
}

/* Запускает один из трёх режимов, засекает время, выводит рез-ты */
void start_game(int gamemode)
{
	int timer;
	int mistakes;

	timer = timer_init();
	if (gamemode == 1) {
		mistakes = random_generated_words_mode(RAND_GEN_WORD_COUNT,
							RAND_GEN_WORD_LEN);
	}
	else if (gamemode == 2) {
		mistakes = random_words_mode(RAND_WORD_COUNT);
	}
	else if (gamemode == 3) {
		mistakes = random_users_strings_mode(RAND_USRS_STRS_COUNT);
	}
	else {
		fprintf(stderr, "%d: incorrect mode", gamemode);
		return;
	}
	if (mistakes != -1) {
		printf("время: %d, ошибки: %d\n", timer_res(timer), mistakes);
	}
}

void clean_stdin()
{
	char c;
	c = getchar();
	while ((c != EOF) && (c != '\n')) {
		c = getchar();
	}
}

/* Предлагает пользователю выбор из нескольких режимов,
	а потом считывает его выбор и возвращает в виде цифры (1, 2 или 3) */
#define MODES_TEXT \
	"\n1. Строка из случайных символов\n" \
	"2. Случайные слова\n" \
	"3. Пользовательский файл\n" \
	"4. Выход из программы\n"
int get_gamemode()
{
	int mode = 0;
	printf("Выберете режим:\n");
	printf(MODES_TEXT);
	scanf("%d", &mode);
	clean_stdin();
	return mode;
}

int main()
{
	int gamemode;
	#ifndef LINUX
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	#endif
	while (1)
	{
		gamemode = get_gamemode(); /* Пользователь вводит режим */
		if (gamemode == 4) {
			break;
		}
		start_game(gamemode);
	}
	return 0;
}

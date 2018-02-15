/* Helper functions for networking that are common between server and client */
#include <regex.h>

/* REGEX HELPERS START */
regex_t compileRegex(char *regex);

int checkRegex(regex_t regex, char *check_string);

struct CommandRegex compileAllCommandChecks();

struct CommandRegex {
    regex_t USER, PASS, LS, PWD, CD;
    int user_len, pass_len, ls_len, pwd_len, cd_len;
};
/* REGEX HELPERS END */

/* STRING HELPERS */
char* stripStartingChars(int num_chars, char* string);

/* STRING HELPERS END */


char *lsCommand(char *cwd);
char *pwdCommand(char *cwd);
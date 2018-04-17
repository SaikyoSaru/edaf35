#include "list.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PERM (0644)   /* default permission rw-r--r-- */
#define MAXBUF (512)  /* max length of input line. */
#define MAX_ARG (100) /* max number of cmd line arguments. */

typedef enum {
  AMPERSAND, /* & */
  NEWLINE,   /* end of line reached. */
  NORMAL,    /* file name or command option. */
  INPUT,     /* input redirection (< file) */
  OUTPUT,    /* output redirection (> file) */
  PIPE,      /* | for instance: ls *.c | wc -l */
  SEMICOLON  /* ; */
} token_type_t;

static char *progname;             /* name of this shell program. */
static char input_buf[MAXBUF];     /* input is placed here. */
static char token_buf[2 * MAXBUF]; /* tokens are placed here. */
static char *input_char;           /* next character to check. */
static char *token;                /* a token such as /bin/ls */

static list_t *path_dir_list; /* list of directories in PATH. */
static int input_fd;          /* for i/o redirection or pipe. */
static int output_fd;         /* for i/o redirection or pipe */

static char *old_dir;

/* fetch_line: read one line from user and put it in input_buf. */
int fetch_line(char *prompt) {
  int c;
  int count;

  input_char = input_buf;
  token = token_buf;

  printf("%s", prompt);
  fflush(stdout);

  count = 0;

  for (;;) {

    c = getchar();

    if (c == EOF)
      return EOF;

    if (count < MAXBUF)
      input_buf[count++] = c;

    if (c == '\n' && count < MAXBUF) {
      input_buf[count] = 0;
      return count;
    }

    if (c == '\n') {
      printf("too long input line\n");
      return fetch_line(prompt);
    }
  }
}

/* end_of_token: true if character c is not part of previous token. */
static bool end_of_token(char c) {
  switch (c) {
  case 0:
  case ' ':
  case '\t':
  case '\n':
  case ';':
  case '|':
  case '&':
  case '<':
  case '>':
    return true;

  default:
    return false;
  }
}

/* gettoken: read one token and let *outptr point to it. */
int gettoken(char **outptr) {
  token_type_t type;

  *outptr = token;

  while (*input_char == ' ' || *input_char == '\t')
    input_char++;

  *token++ = *input_char;

  switch (*input_char++) {
  case '\n':
    type = NEWLINE;
    break;

  case '<':
    type = INPUT;
    break;

  case '>':
    type = OUTPUT;
    break;

  case '&':
    type = AMPERSAND;
    break;

  case '|':
    type = PIPE;
    break;

  default:
    type = NORMAL;

    while (!end_of_token(*input_char))
      *token++ = *input_char++;
  }

  *token++ = 0; /* null-terminate the string. */

  return type;
}

/* error: print error message using formatting string similar to printf. */
void error(char *fmt, ...) {
  va_list ap;

  fprintf(stderr, "%s: error: ", progname);

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  /* print system error code if errno is not zero. */
  if (errno != 0) {
    fprintf(stderr, ": ");
    perror(0);
  } else
    fputc('\n', stderr);
}

/* run_program: fork and exec a program. */
void run_program(char **argv, int argc, bool foreground, bool doing_pipe) {
  /* you need to fork, search for the command in argv[0],
   * setup stdin and stdout of the child process, execv it.
   * the parent should sometimes wait and sometimes not wait for
   * the child process (you must figure out when). if foreground
   * is true then basically you should wait but when we are
   * running a command in a pipe such as PROG1 | PROG2 you might
   * not want to wait for each of PROG1 and PROG2...
   *
   * hints:
   *  snprintf is useful for constructing strings.
   *  access is useful for checking wether a path refers to an
   *      executable program.
   *
   *
   */
  char buf[4096];
  pid_t child;
  int status;

  if (!strcmp(argv[0], "exit")) {
    exit(0);
  }
  // in the case of cd, since
  // the child process cant change the parent process directory
  if (!strcmp(argv[0], "cd")) {

    if (argv[1] == NULL) {
      chdir(getenv("HOME"));
      return;
    }

    char old_dir_tmp[MAXBUF];
    if (old_dir != NULL) {
      strcpy(old_dir_tmp, old_dir);
    }
    old_dir = getcwd(old_dir, MAXBUF);

    if (!strncmp(argv[1], "-", 1)) {
      chdir(old_dir_tmp);

    } else {
      chdir(argv[1]);
    }
    return;
  }

  child = fork();

  if (child == 0) {

    bool cmd = false;
    // access to check if the command exists
    list_t *list = path_dir_list;
    size_t list_len = length(list);

    while (list_len-- > 0) {
      // concat the command with the different pathways
      snprintf(buf, sizeof(buf), "%s/%s", list->data, argv[0]);
      list = list->succ;
      if (access(buf, F_OK) == 0) { // F_OK checks for existence of file
        cmd = true;

        if (input_fd != STDIN_FILENO) {
          dup2(input_fd, STDIN_FILENO);
        }
        if (output_fd != STDOUT_FILENO) {
          dup2(output_fd, STDOUT_FILENO);
        }

        execv(buf, argv);
        //  printf("herro 2\n");
      }
    }

  } else if (foreground && !doing_pipe) {

    waitpid(child, &status, 0);

  } else if (foreground) {
    printf("piping");

  } else {

    error("Fork failed");
    exit(1);
  }
}

void parse_line(void) {
  char *argv[MAX_ARG + 1];
  int argc;
  int pipe_fd[2]; /* 1 for producer and 0 for consumer. */
  token_type_t type;
  bool foreground;
  bool doing_pipe;

  input_fd = STDIN_FILENO;   // defined as 0 in unistd.h
  output_fd = STDOUT_FILENO; // defined as 1 in unistd.h
  argc = 0;

  for (;;) {

    foreground = true;
    doing_pipe = false;

    type = gettoken(&argv[argc]);

    switch (type) {
    case NORMAL:
      argc += 1;
      break;

    case INPUT:
      type = gettoken(&argv[argc]);
      if (type != NORMAL) {
        error("expected file name: but found %s", argv[argc]);
        return;
      }

      input_fd = open(argv[argc], O_RDONLY);

      if (input_fd < 0)
        error("cannot read from %s", argv[argc]);

      break;

    case OUTPUT:
      type = gettoken(&argv[argc]);
      if (type != NORMAL) {
        error("expected file name: but found %s", argv[argc]);
        return;
      }

      output_fd = open(argv[argc], O_CREAT | O_WRONLY, PERM);

      if (output_fd < 0)
        error("cannot write to %s", argv[argc]);
      break;

    case PIPE:
      doing_pipe = true;
      pipe(pipe_fd);
      output_fd = pipe_fd[1];

      /*FALLTHROUGH*/

    case AMPERSAND:
      foreground = false;

      /*FALLTHROUGH*/

    case NEWLINE:
    case SEMICOLON:

      if (argc == 0)
        return;

      argv[argc] = NULL;

      run_program(argv, argc, foreground, doing_pipe);
      if (doing_pipe) {
        // read stuff
        input_fd = pipe_fd[0];
        close(pipe_fd[1]);
      }
      input_fd = STDIN_FILENO;   // 0
      output_fd = STDOUT_FILENO; // 1
      argc = 0;

      if (type == NEWLINE)
        return;

      break;
    }
  }
}

/* init_search_path: make a list of directories to look for programs in. */
static void init_search_path(void) {
  char *dir_start;
  char *path;
  char *s;
  list_t *p;
  bool proceed;

  path = getenv("PATH");

  /* path may look like "/bin:/usr/bin:/usr/local/bin"
   * and this function makes a list with strings
   * "/bin" "usr/bin" "usr/local/bin"
   *
   */

  dir_start = malloc(1 + strlen(path));
  if (dir_start == NULL) {
    error("out of memory.");
    exit(1);
  }

  strcpy(dir_start, path);

  path_dir_list = NULL;

  if (path == NULL || *path == 0) {
    path_dir_list = new_list("");
    return;
  }

  proceed = true;

  while (proceed) {
    s = dir_start;
    while (*s != ':' && *s != 0)
      s++;
    if (*s == ':')
      *s = 0;
    else
      proceed = false;

    insert_last(&path_dir_list, dir_start);

    dir_start = s + 1;
  }

  p = path_dir_list;

  if (p == NULL)
    return;

#if 0
	do {
		printf("%s\n", (char*)p->data);
		p = p->succ;
	} while (p != path_dir_list);
#endif
}

/* main: main program of simple shell. */
int main(int argc, char **argv) {
  progname = argv[0];

  init_search_path();

  while (fetch_line("% ") != EOF)
    parse_line();

  return 0;
}

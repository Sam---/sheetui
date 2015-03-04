#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
const char spaces[256] = "                                                "
    "                                                                     "
    "                                                                     "
    "                                "
    "                                      ";
char header[256];

int header_color, normal_color, border_color;

struct {
    int row, col;
} position;
struct {
    struct {
        int col;
    } dim;
    int scrcols;
} fmt;
struct {
    int row, col;
} cursor;

int divup(int a, int m) {
    return a / m + a % m;
}

void rendercell(WINDOW *w, int row, int col, int rrow, int rcol) {
    wattron(w, COLOR_PAIR(normal_color));
    if (row == cursor.row && col == cursor.col) {
        wattron(w, A_REVERSE);
    }
    mvwprintw(w, rrow, rcol, "%*.d", fmt.dim.col, row * col);
    if (row == cursor.row && col == cursor.col) {
        wattroff(w, A_REVERSE);
    }
}

void render(WINDOW *w) {
    wattron(w, COLOR_PAIR(header_color));
    mvwprintw(w, 0, 0, "%.*s", COLS, spaces);
    mvwprintw(w, 0, 0, "%.*s", COLS, header);
    wattron(w, COLOR_PAIR(normal_color));
    mvwprintw(w, 1, 0, "%.*s", COLS, "Typebuf");
    wclrtoeol(w);
    wattron(w, COLOR_PAIR(border_color));
    mvwprintw(w, 2, 0, "%.*s", 5, spaces);
    char lt[2] = "_";
    for (int i = position.col; i < fmt.scrcols + position.col; i++) {
        wprintw(w, "%.*s", (fmt.dim.col - 1) / 2, spaces);
        lt[0] = i + 'A';
        waddstr(w, lt);
        wprintw(w, "%.*s", divup(fmt.dim.col - 1, 2), spaces);
    }
    char nbr[6];
    for (int i = 3; i < LINES; i++) {
        snprintf(nbr, 6, "%5d", i - 3 + position.row);
        mvwaddstr(w, i, 0, nbr);
    }
    for (int i = 0; i < LINES - 3; i++) {
        for (int j = position.col; j < position.col + fmt.scrcols; j++) {
            rendercell(w,
                    i + position.row,
                    j,
                    i + 3,
                    5 + (j - position.col) * fmt.dim.col
            );
        }
    }
}

void init(WINDOW *w) {
    strcpy(header, "Header");
    normal_color = 1;
    border_color = 2;
    header_color = 3;
    init_pair(normal_color, COLOR_WHITE, COLOR_BLACK);
    init_pair(border_color, COLOR_BLACK, COLOR_WHITE);
    init_pair(header_color, COLOR_MAGENTA, COLOR_GREEN);
    fmt.dim.col = 9;
    fmt.scrcols = (COLS - 5) / fmt.dim.col;
    position.col = 0;
    position.row = 0;
    cursor.row = 0;
    cursor.col = 0;
}

void cleanup(void) {
    nocbreak();
    echo();
    endwin();
}


int main() {
    initscr();
    atexit(cleanup);
    start_color();
    noecho();
    cbreak();

    init(stdscr);

    int k;
    do {
        render(stdscr);
        k = getch();
        switch (k) {
            case 'h':
                if (cursor.col > 0) {
                    cursor.col--;
                }
                break;
            case 'l':
                cursor.col++;
                break;
            case 'j':
                cursor.row++;
                break;
            case 'k':
                if (cursor.row > 0) {
                    cursor.row--;
                }
                break;
        }
        if (cursor.col < position.col) {
            position.col--;
            header[0] = '<';
        } else if (cursor.col >= position.col + fmt.scrcols) {
            position.col++;
            header[0] = '>';
        } else {
            header[0] = '-';
        }

        if (cursor.row < position.row) {
            position.row--;
            header[1] = '<';
        } else if (cursor.row >= position.row + LINES - 3) {
            position.row++;
            header[1] = '>';
        } else {
            header[1] = '-';
        }
    } while (k != 'q');

    return 0;
}


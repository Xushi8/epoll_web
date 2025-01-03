#include <ncurses.h>

int main()
{
    initscr();

    int scrLine, scrCol;
    getmaxyx(stdscr, scrLine, scrCol);     // 获取标准屏幕的行/列数
    move((scrLine / 2) - 1, (scrCol / 2) - 1); // 将光标移至屏幕中央
    printw("Hello World!");

    WINDOW* upwin = subwin(stdscr, scrLine / 2, scrCol, 0, 0);
    WINDOW* downwin = subwin(stdscr, scrLine / 2, scrCol, scrLine / 2, 0);

    box(upwin, '|', '+');
    box(downwin, '|', '+');
    touchwin(stdscr);

    wmove(downwin, 1, 1);
    // refresh();
    int a;
    wscanw(downwin, "%d", &a);

    getch();

    // 清理并退出 ncurses 环境
    endwin();
}
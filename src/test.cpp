#include <ncurses.h>

int main()
{
    // 初始化 ncurses
    initscr();            // 初始化 ncurses 环境
    raw();                // 禁用行缓冲
    keypad(stdscr, TRUE); // 启用特殊键的处理
    noecho();             // 禁止输入字符回显

    printw("Press any key to exit...\n"); // 在屏幕上打印提示信息
    refresh();                            // 刷新屏幕以显示内容

    int ch = getch(); // 等待用户输入一个字符

    // 根据用户输入做出响应
    if (ch == KEY_F(1))
    {
        printw("You pressed F1!\n");
    }
    else
    {
        printw("You pressed %c!\n", ch);
    }
    refresh();

    // 等待一段时间后退出
    getch();

    // 清理并退出 ncurses 环境
    endwin();

    return 0;
}

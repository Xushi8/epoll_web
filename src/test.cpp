#include <ncurses.h>
#include <vector>

int main()
{
    // 初始化 ncurses
    initscr();
    noecho();
    curs_set(0);

    // 示例二维数组的坐标点
    std::vector<std::pair<int, int>> points = {
        {1, 1}, {2, 2}, {3, 3}, {4, 4},
        {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9},
        {10, 10}, {11, 11}, {12, 12}, {13, 13}, {14, 14}};

    // 遍历坐标数组并打印每个点
    for (const auto& point : points)
    {
        int x = point.first;  // 获取 x 坐标
        int y = point.second; // 获取 y 坐标
        mvaddch(y, x, '*');   // 将点绘制在 (x, y) 位置
    }

    // 刷新屏幕以显示绘制的内容
    refresh();

    // 等待按键退出
    getch();
    endwin();

    return 0;
}

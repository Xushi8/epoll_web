include(FindPackageHandleStandardArgs)

# 查找头文件
find_path(NCURSES_INCLUDE_DIR NAMES ncurses.h)

# 查找不同的库
find_library(FORM_LIB form libform formw libformw)
find_library(MENU_LIB menu libmenu menuw libmenuw)
find_library(NCURSES_LIB ncurses libncurses ncursesw libncursesw)
find_library(PANEL_LIB panel libpanel panelw libpanelw)
find_library(TIC_LIB tic libtic)
find_library(TINFO_LIB tinfo libtinfo)
find_library(CURSES_LIB curses libcurses)
find_library(TERM_CAP_LIB termcap libtermcap)

# 汇总所有找到的库
set(NCURSES_LIBRARIES ${FORM_LIB} ${MENU_LIB} ${NCURSES_LIB} ${PANEL_LIB} ${TIC_LIB} ${TINFO_LIB} ${CURSES_LIB} ${TERM_CAP_LIB})

# 检查是否找到了必要的头文件和库
if(NOT NCURSES_INCLUDE_DIR OR NOT NCURSES_LIBRARIES)
    find_package_handle_standard_args(NCURSES REQUIRED_VARS NCURSES_INCLUDE_DIR NCURSES_LIBRARIES)
    mark_as_advanced(NCURSES_INCLUDE_DIR NCURSES_LIBRARIES)
    return()
endif()

set(NCURSES_FOUND TRUE)

# 创建 INTERFACE 库
add_library(NCURSES::ncurses INTERFACE IMPORTED)
set_target_properties(NCURSES::ncurses PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${NCURSES_INCLUDE_DIR}"
    INTERFACE_LINK_LIBRARIES "${NCURSES_LIBRARIES}"
)

# 处理标准参数
find_package_handle_standard_args(NCURSES DEFAULT_MSG NCURSES_INCLUDE_DIR NCURSES_LIBRARIES)

mark_as_advanced(NCURSES_INCLUDE_DIR NCURSES_LIBRARIES)

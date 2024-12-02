#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int port = 7777;
    /* 创建监听socket文件描述符 */
    int listenfd = socket(AF_INET6, SOCK_STREAM, 0);
    /* 创建监听socket的TCP/IP的IPV6 socket地址 */
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET6;
    address.sin_addr.s_addr = htonl(INADDR_ANY); /* INADDR_ANY：将套接字绑定到所有可用的接口 */
    address.sin_port = htons(port);

    int flag = 1;
    /* SO_REUSEADDR 允许端口被重复使用 */
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    /* 绑定socket和它的地址 */
    int ret;
    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    /* 创建监听队列以存放待处理的客户连接，在这些客户连接被accept()之前 */
    ret = listen(listenfd, 5);
}
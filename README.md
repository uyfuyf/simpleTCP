用C语言实现简单的socket通信
服务端尝试了多进程(tcpServerFork.c)、非阻塞(tcpServerNonBlock.c)、epoll
客户端(tcpClient.c)
TCP SERVER：
1.socket() 创建socket
2.bind()   绑定socket、服务端ip和端口
3.listen() 监听外部请求
4.accetp() 等待客户端连接
5.read()、write() 收发消息
6.close()   关闭客户端链接

TCP CLIENT:
1.socket()  创建socket
2.bind()    绑定socket、客户端ip和端口
3.connect() 连接服务端
4.read()、write()   收发消息
5.close()   关闭链接
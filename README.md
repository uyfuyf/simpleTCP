用C语言实现简单的socket通信,服务端尝试了多进程(tcpServerFork.c)、非阻塞(tcpServerNonBlock.c)、epoll
客户端(tcpClient.c)\n
TCP SERVER：\n
1.socket() 创建socket\n
2.bind()   绑定socket、服务端ip和端口\n
3.listen() 监听外部请求\n
4.accetp() 等待客户端连接\n
5.read()、write() 收发消息\n
6.close()   关闭客户端链接\n

TCP CLIENT:\n
1.socket()  创建socket\n
2.bind()    绑定socket、客户端ip和端口\n
3.connect() 连接服务端\n
4.read()、write()   收发消息\n
5.close()   关闭链接\n

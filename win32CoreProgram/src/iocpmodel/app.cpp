#include "app.h"

App::App()
{
    _iocpServer.SetOnConnectedCallback(std::bind(&App::onClientConnected, this,
                                        std::placeholders::_1));
    _iocpServer.SetOnDisconnectedCallback(std::bind(&App::OnClientDisconnected,
                                        this, std::placeholders::_1));
    _iocpServer.SetOnReadCallback(std::bind(&App::OnRead, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    _iocpServer.SetOnWriteCallback(std::bind(&App::OnWrite,
            this,std::placeholders::_1, std::placeholders::_2));
}

App::~App()
{
}

void App::onClientConnected(const Connection *conn)
{
    fprintf(stderr, "�¿ͻ�������\n");
    _iocpServer.AsyncRead(conn);
}

void App::OnClientDisconnected(const Connection *conn)
{
    auto variable_conn = const_cast<Connection *>(conn);
    SOCKET clifd = variable_conn->GetSocket();
    fprintf(stderr, "client:%d �Ͽ�\n", clifd);
}

void App::OnRead(const Connection *con , void *data, std::size_t size)
{
    char *value = reinterpret_cast<char *>(data);
    //���ǲ�Ӧ����ôд�����ܻᳬ��ReadBuffer�ĳ���
	//value[size] = '\0';
    
    // �����const�޶�����GetSocket()�Ͳ��ܵ��ã���Ϊ���ķ���ֵ�Ǹ�����
    auto variable_conn = const_cast<Connection *>(con);

    SOCKET clifd = variable_conn->GetSocket();
    fprintf(stderr, "client:%d, msg:%s\n", clifd, value);

    //ECHO���ܣ����ͻ��˷��ͻ�ȥ
    _iocpServer.AsyncWrite(con, value, size);
}

void App::OnWrite(const Connection *conn, std::size_t bytes_transferred)
{
    // �����첽��ȡ�ͻ���
    _iocpServer.AsyncRead(conn);
}

void App::Run(const char *ip, unsigned short port, unsigned int nListen)
{
    _iocpServer.Run(ip, port, nListen);
}

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
    // _iocpServer.AsyncWrite(conn, nullptr, 0);
    _iocpServer._client_count++;
}

void App::OnRead(const Connection *con , void *data, std::size_t size)
{
    char *value = reinterpret_cast<char *>(data);
    //���ǲ�Ӧ����ôд�����ܻᳬ��ReadBuffer�ĳ���
	//value[size] = '\0';
    
    // �����const�޶�����GetSocket()�Ͳ��ܵ��ã���Ϊ���ķ���ֵ�Ǹ�����
    auto variable_conn = const_cast<Connection *>(con);

    // SOCKET clifd = variable_conn->GetSocket();
    // fprintf(stderr, "client:%d, msg:%s\n", clifd, value);


    MessageHeader *msghead = nullptr;
    int msgpos = variable_conn->GetMsgPos();
    memcpy(variable_conn->GetMsgBuffer() + msgpos, variable_conn->GetReadBuffer(), size);
    variable_conn->SetMsgPos(msgpos + size);
    while (variable_conn->GetMsgPos() >= sizeof(MessageHeader)) {
        msghead = (MessageHeader *)variable_conn->GetMsgBuffer();
        if (msghead->length <= variable_conn->GetMsgPos()) {
            variable_conn->SetMsgPos(variable_conn->GetMsgPos()-msghead->length);
            ProcessMessage(msghead, variable_conn);
            memcpy(variable_conn->GetMsgBuffer(), variable_conn->GetMsgBuffer()+msghead->length,
                variable_conn->GetMsgPos());
        } else {
            break;
        }
    }

    //ECHO���ܣ����ͻ��˷��ͻ�ȥ
    // _iocpServer.AsyncWrite(con, value, size);
    _iocpServer.AsyncRead(con);
}

void App::OnClientDisconnected(const Connection *conn)
{
    auto variable_conn = const_cast<Connection *>(conn);
    SOCKET clifd = variable_conn->GetSocket();
    fprintf(stderr, "client:%d �Ͽ�\n", clifd);
    _iocpServer._client_count--;
}


void App::OnWrite(const Connection *conn, std::size_t bytes_transferred)
{
    // �����첽��ȡ�ͻ���
    _iocpServer.AsyncRead(conn);
    // _iocpServer.AsyncWrite(conn, nullptr, 0);
}

void App::Run(const char *ip, unsigned short port, unsigned int nListen)
{
    _iocpServer.Run(ip, port, nListen);
}

void App::ProcessMessage(MessageHeader *msgHeader, Connection *conn)
{
    MessageHeader *send_msg = nullptr;
    switch(msgHeader->type)
    {
    case T_Login:
    {
        /*
        _iocpServer._msg_count++;
        
        if (_iocpServer._chrono_timer.GetSecInterval() > 1) {
            fprintf(stderr, "client count<%d> msg count <%d> \n", _iocpServer._client_count, _iocpServer._msg_count);
            _iocpServer._msg_count = 0;
            _iocpServer._chrono_timer.FlushTime();
        }

        return;
        */
        Login *login = (Login *)msgHeader;
        fprintf(stderr, "client socket<%d> ID<%d> T_Login name(%s), password(%s) ���ݳ���:%d\n",
                conn->GetSocket(), conn->GetID(), login->name, login->passwrod, login->length);
        send_msg = new Login_Result();
    }
        break;
    case T_Logout:
    {
        Logout *logout = (Logout *) msgHeader;
        fprintf(stderr, "client socket<%d> T_Logout name(%s), ���ݳ���:%d\n", conn->GetSocket(),
                logout->name, logout->length); 
        send_msg = new LogOut_Result();
    }
        break;
    case T_ERROR:
        fprintf(stderr, "T_ERROR\n");
        break;
    default:
        fprintf(stderr, "δ֪��Ϣ\n");
        break;
    }

    if (nullptr != send_msg) {
        _iocpServer.AsyncWrite(conn, send_msg, send_msg->length);
    }
}

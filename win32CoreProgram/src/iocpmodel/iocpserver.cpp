#include "iocpserver.h"
#include "overlapped.h"
#include "socketexfunshunter.h"
#include "worker.h"

#include <memory>
#include <WinSock2.h>
#include <MSWSock.h>

IocpServer::IocpServer()
{
    _wsa_inited = false;
    _socket     = INVALID_SOCKET;
    _msg_count  = 0;
    _client_count = 0;
}

IocpServer::~IocpServer()
{
}

int IocpServer::Init(const char *ip, unsigned short port, unsigned int nListen)
{
    int ret = 0;
    do {
        ret = WinSockInit();
    
        if (-1 == ret) {
            fprintf(stderr, "��ʼ��WinSockInitʧ��\n");
            break;
        }

        _completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
        if (! _completion_port) {
            fprintf(stderr, "������ɶ˿�ʧ��\n");
            ret = -1;
            break;
        }

        if (-1 == (ret = InitSocket())) {
            break;
        }

        if (-1 == (ret = Bind(ip, port))) {
            break;
        }

        if (-1 == (ret = Listen(nListen))) {
            break;
        }

        /*
        if (-1 == Accept()) {
            break;
        }
        */

       SocketExFnsHunter _socketExFunsHunter; 
       _acceptex_func = _socketExFunsHunter.AcceptEx;

        Workers *workers = new Workers(this); 
        workers->Start();

    } while(0);

    return ret;
}

int IocpServer::WinSockInit()
{
    int ret = -1;

    do {
        WORD version = MAKEWORD(2, 2);
        WSADATA wsaData;
        _wsa_inited = WSAStartup(version, &wsaData);
        if (_wsa_inited) { break; }

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            WSACleanup();
            _wsa_inited = false;
            break;
        }
        ret = 0;
    } while(0);

    return ret;
}

int IocpServer::InitSocket()
{
    int ret = 0;

    do {
        // �����������׽��֣�����Ҫע�����һ����������Ϊ��WSA_FLAG_OVERLAPPED
        _socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

        if (_socket == INVALID_SOCKET) {
            fprintf(stderr, "����WSASocket listnesocketʧ��\n");
            ret = -1;
            break;
        }

        if (! CreateIoCompletionPort(reinterpret_cast<HANDLE>(_socket), _completion_port, 0, 0)) {
            fprintf(stderr, "��listen socket��������ɶ˿�ʧ��\n");
            ret = -1;
            break;
        }

    } while(0);

    return ret;
}


int IocpServer::Bind(const char *ip, unsigned short port)
{
    SOCKADDR_IN addr;
    addr.sin_addr.S_un.S_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (::bind(_socket, (SOCKADDR *)&addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        fprintf(stderr, "bind ʧ��\n");
        return -1;
    }

    return 0;
}

int IocpServer::Listen(unsigned int nListen)
{
    if (::listen(_socket, nListen) == SOCKET_ERROR) {
        fprintf(stderr, "listen ʧ��\n");
        return -1;
    }

    return 0;
}


int IocpServer::Accept()
{
    int ret = -1;
    do {
		//�������ǲ���WSAAccept��
		//��Ϊ�������ڲ�ѯ��ɶ˿ڵ�ʱ���жϳ�����accept��read��connect��write����

		//WSAIoctl��ȡacceptex�ĺ�����ַ
		//���AcceptEx������ָ��

#if 0
        LPFN_ACCEPTEX _acceptex_func;
        GUID acceptex_guid = WSAID_ACCEPTEX;
        DWORD bytes_returned;

        ret = WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &acceptex_guid, sizeof(acceptex_guid),
            &_acceptex_func, sizeof(_acceptex_func),
            &bytes_returned, NULL, NULL);

        if (0 != ret) {
            ret = -1;
            fprintf(stderr, "��ȡAcceptEx������ַʧ��\n");
            break;
        }

#endif
        SOCKET accept_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (accept_socket == INVALID_SOCKET) {
            fprintf(stderr, "��ʼ��accept socketʧ��\n");
            ret = -1;
            break;
        }

		//��������Overlapped���ͣ���ʱ��ѯ��ɶ˿ڵ�ʱ�������������
        std::unique_ptr<Connection> new_connection(new Connection(accept_socket));

        DWORD bytes = 0;
        const int accept_ex_result = _acceptex_func(_socket, accept_socket,
                                    new_connection->GetReadBuffer(),
                                    0,
                                    sizeof(sockaddr_in) + 16, 
                                    sizeof(sockaddr_in) + 16,
                                    &bytes,
                                    reinterpret_cast<LPOVERLAPPED>
                                    (new_connection->GetAcceptOverlapped()));

        if (! (accept_ex_result == TRUE || WSAGetLastError() == WSA_IO_PENDING)) {
            ret = -1;
            fprintf(stderr, "����AcceptEx ����ʧ��\n");
            break;
        }

        // ��accpet_socket��������ɶ˿�
        CreateIoCompletionPort(reinterpret_cast<HANDLE>(accept_socket),
                                _completion_port, 0, 0);

        /*
        std::unique_lock<mutex> gurad(mMutex);
        mConnectionVec.push_back(new_connection.get());
        gurad.unlock();
        */
        new_connection.release();

    } while (0);

    return ret;
}
void IocpServer::Run(const char *ip, unsigned short port, unsigned int nListen)
{
    if (Init(ip, port, nListen) == -1) {
        fprintf(stderr, "����������ʧ��\n");
        return;
    }

    Mainloop();
}

void IocpServer::Mainloop()
{
    //�����й������߳�ȥ����ҵ������ѭ����������������������
	for (;;)
	{
		if (_chrono_timer.GetSecInterval() > 1.0)
		{
			fprintf(stderr, "client count<%d> msg count<%d>\n", _client_count, _msg_count);
			_chrono_timer.FlushTime();
			_msg_count = 0;
		}
	}


    /*
    DWORD bytes_transferred;
    ULONG_PTR completion_key;
    DWORD Flags = 0;
    Overlapped *overlapped = nullptr;

    while (1) {
        bool bRet = GetQueuedCompletionStatus(_completion_port,
                                 &bytes_transferred,
                                 &completion_key,
                                 reinterpret_cast<LPOVERLAPPED*>(&overlapped),
                                 INFINITE);

        if (bRet == false) {
            //�ͷ���ֱ���˳���û�е���closesocket�����ر�����
            if (WAIT_TIMEOUT == GetLastError() || 
                ERROR_NETNAME_DELETED == GetLastError()) {

                // �ͻ��˶Ͽ�
                // fprintf(stderr, "client:%d �Ͽ�\n", 
                        // overlapped->connection->GetSocket());
 
                // delete overlapped->connection;
                // overlapped = nullptr;
                // continue;
            }
        }

        if (overlapped->type == Overlapped::Accept_Type) {
            //acceptex����˲������������ǻ�Ҫ�����������ɶ˿ڡ�
			//�����Ȳ����죬�Ⱥ������ǻ�����Ż�����
			//����Ҳ������Ӷ��accept����ɶ�

            Accept();
            // �¿ͻ�������
            // fprintf(stderr, "�¿ͻ��˼���\n");
            // AsyncRead(overlapped->connection);

            if (OnConnected) { OnConnected(overlapped->connection); }

            continue;
        }

        if (bytes_transferred == 0) {
            // �¿ͻ�������
            fprintf(stderr, "client:%d �Ͽ�\n", 
                    overlapped->connection->GetSocket());

            if (OnDisconnected) { OnDisconnected(overlapped->connection); }

            //TODO �Ȳ������ڴ����
            // delete overlapped->connection;
            // overlapped = nullptr;
            // continue;
        }

        if (overlapped->type == Overlapped::Type::Read_Type) {
            // �첽�����
            if (OnRead) { 
                OnRead(overlapped->connection, 
                       overlapped->connection->GetReadBuffer(),
                       bytes_transferred); 
            }
            continue;
        }

        if (overlapped->type == Overlapped::Type::Write_Type) {

            auto conn = overlapped->connection;
            conn->SetSentBytes(conn->GetSentBytes() + bytes_transferred);
            fprintf(stdout, "write type, client:%d, connect ID:%d\n",
                    overlapped->connection->GetSocket(),
                    overlapped->connection->GetID());

            // �ж��Ƿ�ֻ������һ����
            if (conn->GetSentBytes() < conn->GetTotalBytes()) {
                // ��ʣ��Ĳ����ڷ���
                overlapped->wsa_buf.len = conn->GetTotalBytes() - conn->GetSentBytes();
                overlapped->wsa_buf.buf = reinterpret_cast<CHAR *>(
                                conn->GetWriteBuffer()) + conn->GetSentBytes();

                auto send_result = WSASend(conn->GetSocket(),
                                            &overlapped->wsa_buf, 1,
                                            &bytes_transferred,
                                            0,
                                reinterpret_cast<LPWSAOVERLAPPED>(overlapped),
                                            NULL);
                if (!(send_result == NULL || 
                        (send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))) {
                    fprintf(stderr, "��������ʧ��\n");
                }
            } else {
                // �������
                // AsyncRead(overlapped->connection);
                // conn->Clear();    

                if (OnWrite) { 
                    OnWrite(overlapped->connection, bytes_transferred); 
                }
            }
        }

    }
    */
}

void IocpServer::AsyncRead(const Connection *conn)
{
    auto overlapped = conn->GetReadOverlapped();
    overlapped->wsa_buf.len = overlapped->connection->GetReadBufferSize();
    overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(overlapped->connection->GetReadBuffer());
    
    DWORD flags = 0;
    DWORD bytes_transferred = 0;

    // ��������ȡ���ݣ���ʱ�ں˽������Ƶ�Ӧ�ò�
    auto recv_result = WSARecv(overlapped->connection->GetSocket(),
                               &overlapped->wsa_buf, 1,
                               &bytes_transferred, &flags,
                               reinterpret_cast<LPWSAOVERLAPPED>(overlapped), 
                               NULL);

    if (!(recv_result == 0 || 
         (recv_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))) {
        fprintf(stderr, "��������ʧ��<AsyncRead>ʧ��\n");
    }
}

void IocpServer::AsyncWrite(const Connection *conn, void *data, std::size_t size)
{
    /*
    std::unique_lock<mutex> guard(mMutex);
    do { 
        for (auto item : mConnectionVec) {
            auto mutable_conn = const_cast<Connection *>(item);

            char *send_msg = "I am server...";
            auto overlapped = mutable_conn->GetWriteOverlapped();
            overlapped->wsa_buff.len = strlen(send_msg);
            overlapped->wsa_buff.buf = send_msg;// reinterpret_cast<CHAR*>(mutable_conn->GetWriteBuffer());

            DWORD bytes;
            auto send_result = WSASend(mutable_conn->GetSocket(),
                                        &overlapped->wsa_buff, 1,
                                        &bytes, 0,
                                        reinterpret_cast<LPWSAOVERLAPPED>(overlapped),
                                        NULL);
        }
        
        if (mConnectionVec.size() >= 10 && guard.owns_lock()) { 
            guard.unlock(); 
        }

    } while (mConnectionVec.size() >= 10);

    if (guard.owns_lock()) { guard.unlock(); }
    */
    
    auto mutable_conn = const_cast<Connection *>(conn);
    if (mutable_conn->GetWriteBufferSize() < size) {
        mutable_conn->ResizeWriteBuffer(size);
    }

    memcpy_s(mutable_conn->GetWriteBuffer(), 
                mutable_conn->GetWriteBufferSize(), data, size);
    mutable_conn->SetSentBytes(0);
    mutable_conn->SetTotalBytes(size);

    auto overlapped = mutable_conn->GetWriteOverlapped();
    overlapped->wsa_buf.len = size;
    overlapped->wsa_buf.buf = reinterpret_cast<CHAR*>(mutable_conn->GetWriteBuffer());

    DWORD bytes;
    auto send_result = WSASend(mutable_conn->GetSocket(),
                                &overlapped->wsa_buf, 1,
                                &bytes, 0,
                                reinterpret_cast<LPWSAOVERLAPPED>(overlapped),
                                NULL);
    if (!(send_result == 0 || 
        (send_result == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING))) {
        fprintf(stderr, "��������ʧ��\n");
    }
}

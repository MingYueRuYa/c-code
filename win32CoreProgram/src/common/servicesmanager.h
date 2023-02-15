/****************************************************************************
**
** Copyright (C) 2018 635672377@qq.com
** All rights reserved.
**
****************************************************************************/

#ifndef servicesmanager_h
#define servicesmanager_h

#include <windows.h>

#include <map>
#include <memory>
#include <string>

#include "servicewrap.h"
#include "singleton.h"

using std::map;
using std::shared_ptr;
using std::wstring;

namespace XIBAO {

/*! \brief common::servicesmanager
        \author liushixiong (liushixiongcpp@163.cn)
        \version 0.01
        \description ����Ŀ��ƣ�֧�ֹ�����service״̬��֧��״̬�ص�
        \date 2018-12-3 15:24:32
        \version 0.02
        \udpate: ������̬����������̬����ע�ᵽϵͳ������ͨ�������ҵ�SM
        \date 2018-12-11 17:54:12
*/
class ServicesManager : SINGLETON_INHERIT(ServicesManager) {
  DECLARE_PRIVATE_CONSTRUCTOR(ServicesManager, void)
  DECLARE_FRIEND_SINGLETON(ServicesManager)

 public:
  typedef enum SMErrorCode {
    SM_FAILED = -100,
    SM_SERVICEWRAP_EXIST,
    SM_SERVICEWRAP_NOT_EXIST,
    SM_SERVICEMAP_EMPTY,
    // ��������µ�enumֵ
    SM_SUCCESS = 0,
  };

 public:
  static void WINAPI _ServiceMain(_In_ DWORD dwArgc,
                                  _In_reads_(dwArgc)
                                      _Deref_pre_z_ LPWSTR *lpszArgv) throw();
  static void WINAPI _ServiceCtrlHandler(DWORD Opcode);
  // �Ƴ�����
  static SMErrorCode RemoveService(const wstring &serviceName);
  // ��ֹ����
  static SMErrorCode HaltService(const wstring &serviceName);
  // ��ѯ����״̬
  static SMErrorCode QueryServiceStatus(const wstring &serviceName,
                                        DWORD &status);

 private:
  static DWORD DeleteServiceReg(const wstring &wstrServiceName);

 public:
  ~ServicesManager();

 public:
  // ��ʼ
  SMErrorCode Start(const wstring &servicename);
  // ��������
  SMErrorCode InstallService(const wstring &wstrName);
  // ��������
  SMErrorCode StartService(const wstring &wstrName, DWORD argc, LPCWSTR *argv);
  // �жϷ���
  SMErrorCode PauseService(const wstring &serviceName);
  // �ָ�����
  SMErrorCode ResumeService(const wstring &serviceName);

  // service������
  void ServiceMain(_In_ DWORD dwArgc,
                   _In_reads_(dwArgc) _Deref_pre_z_ LPWSTR *lpszArgv);
  void ServiceCtrlHandler(DWORD Opcode);
  SMErrorCode AddServiceWrap(shared_ptr<ServiceWrap> serviceWrap);
  SMErrorCode DeleteServiceWrap(shared_ptr<ServiceWrap> serviceWrap);
  SMErrorCode DeleteService(const wstring &wstrName);

 private:
  SMErrorCode GetServiceWrap(const wstring &ServiceName,
                             shared_ptr<ServiceWrap> &serviceWrap);
  SMErrorCode _FindServiceWrap(const wstring &wstrServiceName);

 private:
  wstring mCurServiceName;
  map<wstring, shared_ptr<ServiceWrap>> mServiceMap;
};

}  // namespace XIBAO

#endif  // servicesmanager_h
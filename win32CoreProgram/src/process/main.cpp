#include <windows.h>
#include <tchar.h>

BOOL PromotePrivilege(BOOL p_IsPromotion, LPCTSTR p_lpszPriviliges)
{
	HANDLE hToken = NULL;
    HANDLE processhandle = GetCurrentProcess();
	if (! OpenProcessToken(processhandle, TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        _tprintf(TEXT("OpenProcessToken error:%u.\n"), GetLastError());
        return FALSE;
    }

    LUID luid;
    //NULL loopup privilege on local system
    //privilege to lookup
    //receives LUID of privilege
    if (! LookupPrivilegeValue(NULL, p_lpszPriviliges, &luid)) {
        _tprintf(TEXT("Loopup PrivilegeValue error:%u.\n"), GetLastError());
        return FALSE; 
    }

    TOKEN_PRIVILEGES tokenpriviliges = {0};
    tokenpriviliges.PrivilegeCount = 1;
    tokenpriviliges.Privileges[0].Luid = luid;
    tokenpriviliges.Privileges[0].Attributes = p_IsPromotion ? SE_PRIVILEGE_ENABLED : 0;

    //enable the privilege or disable all privilege
    if (! AdjustTokenPrivileges(hToken, FALSE, &tokenpriviliges, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
        _tprintf(TEXT("AdjustTokenPriviliges error:%u.\n"), GetLastError());
        return FALSE;
    }

    if (ERROR_NOT_ALL_ASSIGNED == GetLastError()) {
        _tprintf(TEXT("The token dose not have the specified privilege.\n"));
        return FALSE;
    }
	return TRUE;
}


int _tmain02(int argc, char *argv[])
{
    if (PromotePrivilege(TRUE, SE_DEBUG_NAME)) {
        _tprintf(TEXT("Promotion privilege ok!!!"));
    }
    _gettchar();
	return 0;
}

#include <iostream>
#include <windows.h>
    using namespace std;

    int main01()
    {
        int DiskCount = 0;
        DWORD DiskInfo= GetLogicalDrives();
        //����GetLogicalDrives()�������Ի�ȡϵͳ���߼����������������������ص���һ��32λ�޷����������ݡ�
        while (DiskInfo)//ͨ��ѭ�������鿴ÿһλ�����Ƿ�Ϊ1������Ϊ1�����Ϊ��,����Ϊ0����̲����ڡ�
        {
            if (DiskInfo & 1)//ͨ��λ������߼���������ж��Ƿ�Ϊ1
            {
                ++DiskCount;
            }
            DiskInfo = DiskInfo >> 1;//ͨ��λ��������Ʋ�����֤ÿѭ��һ��������λ�������ƶ�һλ��
            //DiskInfo = DiskInfo/2;
        }
        cout << "�߼���������:" << DiskCount << endl;
        //-------------------------------------------------------------------

        int DSLength = GetLogicalDriveStrings(0, NULL);
        //ͨ��GetLogicalDriveStrings()������ȡ�����������ַ�����Ϣ���ȡ�
        TCHAR* DStr = new TCHAR[DSLength];//�û�ȡ�ĳ����ڶ�������һ��c�����ַ�������
        GetLogicalDriveStrings(DSLength, (LPTSTR)DStr);
        //ͨ��GetLogicalDriveStrings���ַ�����Ϣ���Ƶ�����������,���б�������������������Ϣ��

        int DType;
        int si = 0;
        BOOL fResult;
        unsigned _int64 i64FreeBytesToCaller;
        unsigned _int64 i64TotalBytes;
        unsigned _int64 i64FreeBytes;
        //string strdiskinfo(DStr);
        for (int i = 0; i < DSLength / 4; ++i)
            //Ϊ����ʾÿ����������״̬����ͨ��ѭ�����ʵ�֣�����DStr�ڲ������������A:\NULLB:\NULLC:\NULL����������Ϣ������DSLength/4���Ի�þ����ѭ����Χ
        {
            //char dir[3] = { DStr[si], ':', '\\' };
            //char dir[3] = { DStr[si], ' ', ' ' };
            //cout << dir;
            char ch = (char)DStr[si];
            cout << (char)DStr[si];
            /*
            DType = GetDriveType((LPCTSTR)DStr + i * 4);
            //GetDriveType���������Ի�ȡ���������ͣ�����Ϊ�������ĸ�Ŀ¼
            if (DType == DRIVE_FIXED)
            {
                cout << "Ӳ��";
            }
            else if (DType == DRIVE_CDROM)
            {
                cout << "����";
            }
            else if (DType == DRIVE_REMOVABLE)
            {
                cout << "���ƶ�ʽ����";
            }
            else if (DType == DRIVE_REMOTE)
            {
                cout << "�������";
            }
            else if (DType == DRIVE_RAMDISK)
            {

                cout << "����RAM����";
            }
            else if (DType == DRIVE_UNKNOWN)
            {
                cout << "δ֪�豸";
            }

            fResult = GetDiskFreeSpaceEx(
                (LPCTSTR)dir,
                (PULARGE_INTEGER)&i64FreeBytesToCaller,
                (PULARGE_INTEGER)&i64TotalBytes,
                (PULARGE_INTEGER)&i64FreeBytes);
            //GetDiskFreeSpaceEx���������Ի�ȡ���������̵Ŀռ�״̬,�������ص��Ǹ�BOOL��������
            if (fResult)//ͨ�����ص�BOOL�����ж��������Ƿ��ڹ���״̬
            {
                //cout << " totalspace:" << (float)i64TotalBytes / 1024 / 1024 << " MB";//����������
                //cout << " freespace:" << (float)i64FreeBytesToCaller / 1024 / 1024 << " MB";//����ʣ��ռ�
            }
            else
            {
                cout << " �豸δԤ����";
            }
            */
            cout << endl;
            si += 4;
        }

        system("pause");
        return 0;
    }
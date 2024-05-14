#ifndef _SHM_HPP_ // ͷ�ļ�ֻ�ܰ���һ��
#define _SHM_HPP_

#include <iostream>
#include <string>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// ����ʹ���ߵı�ʶ
const int gCreater = 1;
const int gUser = 2;

// �Զ���ȫ��ȫ�ֲ�������������ͨ�ŵĽ�������ͬ����key
const std::string gpathname = "/home/ubuntu/2024.5.13";
const int gproj_id = 0x66;

// �Զ��干���ڴ�ռ�Ĵ�С,һ��Ϊ4096 * n
const int gShmSize = 4096;

class Shm
{
    // ��ϣ������Щ�ӿڱ�¶���û����û�ֻ��Ҫʹ�ü���
private:
    // ��ȡkey
    key_t GetCommKey()
    {
        key_t k = ftok(_pathname.c_str(), _proj_id);
        if (k < 0)
        {
            perror("ftok");
        }
        return k;
    }

    // ���������ڴ�ռ�
    int GetShmHelper(key_t key, int size, int flag)
    {
        int shmid = shmget(key, size, flag);
        if (shmid < 0)
        {
            perror("shmget");
        }
        return shmid;
    }

    // ����ɫת��Ϊ�ַ���
    std::string RoleToString(int who)
    {
        if (who == gCreater)
            return "Creater";
        else if (who == gUser)
            return "User";
        else
            return "None";
    }

    // �������ڴ�ռ�ҽӵ����̵�ַ�ռ�,���øú����൱�ڵ���malloc����
    void* AttachShm()
    {
        if (_addrshm != nullptr)
            DatechShm(_addrshm);

        std::cout
            << "���йҽ�" << std::endl;
        void* shmaddr = shmat(_shmid, nullptr, 0);
        if (shmaddr == nullptr) // �ҽ�ʧ��
        {
            perror("shmat");
        }

        std::cout << "who: " << RoleToString(_who) << " attach shm..." << std::endl;
        return shmaddr;
    }

    // ����ҽ�
    void DatechShm(void* shmaddr)
    {
        std::cout << "����ҽ�" << std::endl;
        if (shmaddr == nullptr)
            return;
        shmdt(shmaddr);
        std::cout << "who: " << RoleToString(_who) << " detach shm..." << std::endl;
    }

public:
    // ���캯��
    Shm(const std::string& pathname, int proj_id, int who)
        : _pathname(pathname), _proj_id(proj_id), _who(who), _addrshm(nullptr)
    {
        _key = GetCommKey();
        if (_who == gCreater) // ����Ǵ����߾͵���ר��Ϊ������׼���ĺ���GetShmForCreater
            GetShmForCreater();
        else if (_who == gUser) // �����ʹ���߾͵���ר��Ϊʹ����׼���ĺ���GetShmForUser
            GetShmForUser();
        std::cout << "shmid: " << _shmid << std::endl;
        std::cout << "_key: " << ToHex(_key) << std::endl;
        if (_who == gUser)
        {
            std::cout << "shm get done..." << std::endl;
        }
        // ������˭ֻҪ��ȡ��һ�������ڴ�ռ��shmid���ͽ��ù����ڴ�ռ�ҽӵ����̵�ַ�ռ���
        _addrshm = AttachShm();
    }

    // ��������:ɾ�������ڴ�ռ�
    ~Shm()
    {
        if (_who == gCreater)
        {
            int res = shmctl(_shmid, IPC_RMID, nullptr);
            std::cout << "shm remove done..." << std::endl;
        }
    }

    // ��keyת��Ϊ16���ƣ����ں����Ĳ鿴
    std::string ToHex(key_t key)
    {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "0x%x", _key);
        return buffer;
    }

    // ����˴��������ڴ�ռ�
    bool GetShmForCreater()
    {
        _shmid = GetShmHelper(_key, gShmSize, IPC_CREAT | IPC_EXCL | 0666); // ���û�оʹ�����������˾ͷ��ش��󣬼�û�з��ش�����ô���Ǵ�����һ���µĹ����ڴ�ռ�
        if (_shmid >= 0)                                                    // 0666��ʾ�����ߡ�������������û��Թ����ڴ�ռ䶼�ж�дȨ�ޣ���û��ִ��Ȩ��
        {
            std::cout << "shm creat done..." << std::endl;
            return true; // �����ɹ�����true
        }
        std::cout << "shm creat fail..." << std::endl;
        return false;
    }

    // �ͻ���ʹ�ù����ڴ�
    bool GetShmForUser()
    {
        _shmid = GetShmHelper(_key, gShmSize, IPC_CREAT | 0666); // ʹ���߸����봴����һ����_key������ݴ����ߴ���Ĺ����ڴ�ռ��shmid,����IPC_CREAT����Ϊ��ʱ������Ѿ��������˹����ڴ�ռ��ʱ�û���ֻ��Ҫ�ҵ�����Ӧ�Ĺ����ڴ�ռ��shmind����
        if (_shmid >= 0)
        {
            return true; // ��ȡ�ɹ�����true
        }
        std::cout << "shm get fail..." << std::endl;
        return false;
    }

    // ��ȡ���̿ռ�ĵ�ַ
    void* Addr()
    {
        return _addrshm;
    }

    // ��չ����ڴ�ռ�
    void Zero()
    {
        if (_addrshm)
        {
            memset(_addrshm, 0, gShmSize);
        }
    }

    //  ��ȡ�����ڴ�ռ����Խṹ���е�����
    void DebugShm()
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid, IPC_STAT, &ds); //
        if (n < 0)
            return;
        std::cout << "ds.shm_perm.__key : " << ToHex(ds.shm_perm.__key) << std::endl;
        std::cout << "ds.shm_nattch: " << ds.shm_nattch << std::endl;
    }

private:
    key_t _key;
    int _shmid;

    std::string _pathname;
    int _proj_id;
    int _who;

    void* _addrshm;
};

#endif
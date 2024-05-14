#ifndef _SHM_HPP_ // 头文件只能包含一次
#define _SHM_HPP_

#include <iostream>
#include <string>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

// 创建使用者的标识
const int gCreater = 1;
const int gUser = 2;

// 自定义全局全局参数用于让两个通信的进程生成同样的key
const std::string gpathname = "/home/ubuntu/2024.5.13";
const int gproj_id = 0x66;

// 自定义共享内存空间的大小,一般为4096 * n
const int gShmSize = 4096;

class Shm
{
    // 不希望将这些接口暴露给用户，用户只需要使用即可
private:
    // 获取key
    key_t GetCommKey()
    {
        key_t k = ftok(_pathname.c_str(), _proj_id);
        if (k < 0)
        {
            perror("ftok");
        }
        return k;
    }

    // 创建共享内存空间
    int GetShmHelper(key_t key, int size, int flag)
    {
        int shmid = shmget(key, size, flag);
        if (shmid < 0)
        {
            perror("shmget");
        }
        return shmid;
    }

    // 将角色转换为字符串
    std::string RoleToString(int who)
    {
        if (who == gCreater)
            return "Creater";
        else if (who == gUser)
            return "User";
        else
            return "None";
    }

    // 将共享内存空间挂接到进程地址空间,调用该函数相当于调用malloc函数
    void* AttachShm()
    {
        if (_addrshm != nullptr)
            DatechShm(_addrshm);

        std::cout
            << "进行挂接" << std::endl;
        void* shmaddr = shmat(_shmid, nullptr, 0);
        if (shmaddr == nullptr) // 挂接失败
        {
            perror("shmat");
        }

        std::cout << "who: " << RoleToString(_who) << " attach shm..." << std::endl;
        return shmaddr;
    }

    // 解除挂接
    void DatechShm(void* shmaddr)
    {
        std::cout << "解除挂接" << std::endl;
        if (shmaddr == nullptr)
            return;
        shmdt(shmaddr);
        std::cout << "who: " << RoleToString(_who) << " detach shm..." << std::endl;
    }

public:
    // 构造函数
    Shm(const std::string& pathname, int proj_id, int who)
        : _pathname(pathname), _proj_id(proj_id), _who(who), _addrshm(nullptr)
    {
        _key = GetCommKey();
        if (_who == gCreater) // 如果是创建者就调用专门为创建者准备的函数GetShmForCreater
            GetShmForCreater();
        else if (_who == gUser) // 如果是使用者就调用专门为使用者准备的函数GetShmForUser
            GetShmForUser();
        std::cout << "shmid: " << _shmid << std::endl;
        std::cout << "_key: " << ToHex(_key) << std::endl;
        if (_who == gUser)
        {
            std::cout << "shm get done..." << std::endl;
        }
        // 不论是谁只要获取到一个共享内存空间的shmid，就将该共享内存空间挂接到进程地址空间上
        _addrshm = AttachShm();
    }

    // 析构函数:删除共享内存空间
    ~Shm()
    {
        if (_who == gCreater)
        {
            int res = shmctl(_shmid, IPC_RMID, nullptr);
            std::cout << "shm remove done..." << std::endl;
        }
    }

    // 将key转换为16进制，便于后续的查看
    std::string ToHex(key_t key)
    {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "0x%x", _key);
        return buffer;
    }

    // 服务端创建共享内存空间
    bool GetShmForCreater()
    {
        _shmid = GetShmHelper(_key, gShmSize, IPC_CREAT | IPC_EXCL | 0666); // 如果没有就创建，如果有了就返回错误，即没有返回错误那么就是创建了一个新的共享内存空间
        if (_shmid >= 0)                                                    // 0666表示所有者、所属组和其他用户对共享内存空间都有读写权限，但没有执行权限
        {
            std::cout << "shm creat done..." << std::endl;
            return true; // 创建成功返回true
        }
        std::cout << "shm creat fail..." << std::endl;
        return false;
    }

    // 客户端使用共享内存
    bool GetShmForUser()
    {
        _shmid = GetShmHelper(_key, gShmSize, IPC_CREAT | 0666); // 使用者根据与创造者一样得_key获得依据创造者创造的共享内存空间得shmid,仅有IPC_CREAT是因为此时服务端已经创建好了共享内存空间此时用户端只需要找到它对应的共享内存空间的shmind即可
        if (_shmid >= 0)
        {
            return true; // 获取成功返回true
        }
        std::cout << "shm get fail..." << std::endl;
        return false;
    }

    // 获取进程空间的地址
    void* Addr()
    {
        return _addrshm;
    }

    // 清空共享内存空间
    void Zero()
    {
        if (_addrshm)
        {
            memset(_addrshm, 0, gShmSize);
        }
    }

    //  获取共享内存空间属性结构体中的内容
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
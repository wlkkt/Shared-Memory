/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-12 23:03:25
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-14 20:02:49
 * @FilePath: /2024.5.8/home/ubuntu/2024.5.13/server.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "shm.hpp"
#include "namedPipe.hpp"

int main()
{
    // 1. 创建共享内存
    Shm shm(gpathname, gproj_id, gCreater);
    char* shmaddr = (char*)shm.Addr();

    shm.DebugShm();

    // 2. 创建管道
    NamePiped fifo(comm_path, Creater);
    fifo.OpenForRead(); // 以读方式打开管道

    while (true)
    {
        std::string temp;
        fifo.ReadNamedPipe(&temp); // 服务端读取管道

        std::cout << "shm memory content: " << shmaddr << std::endl;//打印此时共享内存空间中的内容（客户端已经向里面写入了），shmaddr是一个字符指针，打印它就会将它所指向字符串
    }

    sleep(5);

    return 0;
}
/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-12 23:03:16
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-14 21:34:39
 * @FilePath: /2024.5.8/home/ubuntu/2024.5.13/client.cc
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "shm.hpp"
#include "namedPipe.hpp"

int main()
{
    // 1. 创建共享内存

    Shm shm(gpathname, gproj_id, gUser);
    shm.Zero();
    char* shmaddr = (char*)shm.Addr(); // shmaddr指向一段内存空间
    sleep(3);

    // 2. 打开管道（利用管道保护共享内存空间）
    NamePiped fifo(comm_path, User); // 有了fifo才能使用与管道相关的方法，即便有的方法根本用不上
    fifo.OpenForWrite();             // 以写方式打开管道

    // 当成string
    char ch = 'A';
    while (ch <= 'Z')
    {
        shmaddr[ch - 'A'] = ch; // shmadrr是一个指针，而数组名相当于一个指针，这里我们将hmaddr视为一个数组，循环向数组中写入即循环向共享内存空间中写入，起始时ch-'A' = 0即shmaddr[0] = 'A'

        std::string temp = "wakeup";                                                // 定义一个提醒字符串
        std::cout << "add " << ch << " into Shm, " << "wakeup reader" << std::endl; // 客户端打印自己此时向共享内存空间中所写字母
        fifo.WriteNamedPipe(temp);                                                  // 向管道中写入提醒字符串，提醒服务端该读取了
        sleep(2);
        ch++;
    }
    return 0;
}
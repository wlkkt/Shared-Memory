/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-12 13:57:58
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-14 20:39:56
 * @FilePath: /2024.5.8/home/ubuntu/2024.5.12/namePipe.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <iostream>
#include <cstdio>
#include <cerrno>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const std::string comm_path = "./myfifo"; // 提供文件的路径（相当于在当前目录下生成一个叫myfifo的命名管道）,string字符串在使用时需要转换为c类型的字符串

#define DefaultFd -1 // 将-1用于标识管道的默认fd（有进程打开管道时再分配新的fd）

#define Creater 1 // 将1用于标识管道的申请者
#define User 2    // 将2用于标识管道的使用者

#define Read O_RDONLY  // 将Read用于标识只读方式
#define Write O_WRONLY // 将Write用于标识只写方式

#define BaseSize 4096 // 将4096作为读取内容的最大字节数

class NamePiped
{
private:
    bool OpenNamedPipe(int mode) // 依据对管道的操作方式
    {
        _fd = open(_fifo_path.c_str(), mode); // 每个进程打开管道时都会获得OS分配给该进程得文件描述符
        if (_fd < 0)
            return false; // 打开文件失败返回false
        return true;      // 打开文件成功返回true
    }

public:
    NamePiped(const std::string& path, int who) // 依据文件路径和身份信息构建管道
        : _fifo_path(path), _id(who), _fd(DefaultFd)
    {
        if (_id == Creater) // 只有申请者才能创建管道
        {
            int res = mkfifo(_fifo_path.c_str(), 0666); // 创建管道
            if (res != 0)
            {
                perror("mkfifo");
            }
            std::cout << "creater create named pipe" << std::endl;
        }
    }

    // 以读方式打开管道
    bool OpenForRead()
    {
        return OpenNamedPipe(Read);
    }

    // 以写方式打开管道
    bool OpenForWrite()
    {
        return OpenNamedPipe(Write);
    }

    // 读管道的方式
    int ReadNamedPipe(std::string* out) // 输出型参数
    {
        char buffer[BaseSize]; // 一次能读取到的最大内容
        int n = read(_fd, buffer, sizeof(buffer));//从管道中读取
        if (n > 0)
        {
            buffer[n] = 0;
            *out = buffer; //*out指向读取到的内容
        }
        return n;
    }

    // 写管道的方式
    int WriteNamedPipe(const std::string& in)
    {
        return write(_fd, in.c_str(), in.size()); // 依据wfd读取
    }

    ~NamePiped()
    {
        if (_id == Creater) // 只有申请者才能创建管道
        {
            int res = unlink(_fifo_path.c_str());
            if (res != 0)
            {
                perror("unlink");
            }
            std::cout << "creater free named pipe" << std::endl;
        }
        if (_fd != DefaultFd) // 关闭写端fd
            close(_fd);
    }

private:
    const std::string _fifo_path; // 提供的文件路径
    int _id;                      // 标识使用者的身份信息
    int _fd;                      // 标识文件描述符
};
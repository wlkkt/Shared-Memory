/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-12 13:57:58
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-14 20:39:56
 * @FilePath: /2024.5.8/home/ubuntu/2024.5.12/namePipe.hpp
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
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

const std::string comm_path = "./myfifo"; // �ṩ�ļ���·�����൱���ڵ�ǰĿ¼������һ����myfifo�������ܵ���,string�ַ�����ʹ��ʱ��Ҫת��Ϊc���͵��ַ���

#define DefaultFd -1 // ��-1���ڱ�ʶ�ܵ���Ĭ��fd���н��̴򿪹ܵ�ʱ�ٷ����µ�fd��

#define Creater 1 // ��1���ڱ�ʶ�ܵ���������
#define User 2    // ��2���ڱ�ʶ�ܵ���ʹ����

#define Read O_RDONLY  // ��Read���ڱ�ʶֻ����ʽ
#define Write O_WRONLY // ��Write���ڱ�ʶֻд��ʽ

#define BaseSize 4096 // ��4096��Ϊ��ȡ���ݵ�����ֽ���

class NamePiped
{
private:
    bool OpenNamedPipe(int mode) // ���ݶԹܵ��Ĳ�����ʽ
    {
        _fd = open(_fifo_path.c_str(), mode); // ÿ�����̴򿪹ܵ�ʱ������OS������ý��̵��ļ�������
        if (_fd < 0)
            return false; // ���ļ�ʧ�ܷ���false
        return true;      // ���ļ��ɹ�����true
    }

public:
    NamePiped(const std::string& path, int who) // �����ļ�·���������Ϣ�����ܵ�
        : _fifo_path(path), _id(who), _fd(DefaultFd)
    {
        if (_id == Creater) // ֻ�������߲��ܴ����ܵ�
        {
            int res = mkfifo(_fifo_path.c_str(), 0666); // �����ܵ�
            if (res != 0)
            {
                perror("mkfifo");
            }
            std::cout << "creater create named pipe" << std::endl;
        }
    }

    // �Զ���ʽ�򿪹ܵ�
    bool OpenForRead()
    {
        return OpenNamedPipe(Read);
    }

    // ��д��ʽ�򿪹ܵ�
    bool OpenForWrite()
    {
        return OpenNamedPipe(Write);
    }

    // ���ܵ��ķ�ʽ
    int ReadNamedPipe(std::string* out) // ����Ͳ���
    {
        char buffer[BaseSize]; // һ���ܶ�ȡ�����������
        int n = read(_fd, buffer, sizeof(buffer));//�ӹܵ��ж�ȡ
        if (n > 0)
        {
            buffer[n] = 0;
            *out = buffer; //*outָ���ȡ��������
        }
        return n;
    }

    // д�ܵ��ķ�ʽ
    int WriteNamedPipe(const std::string& in)
    {
        return write(_fd, in.c_str(), in.size()); // ����wfd��ȡ
    }

    ~NamePiped()
    {
        if (_id == Creater) // ֻ�������߲��ܴ����ܵ�
        {
            int res = unlink(_fifo_path.c_str());
            if (res != 0)
            {
                perror("unlink");
            }
            std::cout << "creater free named pipe" << std::endl;
        }
        if (_fd != DefaultFd) // �ر�д��fd
            close(_fd);
    }

private:
    const std::string _fifo_path; // �ṩ���ļ�·��
    int _id;                      // ��ʶʹ���ߵ������Ϣ
    int _fd;                      // ��ʶ�ļ�������
};
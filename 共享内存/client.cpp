/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-12 23:03:16
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-14 21:34:39
 * @FilePath: /2024.5.8/home/ubuntu/2024.5.13/client.cc
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "shm.hpp"
#include "namedPipe.hpp"

int main()
{
    // 1. ���������ڴ�

    Shm shm(gpathname, gproj_id, gUser);
    shm.Zero();
    char* shmaddr = (char*)shm.Addr(); // shmaddrָ��һ���ڴ�ռ�
    sleep(3);

    // 2. �򿪹ܵ������ùܵ����������ڴ�ռ䣩
    NamePiped fifo(comm_path, User); // ����fifo����ʹ����ܵ���صķ����������еķ��������ò���
    fifo.OpenForWrite();             // ��д��ʽ�򿪹ܵ�

    // ����string
    char ch = 'A';
    while (ch <= 'Z')
    {
        shmaddr[ch - 'A'] = ch; // shmadrr��һ��ָ�룬���������൱��һ��ָ�룬�������ǽ�hmaddr��Ϊһ�����飬ѭ����������д�뼴ѭ�������ڴ�ռ���д�룬��ʼʱch-'A' = 0��shmaddr[0] = 'A'

        std::string temp = "wakeup";                                                // ����һ�������ַ���
        std::cout << "add " << ch << " into Shm, " << "wakeup reader" << std::endl; // �ͻ��˴�ӡ�Լ���ʱ�����ڴ�ռ�����д��ĸ
        fifo.WriteNamedPipe(temp);                                                  // ��ܵ���д�������ַ��������ѷ���˸ö�ȡ��
        sleep(2);
        ch++;
    }
    return 0;
}
/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-05-12 23:03:25
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-05-14 20:02:49
 * @FilePath: /2024.5.8/home/ubuntu/2024.5.13/server.cc
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "shm.hpp"
#include "namedPipe.hpp"

int main()
{
    // 1. ���������ڴ�
    Shm shm(gpathname, gproj_id, gCreater);
    char* shmaddr = (char*)shm.Addr();

    shm.DebugShm();

    // 2. �����ܵ�
    NamePiped fifo(comm_path, Creater);
    fifo.OpenForRead(); // �Զ���ʽ�򿪹ܵ�

    while (true)
    {
        std::string temp;
        fifo.ReadNamedPipe(&temp); // ����˶�ȡ�ܵ�

        std::cout << "shm memory content: " << shmaddr << std::endl;//��ӡ��ʱ�����ڴ�ռ��е����ݣ��ͻ����Ѿ�������д���ˣ���shmaddr��һ���ַ�ָ�룬��ӡ���ͻὫ����ָ���ַ���
    }

    sleep(5);

    return 0;
}
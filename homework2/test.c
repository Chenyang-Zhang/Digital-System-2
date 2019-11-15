#include <QtCore/QCoreApplication>
#include <QFile>
#include <QDataStream>
#include <QDebug>
 
#pragma pack(push)
#pragma pack(1)
 
struct Fat12Header
{
    char BS_OEMName[8];//OEM字符串，必须为8个字符，不足以空格填空
    ushort BPB_BytsPerSec;//每扇区字节数
    uchar BPB_SecPerClus;//每簇占用的扇区数
    ushort BPB_RsvdSecCnt;//Boot占用的扇区数
    uchar BPB_NumFATs;//FAT表的记录数
    ushort BPB_RootEntCnt;//最大根目录文件数
    ushort BPB_TotSec16;//每个FAT占用扇区数
    uchar BPB_Media;//媒体描述符
    ushort BPB_FATSz16;//每个FAT占用扇区数
    ushort BPB_SecPerTrk;//每个磁道扇区数
    ushort BPB_NumHeads;//磁头数
    uint BPB_HiddSec;//隐藏扇区数
    uint BPB_TotSec32;//如果BPB_TotSec16是0，则在这里记录
    uchar BS_DrvNum;//中断13的驱动器号
    uchar BS_Reserved1;//未使用
    uchar BS_BootSig;//扩展引导标志
    uint BS_VolID;//卷序列号
    char BS_VolLab[11];//卷标，必须是11个字符，不足以空格填充
    char BS_FileSysType[8];//文件系统类型，必须是8个字符，不足填充空格
};
 
#pragma pack(pop)
 
void PrintHeader(Fat12Header& rf, QString p)
{
    QFile file(p);
 
    if( file.open(QIODevice::ReadOnly) )
    {
        QDataStream in(&file);
 
        file.seek(3);
 
        in.readRawData(reinterpret_cast<char*>(&rf), sizeof(rf));
 
        rf.BS_OEMName[7] = 0;
        rf.BS_VolLab[10] = 0;
        rf.BS_FileSysType[7] = 0;
 
        qDebug() << "BS_OEMName: " << rf.BS_OEMName;
        qDebug() << "BPB_BytsPerSec: " << hex << rf.BPB_BytsPerSec;
        qDebug() << "BPB_SecPerClus: " << hex << rf.BPB_SecPerClus;
        qDebug() << "BPB_RsvdSecCnt: " << hex << rf.BPB_RsvdSecCnt;
        qDebug() << "BPB_NumFATs: " << hex << rf.BPB_NumFATs;
        qDebug() << "BPB_RootEntCnt: " << hex << rf.BPB_RootEntCnt;
        qDebug() << "BPB_TotSec16: " << hex << rf.BPB_TotSec16;
        qDebug() << "BPB_Media: " << hex << rf.BPB_Media;
        qDebug() << "BPB_FATSz16: " << hex << rf.BPB_FATSz16;
        qDebug() << "BPB_SecPerTrk: " << hex << rf.BPB_SecPerTrk;
        qDebug() << "BPB_NumHeads: " << hex << rf.BPB_NumHeads;
        qDebug() << "BPB_HiddSec: " << hex << rf.BPB_HiddSec;
        qDebug() << "BPB_TotSec32: " << hex << rf.BPB_TotSec32;
        qDebug() << "BS_DrvNum: " << hex << rf.BS_DrvNum;
        qDebug() << "BS_Reserved1: " << hex << rf.BS_Reserved1;
        qDebug() << "BS_BootSig: " << hex << rf.BS_BootSig;
        qDebug() << "BS_VolID: " << hex << rf.BS_VolID;
        qDebug() << "BS_VolLab: " << rf.BS_VolLab;
        qDebug() << "BS_FileSysType: " << rf.BS_FileSysType;
 
        file.seek(510);
 
        uchar b510 = 0;
        uchar b511 = 0;
 
        in.readRawData(reinterpret_cast<char*>(&b510), sizeof(b510));
        in.readRawData(reinterpret_cast<char*>(&b511), sizeof(b511));
 
        qDebug() << "Byte 510: " << hex << b510;
        qDebug() << "Byte 511: " << hex << b511;
    }
 
    file.close();
}
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
 
    Fat12Header f12;
 
    PrintHeader(f12, "a.img");
    
    return a.exec();
}

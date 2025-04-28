#pragma once

#include <QVariant>
#include <gen/uint24.h>

#ifdef __linux__
#include <ctime>
Q_DECLARE_METATYPE(timespec)
#endif

namespace DataTypes
{

enum Quality : quint8
{
    Good = 0,
    Bad = 0x80
};

enum DataBlockTypes
{
    BacBlock,
    BdaBlock,
    BdBlock,
    BciBlock
};

enum SignalTypes
{
    Float,
    BitString,
    FloatWithTime,
    SinglePointWithTime,
    ByteArray,
    File,
    DataRecVList,
    Block,
    OscillogramInfo,
    SwitchJournalInfo,
#ifdef __linux
    Timespec,
#endif
    Command,
    GeneralResponse
};

enum GeneralResponseTypes
{
    Ok,
    Error,
    DataSize,
    DataCount
};

enum FileFormat : quint32
{
    Binary = 0,
    DefaultS2 = 1,
    CustomS2
};

struct BitStringStruct
{
    //    quint8 sigAdr[3];
    quint32 sigAdr;
    quint32 sigVal;
    quint8 sigQuality;
};
// первое - номера сигналов, второе - их значения ("" ~ недостоверное
// значение), третье - метка времени

struct FloatWithTimeStruct
{
    quint32 sigAdr;
    float sigVal;
    quint64 CP56Time;
    quint8 sigQuality;
};

struct FloatStruct
{
    quint32 sigAdr;
    float sigVal;
    quint8 sigQuality;
};

struct SinglePointWithTimeStruct
{
    quint32 sigAdr;
    quint8 sigVal;
    quint64 CP56Time;
    quint8 sigQuality;
};

struct BlockStruct
{
    quint32 ID;
    QByteArray data;
};

struct GeneralResponseStruct
{
    GeneralResponseTypes type;
    quint64 data;
};

using S2Record = BlockStruct;
using HardwareStruct = BlockStruct;
using S2FilePack = QList<S2Record>;

struct SignalsStruct
{
    SignalTypes type;
    QVariant data;
};

struct Signal
{
    quint16 addr;
    quint16 value;
};

struct SingleCommand
{
    uint24 addr;
    bool value;
};

}

Q_DECLARE_METATYPE(uint24)
Q_DECLARE_METATYPE(DataTypes::SingleCommand)
Q_DECLARE_METATYPE(DataTypes::BitStringStruct)
Q_DECLARE_METATYPE(DataTypes::FloatWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::FloatStruct)
Q_DECLARE_METATYPE(DataTypes::SinglePointWithTimeStruct)
Q_DECLARE_METATYPE(DataTypes::BlockStruct)
Q_DECLARE_METATYPE(DataTypes::S2FilePack)
Q_DECLARE_METATYPE(DataTypes::SignalsStruct)
Q_DECLARE_METATYPE(DataTypes::Signal)
Q_DECLARE_METATYPE(DataTypes::GeneralResponseStruct)

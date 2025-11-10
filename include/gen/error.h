#pragma once

#include <QDebug>
#include <QMap>
#include <QObject>
#include <QString>
#include <gen/gen_export.h>

/// \brief Namespace for critical message logging
namespace Error
{
#ifdef HAVE_STATIC_BUILD
Q_NAMESPACE
#else
Q_NAMESPACE_EXPORT(GENLIB_EXPORT)
#endif

/// Enumeration for possible errors in program runtime.
enum Msg
{
    ResEmpty = -2,            ///< \warning ResEmpty hardcoded as -2
    GeneralError = -1,        ///< \warning GeneralError hardcoded as -1
    NoError = 0,              ///< \warning NoError hardcoded as 0
    Timeout = 0x01,           ///< Others hardcoded as Protocom
    CrcError = 0x02,          ///< Test01
    FlashError = 0x03,        ///< Test02
    SizeError = 0x04,         ///< Test03
    WrongCommandError = 0x05, ///< Test04
    WrongFormatError = 0x06,  ///< Test05
    UnknownBlock = 0x07,      ///< Test06
    UnknownFileReq = 0x08,    ///< Test07
    UnknownFileSent = 0x09,   ///< Test08
    NoMezzanine = 0x0a,       ///< Test09
    WrongType = 0x0b,         ///< Test10
    FileNameError = 0x0c,     ///< Test11
    WrongBlockAC = 0x0d,      ///< Test12
    DescError = 0x0f,         ///< Test13
    StartupInitError = 0x10,  ///< Error startup values initialization
    HeaderSizeError,          ///< Test14
    NoIdError,                ///< Test15
    NoConfError,              ///< Test16
    NoTuneError,              ///< Test17
    WriteError,               ///< Test18
    NoDeviceError,            ///< Test19
    ReadError,                ///< Test20
    OpenError,                ///< Test21
    NullDataError,            ///< Test22
    WrongFileError,           ///< Test23
    FileOpenError,            ///< Test24
    FileWriteError,           ///< Test25
    Cancelled,                ///< Tune process was cancelled
    StepError,                ///
    TuneCoefError,            ///
    PswCheckError,            ///
    DataError,                ///
    UnknownError = 0xff       ///< Test26
};

/// \brief Map with keys as Msg and values as QString with description of error.
const QMap<Msg, QString> MsgStr {
    { ResEmpty, QString::fromUtf8("Пустой результат") },                                         //
    { GeneralError, QString::fromUtf8("Ошибка") },                                               //
    { NoError, QString::fromUtf8("Успешно") },                                                   //
    { Timeout, QString::fromUtf8("Таймаут операции") },                                          //
    { CrcError, QString::fromUtf8("Ошибка контрольной суммы") },                                 //
    { FlashError, QString::fromUtf8("Ошибка записи во Flash-память") },                          //
    { SizeError, QString::fromUtf8("Ошибка размера данных") },                                   //
    { WrongCommandError, QString::fromUtf8("Некорректная команда") },                            //
    { WrongFormatError, QString::fromUtf8("Формат неверен") },                                   //
    { UnknownBlock, QString::fromUtf8("Неизвестный блок данных") },                              //
    { UnknownFileReq, QString::fromUtf8("Запрошен неизвестный файл") },                          //
    { UnknownFileSent, QString::fromUtf8("Отправлен неизвестный файл") },                        //
    { NoMezzanine, QString::fromUtf8("Нет мезонина") },                                          //
    { WrongType, QString::fromUtf8("Неверный тип платы") },                                      //
    { FileNameError, QString::fromUtf8("Нет такого файла") },                                    //
    { WrongBlockAC, QString::fromUtf8("Ошибка блока Bac") },                                     //
    { DescError, QString::fromUtf8("Ошибка описания S2") },                                      //
    { StartupInitError, QString::fromUtf8("Ошибка задания начальных значений") },                //
    { HeaderSizeError, QString::fromUtf8("Неверный размер заголовка S2") },                      //
    { NoIdError, QString::fromUtf8("Нет такого ИД S2") },                                        //
    { NoConfError, QString::fromUtf8("Нет конфигурации") },                                      //
    { NoTuneError, QString::fromUtf8("Нет настроечных параметров") },                            //
    { WriteError, QString::fromUtf8("Ошибка записи") },                                          //
    { NoDeviceError, QString::fromUtf8("Нет такого устройства") },                               //
    { ReadError, QString::fromUtf8("Ошибка чтения") },                                           //
    { OpenError, QString::fromUtf8("Ошибка открытия порта") },                                   //
    { NullDataError, QString::fromUtf8("Приняты нулевые данные") },                              //
    { FileNameError, QString::fromUtf8("Ошибка номера файла") },                                 //
    { FileOpenError, QString::fromUtf8("Ошибка открытия файла") },                               //
    { FileWriteError, QString::fromUtf8("Ошибка записи файла") },                                //
    { Cancelled, QString::fromUtf8("Операция прервана") },                                       //
    { StepError, QString::fromUtf8("Некорректный номер шага") },                                 //
    { TuneCoefError, QString::fromUtf8("Ошибка в настроечных коэффициентах, деление на ноль") }, //
    { PswCheckError, QString::fromUtf8("Неправильный пароль") },                                 //
    { DataError, QString::fromUtf8("Ошибка данных") },                                           //
    { UnknownError, QString::fromUtf8("Неизвестная ошибка") }                                    //
};

Q_ENUM_NS(Msg)

}
Q_DECLARE_METATYPE(Error::Msg)

/// \brief Structure for storage error message data.
struct ErrorMsg
{
    QString DateTime; ///< Time and date of error happens
    QtMsgType type;   ///< Type of message for logging
    QString file;     ///< File with error
    int line;         ///< Line with error in file
    QString msg;      ///< String with message about error
};

/// \brief Output to string list operator overloading.
GENLIB_EXPORT QStringList &operator<<(QStringList &l, const ErrorMsg &obj);

/// \brief Output to QDebug (stdout) operator overloading.
GENLIB_EXPORT QDebug operator<<(QDebug debug, const ErrorMsg &error);

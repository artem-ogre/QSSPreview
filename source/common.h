#ifndef COMMON_H
#define COMMON_H

#define qDebugWithInfo() qDebug() <<__FILE__<< "line" <<__LINE__ << "in" << Q_FUNC_INFO << ":"
#define LogErrorAndThrowException( error ) qDebugWithInfo() << error; throw std::runtime_error ( error )

namespace Common
{
}

#endif // COMMON_H

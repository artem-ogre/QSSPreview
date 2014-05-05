/*
 Copyright Artem Amirkhanov 2014
 Distributed under the MIT Software License (See accompanying file LICENSE.txt)
 Contact the author: artem.ogre@gmail.com
*/

#ifndef COMMON_H
#define COMMON_H

#include <stdexcept>

#define qDebugWithInfo() qDebug() <<__FILE__<< "line" <<__LINE__ << "in" << Q_FUNC_INFO << ":"
#define LogErrorAndThrowException( error ) qDebugWithInfo() << error; throw std::runtime_error ( error )

namespace Common
{
}

#endif // COMMON_H

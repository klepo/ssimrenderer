/**
 * @file        ssimrenderer_global.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        4 December 2015
 *
 * @brief       This is include file for shared library export.
 *
 * @copyright   Copyright (C) 2015 Petr Kleparnik, Ondrej Klima. All Rights Reserved.
 *
 * @license     This file may be used, distributed and modified under the terms of the LGPL version 3
 *              open source license. A copy of the LGPL license should have
 *              been recieved with this file. Otherwise, it can be found at:
 *              http://www.gnu.org/copyleft/lesser.html
 *              This file has been created as a part of the Traumatech project:
 *              http://www.fit.vutbr.cz/research/grants/index.php.en?id=733.
 *
 */

#ifndef SSIMRENDERER_GLOBAL_H
#define SSIMRENDERER_GLOBAL_H

#include <QtCore/qglobal.h>
#include <stdexcept>

#if defined(SHARED_SSIMRENDERER_LIBRARY)
#   if defined(SHARED_LIBRARY_EXPORT)
#       define SHARED_EXPORT Q_DECL_EXPORT
#   else
#       define SHARED_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define SHARED_EXPORT
#endif

#endif // SSIMRENDERER_GLOBAL_H

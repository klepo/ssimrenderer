/**
 * @file        ssimrenderer.h
 * @author      Petr Kleparnik, VUT FIT Brno, ikleparnik@fit.vutbr.cz
 * @author      Ondrej Klima, VUT FIT Brno, iklima@fit.vutbr.cz
 * @version     1.0
 * @date        4 December 2015
 *
 * @brief       This is the main public SSIMRenderer include file.
 *
 * SSIMRenderer - Statistical shape and intensity models renderer - library
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

#ifndef SSIMRENDERER_H
#define SSIMRENDERER_H

/**
 * @namespace SSIMRenderer
 * @brief Namespace for %SSIMRenderer library
 */
namespace SSIMRenderer {}

#include "input/csvwrapper.h"
#include "input/hdf5wrapper.h"

#include "input/pyramid.h"
#include "input/mesh.h"
#include "input/statisticaldata.h"

#include "input/csvcoeffsfile.h"
#include "input/csvpyramidfile.h"
#include "input/csvgeneralfile.h"
#include "input/hdf5statisticaldatafile.h"
#include "input/hdf5meshfile.h"
#include "input/plymeshfile.h"
#include "input/matstatisticaldatafile.h"
#include "input/lm6meshfile.h"

#include "input/xmlcalibsfile.h"

#include "opengl/openglwrapper.h"

#include "rendering/densityfsgenerator/densityfsgenerator.h"

#include "rendering/mainrenderer.h"
#include "rendering/offscreenrenderer.h"
#include "rendering/sharedwindow.h"
#include "rendering/window.h"

#include "metric/metricwrapper.h"
#include "metric/nmiwrapper.h"
#include "metric/nmicomputingopengl.h"
#include "metric/nmicomputingcpu.h"
#include "metric/ssdwrapper.h"
#include "metric/ssdcomputingopengl.h"
#include "metric/ssdcomputingcpu.h"

#ifdef USE_OPENCL
    #include "opencl/openclwrapper.h"
    #include "metric/nmicomputingopencl.h"
    #include "metric/ssdcomputingopencl.h"
#endif // USE_OPENCL

#endif // SSIMRENDERER_H

SSIM Renderer Library 
=====================
SSIM Renderer is a Qt-based C++ library for OpenGL enabled rendering
of virtual X-Ray images from statistical shape and intensity models.

Description
-----------
The main focus of the library is a rendering of virtual X-Ray images from 
a statistical shape and intensity model (SSIM) proposed by R. Yao [1]. 
The library provides OpenGL accelerated rendering of the SSIM according to [2]. 
Moreover, it includes OpenGL and OpenCL implementations of two image similarity 
metrics: joint histogram normalized mutual information metric and sum 
of squared differences metric. The library is highly optimised and suitable 
for purposes of 2D-3D deformable registration [3]. It is also capable to work 
with simple polygonal statistical shape models (SSM) stored in Statismo file 
format.

Version from 17 December 2015.

Features
--------
 * Rendering of virtual X-Ray images from SSIM.  
 * Rendering of surface images from SSIM/SSM.  
 * Rendering of silhouettes of SSIM/SSM.      
 * Mirroring of the shape models.
 * Sharing shape model between many renderers using OpenGL shared contexts.
 * Exporting the surface of the shape model in STL file format.
 * Computation of OpenGL and OpenCL accelerated image similarity metrics.
 * etc.      

Prerequisites
-------------
Qt Toolkit in version 5.x or later is required. Other required librarires are 
redistributed within the SSIM Renderer package. The libraries are redistributed 
in both source and binary form. Following 3rd party libraries are included:

 * HDF5
 * libmesh6
 * matio
 * RPly library   
 * zlib
    
The library has been developped for usage with Windows 7 or later.

Examples
--------
To get started with the SSIM Renderer library, please have a look 
at the included examples:

 * ImageMetrics.cpp
   - example demonstrating computation of image similarity metrics 
     between two virtual X-Ray images
 
 * SimpleStatismoModel.cpp
   - demonstration of the offscreen rendering of a simple polygonal 
     shape model

 * IntensityShapeModel.cpp
   - example of rendering virtual X-Ray images from a statistical 
     shape and intensity model

There is also a full reference manual available.

Downloading
-----------
The SSIM Renderer library can be further redistributed under the terms 
of the LGPL version 3 open source license. 
The library can be obtained from the following location: 
<http://www.fit.vutbr.cz/research/prod/?id=458>

Acknowledgement
---------------
This work has been supported by the Technology Agency of the Czech Republic 
(TA CR, Project Id: TA04011606).


Authors
-------
 * Petr Kleparnik   (<ikleparnik@fit.vutbr.cz>)
 * Ondrej Klima     (<iklima@fit.vutbr.cz>)
 * Michal Spanel    (<spanel@fit.vutbr.cz>)
 * Pavel Zemcik     (<zemcik@stud.fit.vutbr.cz>)

References
----------
[1] J. Yao, R. Taylor, "Construction and simplification of bone density models"
    SPIE Medical Imaging: 2001.
    
[2] M. Ehlke, HRamm, H. Lamecker, H.C. Hege, S. Zachow. "Fast generation 
    of virtual X-ray images for reconstruction of 3D anatomy." IEEE Trans Vis 
    Comput Graph: Dec 2013.
    
[3] O. Klima, P. Kleparnik, M. Spanel, P. Zemcik, "GP-GPU accelerated 
    intensity-based 2D/3D registration pipeline" In Proceedings of Shape 
    Symposium, p. 19, Delemont, Switzerland, 2014. 
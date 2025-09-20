/* This file is part of the CARTA Image Viewer: https://github.com/CARTAvis/carta-backend
   Copyright 2018- Academia Sinica Institute of Astronomy and Astrophysics (ASIAA),
   Associated Universities, Inc. (AUI) and the Inter-University Institute for Data Intensive Astronomy (IDIA)
   SPDX-License-Identifier: GPL-3.0-or-later
*/

//# CartaHdf5Image.h : HDF5 Image class derived from casacore::ImageInterface

#ifndef SOFIA_SRC_IMAGEDATA_CARTAHDF5IMAGE_H_
#define SOFIA_SRC_IMAGEDATA_CARTAHDF5IMAGE_H_

#include <casacore/lattices/Lattices/HDF5Lattice.h>
#include <casacore/images/Images/MaskSpecifier.h>
#include "CartaHdf5Image.h"

namespace carta {

class SofiaHdf5Image : public CartaHdf5Image {
public:
    using CartaHdf5Image::CartaHdf5Image;
    casacore::Lattice<bool>& Add_Mask();

private:
    casacore::Lattice<bool>* _mask;
    // Function to return the internal HDF5File object to the RegionHandlerHDF5
  
};

} // namespace carta

#endif // SOFIA_SRC_IMAGEDATA_CARTAHDF5IMAGE_H_

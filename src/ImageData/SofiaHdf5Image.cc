/* This file is part of the CARTA Image Viewer: https://github.com/CARTAvis/carta-backend
   Copyright 2018- Academia Sinica Institute of Astronomy and Astrophysics (ASIAA),
   Associated Universities, Inc. (AUI) and the Inter-University Institute for Data Intensive Astronomy (IDIA)
   SPDX-License-Identifier: GPL-3.0-or-later
*/

//# CartaHdf5Image.cc : specialized Image implementation for IDIA HDF5 schema

#include "SofiaHdf5Image.h"
#include <casacore/coordinates/Coordinates/DirectionCoordinate.h>
#include <casacore/coordinates/Coordinates/Projection.h>
#include <casacore/coordinates/Coordinates/SpectralCoordinate.h>
#include <casacore/fits/FITS/FITSDateUtil.h>
#include <casacore/fits/FITS/FITSKeywordUtil.h>
#include <casacore/fits/FITS/fits.h>
#include <casacore/images/Images/ImageFITSConverter.h>
#include <casacore/images/Images/ImageOpener.h>
#include <casacore/lattices/Lattices/HDF5Lattice.h>

#include "../Logger/Logger.h"
#include "Util/String.h"

#include "Hdf5Attributes.h"

using namespace carta;

casacore::Lattice<bool>& SofiaHdf5Image::Add_Mask(){
    if (!hasMask()) {
        _mask = new casacore::ArrayLattice<bool>();
    }

    if (_mask->shape().empty()) {
        // get mask for entire image
        casacore::Array<bool> array_mask;
        casacore::IPosition start(_shape.size(), 0);
        casacore::IPosition end(_shape);
        casacore::Slicer slicer(start, end);
        doGetMaskSlice(array_mask, slicer);
        // replace pixel mask
        delete __mask;
        _mask = new casacore::ArrayLattice<bool>(array_mask);
    }
    return *_mask;
}
#include "dss/datain.hpp"
#include "dss/datain_isoiec9796-3.hpp"

DataInput * DataInputProvider::newDataInput(DataInputType type) const
{
    switch (type)
    {
        case DATA_ECKNR:
            return new DataInput_STD4(_dip.getRecoverableSize(),
                                      _dip.getRedundancySize(),
                                      _dip.getHashType());
            
        case DATA_ECNR:
            return new DataInput_ECNR(_dip.getRecoverableSize(),
                                      _dip.getRedundancySize(),
                                      _dip.getHashType());
            
        case DATA_ECMR:
            return new DataInput_ECMR(_dip.getRecoverableSize(),
                                      _dip.getRedundancySize(),
                                      _dip.getHashType());

        case DATA_ECAO:
            return new DataInput_ECAO(_dip.getRecoverableSize(),
                                      _dip.getRedundancySize(),
                                      _dip.getMaximalRedundancySize(),
                                      _dip.getHashType());
        case DATA_ECPV:
            return new DataInput_ECPV(_dip.getRecoverableSize(),
                                      _dip.getRedundancySize(),
                                      _dip.getHashType());
            
        default:
            throw;
    }

    return NULL;
}

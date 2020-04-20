#include "PVariableSelector.h"
#include "VCheckBox.h"
#include "VLineItem.h"
#include <vapor/ParamsBase.h>
#include <vapor/RenderParams.h>
#include <VComboBox.h>
#include <assert.h>

using VAPoR::RenderParams;
using VAPoR::Box;

PVariableSelector::PVariableSelector(const std::string &tag, const std::string &label)
: PStringDropdown(tag, {}, label),
  _nDims( 3 )
{}

void PVariableSelector::updateGUI() const
{
    RenderParams *rp = dynamic_cast<RenderParams*>(getParams());
    assert(rp && "Params must be RenderParams");
    
    //int nDims = getDimensionality();
    
    auto varNames = getDataMgr()->GetDataVarNames(_nDims);
    SetItems(varNames);
    
    PStringDropdown::updateGUI();
}

void PVariableSelector::SetDimensionality( int nDims ) {
    _nDims = nDims;
}

int PVariableSelector::getDimensionality() const
{
    // return rp->GetBox()->GetOrientation() == Box::XY ? 2 : 3;
    
    //return getDataMgr()->GetNumDimensions(getParamsString());

    return _nDims;
}

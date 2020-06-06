#include "PVariableSelector.h"
#include "VCheckBox.h"
#include "VLineItem.h"
#include <vapor/ParamsBase.h>
#include <vapor/RenderParams.h>
#include <VComboBox.h>
#include <assert.h>

using VAPoR::RenderParams;
using VAPoR::Box;

#define NULL_TEXT "<none>"

PVariableSelector::PVariableSelector(const std::string &tag, const std::string &label)
: PStringDropdown(tag, {}, label)
{}

void PVariableSelector::updateGUI() const
{
    RenderParams *rp = dynamic_cast<RenderParams*>(getParams());
    assert(rp && "Params must be RenderParams");
    
    int nDims = getDimensionality();
    
    auto varNames = getDataMgr()->GetDataVarNames(nDims);
    
    if (_addNull)
        varNames.insert(varNames.begin(), NULL_TEXT);
    
    SetItems(varNames);
    
    PStringDropdown::updateGUI();
}

int PVariableSelector::getDimensionality() const
{
    // return rp->GetBox()->GetOrientation() == Box::XY ? 2 : 3;
    
    return getDataMgr()->GetNumDimensions(getParamsString());
}

void PVariableSelector::dropdownTextChanged(std::string text)
{
    if (_addNull && text == NULL_TEXT)
        text = "";
    
    PStringDropdown::dropdownTextChanged(text);
}

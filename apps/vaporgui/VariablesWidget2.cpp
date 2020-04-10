#include "VariablesWidget2.h"
#include <vapor/RenderParams.h>

VariablesWidget2::VariablesWidget2()
: VSection("Variable Selection")
{
    _scalarVar = new PVariableSelector( 
        VAPoR::RenderParams::_variableNameTag,
        "Variable Name"
    );
    layout()->addWidget( _scalarVar );
    /*_scalarVarHLI = new PVariableSelectorHLI( 
        "Variable Name HLI",
        &VAPoR::RenderParams::GetVariableName,
        &VAPoR::RenderParams::SetVariableName
    );
    layout()->addWidget( _scalarVar );*/

    /*_colorVar = new PVariableSelector( 
        VAPoR::RenderParams::_colorMapVariableNameTag,
        "Variable Name"
    );
    layout()->addWidget( _scalarVar );
    
    _heightVar = new PVariableSelector( 
        VAPoR::RenderParams::_heightVariableNameTag,
        "Variable Name"
    );
    layout()->addWidget( _scalarVar );*/

    //_fieldVars = new PFieldVariableSelector(
    //    VAPoR::RenderParams::_fieldVariableNamesTag,
    //    "Field Variables"
    //);

}

void VariablesWidget2::Reinit(
    VariableFlags varFlags,
    DimFlags dimFlags
) {}

void VariablesWidget2::Update(
    VAPoR::DataMgr* dataMgr,
    VAPoR::ParamsMgr* paramsMgr,
    VAPoR::RenderParams* rParams
) {
    _scalarVar->Update(    rParams, paramsMgr, dataMgr );
    //_scalarVarHLI->Update( rParams, paramsMgr, dataMgr );
    //_colorVar->Update( rParams,  paramsMgr, dataMgr );
    //_heightVar->Update( rParams,  paramsMgr, dataMgr );
}

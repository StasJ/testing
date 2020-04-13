#pragma once

#include "PVariableSelectorHLI.h"
#include "PVariableSelector.h"
#include "VSection.h"
#include "Flags.h"

class PStringDowndownHLI;

namespace VAPoR {
    class DataMgr;
    class ParamsMgr;
    class RenderParams;
}

class VariablesWidget2 : public VSection {
    Q_OBJECT

public:
    VariablesWidget2();

    void Reinit(
        VariableFlags varFlags,
        DimFlags dimFlags
    );

    void Update(
        VAPoR::DataMgr* dataMgr,
        VAPoR::ParamsMgr* paramsMgr,
        VAPoR::RenderParams* rParams
    );

private:
    PStringDropdown*        _dimSelector;

    PVariableSelector*      _scalarVar;
    PVariableSelectorHLI*   _scalarVarHLI;
    //PFieldVariableSelector  _fieldVars;
    //PVariableSelector*      _colorVar;
    //PVariableSelector*      _heightVar;
};

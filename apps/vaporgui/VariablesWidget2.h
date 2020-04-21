#pragma once

#include "VSection.h"
#include "Flags.h"

namespace VAPoR {
    class DataMgr;
    class ParamsMgr;
    class RenderParams;
}

class PVariableSelector;
class VComboBox;
class VLineItem;

class VariablesWidget2 : public VSection {

    Q_OBJECT

public:

    VariablesWidget2();
    void Reinit(
        VariableFlags varFlags,
        DimFlags      dimFlags
    );

    virtual void Update(
        //const VAPoR::DataMgr *dataMgr,
        VAPoR::DataMgr *dataMgr,
        VAPoR::ParamsMgr *paramsMgr,
        VAPoR::RenderParams *rParams
    );

    int GetActiveDimension() const;
    DimFlags GetDimFlags() const;

    void Configure2DFieldVars();
    void Configure3DFieldVars();

private:

    VComboBox* _dimComboBox;
    VLineItem* _dimLineItem;

    PVariableSelector* _scalarCombo;

    //const VAPoR::DataMgr* _dataMgr;
    VAPoR::DataMgr* _dataMgr;
    VAPoR::ParamsMgr* _paramsMgr;
    VAPoR::RenderParams* _rParams;

private slots:
    void _dimChanged();
};

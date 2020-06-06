#pragma once

#include "PFidelityWidget.h"
#include "PGroup.h"
#include "PVariableSelector.h"
#include "PLineItem.h"
class VComboBox;



class PFieldVariablesSelector : public PGroup {
public:
    PFieldVariablesSelector();
};



class PDimensionSelector : public PLineItem {
    Q_OBJECT
    VComboBox *_vComboBox;
public:
    PDimensionSelector();

protected:
    virtual void updateGUI() const override;
    bool requireDataMgr() const override { return true; }
    
private slots:
    void dropdownTextChanged(std::string text);
};



class PScalarVariableSelector   : public PVariableSelector   { public: PScalarVariableSelector(); };
class PColorMapVariableSelector : public PVariableSelector   { public: PColorMapVariableSelector(); };
class PHeightVariableSelector   : public PVariableSelector2D { public: PHeightVariableSelector(); };


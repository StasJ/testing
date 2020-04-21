#include "VariablesWidget2.h"

#include "PDisplay.h"

#include "PEnumDropdownHLI.h"
#include "PVariableSelectorHLI.h"
#include "VComboBox.h"

#include "vapor/RenderParams.h"

#include <QLayout>
#include <QLabel>

VariablesWidget2::VariablesWidget2() 
    : VSection("Variable Selection")
{
    //Add(
    layout()->addWidget(
        new PEnumDropdownHLI<VAPoR::RenderParams>(
            "Dimensionality HLI",
            {"2D", "3D"},
            {2, 3},
            &VAPoR::RenderParams::GetRefinementLevel,
            &VAPoR::RenderParams::SetRefinementLevel
        )
    );
    /*_dimDropdown = new
        new PEnumDropdown(
            "",
            {"2D", "3D"},
            {2, 3},
            "Dimensionality"
        );
    Add( _dimDropdown );*/

    _dimComboBox = new VComboBox( {"2D", "3D"} );
    _dimLineItem = new VLineItem( "Variable Dimension", _dimComboBox );
    layout()->addWidget( _dimLineItem );
    connect( _dimComboBox, &VComboBox::ValueChanged,
        this, &VariablesWidget2::_dimChanged );

    // No signals from PWidgets?!
    //connect( _dimDropdown, 
    //Add(
    _scalarCombo = new PVariableSelectorHLI<VAPoR::RenderParams>(
            "Variable Name",
            &VAPoR::RenderParams::GetVariableName,
            &VAPoR::RenderParams::SetVariableName
        );
    layout()->addWidget( _scalarCombo );
  
    // 
    // Cannot add widgets directly to the layout because it's not vertical.
    // Widgets must be added with the Add() method, which only accepts PWidgets.
    // This feels like overkill, if I need to add a simple label, then I must use a PWidget?
    // 
    //layout()->addWidget( new PStringDisplay("Field Variables","") );
    /*Add( new PStringDisplay("","Field Variables") );
    Add(
        new PVariableSelectorHLI<VAPoR::RenderParams>(
            "   X",
            &VAPoR::RenderParams::GetXFieldVariableName,
            &VAPoR::RenderParams::SetXFieldVariableName
        )
    );
    Add(
        new PVariableSelectorHLI<VAPoR::RenderParams>(
            "   Y",
            &VAPoR::RenderParams::GetXFieldVariableName,
            &VAPoR::RenderParams::SetXFieldVariableName
        )
    );
    Add(
        new PVariableSelectorHLI<VAPoR::RenderParams>(
            "   Z",
            &VAPoR::RenderParams::GetXFieldVariableName,
            &VAPoR::RenderParams::SetXFieldVariableName
        )
    );*/

}

void VariablesWidget2::Reinit(
    VariableFlags varFlags,
    DimFlags dimFlags
) {

}

void VariablesWidget2::Update(
    //const VAPoR::DataMgr *dataMgr,
    VAPoR::DataMgr *dataMgr,
    VAPoR::ParamsMgr *paramsMgr,
    VAPoR::RenderParams *rParams
) {
    _dataMgr = dataMgr;
    _paramsMgr = paramsMgr;
    _rParams = rParams;

    _scalarCombo->Update( _rParams, _paramsMgr, _dataMgr );
};

void VariablesWidget2::_dimChanged() {
    cout << "dim changed to " << _dimComboBox->GetValue() << endl;
}

int VariablesWidget2::GetActiveDimension() const {

}

DimFlags VariablesWidget2::GetDimFlags() const {

}

void VariablesWidget2::Configure2DFieldVars() {

}

void VariablesWidget2::Configure3DFieldVars() {

}

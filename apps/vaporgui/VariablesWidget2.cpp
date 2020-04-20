#include "VariablesWidget2.h"

#include "PDisplay.h"

#include "PVariableSelectorHLI.h"
#include "PEnumDropdownHLI.h"

#include "vapor/RenderParams.h"

#include <QLayout>
#include <QLabel>

VariablesWidget2::VariablesWidget2() 
    : PSection("Variable Selection")
{
    Add(
        new PEnumDropdownHLI<VAPoR::RenderParams>(
            "Dimensionality HLI",
            {"2D", "3D"},
            {2, 3},
            &VAPoR::RenderParams::GetRefinementLevel,
            &VAPoR::RenderParams::SetRefinementLevel
        )
    );
    _dimDropdown = new
        new PEnumDropdown(
            "",
            {"2D", "3D"},
            {2, 3},
            "Dimensionality"
        )
    Add( _dimDropdown );

    // No signals from PWidgets?!
    //connect( _dimDropdown, 
    Add(
        new PVariableSelectorHLI<VAPoR::RenderParams>(
            "Variable Name",
            &VAPoR::RenderParams::GetVariableName,
            &VAPoR::RenderParams::SetVariableName
        )
    );
  
    // 
    // Cannot add widgets directly to the layout because it's not vertical.
    // Widgets must be added with the Add() method, which only accepts PWidgets.
    // This feels like overkill, if I need to add a simple label, then I must use a PWidget?
    // 
    //layout()->addWidget( new PStringDisplay("Field Variables","") );
    Add( new PStringDisplay("","Field Variables") );
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
    );

}

void VariablesWidget2::Reinit(
    VariableFlags varFlags,
    DimFlags dimFlags
) {

}

int VariablesWidget2::GetActiveDimension() const {

}

DimFlags VariablesWidget2::GetDimFlags() const {

}

void VariablesWidget2::Configure2DFieldVars() {

}

void VariablesWidget2::Configure3DFieldVars() {

}

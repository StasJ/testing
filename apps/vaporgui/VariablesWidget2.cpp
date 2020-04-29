#include "VariablesWidget2.h"

#include "PDisplay.h"

#include "PGroup.h"
#include "PEnumDropdownHLI.h"
#include "PVariableSelectorHLI.h"
#include "VComboBox.h"

#include "vapor/RenderParams.h"

#include <QLayout>
#include <QLabel>

VariablesWidget2::VariablesWidget2() 
    : VSection("Variable Selection"),
    _activeDim( 3 ),
    _initialized( false )
{
    _dimCombo = new VComboBox( {"3D", "2D"} );
    _dimLineItem = new VLineItem( "Variable Dimension", _dimCombo );
    layout()->addWidget( _dimLineItem );
    connect( _dimCombo, &VComboBox::ValueChanged,
        this, &VariablesWidget2::_dimChanged );

    _pg = new PGroup();
    _scalarCombo = new PVariableSelectorHLI<VAPoR::RenderParams>(
            "Variable Name",
            &VAPoR::RenderParams::GetVariableName,
            &VAPoR::RenderParams::SetVariableName
        );
    _scalarCombo->SetDimensionality( 3 );
    _pg->Add(_scalarCombo);
 
    _xFieldCombo = new PVariableSelectorHLI<VAPoR::RenderParams>(
            "X",
            &VAPoR::RenderParams::GetXFieldVariableName,
            &VAPoR::RenderParams::SetXFieldVariableName
        );
    _xFieldCombo->SetDimensionality( 3 );
    _pg->Add(_xFieldCombo);
    
    _yFieldCombo = new PVariableSelectorHLI<VAPoR::RenderParams>(
            "Y",
            &VAPoR::RenderParams::GetYFieldVariableName,
            &VAPoR::RenderParams::SetYFieldVariableName
        );
    _yFieldCombo->SetDimensionality( 3 );
    _pg->Add(_yFieldCombo);
 
    _zFieldCombo = new PVariableSelectorHLI<VAPoR::RenderParams>(
            "Z",
            &VAPoR::RenderParams::GetZFieldVariableName,
            &VAPoR::RenderParams::SetZFieldVariableName
        );
    _zFieldCombo->SetDimensionality( 3 );
    _pg->Add(_zFieldCombo);
    
    _colorCombo = new PVariableSelectorHLI<VAPoR::RenderParams>(
            "Color mapped variable",
            &VAPoR::RenderParams::GetColorMapVariableName,
            &VAPoR::RenderParams::SetColorMapVariableName
        );
    _colorCombo->SetDimensionality( 3 );
    _pg->Add(_colorCombo);

    _heightCombo = new PVariableSelectorHLI<VAPoR::RenderParams>(
            "Height variable",
            &VAPoR::RenderParams::GetHeightVariableName,
            &VAPoR::RenderParams::SetHeightVariableName
        );
    _heightCombo->SetDimensionality( 2 );
    _pg->Add(_heightCombo);

    layout()->addWidget( _pg );
  
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





/*
    if (! ( ( _dimFlags & 2 ) && ( _dimFlags & 3 ) )
    ) {
        _dimCombo->setVisible( false );
        if (dimFlags & THREED) {
            _activeDim = 3;
        }
        else {
            _activeDim = 2;
        }
    }

    if (_variableFlags & SCALAR) {
        _scalarCombo->setVisible( true );
        _scalarCombo->setVisible( true );
    }
    else {
        _scalarCombo->setVisible( false );
    }

    if ( _variableFlags & VECTOR ) {
        _xFieldCombo->setVisible( true );
        _yFieldCombo->setVisible( true );
        _zFieldCombo->setVisible( true );
    }
    else {
        _xFieldCombo->setVisible( false );
        _yFieldCombo->setVisible( false );
        _zFieldCombo->setVisible( false );
    }

    if ( _variableFlags & HEIGHT ) {
        _heightCombo->setVisible( true );
    }
    else {
        _heightCombo->setVisible( false );
    }
*/


void VariablesWidget2::Reinit(
    VariableFlags variableFlags,
    DimFlags dimFlags
) {
    _variableFlags = variableFlags;
    _dimFlags = dimFlags;

    // If the renderer is not both 2D and 3D, hide
    // the dimension selector and set the _activeDim
    if (! ( ( _dimFlags & 2 ) && ( _dimFlags & 3 ) )
    ) { 
        _dimCombo->hide();
        if (dimFlags & THREED) {
            _activeDim = 3;
        }
        else {
            _activeDim = 2;
        }
    }

    if (_variableFlags & SCALAR) {
        _scalarCombo->show();
    }
    else {
        _scalarCombo->hide();
    }

    if ( _variableFlags & VECTOR ) {
        cout << "showing vecs" << endl;
        _xFieldCombo->show();
        _yFieldCombo->show();
        _zFieldCombo->show();
        _xFieldCombo->setVisible( true );
        _yFieldCombo->setVisible( true );
        _zFieldCombo->setVisible( true );
    }
    else {
        cout << "hiding vecs" << endl;
        _xFieldCombo->hide();
        _yFieldCombo->hide();
        _zFieldCombo->hide();
        _xFieldCombo->setVisible( false );
        _yFieldCombo->setVisible( false );
        _zFieldCombo->setVisible( false );
    }

    if ( _variableFlags & HEIGHT ) {
        _heightCombo->show();
    }
    else {
        _heightCombo->hide();
    }

    //_rParams->SetDefaultVariables( _activeDim, false );

    // If the renderer is only 3D, hide the 2D orientation selector
    /*orientationFrame->hide();

    VariableFlags fdf = (VariableFlags)0;
    if (_variableFlags & SCALAR)
        fdf = (VariableFlags)(fdf | SCALAR);

    if (_variableFlags & VECTOR)
        fdf = (VariableFlags)(fdf | VECTOR);

    if (_variableFlags & HEIGHT)
        fdf = (VariableFlags)(fdf | HEIGHT);

    _fidelityWidget->Reinit(fdf); */

    //variableSelectionWidget->adjustSize();
    //adjustSize();
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

    _pg->Update( _rParams, paramsMgr, _dataMgr );
    /*_scalarCombo->Update( _rParams, _paramsMgr, _dataMgr );
    _xFieldCombo->Update( _rParams, _paramsMgr, _dataMgr );
    _yFieldCombo->Update( _rParams, _paramsMgr, _dataMgr );
    _zFieldCombo->Update( _rParams, _paramsMgr, _dataMgr );
    _colorCombo->Update( _rParams, _paramsMgr, _dataMgr );*/
};

void VariablesWidget2::_dimChanged() {
    // Index 0 is 3D, 1 is 2D
    _activeDim = _dimCombo->GetCurrentIndex() == 0 ? 3 : 2;

    _rParams->SetDefaultVariables( _activeDim, false );
    
    _scalarCombo->SetDimensionality( _activeDim );
    _xFieldCombo->SetDimensionality( _activeDim );
    _yFieldCombo->SetDimensionality( _activeDim );
    _zFieldCombo->SetDimensionality( _activeDim );
    _colorCombo->SetDimensionality(  _activeDim );
}

int VariablesWidget2::GetActiveDimension() const {

}

DimFlags VariablesWidget2::GetDimFlags() const {

}

void VariablesWidget2::Configure2DFieldVars() {

}

void VariablesWidget2::Configure3DFieldVars() {

}

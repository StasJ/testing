#include "ParamsWidget.h"
#include "PSpinBox.h"
#include "VSpinBox.h"

PSpinBox::PSpinBox(
    QWidget* parent,
    const std::string& tag,
    const std::string& description,
    const std::string& label,
    int min,
    int max,
    int val
) :
    ParamsWidget(
        parent,
        tag,
        description
    )
{
    _vSpinBox = new VSpinBox( parent, label, min, max, val );
    layout()->addWidget( _vSpinBox );

    connect( _vSpinBox, SIGNAL( valueChanged() ),
        this, SLOT( _updateParams() ) );
}

void PSpinBox::Update( VAPoR::ParamsBase* params ) {
    VAssert( params != nullptr );
    _params = params;

    int value = (int)_params->GetValueDouble( _tag, 0 );
    _vSpinBox->SetValue( value );
}

int PSpinBox::GetValue() const {
    return _vSpinBox->GetValue();
}

void PSpinBox::_updateParams() {
    int value = _vSpinBox->GetValue();
    _params->SetValueDouble( _tag, _description, (double)value );
}

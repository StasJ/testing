#include "ParamsWidgets.h"
#include "VaporWidgets.h"
#include "VSlider.h"
#include "VSpinBox.h"

#include <QVBoxLayout>

#define MIN 0
#define MAX 1

ParamsWidget::ParamsWidget( 
    QWidget* parent,
    const std::string& tag,
    const std::string& description
) : 
    QWidget( parent ),
    _params( nullptr ),
    _tag( tag ),
    _description( description )
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( layout );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}

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

PSlider::PSlider(
    QWidget* parent,
    const std::string& tag,
    const std::string& description,
    const std::string& label,
    double min,
    double max,
    double val
) :
    ParamsWidget(
        parent,
        tag,
        description
    )
{
    _vSlider = new VSlider( parent, label, min, max, val );
    layout()->addWidget( _vSlider );

    connect( _vSlider, SIGNAL( valueChanged() ),
        this, SLOT( _updateParams() ) );
}

void PSlider::Update( VAPoR::ParamsBase* params ) {
    VAssert( params != nullptr );
    _params = params;

    double value = _params->GetValueDouble( _tag, 0.f );
    _vSlider->SetValue( value );
}

double PSlider::GetValue() const {
    return _vSlider->GetValue();
}

void PSlider::_updateParams() {
    double value = _vSlider->GetValue();
    _params->SetValueDouble( _tag, _description, value );
}


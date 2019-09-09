  #include "ParamsWidget.h"
  #include "PSlider.h"
  #include "VSlider.h"

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

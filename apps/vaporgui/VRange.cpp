#include "VRange.h"

VRange::VRange(
    QWidget* parent,
    double min,
    double max,
    const std::string& minLabel,
    const std::string& maxLabel
) :
    VaporWidget( parent )
{
    _layout    = new QVBoxLayout(this);
    _layout->setContentsMargins( 0, 0, 0, 0 );

    _minSlider = new VSlider( this, minLabel, min, max, min );
    _maxSlider = new VSlider( this, maxLabel, min, max, max );
    connect( _minSlider, SIGNAL( valueChanged() ), this, SLOT( _respondMinSlider() ) );
    connect( _maxSlider, SIGNAL( valueChanged() ), this, SLOT( _respondMaxSlider() ) );

    _layout->addWidget( _minSlider );
    _layout->addWidget( _maxSlider );
}

VRange::VRange(
    double min,
    double max,
    const std::string& minLabel,
    const std::string& maxLabel
) : VRange(
        nullptr,
        min,
        max,
        minLabel,
        maxLabel
    )
{}

VRange::~VRange() {}

void
VRange::SetSelection( double min, double max ) {
    VAssert( min <= max );
    _minSlider->SetValue( min );
    _maxSlider->SetValue( max );
}

void
VRange::GetSelection( double& min, double& max ) const {
    min = _minSlider->GetValue();
    max = _maxSlider->GetValue();
}

void
VRange::SetExtents( double extentsLow, double extentsHigh )
{
    VAssert( extentsHigh >= extentsLow );
    _minSlider->SetExtents( extentsLow, extentsHigh );
    _maxSlider->SetExtents( extentsLow, extentsHigh );
}

void
VRange::_adjustMaxToMin()
{
    double low = _minSlider->GetValue();
    double high = _maxSlider->GetValue();
    if( low > high )
        _maxSlider->SetValue( low );
}

void
VRange::_adjustMinToMax()
{
    double low = _minSlider->GetValue();
    double high = _maxSlider->GetValue();
    if( high < low )
        _minSlider->SetValue( high );
}

void
VRange::_respondMinSlider()
{
    _adjustMaxToMin();
    emit valueChanged();
}

void
VRange::_respondMaxSlider()
{
    _adjustMinToMax();
    emit valueChanged();
}

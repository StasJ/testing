#include <QWidget>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>

#include "VSlider.h"

VSlider::VSlider(
    QWidget* parent,
    const std::string& label,
    double min,
    double max,
    double value )
:
    VLabel( parent, label ),
    _min( min ),
    _max( max ),
    _value( value )
{
    VAssert( _max > _min );
    _value = (_min + _max) / 2.0f;

    _qslider = new QSlider( this );
    _qslider->setOrientation( Qt::Horizontal );
    // QSlider will always have its range in integers from 0 to 100.
    _qslider->setMinimum( 0 );
    _qslider->setMaximum( 100 );
    connect( _qslider, SIGNAL( sliderReleased() ), this, SLOT( _respondQSliderReleased() ) );
    connect( _qslider, SIGNAL( sliderMoved(int) ), this, SLOT( _respondQSliderMoved(int) ) );
    _layout->addWidget( _qslider );

    _qedit = new QLineEdit( this );
    connect( _qedit, SIGNAL( editingFinished() ), this, SLOT( _respondQLineEdit() ) );
    _layout->addWidget( _qedit );

    // update widget display
    double percent = (_value - _min) / (_max - _min) * 100.0f;
    _qslider->setValue( std::lround( percent ) );
    _qedit->setText( QString::number( _value, 'f', 3 ) );
}

VSlider::VSlider(
    const std::string& label,
    double min,
    double max,
    double value )
: VSlider(
        nullptr,
        label,
        min,
        max,
        value
) {}

VSlider::~VSlider() {}

void
VSlider::SetExtents( double min, double max )
{
    VAssert( min <= max );
    _min = min;
    _max = max;

    // keep the old _value if it's still within the range.
    //   Otherwise, re-assign the middle point to _value
    if( _value < min ||  _value > max )
    {
        _value   = (min + max) / 2.0f;
        _qedit->setText( QString::number( _value, 'f', 3 ) );
    }

    // update the slider position based on new range.
    double percent = (_value - _min) / (_max - _min) * 100.0f;
    _qslider->setValue( std::lround( percent ) );
}

void
VSlider::SetValue( double val )
{
    // Only respond if val is within range
    if( val >= _min && val <= _max )
    {
        _value   = val;
        double percent = (_value - _min) / (_max - _min) * 100.0f;
        _qslider->setValue( std::lround( percent ) );
        _qedit->setText( QString::number( _value, 'f', 3 ) );
    }
}

double
VSlider::GetValue() const
{
    return _value;
}

void
VSlider::_respondQSliderReleased()
{
    // QSlider is always giving a valid value, so no need to validate range
    int newvalI    = _qslider->value();
    double percent  = (double)newvalI / 100.0f;
    _value = _min + percent * (_max - _min);
    _qedit->setText( QString::number( _value, 'f', 3 ) );

    emit valueChanged();
}

void
VSlider::_respondQSliderMoved( int newPos )
{
    // QSlider is always at a valid position, so no need to validate range
    double percent   = (double)newPos / 100.0f;
    double tmpVal    = _min + percent * (_max - _min);
    _qedit->setText( QString::number( tmpVal, 'f', 3 ) );
    emit valueChanged();
}

void
VSlider::_respondQLineEdit()
{
    std::string newtext = _qedit->text().toStdString();
    double   newval;

    try
    {
        newval = std::stof( newtext );
    }
    catch ( const std::invalid_argument& e )
    {
        _qedit->setText( QString::number( _value, 'f', 3 ) );
        return;
    }

    // Now validate the input is within range
    if( newval < _min || newval > _max )
    {
        _qedit->setText( QString::number( _value, 'f', 3 ) );
        return;
    }

    // Now update _value, _qslider, and emit signal
    _value   = newval;
    double percent = (_value - _min) / (_max - _min) * 100.0f;
    _qslider->setValue( std::lround( percent ) );

    emit valueChanged();
}

#include <QWidget>
#include <QSpinBox>
#include "VSpinBox.h"

VSpinBox::VSpinBox(
        QWidget *parent,
        const std::string& labelText,
        int min,
        int max,
        int defaultValue
    ) :
    VLabel(parent, labelText),
    _min( min ),
    _max( max ),
    _value( defaultValue )
{
    _spinBox = new QSpinBox( this );
    SetRange( min, max );

    _spinBox->setValue( _value );

    _layout->addWidget( _spinBox );

    connect( _spinBox, SIGNAL( editingFinished() ),
        this, SLOT( _validateAndEmit() ) );
}

VSpinBox::VSpinBox(
        const std::string& labelText,
        int min,
        int max,
        int defaultValue
) : VSpinBox( nullptr,
              labelText,
              min,
              max,
              defaultValue
    )
{}

void VSpinBox::SetValue( int value ) {
     if ( value != _value &&
          value <= _max   &&
          value >= _min   )
    {
        _value = value;
        _spinBox->setValue( value );
    }
}

int VSpinBox::GetValue() const {
    return _value;
}

void VSpinBox::_validateAndEmit() {
    int newValue = _spinBox->value();

     if ( newValue != _value &&
          newValue <= _max   &&
          newValue >= _min   )
    {
        _value = newValue;
        emit valueChanged();
    }
}

void VSpinBox::SetRange( int min, int max ) {
    if ( max < min ) VAssert( max > min );

    _min = min;
    _max = max;

    _spinBox->setMinimum( min );
    _spinBox->setMaximum( max );
}

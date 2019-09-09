#ifndef VSPINBOX_H
#define VSPINBOX_H

#include "VLabel.h"

class VSpinBox : public VLabel
{
    Q_OBJECT

public:
    VSpinBox(
        QWidget* parent = nullptr,
        const std::string& labelText = "VSpinBox",
        int min = 0,
        int max = 100,
        int defaultValue = 0
    );
    VSpinBox(
        const std::string& labelText = "VSpinBox",
        int min = 0,
        int max = 100,
        int defaultValue = 0
    );

    void SetValue( int value );
    int  GetValue() const;

    void SetRange( int minimum, int maximum );

protected:
    QSpinBox* _spinBox;

private:
    int _min;
    int _max;
    int _value;

protected slots:
    void _validateAndEmit();

signals:
    void valueChanged();

};

#endif

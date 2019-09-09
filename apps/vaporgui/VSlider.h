#ifndef VSLIDER_H
#define VSLIDER_H

#include "VLabel.h"

class VSlider : public VLabel
{
    Q_OBJECT

public:
    VSlider(
        QWidget* parent = nullptr,
        const std::string& label = "VSlider",
        double min   = 0,
        double max   = 100,
        double value = 0
    );
    VSlider(
        const std::string& label = "VSlider",
        double min   = 0,
        double max   = 100,
        double value = 0
    );
    ~VSlider();

    void SetValue( double val );
    double GetValue() const;

    void SetExtents( double min, double max );

signals:
    void  valueChanged();

private slots:
    void  _respondQSliderReleased();    // emit signal
    void  _respondQSliderMoved(int);    // sync qSlider and qLineEdit
    void  _respondQLineEdit();          // emit signal

private:
    double      _min, _max, _value;
    QSlider*    _qslider;
    QLineEdit*  _qedit;
};

#endif

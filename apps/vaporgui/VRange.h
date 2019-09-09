#ifndef VRANGE_H
#define VRANGE_H

#include "VSlider.h"

class VRange : public VaporWidget
{
    Q_OBJECT

public:
    VRange( QWidget* parent = nullptr,
            double min = 0.0,
            double max = 1.0,
            const std::string& minLabel = "Min",
            const std::string& maxLabel = "Max"  );
    VRange( double min = 0.0,
            double max = 1.0,
            const std::string& minLabel = "Min",
            const std::string& maxLabel = "Max"  );
    ~VRange();

    void  SetSelection( double  rangeLow, double  rangeHigh );
    void  GetSelection( double& rangeLow, double& rangeHigh ) const;

    void  SetExtents( double extentsLow, double extentsHigh );

signals:
    void  valueChanged();

private slots:
    void  _respondMinSlider();
    void  _respondMaxSlider();

private:
    VSlider        *_minSlider, *_maxSlider;
    //QVBoxLayout*    _layout;

    // In case _minSlider is changed, adjust _maxSlider if necessary.
    void  _adjustMaxToMin();
    // In case _maxSlider is changed, adjust _minSlider if necessary.
    void  _adjustMinToMax();
};

#endif

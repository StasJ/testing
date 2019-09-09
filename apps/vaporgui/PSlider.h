#ifndef PSLIDER_H
#define PSLIDER_H

#include "ParamsWidget.h"

class VSlider;

class PSlider : public ParamsWidget
{
    Q_OBJECT

public:
    PSlider(
        QWidget* parent,
        const std::string& tag,
        const std::string& description,
        const std::string& label,
        double min=0,
        double max=100,
        double val = 0
    );

    void Update( VAPoR::ParamsBase* params ) override;

    double GetValue() const;

protected slots:
    void _updateParams() override;

private:
    VSlider* _vSlider;
};

#endif

#ifndef PSPINBOX_H
#define PSPINBOX_H

#include "ParamsWidget.h"

class VSpinBox;

class PSpinBox : public ParamsWidget
{
    Q_OBJECT

public:
    PSpinBox(
        QWidget* parent,
        const std::string& tag,
        const std::string& description,
        const std::string& label,
        int min = 0,
        int max = 100,
        int val = 0
    );

    virtual void Update( VAPoR::ParamsBase* params) override;

    int GetValue() const;

protected slots:
    void _updateParams() override;

private:
    VSpinBox* _vSpinBox;
};

#endif


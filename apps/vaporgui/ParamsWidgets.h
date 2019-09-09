#ifndef PARAMSWIDGETS_H
#define PARAMSWIDGETS_H

#include <vapor/ParamsBase.h>

#include <QWidget>

class VSpinBox;
class VSlider;
class VRange;
class VGeometry;

class ParamsWidget : public QWidget
{
    Q_OBJECT

public:
    virtual void Update( VAPoR::ParamsBase* params ) = 0;

protected:
    ParamsWidget( 
        QWidget* parent, 
        const std::string& tag,
        const std::string& description
    );

    ~ParamsWidget() {};

    VAPoR::ParamsBase* _params;
    std::string _tag;
    std::string _description;

protected slots:
    virtual void _updateParams() = 0;
};

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

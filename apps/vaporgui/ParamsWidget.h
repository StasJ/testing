#ifndef PARAMSWIDGETS_H
#define PARAMSWIDGETS_H

#include <vapor/ParamsBase.h>

#include <QWidget>

class VSlider;
class VRange;
class VGeometry;

class ParamsWidget : public QWidget
{
    Q_OBJECT

public:
    virtual void Update( VAPoR::ParamsBase* params ) = 0;
    void SetToolTip( std::string& toolTip);

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

#endif

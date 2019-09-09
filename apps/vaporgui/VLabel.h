#ifndef VLABEL_H
#define VLABEL_H

#include "VaporWidget.h"

//class QWidget;
//class QLabel;
//class QSpacerItem;

class VLabel : public VaporWidget
{
    Q_OBJECT

public:
    VLabel(
        QWidget* parent = nullptr,
        const std::string& labelText = "VLabel"
    );
    VLabel(
        const std::string& labelText = "VLabel"
    );

    void SetLabelText( const std::string& text );

protected:
    QLabel*      _label;
    QSpacerItem* _spacer;
};

#endif

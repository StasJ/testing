#include "VLabel.h"

#include <QLabel>
#include <QSpacerItem>
#include <QHBoxLayout>

VLabel::VLabel(
    QWidget* parent,
    const std::string& labelText
    ) :
    VaporWidget( parent )
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins( 0, 0, 0, 0 );

    _label = new QLabel(this);
    _spacer = new QSpacerItem(
        10,
        10,
        QSizePolicy::MinimumExpanding,
        QSizePolicy::Minimum
    );

    _layout->addWidget( _label );
    _layout->addItem( _spacer );  // sets _spacer's parent to _layout

    SetLabelText( labelText );

    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}

VLabel::VLabel(
    const std::string& labelText
    ) :
    VLabel( nullptr, labelText )
{}


void VLabel::SetLabelText( const std::string& text )
{
    _label->setText( QString::fromStdString( text ) );
}

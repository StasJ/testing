#include "ParamsWidget.h"
#include "VaporWidget.h"
#include "VSlider.h"
#include "VSpinBox.h"

#include <QVBoxLayout>

#define MIN 0
#define MAX 1

ParamsWidget::ParamsWidget( 
    QWidget* parent,
    const std::string& tag,
    const std::string& description
) : 
    QWidget( parent ),
    _params( nullptr ),
    _tag( tag ),
    _description( description )
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( layout );
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
}

void ParamsWidget::SetToolTip( std::string& toolTip ){
    setToolTip( QString::fromStdString( toolTip ) );
}

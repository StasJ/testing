#include "VGeometry.h"
#include "VRange.h"

#define XMIN 0
#define YMIN 1
#define ZMIN 2
#define XMAX 3
#define YMAX 4
#define ZMAX 5

VGeometry::VGeometry(
        QWidget* parent,
        const std::vector<double>& range,
        const std::vector<std::string>& label
) : VaporWidget( parent )
{
    _dim = range.size() / 2;
    VAssert( _dim == 2 || _dim == 3 );

    for( int i = 0; i < _dim; i++ )
        VAssert( range[ i ] < range[ i+3 ] );

    //QWidget* pageWidget = new QWidget();
    _layout = new QVBoxLayout( this );
    _layout->setContentsMargins( 0, 0, 0, 0 );
//    setLayout( layout );

    _xrange = new VRange( this, range[ XMIN ], range[ XMAX ], label[ XMIN ], label[ XMAX ] );
    _yrange = new VRange( this, range[ YMIN ], range[ YMAX ], label[ YMIN ], label[ YMAX ] );
    if( _dim == 3 )
        _zrange = new VRange( this, range[ ZMIN ], range[ ZMAX ], label[ ZMIN ], label[ ZMAX ] );
    else    // Create anyway. Will be hidden though.
    {
        _zrange = new VRange( this, 0.0f, 100.0f, "ZMin", "ZMax" );
        _zrange->hide();
    }

    connect( _xrange, SIGNAL( valueChanged() ), this, SLOT( _respondChanges() ) );
    connect( _yrange, SIGNAL( valueChanged() ), this, SLOT( _respondChanges() ) );
    connect( _zrange, SIGNAL( valueChanged() ), this, SLOT( _respondChanges() ) );

    //_layout = new QVBoxLayout(this);
    _layout->addWidget( _xrange );
    _layout->addWidget( _yrange );
    _layout->addWidget( _zrange );
    //addTab( pageWidget, QString::fromStdString(label) );
}

VGeometry::VGeometry(
        const std::vector<double>& range,
        const std::vector<std::string>& label
) : VGeometry( nullptr, range, label ) {}

VGeometry::~VGeometry() {}

void
VGeometry::SetRange( const std::vector<double>& range )
{
    VAssert( range.size() == _dim * 2 );
    _dim = range.size() / 2;
    VAssert( _dim == 2 || _dim == 3 );

    for( int i = 0; i < _dim; i++ )
        VAssert( range[ i ] < range[ i+3 ] );

    // Adjust the appearance if necessary
    if( _dim == 3 )
        _zrange->show();
    else
        _zrange->hide();

    _xrange->SetExtents( range[ XMIN ], range[ XMAX ] );
    _yrange->SetExtents( range[ YMIN ], range[ YMAX ] );
    if( _dim == 3 )
        _zrange->SetExtents( range[ ZMIN ], range[ ZMAX ] );
}

void
VGeometry::SetGeometry( const std::vector<double>& vals )
{
    if ( vals.size() != _dim * 2 )
        return;

    for( int i = 0; i < _dim; i++ )
        VAssert( vals[ i ] <= vals[ i+3 ] );

    // VRange widgets will only respond to values within their ranges
    _xrange->SetSelection( vals[ XMIN ], vals[ XMAX ] );
    _yrange->SetSelection( vals[ YMIN ], vals[ YMAX ] );
    if( _dim == 3 )
        _zrange->SetSelection( vals[ ZMIN ], vals[ ZMAX ] );
}

void
VGeometry::GetGeometry( std::vector<double>& vals ) const
{
    vals.resize( _dim * 2, 0.0f );
    _xrange->GetSelection( vals[ XMIN ], vals[ XMAX ] );
    _yrange->GetSelection( vals[ YMIN ], vals[ YMAX ] );
    if( _dim == 3 )
        _zrange->GetSelection( vals[ ZMIN ], vals[ ZMAX ] );
}

void
VGeometry::_respondChanges()
{
    emit valueChanged();
}

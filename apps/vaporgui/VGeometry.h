#ifndef VGEOMETRY_H
#define VGEOMETRY_H

#include "VaporWidget.h"

class VRange;
class QWidget;

class VGeometry : public VaporWidget
{
    Q_OBJECT

public:
    // Constructor for 2D or 3D geometries.
    //   Four floating point values imply a 2D geometry, while Six floating point
    //   values imply a 3D geometry. All other numbers are illegal.
    VGeometry(
        QWidget* parent,
        const std::vector<double>& extents,
        const std::vector<std::string>& labels
    );
    VGeometry(
        const std::vector<double>& extents,
        const std::vector<std::string>& labels
    );

    ~VGeometry();

    // Adjust the value ranges through this function.
    //   Argument range must contain 4 or 6 values organized in the following order:
    //   xmin, ymin, (zmin), xmax, ymax, (zmax)
    void  SetRange( const std::vector<double>& range );
    // The number of incoming values MUST match the current dimensionality.
    //   I.e., 4 values for 2D widgets, and 6 values for 3D widgets.
    void  SetGeometry( const std::vector<double>& vals );
    void  GetGeometry( std::vector<double>& vals ) const;

signals:
    void  valueChanged();

private slots:
    void  _respondChanges();

private:
    int          _dim;
    VRange      *_xrange, *_yrange, *_zrange;
};

#endif

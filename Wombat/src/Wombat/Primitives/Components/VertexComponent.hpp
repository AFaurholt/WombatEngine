#include "ColorComponent.hpp"
#include "PositionComponent.hpp"

namespace Wombat::Primitives
{
    //TODO make template
    class VertexComponent
    {
    private:
        /* data */
        ColorComponent _colorComponent;
        PositionComponent _positionComponent;

    public:
        VertexComponent(/* args */);
        ~VertexComponent();
    };
} // namespace Wombat::Primitives

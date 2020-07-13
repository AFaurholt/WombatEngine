#include <vector>

#include "VertexComponent.hpp"

namespace Wombat::Primitives
{
    class MeshComponent
    {
    private:
        /* data */
        std::vector<VertexComponent> _vertexComponents;

    public:
        MeshComponent(/* args */);
        ~MeshComponent();
    };
    
    
    
} // namespace Wombat::Primitives

namespace Wombat::Rendering
{
    class RendererOptionsComponent
    {
    private:
        bool _isDebugEnabled;
        const char* _appTitle;

    public:
        RendererOptionsComponent(bool isDebugEnabled, const char *appTitle);
        ~RendererOptionsComponent();
    };

    RendererOptionsComponent::RendererOptionsComponent(bool isDebugEnabled, const char *appTitle)
    {
        _isDebugEnabled = isDebugEnabled;
        _appTitle = appTitle;
    }

    RendererOptionsComponent::~RendererOptionsComponent()
    {
    }

} // namespace Wombat::Rendering

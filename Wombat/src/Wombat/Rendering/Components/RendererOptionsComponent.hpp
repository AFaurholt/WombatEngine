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
} // namespace Wombat::Rendering

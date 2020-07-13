namespace Wombat::Window
{
    //TODO make template
    class WindowComponent
    {
    private:
        /* data */
        //GLFW specialization
        GLFWwindow* _glfwWindowHandle;

    public:
        WindowComponent(/* args */);
        ~WindowComponent();
    };
    
} // namespace Wombat::Window

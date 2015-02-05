#include <Lums/Lums.hpp>

class Hello : public lm::GameState
{
public:
    Hello()
    {

    }

    void
    load()
    {
        glMatrixMode(GL_PROJECTION);
        glOrtho(0, 200, 200, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        _font.load("Arial.ttf", 50);
    }

    void
    update()
    {

    }

    void
    render() const
    {/*
        glBegin(GL_QUADS);
        glColor3d(1, 0, 0);
        glVertex2i(0, 0);
        glColor3d(0, 1, 0);
        glVertex2i(200, 0);
        glColor3d(0, 0, 1);
        glVertex2i(200, 200);
        glColor3d(1, 1, 0);
        glVertex2i(0, 200);
        glEnd();
        glColor3d(1, 1, 1);*/
        //_font.puts("HelloHelloHello");
        _font.debug();
    }

    void
    handleEvent(const lm::Event& event)
    {
        if (event.type == lm::Event::Type::KeyDown)
            lm::Core::get().stop();
    }

private:
    lm::Font _font;
};

int
main()
{
    lm::Core core(200, 200, "Hello !");

    core.push<Hello>();
    core.start();
    return 0;
}
#include <Lums/Lums.h>

class Nyan : public lm::GameState
{
public:
    Nyan(lm::Core* core)
    : lm::GameState(core), _linear(true)
    {
        _image.LoadFile("Nyan.png");
    }

    void
    Update()
    {

    }

    void
    HandleEvent(const lm::Event& event)
    {
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_v)
            {
                _linear = !_linear;
                _image.Linear(_linear);
            }
            else
                Core().Stop();
        }
    }

    void
    Render()
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glEnable(GL_TEXTURE_2D);
        _image.Bind();
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(-1, -1);
        glTexCoord2f(1, 0);
        glVertex2f(1, -1);
        glTexCoord2f(1, 1);
        glVertex2f(1, 1);
        glTexCoord2f(0, 1);
        glVertex2f(-1, 1);
        glEnd();
        glDisable(GL_TEXTURE_2D);

    }

private:
    lm::Image   _image;
    bool        _linear;
};

int
main()
{
    lm::Core core(400, 400, "Nyan");

    core.Push<Nyan>();
    core.Start();
}
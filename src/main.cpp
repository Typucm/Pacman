#include <SFML/Graphics.hpp>
#include <ctime>
#include <cmath>
#include <iostream>
#include <cassert>

using namespace sf;

float offsetX=0, offsetY=0;
int k;
const int H = 16;
const int W = 16;

String TileMap[H] = {

    "BBBBBBBBBBBBBBBB",
    "B00000000000000B",
    "B0BBBBBBBBBBBB0B",
    "B0B0000000000B0B",
    "B0B0        0B0B",
    "B0BB  000   BB0B",
    "B0    BBB     0B",
    "BBB   0 0    BBB",
    "B0    BBB     0B",
    "B0BB  000   BB0B",
    "B0B0        0B0B",
    "B0B0        0B0B",
    "B0B0000000000B0B",
    "B0BBBBBBBBBBBB0B",
    "B00000000000000B",
    "BBBBBBBBBBBBBBBB",
};



class Pacman {

public:

    float dx,dy;
    FloatRect rect;
    Sprite sprite;
    float currentFrame;

    Pacman (Texture &image)
    {
        sprite.setTexture(image);
        rect = FloatRect(266,256,32,32);
        dx=dy=0.04;
        currentFrame=0;
    }


    void update(float time)
    {
        collision(0);
        rect.left += dx * time;
        currentFrame+=0.003*time;
        if (currentFrame >3) currentFrame-=3;
        if (dx<0)sprite.setTextureRect(IntRect(34*int(currentFrame),0,33,33));
        if (dx>0)sprite.setTextureRect(IntRect(34*int(currentFrame)+33,0,-33,33));

        collision(1);
        rect.top += dy * time;
        if (currentFrame >3) currentFrame-=3;
        if (dy<0)sprite.setTextureRect(IntRect(33*int(currentFrame),98,33,33));
        if (dy>0)sprite.setTextureRect(IntRect(33*int(currentFrame),32,33,33));

        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);

        dx=0;
        dy=0;
    }

    void collision(int dir)
    {
        for (int i = rect.top/32 ; i<(rect.top+rect.height)/33; i++)
            for (int j = rect.left/32; j<(rect.left+rect.width)/33; j++)
            {
                if (TileMap[i][j]=='B')
                {
                    if ((dx>0) && (dir==1)) rect.left =  j*33 -  rect.width;
                    if ((dx<0) && (dir==1)) rect.left =  j*33 + 32.9;
                    if ((dy>0) && (dir==1))  { rect.top =   i*32.9 -  rect.height;}
                    if ((dy<0) && (dir==1))  { rect.top = i*33 + 33; }
                }

                if (TileMap[i][j]=='0')

                {
                    k=k+1;
                    TileMap[i][j]=' ';
                }
                if(k==84){

                    RenderWindow window(VideoMode(275, 183), "WIN!");

                    Texture l;
                    l.loadFromFile("win.png");
                    Sprite n;
                    n.setTexture(l);
                    n.setTextureRect(IntRect(0,0,275,183));
                    n.setPosition(0,0);

                    while (window.isOpen())
                    {
                        Event event;
                        while (window.pollEvent(event))
                        {
                            if (event.type == Event::Closed)
                                window.close();
                        }

                        window.clear();
                        window.draw(n);
                        window.display();
                    }

                }

            }
    }
};

struct Position
{
    double x;
    double y;
};


class Enemy
{
public:
    float dx,dy;
    FloatRect rect;
    Sprite sprite;

    float currentFrame;

    bool life;

    bool isHorizontal;

    enum Direction {None, Left, Right, Up, Down};

    Direction dir;

    void set(Texture &image, int x, int y)
    {
        srand(std::time(0));

        sprite.setTexture(image);
        rect = FloatRect(x,y,32,32);

        dy=0.04;
        dx=0.04;
        currentFrame = 0;
        life=true;

        dir = randomDirection();
    }

    void update(float time)
    {
        Position oldPos = {rect.left, rect.top};
        Position pos = nextPosition(oldPos, time);

        Direction dir = moveDirection(oldPos, pos);
        int frame = animationFrame(time);

        setupPosition(pos);
        setupSprite(dir, frame);
    }

private:
    Position positionByDir(const Position& pos, float time)
    {
        static double const dl = 0.04;
        double len = time*dl;

        switch (dir) {
        case Left: return {pos.x-len, pos.y};
        case Down: return {pos.x, pos.y+len};
        case Right: return {pos.x+len, pos.y};
        case Up: return {pos.x, pos.y-len};
        }

        return {pos.x, pos.y};
    }

    bool isValidPosition(const Position& pos)
    {
        int i = pos.y/33;
        int j = pos.x/33;

        return TileMap[i][j] != 'B'
                && TileMap[i+1][j] != 'B'
                && TileMap[i][j+1] != 'B'
                && TileMap[i+1][j+1] != 'B';
    }

    Position nextPosition(const Position& pos, float time)
    {
        if (rand()%100 == 0)
            dir = randomDirection();

        for (int trial=0; trial<5; ++trial) {
            Position wantPos = positionByDir(pos, time);
            if (isValidPosition(wantPos))
                return wantPos;
            dir = randomDirection();
        }

        dir = None;
        return pos;
    }

    Direction moveDirection(const Position& oldPos, const Position& pos)
    {
        double dx = pos.x - oldPos.x;
        double dy = pos.y - oldPos.y;

        if (dx!=0)
            return (dx<0)? Left: Right;

        return (dy<0)? Up: Down;
    }

    int animationFrame(float time)
    {
        const double dt = 0.003;
        return int(floor(time/dt))%3;
    }

    void setupPosition(const Position& pos)
    {
        rect.left = pos.x;
        rect.top = pos.y;
        sprite.setPosition(rect.left - offsetX, rect.top - offsetY);
    }

    void setupSprite(Direction dir, int frame)
    {
        if (!life) {
            sprite.setTextureRect(IntRect(20, 280, 33,33));
            return;
        }

        int d;
        switch (dir) {
        case None: d = 0; break;
        case Left: d = 0; break;
        case Down: d = 1; break;
        case Right: d = 2; break;
        case Up: d = 3; break;
        }

        sprite.setTextureRect(IntRect(33*frame, 198+d*33, 32,32));
    }

    Direction randomDirection()
    {
        return static_cast<Direction>(rand()%5);
    }
};

int main()
{
    RenderWindow window(VideoMode(528, 528), "PACMAN");

    Texture p;
    p.loadFromFile("Pakman.jpg");

    //float currentFrame=0;

    Pacman t(p);
    Enemy enemy;

    enemy.set(p,200,130);

    Clock clock;

    Sprite k;
    k.setTexture(p);
    k.setTextureRect(IntRect(0,0,33,33));
    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();

        time = time/500;

        if (time>1000) time = 1000;
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Left))
            t.dx=-0.04;
        else if (Keyboard::isKeyPressed(Keyboard::Right))
            t.dx=0.04;
        else if (Keyboard::isKeyPressed(Keyboard::Up))
            t.dy=-0.04;
        else if (Keyboard::isKeyPressed(Keyboard::Down))
            t.dy=0.04;

        t.update(time);
        enemy.update(time);

        if  (t.rect.intersects( enemy.rect ) )
        {
            if (enemy.life) {
                if (t.dy>0) {
                    enemy.dx=0; t.dy=-0.2; enemy.life=false;
                }
                else {
                    RenderWindow window(VideoMode(275, 183), "Lose!");

                    Texture m;
                    m.loadFromFile("index.jpg");
                    Sprite b;
                    b.setTexture(m);
                    b.setTextureRect(IntRect(0,0,275,183));
                    b.setPosition(0,0);

                    while (window.isOpen())
                    {
                        Event event;
                        while (window.pollEvent(event))
                        {
                            if (event.type == Event::Closed)
                                window.close();
                        }

                        window.clear();
                        window.draw(b);
                        window.display();
                    }
                }
            }
        }

        window.clear(Color::White);


        for (int i=0; i<H; i++)
            for (int j=0; j<W ; j++)
            {
                if (TileMap[i][j]=='B') k.setTextureRect(IntRect(0,132,33,33) );

                if (TileMap[i][j]=='0') k.setTextureRect(IntRect(01,166,30,30) );

                if (TileMap[i][j]==' ') continue;

                k.setPosition(j*33,i*33) ;

                window.draw(k);

            }

        window.draw(t.sprite);
        window.draw(enemy.sprite);
        window.display();
    }

    return 0;

}

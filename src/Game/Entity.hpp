#pragma once
#include <stdint.h>
enum class Type{
    obstacle,
    Grass,
    Cloud,
    Character
};
enum class Status{
    Death,
    Survive,
    Immortal
};

struct Vec2{
    float x;
    float y;
};

class Entity{
public:
    Entity(Vec2 pos,Vec2 box):vec2l(pos),Box(box){}
    Entity()=default;

    ~Entity(){
        vec2l={0,0};
        Box={0,0};
        HP=0;
        speed=0;
    }

public:
    float Gravity=0.7;
    Vec2 vec2l;
    Vec2 Box;
    uint8_t HP;
    float speed;
    uint8_t* Shape;
    bool Ground;
    Type type;

};
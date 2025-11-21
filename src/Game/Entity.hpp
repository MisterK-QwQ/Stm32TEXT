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

    Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }
    Vec2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }
};

class Entity{
public:
    Entity(Vec2 pos, Vec2 box) : vec2l(pos), Box(box) {}
    Entity() = default;

    ~Entity(){
        vec2l = {0,0};
        Box = {0,0};
        HP = 0;
        speed = 0;
        velocity = {0,0};
    }
void updatePhysics(float deltaTime, int groundY) {
    velocity.y += Gravity * deltaTime;
    
    if (vec2l.y + Box.y >= groundY) {
        vec2l.y = groundY - Box.y;
        velocity.y = 0;
        isOnGround = true;
    } else {
        isOnGround = false;
    }
    
    if (vec2l.y < 0) {
        vec2l.y = 0;
        velocity.y = 0; 
    }
    
    if (vec2l.x < 0) vec2l.x = 0;
    if (vec2l.x + Box.x > 128) vec2l.x = 128 - Box.x;
    
    vec2l = vec2l + velocity * deltaTime;
}
    void jump(float force) {
        if (isOnGround) {
            velocity.y = -force;
            isOnGround = false; 
        }
    }
public:
    float Gravity = 20.0f;  
    Vec2 vec2l;          
    Vec2 Box;               
    Vec2 velocity = {0,0}; 
    uint8_t HP = 100;
    float speed = 15.0f;    
    uint8_t* Shape;
    Type type;
    Status status = Status::Survive;
    bool isOnGround = false;
};
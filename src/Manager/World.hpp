#pragma once
#include "../Game/Entity.hpp"
#include <vector>
#include "Manager.hpp"
#include "../Utils/Utils.hpp" 

class World{
public:
    std::vector<Entity> entities;  // 修正变量名（复数形式更合理）
    int groundY = 50;              // 地面Y坐标
    uint32_t lastUpdateTime = 0;   // 用于计算deltaTime

    void addEntity(Vec2 pos, Vec2 box, Type _type, uint8_t* _Shape){
        Entity _entity(pos, box);
        _entity.type = _type;
        _entity.Shape = _Shape;
        entities.push_back(_entity);
    }

    bool checkCollision(const Entity& a, const Entity& b) {
        return a.vec2l.x < b.vec2l.x + b.Box.x &&
               a.vec2l.x + a.Box.x > b.vec2l.x &&
               a.vec2l.y < b.vec2l.y + b.Box.y &&
               a.vec2l.y + a.Box.y > b.vec2l.y;
    }

    void handleCollisions() {
        for (size_t i = 0; i < entities.size(); ++i) {
            for (size_t j = i + 1; j < entities.size(); ++j) {
                if (checkCollision(entities[i], entities[j])) {
                    if ((entities[i].type == Type::Character && entities[j].type == Type::obstacle) ||
                        (entities[j].type == Type::Character && entities[i].type == Type::obstacle)) {
                        Entity& player = (entities[i].type == Type::Character) ? entities[i] : entities[j];
                        player.HP -= 10;
                        if (player.HP <= 0) {
                            player.status = Status::Death;
                        }
                    }
                }
            }
        }
    }

    void update() {
        uint32_t currentTime = manager.tick;
        float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
        lastUpdateTime = currentTime;
        manager.LDC.clearScreen();
        manager.LDC.drawLine(0, groundY, 127, groundY, 1);
        for (auto& entity : entities) {
              if (entity.type == Type::Character) {
                std::string HPstr="";
                if(entity.status == Status::Death){
                    HPstr="You are dead!";
                    manager.LDC.showAsciiStr(10,30,HPstr.c_str());
                }//else{
                    HPstr="HP "+std::to_string(entity.HP);
                    manager.LDC.showAsciiStr(0,10,HPstr.c_str());
              //  }
            }
           // if (entity.status == Status::Death) continue;
            entity.updatePhysics(deltaTime, groundY);
            if (entity.type == Type::obstacle) {
                entity.vec2l.x -= entity.speed * deltaTime;
                if (entity.vec2l.x + entity.Box.x < 0) {
                    entity.vec2l.x = 128; 
                }
            }
          
          
            manager.LDC.showCustomChar(
                static_cast<uint8_t>(entity.vec2l.x),
                static_cast<uint8_t>(entity.vec2l.y),
                entity.Shape,
                static_cast<uint8_t>(entity.Box.y),
                static_cast<uint8_t>(entity.Box.x)
            );
        }
        handleCollisions();
        manager.LDC.refreshScreen();
    }
};

World GWorld = World();
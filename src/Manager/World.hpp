#pragma once
#include "../Game/Entity.hpp"
#include <vector>
#include "Manager.hpp"
class World{
public:
    std::vector<Entity> entity;
    int Ground = 50;

    void addEntity(Vec2 pos,Vec2 box,Type _type,uint8_t* _Shape){
        Entity _entity;
        _entity.Box=box;
        _entity.vec2l=pos;
        _entity.type=_type;
        _entity.Ground=false;
        _entity.HP=100;
        _entity.Shape=_Shape;
        entity.push_back(_entity);
    }

    void upload(){
        for(auto& m_entity:entity){

            if(m_entity.vec2l.y>=64 || m_entity.vec2l.y<=0){
                m_entity.vec2l.y=Ground+1;
            }
            if(m_entity.vec2l.y<Ground){
                m_entity.vec2l.y=Ground+1;
            }
            manager.LDC.clearScreen();
            if(m_entity.type==Type::obstacle){
                if(m_entity.vec2l.x>=128 || m_entity.vec2l.x<=0){
                    m_entity.vec2l.y=Ground+1;
                }
                m_entity.vec2l.x-m_entity.speed;
            }
            manager.LDC.drawLine(0, 50,127,50, 1);          //水平线
            manager.LDC.showCustomChar(m_entity.vec2l.x,m_entity.vec2l.y,m_entity.Shape,m_entity.Box.y,m_entity.Box.x);
            manager.LDC.refreshScreen();
        }

    }

};
World GWorld=World();
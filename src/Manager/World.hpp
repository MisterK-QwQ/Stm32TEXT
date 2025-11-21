#pragma once
#include "../Game/Entity.hpp"
#include <vector>
#include "Manager.hpp"
#include "../Utils/Utils.hpp" 

struct DirtyArea {
    uint8_t min_x = 128, max_x = 0;  // LCD宽度128，初始值设为超出范围
    uint8_t min_page = 8, max_page = 0; // LCD页数8（64/8）
};


class World{
public:
    std::vector<Entity> entities;  // 修正变量名（复数形式更合理）
    int groundY = 50;              // 地面Y坐标
    uint32_t lastUpdateTime = 0;   // 用于计算deltaTime
      DirtyArea dirtyArea;  // 脏区域标记

    void addEntity(Vec2 pos, Vec2 box, Type _type, uint8_t* _Shape){
        Entity _entity;
        _entity.Box = box;
        _entity.vec2l = pos;
        _entity.type = _type;
        _entity.isOnGround = false;
        _entity.HP = 100;
        _entity.Shape = _Shape;
        _entity.speed = 1.0f;  // 初始化速度
        entities.push_back(_entity);
    }
    void resetDirtyArea() {
        dirtyArea = {128, 0, 8, 0};
    }

    void updateDirtyArea(const Entity& ent) {
        uint8_t x1 = static_cast<uint8_t>(ent.vec2l.x);
        uint8_t x2 = x1 + static_cast<uint8_t>(ent.Box.x);
        uint8_t y1 = static_cast<uint8_t>(ent.vec2l.y);
        uint8_t y2 = y1 + static_cast<uint8_t>(ent.Box.y);
        
        dirtyArea.min_x = MIN(dirtyArea.min_x, x1);
        dirtyArea.max_x = MAX(dirtyArea.max_x, x2);
        dirtyArea.min_page = MIN(dirtyArea.min_page, y1 / 8);  // 转换为页
        dirtyArea.max_page = MAX(dirtyArea.max_page, y2 / 8);
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

     void refreshDirty() {
        if (dirtyArea.min_x > dirtyArea.max_x) return;  // 无脏区域
        
        for (uint8_t page = dirtyArea.min_page; page <= dirtyArea.max_page; page++) {
            manager.LDC.setCursor(dirtyArea.min_x, page);  // 定位到脏区域起始页和列
            for (uint8_t x = dirtyArea.min_x; x <= dirtyArea.max_x; x++) {
                if (x >= 128) break;  // 超出屏幕范围
                manager.LDC.writeData(manager.LDC.lcd_buffer[page][x]);
            }
        }
    }
     void upload(){
        memset(manager.LDC.lcd_buffer, 0, sizeof(manager.LDC.lcd_buffer));  // 清空1024字节缓冲区
       // manager.LDC.clearScreen();  // 新增：只清缓冲区，不立即刷新
        resetDirtyArea();

        manager.LDC.drawLine(0, groundY, 127, groundY, 1);
        dirtyArea.min_page = 0;
        dirtyArea.max_page = 7;
        dirtyArea.min_x = 0;
        dirtyArea.max_x = 127;
        for (auto& ent : entities) {
            if (!ent.isOnGround) {
                ent.vec2l.y += ent.Gravity;  // 使用Entity的Gravity属性
                // 落地检测
                if (ent.vec2l.y + ent.Box.y >= groundY) {
                    ent.vec2l.y = groundY - ent.Box.y;  // 贴合地面
                    ent.isOnGround = true;
                }
            }
            if (ent.type == Type::obstacle) {
                ent.vec2l.x -= ent.speed;
                if (ent.vec2l.x + ent.Box.x < 0) {
                    ent.vec2l.x = 128;  // 从右侧重新进入
                }
            }
            manager.LDC.showCustomChar(
                static_cast<uint8_t>(ent.vec2l.x),
                static_cast<uint8_t>(ent.vec2l.y),
                ent.Shape,
                static_cast<uint8_t>(ent.Box.y),
                static_cast<uint8_t>(ent.Box.x),
                1
            );
            updateDirtyArea(ent);
        }
        refreshDirty();
    }
};

World GWorld = World();
#pragma once
#include "stm32f1xx_hal.h"

class Flash{
public:
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    Flash(FLASH_EraseInitTypeDef eraseInitstruct) : EraseInitStruct(eraseInitstruct) {};

    Flash()=default;
    Flash(const Flash&) = delete;
    Flash& operator=(const Flash&) = delete;

    bool FlashWriteMulti(uint32_t StartAddress, uint32_t* pData, uint32_t Length) {
        HAL_FLASH_Unlock();
        bool success = true;
        for (uint32_t i = 0; i < Length; i++) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartAddress + i * 4, pData[i]) != HAL_OK) {
                success = false;
                break;
            }
        }
        HAL_FLASH_Lock();
        return success;
    }

    void FlashReadMulti(uint32_t StartAddress, uint32_t* pData, uint32_t Length) {
        for (uint32_t i = 0; i < Length; i++) {
            pData[i] = *(__IO uint32_t*)(StartAddress + i * 4);
        }
    }

    void FlashErase() {
        HAL_FLASH_Unlock();
        if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK) {
        }
        HAL_FLASH_Lock();
    }
};
#include "SPI_Driver.h"

void SPI_Init(uint32_t SPI, eUSCI_SPI_MasterConfig SPIConfig)
{
    switch(SPI)
    {
    case EUSCI_B3_BASE:
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,
                GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

        MAP_SPI_initMaster(SPI, &SPIConfig);
        MAP_SPI_enableModule(SPI);
        break;
    case EUSCI_B1_BASE:
        break;
    default:
        break;
    }
}

void SPI_Write(uint32_t SPI, uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    for(i = 0; i < Size; i++)
    {
        MAP_SPI_transmitData(SPI, Data[i]);
    }
}

void SPI_Read(uint32_t SPI, uint8_t *Data, uint32_t Size)
{
    uint32_t i;
    for(i = 0; i < Size; i++)
    {
        Data[i] = MAP_SPI_receiveData(SPI);
    }
}


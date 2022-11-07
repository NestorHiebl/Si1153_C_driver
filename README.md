# Si1153 STM32 HAL Driver

The code in this repo is a driver for the Si1153 proximity sensor, written in C and using the STM32 hardware abstraction layer to handle I²C communication. The driver does not encompass all possible functionality but it is well documented and easy to build on. Interfacing with the parameter table has been implemented in the exact manner described as optimal in the official Si115x documentation, with command counter checks taking place after reads and writes.

In case you want to adapt this driver for a different hardware abstraction library, the bulk of the work you need to do is rewrite the generic read and write functions:

```c
HAL_StatusTypeDef Si1153_generic_read_single(I2C_HandleTypeDef *hi2c, uint8_t device_register, uint8_t *data);
HAL_StatusTypeDef Si1153_generic_write_single(I2C_HandleTypeDef *hi2c, uint8_t device_register, uint8_t data);
```

All direct I²C interfacing takes place within their bodies.

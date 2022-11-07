#include "Si1153.h"
#include <string.h>

#define SI1153_COMMAND_MAX_RETRIES 25

HAL_StatusTypeDef Si1153_generic_read_single(I2C_HandleTypeDef *hi2c, uint8_t device_register, uint8_t *data) {
	if ((NULL == hi2c) || (NULL == data))
	{
		return HAL_ERROR;
	}

	HAL_StatusTypeDef transmit_retval = HAL_I2C_Master_Transmit(hi2c, SI1153_I2C_ADDRESS | 0, &device_register, 1, 1000);

	if (HAL_OK != transmit_retval )
	{
		return transmit_retval;
	}

	return HAL_I2C_Master_Receive(hi2c, SI1153_I2C_ADDRESS | 1, data, 1, 1000);
}

HAL_StatusTypeDef Si1153_generic_write_single(I2C_HandleTypeDef *hi2c, uint8_t device_register, uint8_t data) {
	if (NULL == hi2c)
	{
		return HAL_ERROR;
	}

	uint8_t message[] = { device_register, data };

	return HAL_I2C_Master_Transmit(hi2c, SI1153_I2C_ADDRESS | 0, message, 2, 1000);
}


HAL_StatusTypeDef Si1153_get_part_id(I2C_HandleTypeDef *hi2c, uint8_t *read_result) {

	return Si1153_generic_read_single(hi2c, SI1153_PART_ID_REG, read_result);

}


HAL_StatusTypeDef Si1153_query_param(I2C_HandleTypeDef *hi2c, uint8_t parameter, uint8_t *read_result) {
	if ((NULL == read_result) || (NULL == hi2c))
	{
		return HAL_ERROR;
	}

	for (int i = 0; i < SI1153_COMMAND_MAX_RETRIES; i++)
	{
		HAL_StatusTypeDef i2c_error_counter = 0;

		/* Check the initial state of the command counter */
		uint8_t command_counter_before = 0;
		i2c_error_counter += Si1153_generic_read_single(hi2c, SI1153_RESPONSE0_REG, &command_counter_before);

		/* Send query command */
		i2c_error_counter += Si1153_generic_write_single(hi2c, SI1153_COMMAND_REG, SI1153_PARAM_QUERY_COMMAND_PREFIX | parameter);

		/* Check the state of the command counter again*/
		uint8_t command_counter_after = 0;
		i2c_error_counter += Si1153_generic_read_single(hi2c, SI1153_RESPONSE0_REG, &command_counter_after);


		if ((command_counter_after & SI1153_RESPONSE_ERROR_MASK) || i2c_error_counter)
		{
			/* An I²C or command execution error has occurred */
			Si1153_reset_command_counter(hi2c);
			return HAL_ERROR;
		}

		if ((command_counter_after & SI1153_RESPONSE_COMMAND_COUNTER_MASK) > (command_counter_before & SI1153_RESPONSE_COMMAND_COUNTER_MASK))
		{
			/* The command counter has incremented, stop looping */
			return Si1153_generic_read_single(hi2c, SI1153_RESPONSE1_REG, read_result);
		}
	}


	return HAL_ERROR;
}


HAL_StatusTypeDef Si1153_set_param(I2C_HandleTypeDef *hi2c, uint8_t parameter, uint8_t parameter_value) {
	if (NULL == hi2c)
	{
		return HAL_ERROR;
	}

	for (int i = 0; i < SI1153_COMMAND_MAX_RETRIES; i++)
	{
		HAL_StatusTypeDef i2c_error_counter = 0;

		/* Check the initial state of the command counter */
		uint8_t command_counter_before = 0;
		i2c_error_counter += Si1153_generic_read_single(hi2c, SI1153_RESPONSE0_REG, &command_counter_before);

		/* Write the parameter value to the corresponding buffer register */
		i2c_error_counter += Si1153_generic_write_single(hi2c, SI1153_HOSTIN0_REG, parameter_value);

		/* Send query command */
		i2c_error_counter += Si1153_generic_write_single(hi2c, SI1153_COMMAND_REG, SI1153_PARAM_SET_COMMAND_PREFIX | parameter);

		/* Check the state of the command counter again*/
		uint8_t command_counter_after = 0;
		i2c_error_counter += Si1153_generic_read_single(hi2c, SI1153_RESPONSE0_REG, &command_counter_after);


		if ((command_counter_after & SI1153_RESPONSE_ERROR_MASK) || i2c_error_counter)
		{
			/* An I²C or command execution error has occurred */
			Si1153_reset_command_counter(hi2c);
			return HAL_ERROR;
		}

		if ((command_counter_after & SI1153_RESPONSE_COMMAND_COUNTER_MASK) > (command_counter_before & SI1153_RESPONSE_COMMAND_COUNTER_MASK))
		{
			/* The command counter has incremented, stop looping */
			return HAL_OK;
		}
	}


	return HAL_ERROR;
}

HAL_StatusTypeDef Si1153_reset_command_counter(I2C_HandleTypeDef *hi2c) {
	if (NULL == hi2c)
	{
		return HAL_ERROR;
	}

	for (int i = 0; i < SI1153_COMMAND_MAX_RETRIES; i++)
	{
		HAL_StatusTypeDef i2c_error_counter = 0;

		/* Send counter reset command */
		i2c_error_counter += Si1153_generic_write_single(hi2c, SI1153_COMMAND_REG, SI1153_RST_CMD_CTR_COMMAND);

		/* Check the state of the command counter */
		uint8_t command_counter_after = 0;
		i2c_error_counter += Si1153_generic_read_single(hi2c, SI1153_RESPONSE0_REG, &command_counter_after);

		if ((command_counter_after & SI1153_RESPONSE_ERROR_MASK) || i2c_error_counter)
		{
			/* An I²C or command execution error has occurred */
			return HAL_ERROR;
		}

		if (0 == (command_counter_after & SI1153_RESPONSE_COMMAND_COUNTER_MASK))
		{
			/* The command counter has incremented, stop looping */
			return HAL_OK;
		}
	}


	return HAL_ERROR;
}

HAL_StatusTypeDef Si1153_start_autonomous_mode(I2C_HandleTypeDef *hi2c) {
	if (NULL == hi2c)
	{
		return HAL_ERROR;
	}

	for (int i = 0; i < SI1153_COMMAND_MAX_RETRIES; i++)
	{
		HAL_StatusTypeDef i2c_error_counter = 0;

		/* Check the initial state of the command counter */
		uint8_t command_counter_before = 0;
		i2c_error_counter += Si1153_generic_read_single(hi2c, SI1153_RESPONSE0_REG, &command_counter_before);

		/* Send start command */
		i2c_error_counter += Si1153_generic_write_single(hi2c, SI1153_COMMAND_REG, SI1153_START_MEASUREMENTS_COMMAND);

		/* Check the state of the command counter again*/
		uint8_t command_counter_after = 0;
		i2c_error_counter += Si1153_generic_read_single(hi2c, SI1153_RESPONSE0_REG, &command_counter_after);

		if ((command_counter_after & SI1153_RESPONSE_ERROR_MASK) || i2c_error_counter)
		{
			/* An I²C or command execution error has occurred */
			Si1153_reset_command_counter(hi2c);
			return HAL_ERROR;
		}

		if ((command_counter_after & SI1153_RESPONSE_COMMAND_COUNTER_MASK) > (command_counter_before & SI1153_RESPONSE_COMMAND_COUNTER_MASK))
		{
			/* The command counter has incremented, stop looping */
			return HAL_OK;
		}
	}

	return HAL_ERROR;

}

HAL_StatusTypeDef Si1153_read_channel_0_16bit(I2C_HandleTypeDef *hi2c, uint16_t *channel_value) {
	if ((NULL == hi2c) || (NULL == channel_value))
	{
		return HAL_ERROR;
	}

	uint8_t channel_0_high = 0;
	uint8_t channel_0_low = 0;

	HAL_StatusTypeDef read_result = Si1153_generic_read_single(hi2c, SI1153_HOSTOUT0_REG, &channel_0_high);

	if (HAL_OK != read_result)
	{
		return read_result;
	}

	read_result = Si1153_generic_read_single(hi2c, SI1153_HOSTOUT1_REG, &channel_0_low);

	if (HAL_OK != read_result)
	{
		return read_result;
	}

	*channel_value = (channel_0_high << 8) + channel_0_low;

	return HAL_OK;
}

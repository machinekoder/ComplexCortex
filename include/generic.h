 /**
 * @file generic.h
 * @author Alexander Rössler
 * @brief Generic functions useful for embedded development
 * @date 09-01-2014
 */
/** @defgroup Generic Generic function library
 * @ingroup Functional
 * @{
 */
#pragma once

#include <types.h>

/** Converts a hex formatted string to an integer
 *  @param a Pointer to the hex formatted string.
 *  @param len The length of the hex formatted string.
 *  @return The converted integer.
 */
uint32 Generic_hex2int(char *a, uint32 len);
/** Builds the decoding table for decoding Base64 encoded data
 */
void Generic_buildBase64DecodingTable();
/** Encodes data to Base64
 *  @param data Pointer to the data to be encoded.
 *  @param inputLength The length of the data to encode in bytes
 *  @param putFunction Function to output the encoded characters.
 */
void Generic_base64Encode(const uint8 *data,
                          uint16 inputLength,
                          int8(*putFunction)(char));
/** Decodes a Base64 encoded string
 *  @param data Pointer to the encoded data.
 *  @param inputLength The length of the encoded data.
 *  @param outputData Pointer to where the decoded data should be stored.
 */
int8 Generic_base64Decode(const char *data, uint16 inputLength, uint8 *outputData);

/**
 * @}
 */

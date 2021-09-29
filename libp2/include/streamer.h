#ifndef _STREAMER_H
#define _STREAMER_H

#define STREAMER_MODE_OFFSET            28
#define STREAMER_DAC_OFFSET             24
#define STREAMER_EN_OFFSET              23

/**
 * get the configuration value for the streamer for 1 pin mode
 *
 * @param mode: the streamer mode
 * @param en_pins: 1 or 0 to enable pin input/output or not
 * @param dac: DAC output setting.
 * @param pins: pin the streamer should use
 * @param rev: reverse bits within the output from bottom first to top first
 */
#define GET_STREAMER_CONFIG_1(mode, dac, en_pins, pins, rev)    (((mode & 0xf) << STREAMER_MODE_OFFSET) | \
                                                                ((dac & 0xf) << STREAMER_DAC_OFFSET) | \
                                                                ((en_pins & 0x1) << STREAMER_EN_OFFSET) | \
                                                                ((pins & 0x3f) << 17) | \
                                                                ((rev & 0x1) << 16))

/**
 * get the configuration value for the streamer for 2 pin mode
 *
 * @param mode: the streamer mode
 * @param submode: sub mode (to set # of dac outputs)
 * @param en_pins: 1 or 0 to enable pin input/output or not
 * @param dac: DAC output setting.
 * @param pins: the pin group the streamer should use
 * @param rev: reverse bits within the output from bottom first to top first
 */
#define GET_STREAMER_CONFIG_2(mode, submode, dac, en_pins, pins, rev)   (((mode & 0xf) << STREAMER_MODE_OFFSET) | \
                                                                        ((dac & 0xf) << STREAMER_DAC_OFFSET) | \
                                                                        ((en_pins & 0x1) << STREAMER_EN_OFFSET) | \
                                                                        ((pins & 0x1f) << 18) | \
                                                                        ((submode & 0x1) << 17) | \
                                                                        ((rev & 0x1) << 16))

/**
 * get the configuration value for the streamer for 4 pin mode
 *
 * @param mode: the streamer mode
 * @param submode: sub mode (to set # of dac outputs)
 * @param en_pins: 1 or 0 to enable pin input/output or not
 * @param dac: DAC output setting.
 * @param pins: the pin group the streamer should use
 * @param rev: reverse bits within the output from bottom first to top first
 */
#define GET_STREAMER_CONFIG_4(mode, submode, dac, en_pins, pins, rev)   (((mode & 0xf) << STREAMER_MODE_OFFSET) | \
                                                                        ((dac & 0xf) << STREAMER_DAC_OFFSET) | \
                                                                        ((en_pins & 0x1) << STREAMER_EN_OFFSET) | \
                                                                        ((pins & 0xf) << 19) | \
                                                                        ((submode & 0x2) << 17) | \
                                                                        ((rev & 0x1) << 16))

/**
 * get the configuration value for the streamer for 8 pin mode
 *
 * @param mode: the streamer mode
 * @param submode: sub mode (to set # of dac outputs)
 * @param en_pins: 1 or 0 to enable pin input/output or not
 * @param dac: DAC output setting.
 * @param pins: the pin group the streamer should use
 */
#define GET_STREAMER_CONFIG_8(mode, submode, dac, en_pins, pins)    (((mode & 0xf) << STREAMER_MODE_OFFSET) | \
                                                                    ((dac & 0xf) << STREAMER_DAC_OFFSET) | \
                                                                    ((en_pins & 0x1) << STREAMER_EN_OFFSET) | \
                                                                    ((pins & 0x7) << 20) | \
                                                                    ((submode & 0xf) << 16))

/**
 * get the configuration value for the streamer for 16 pin mode
 *
 * @param mode: the streamer mode
 * @param submode: sub mode (to set # of dac outputs)
 * @param en_pins: 1 or 0 to enable pin input/output or not
 * @param dac: DAC output setting.
 * @param pins: the pin group the streamer should use
 */
#define GET_STREAMER_CONFIG_16(mode, submode, dac, en_pins, pins) GET_STREAMER_CONFIG_8(mode, submode, dav, en_pins, pins)

/**
 * get the configuration value for the streamer for 32 pin mode
 *
 * @param mode: the streamer mode
 * @param submode: sub mode (to set # of dac outputs)
 * @param en_pins: 1 or 0 to enable pin input/output or not
 * @param dac: DAC output setting.
 * @param pins: the pin group the streamer should use
 */
#define GET_STREAMER_CONFIG_32(mode, submode, dac, en_pins, pins) GET_STREAMER_CONFIG_8(mode, submode, dav, en_pins, pins)

#endif
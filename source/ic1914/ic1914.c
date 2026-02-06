
    #include "ic1914.h"
    #include <stdio.h>
    #include <ets_sys.h>
    #include "main.h"

    #include "esp8266_gpio.h"
    #include "esp8266_interrupt.h"

    #define IC1914_PORT_SET(mask)      (GPIO_1914_PORT_SET = mask)
    #define IC1914_PORT_CLR(mask)      (GPIO_1914_PORT_CLR = mask)

    #define IC1914_PIN_MASK_0      GPIO1914_DI_CH0_BIT_MASK
    #define IC1914_PIN_MASK_1      GPIO1914_DI_CH1_BIT_MASK

    #define IC1914_TIMMING_0   asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    #define IC1914_TIMMING_1   asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    #define IC1914_TIMMING_2   asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); \
                            asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");

/*============================= IC 1914 ==================================*/ 
static inline void led1914_send_start(void)
{
    uint8_t i, bit;

    for (i = 0; i < 6; i++)   // FF FF FF 00 00 00
    {
        uint8_t val = (i < 3) ? 0xFF : 0x00;

        for (bit = 0x80; bit; bit >>= 1)
        {
            IC1914_PORT_CLR(IC1914_ALL_MASK);
            IC1914_TIMMING_2;

            if ( (val & bit) == 0)
                IC1914_PORT_SET(IC1914_ALL_MASK);

            IC1914_TIMMING_1;
           IC1914_PORT_SET(IC1914_ALL_MASK);
            IC1914_TIMMING_0;
        }
    }
}

__attribute__((section(".text"))) void ic1914_update(Color_RGB_t *color_buf, uint16_t len, uint16_t full_len)
{
    uint8_t  idx_bit;
    uint16_t idx_dev_serial;
    uint32_t color_to_io;

    disable_all_interrupts();

    led1914_send_start();

    for (idx_dev_serial = 0; idx_dev_serial < len; idx_dev_serial++)
    {
        /* ===== R ===== */
        for (idx_bit = 0x80; idx_bit; idx_bit >>= 1)
        {
            color_to_io = 0;

            
            IC1914_PORT_CLR(IC1914_ALL_MASK);

            if ((color_buf[idx_dev_serial].R & idx_bit) == 0)
                color_to_io |= IC1914_PIN_MASK_0;

            if ((color_buf[full_len + idx_dev_serial].R & idx_bit) == 0)
                color_to_io |= IC1914_PIN_MASK_1;

            IC1914_TIMMING_2;
            IC1914_PORT_SET(color_to_io);
            IC1914_TIMMING_1;

            IC1914_PORT_SET(IC1914_ALL_MASK);

            IC1914_TIMMING_0;
        }

        /* ===== G ===== */
        for (idx_bit = 0x80; idx_bit; idx_bit >>= 1)
        {
            color_to_io = 0;

            
            IC1914_PORT_CLR(IC1914_ALL_MASK);

            if ((color_buf[idx_dev_serial].G & idx_bit) == 0)
                color_to_io |= IC1914_PIN_MASK_0;

            if ((color_buf[full_len + idx_dev_serial].G & idx_bit) == 0)
                color_to_io |= IC1914_PIN_MASK_1;

            IC1914_TIMMING_2;
            IC1914_PORT_SET(color_to_io);
            IC1914_TIMMING_1;

            IC1914_PORT_SET(IC1914_ALL_MASK);

            IC1914_TIMMING_0;
        }

        /* ===== B ===== */
        for (idx_bit = 0x80; idx_bit; idx_bit >>= 1)
        {
            color_to_io = 0;

            
            IC1914_PORT_CLR(IC1914_ALL_MASK);

            if ((color_buf[idx_dev_serial].B & idx_bit) == 0)
                color_to_io |= IC1914_PIN_MASK_0;

            if ((color_buf[full_len + idx_dev_serial].B & idx_bit) == 0)
                color_to_io |= IC1914_PIN_MASK_1;

            IC1914_TIMMING_2;
            IC1914_PORT_SET(color_to_io);
            IC1914_TIMMING_1;

            IC1914_PORT_SET(IC1914_ALL_MASK);

            IC1914_TIMMING_0;
        }
    }

    allow_interrupts();
}


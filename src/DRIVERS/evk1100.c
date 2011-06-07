/*
 * driver.c
 *
 *  Created on: 22 Jan 2011
 *      Author:
 */


#include "../link_uc_mac.h"
#include "../ethernet.h"

#include "board.h"
#include "intc.h"
#include "tc.h"
#include "led.h"
#include "macb.h"


#include <avr32/io.h>
#include "usart.h"
#include "board.h"
#include "macb.h"
#include "gpio.h"
#include "compiler.h"
#include "conf_eth.h"
#include "flashc.h"
#include "intc.h"
#include "power_clocks_lib.h"



#define EXAMPLE_USART               (&AVR32_USART1)
#define EXAMPLE_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#define EXAMPLE_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#define EXAMPLE_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#define EXAMPLE_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION

#define MACB_EXAMPLE_CPU_HZ			48000000	
#define MACB_EXAMPLE_PBA_HZ			24000000

#define MS_TIMER_CHANNEL	0
#define MAC_STATUS_CHANNEL	1

/* 'Private' functions */
__attribute__((__interrupt__)) void tc_irq(void);
__attribute__((__interrupt__)) void mac_irq(void);

/* 'Private' variables */

// Don't keep initialising mac/uc
static bool bMACInitialised = false;
static bool bUCInitialised = false;

// Frame complete callback
static void (*cb_frame_complete)(uint8_t *buffer, const uint16_t buffer_len) = NULL;

// Timer callback.
static void(*cb_timer)(void) = NULL;

/**
 * Timer Counter Interrupt #1
 *
 * This is configured to fire once every ms, and
 * notify the timer code (callback), to give our 
 * application time awareness.
 */
__attribute__((__interrupt__)) void tc_irq(void)
{
	tc_read_sr(&AVR32_TC, MS_TIMER_CHANNEL);

	if(cb_timer != NULL)
	{
		(cb_timer)();
	}
}

/**
 * Timer Counter Interrupt #2
 *
 * Checks the MAC for new data, as 
 * the MAC interface we are using doesn't
 * interrupt on received packet, so we
 * emulate it instead.
 */
__attribute__((__interrupt__)) void mac_irq(void)
{
	// Read bit to clear it
	tc_read_sr(&AVR32_TC, MAC_STATUS_CHANNEL);

	if(cb_frame_complete == NULL)
	{
		return;
	}

	// Test MAC status
	const unsigned long rx_len = ulMACBInputLength();
	if(rx_len > 0)
	{
		uint8_t rx_buffer[200];
		vMACBRead(rx_buffer, 200, rx_len);
	/*	
		usart_write_line(EXAMPLE_USART, "RX did good (");
		
		char temp[10] = {0};
		int i = 9;
		unsigned long temp_rx_len = rx_len;
		do 
		{
			temp[--i] = (temp_rx_len % 10)+'0';
			temp_rx_len /= 10;
		}while(temp_rx_len > 0);
		
		usart_write_line(EXAMPLE_USART, &temp[i]);
		usart_write_line(EXAMPLE_USART, ")\r\n");		
*/
		(cb_frame_complete)(rx_buffer, rx_len);
	}
}


RETURN_STATUS register_ms_callback(void(*handler)(void))
{
	// Create timer thingy;
	cb_timer = handler;

	//return SUCCESS;
	return SUCCESS;
}

RETURN_STATUS init_uc()
{

	if(bUCInitialised == true)
		return SUCCESS;
	else
		bUCInitialised = true;


  static pcl_freq_param_t pcl_freq_param =
  {
    .cpu_f        = MACB_EXAMPLE_CPU_HZ,
    .pba_f        = MACB_EXAMPLE_PBA_HZ,
    .osc0_f       = FOSC0,
    .osc0_startup = OSC0_STARTUP
  };

  // Configure system clock
  if (pcl_configure_clocks(&pcl_freq_param) != PASS)
  {
	return FAILURE;  
  }	

	  volatile avr32_tc_t *tc = &AVR32_TC;

	  // Options for waveform generation.
	  static const tc_waveform_opt_t WAVEFORM_OPT_MS =
	  {
	    .channel  = MS_TIMER_CHANNEL,                        // Channel selection.

	    .bswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOB.
	    .beevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOB.
	    .bcpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOB.
	    .bcpb     = TC_EVT_EFFECT_NOOP,                // RB compare effect on TIOB.

	    .aswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOA.
	    .aeevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOA.
	    .acpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOA: toggle.
	    .acpa     = TC_EVT_EFFECT_NOOP,                // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

	    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
	    .enetrg   = FALSE,                             // External event trigger enable.
	    .eevt     = 0,                                 // External event selection.
	    .eevtedg  = TC_SEL_NO_EDGE,                    // External event edge selection.
	    .cpcdis   = FALSE,                             // Counter disable when RC compare.
	    .cpcstop  = FALSE,                             // Counter clock stopped with RC compare.

	    .burst    = FALSE,                             // Burst signal selection.
	    .clki     = FALSE,                             // Clock inversion.
	    .tcclks   = TC_CLOCK_SOURCE_TC2                // Internal source clock 2 - connected to PBA/4
	  };
	  static const tc_waveform_opt_t WAVEFORM_OPT_MAC =
	  {
	    .channel  = MAC_STATUS_CHANNEL,                        // Channel selection.

	    .bswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOB.
	    .beevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOB.
	    .bcpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOB.
	    .bcpb     = TC_EVT_EFFECT_NOOP,                // RB compare effect on TIOB.

	    .aswtrg   = TC_EVT_EFFECT_NOOP,                // Software trigger effect on TIOA.
	    .aeevt    = TC_EVT_EFFECT_NOOP,                // External event effect on TIOA.
	    .acpc     = TC_EVT_EFFECT_NOOP,                // RC compare effect on TIOA: toggle.
	    .acpa     = TC_EVT_EFFECT_NOOP,                // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

	    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
	    .enetrg   = FALSE,                             // External event trigger enable.
	    .eevt     = 0,                                 // External event selection.
	    .eevtedg  = TC_SEL_NO_EDGE,                    // External event edge selection.
	    .cpcdis   = FALSE,                             // Counter disable when RC compare.
	    .cpcstop  = FALSE,                             // Counter clock stopped with RC compare.

	    .burst    = FALSE,                             // Burst signal selection.
	    .clki     = FALSE,                             // Clock inversion.
	    .tcclks   = TC_CLOCK_SOURCE_TC2                // Internal source clock 2 - connected to PBA/4
	  };
	  static const tc_interrupt_t TC_INTERRUPT =
	  {
	    .etrgs = 0,
	    .ldrbs = 0,
	    .ldras = 0,
	    .cpcs  = 1,
	    .cpbs  = 0,
	    .cpas  = 0,
	    .lovrs = 0,
	    .covfs = 0
	  };

	  static const gpio_map_t USART_GPIO_MAP =
	  {
	    {EXAMPLE_USART_RX_PIN, EXAMPLE_USART_RX_FUNCTION},
	    {EXAMPLE_USART_TX_PIN, EXAMPLE_USART_TX_FUNCTION}
	  };

	  // USART options.
	  static const usart_options_t USART_OPTIONS =
	  {
	    .baudrate     = 57600,
	    .charlength   = 8,
	    .paritytype   = USART_NO_PARITY,
	    .stopbits     = USART_1_STOPBIT,
	    .channelmode  = 0
	  };

	  // Assign GPIO pins to USART.
	  gpio_enable_module(USART_GPIO_MAP, sizeof(USART_GPIO_MAP) / sizeof(USART_GPIO_MAP[0]));

	  // Initialize USART in RS232 mode at 12MHz.
	  usart_init_rs232(EXAMPLE_USART, &USART_OPTIONS, MACB_EXAMPLE_PBA_HZ);

	  // Welcome sentence.
	  usart_write_line(EXAMPLE_USART, "\x1B[2J\x1B[H\r\nsIP\r\n");
	  usart_write_line(EXAMPLE_USART, "Example\r\n");

	  Disable_global_interrupt();

	  // The INTC driver has to be used only for GNU GCC for AVR32.
	  // Initialize interrupt vectors.
	  INTC_init_interrupts();

	  // Register the RTC interrupt handler to the interrupt controller.
	  INTC_register_interrupt(&tc_irq, AVR32_TC_IRQ0, AVR32_INTC_INT0);
	  INTC_register_interrupt(&mac_irq, AVR32_TC_IRQ1, AVR32_INTC_INT0);

	  Enable_global_interrupt();

	  // Initialize the timer/counters.
	  tc_init_waveform(tc, &WAVEFORM_OPT_MS);
	  tc_init_waveform(tc, &WAVEFORM_OPT_MAC);

	  // Set the compare triggers to count ms.
	  // We want: (1/(FPBA/4)) * RC = 1000 Hz => RC = (FPBA/4) / 1000 = 3000 to get an interrupt every 1ms
	  tc_write_rc(tc, MS_TIMER_CHANNEL, (MACB_EXAMPLE_PBA_HZ/4)/1000);  // Set RC value.
	  
	  // Can afford to check the mac a little slower
	  tc_write_rc(tc, MAC_STATUS_CHANNEL, (MACB_EXAMPLE_PBA_HZ/4)/500);

	  tc_configure_interrupts(tc, MS_TIMER_CHANNEL, &TC_INTERRUPT);
	  tc_configure_interrupts(tc, MAC_STATUS_CHANNEL, &TC_INTERRUPT);

	  // Start the timer/counter.
	  tc_start(tc, MS_TIMER_CHANNEL);                    // And start the timer/counter.
	  tc_start(tc, MAC_STATUS_CHANNEL);

//	LED_On(LED0);

	return SUCCESS;
}



RETURN_STATUS read_buffer(uint8_t *buffer, const unsigned int buffer_len, unsigned int *actual_len, const unsigned int timeout_ms)
{
return NOT_AVAILABLE;
}


RETURN_STATUS init_mac()
{
	
	if(bMACInitialised)
	{
		return SUCCESS;
	}
	
	const uint8_t * addr = get_ether_addr();
	vMACBSetMACAddress(addr);
	
	static const gpio_map_t MACB_GPIO_MAP =
{
    {EXTPHY_MACB_MDC_PIN,     EXTPHY_MACB_MDC_FUNCTION   },
    {EXTPHY_MACB_MDIO_PIN,    EXTPHY_MACB_MDIO_FUNCTION  },
    {EXTPHY_MACB_RXD_0_PIN,   EXTPHY_MACB_RXD_0_FUNCTION },
    {EXTPHY_MACB_TXD_0_PIN,   EXTPHY_MACB_TXD_0_FUNCTION },
    {EXTPHY_MACB_RXD_1_PIN,   EXTPHY_MACB_RXD_1_FUNCTION },
    {EXTPHY_MACB_TXD_1_PIN,   EXTPHY_MACB_TXD_1_FUNCTION },
    {EXTPHY_MACB_TX_EN_PIN,   EXTPHY_MACB_TX_EN_FUNCTION },
    {EXTPHY_MACB_RX_ER_PIN,   EXTPHY_MACB_RX_ER_FUNCTION },
    {EXTPHY_MACB_RX_DV_PIN,   EXTPHY_MACB_RX_DV_FUNCTION },
    {EXTPHY_MACB_TX_CLK_PIN,  EXTPHY_MACB_TX_CLK_FUNCTION} 
};

  // Assign GPIO to MACB
  gpio_enable_module(MACB_GPIO_MAP, sizeof(MACB_GPIO_MAP) / sizeof(MACB_GPIO_MAP[0]));
		
	if (xMACBInit(&AVR32_MACB))
	{
		bMACInitialised = true;
		return SUCCESS;
	}
	
	bMACInitialised = false;
	return FAILURE;		
}


RETURN_STATUS send_frame(const uint8_t *buffer, const uint16_t buffer_len)
{
	usart_write_line(EXAMPLE_USART, "Sending Packet\r\n");
	
	lMACBSend(&AVR32_MACB, buffer, buffer_len, TRUE);
	return SUCCESS;
}



RETURN_STATUS set_frame_complete(void (*frame_complete_callback)(uint8_t *buffer, const uint16_t buffer_len))
{
	cb_frame_complete = frame_complete_callback;
	return SUCCESS;
}

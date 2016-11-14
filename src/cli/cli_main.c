
#include <stdio.h>
#include "includes.h"
#include "tinysh.h"
#include "cli_app.h"
#include "cli_rtos.h"

extern uint8_t aCliRxBuffer[cliRXBUFFERSIZE];
extern uint16_t CLI_RX_STA;

void tinysh_char_out(unsigned char c)
{
  putchar((int)c);
}

void cli_init(void)
{
  /* change the prompt */
  tinysh_set_prompt("\n[Charger]$ ");

  tinysh_add_command(&cli_hello_cmd);
  tinysh_add_command(&cli_tasklist_cmd);
  tinysh_add_command(&cli_testsdram_cmd);
}
void cli_main(void)
{
    cli_init();
    while(1)
    {
        if(CLI_RX_STA == SET)
        {
            tinysh_char_in(aCliRxBuffer[0]);
            CLI_RX_STA = RESET;
        }
        //vTaskSuspend( NULL );
//      tinysh_char_in((unsigned char)getchar());
      vTaskDelay(100);
    }
}


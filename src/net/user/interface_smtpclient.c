#include "stm32f4xx.h"

#include "core/net.h"
#include "smtp/smtp_client.h"
#include "rng/yarrow.h"
#include "error.h"
#include "debug.h"
#include "os_port.h"


YarrowContext yarrowContext;
uint8_t seed[32];
/**
 * @brief SMTP client test routine
 * @return Error code
 **/

error_t smtpClientTest(void)
{
    error_t error;
    int i;
    uint32_t value;

    RNG_HandleTypeDef hrng; 
    __HAL_RCC_RNG_CLK_ENABLE();
    hrng.Instance = RNG;
    HAL_RNG_Init(&hrng);

       //Generate a random seed
    for (i = 0; i < 32; i += 4)
    {
       //Wait for the RNG to contain a valid data
        while (HAL_RNG_GetState(&hrng) != HAL_RNG_STATE_READY)
            ;

              //Get 32-bit random value
        value = HAL_RNG_GetRandomNumber(&hrng);

        //Copy random value
        seed[i] = value & 0xFF;
        seed[i + 1] = (value >> 8) & 0xFF;
        seed[i + 2] = (value >> 16) & 0xFF;
        seed[i + 3] = (value >> 24) & 0xFF;
    }
    
       //PRNG initialization
    error = yarrowInit(&yarrowContext);
    //Any error to report?
    if (error)
    {
       //Debug message
        TRACE_ERROR("Failed to initialize PRNG!\r\n");
    }

       //Properly seed the PRNG
    error = yarrowSeed(&yarrowContext, seed, sizeof(seed));
    //Any error to report?
    if (error)
    {
       //Debug message
        TRACE_ERROR("Failed to seed PRNG!\r\n");
    }
    
       //Authentication information
    static SmtpAuthInfo authInfo =
    {
        NULL,             //Network interface
        "smtp.qq.com", //SMTP server name
        25,               //SMTP server port
        "rgw5267",       //User name
        "13131685886a=",       //Password
                  FALSE,            //Use STARTTLS rather than implicit TLS
      YARROW_PRNG_ALGO, //PRNG algorithm
      &yarrowContext    //PRNG context
    };

       //Recipients
    static SmtpMailAddr recipients[2] =
    {
        { "RGW", "rgw5267@gmail.com", SMTP_RCPT_TYPE_TO }, //First recipient
        { "RGW", "renguangwei@dpc.com.cn", SMTP_RCPT_TYPE_CC }      //Second recipient
    };

       //Mail contents
    static SmtpMail mail =
    {
        { "RGW", "rgw5267@qq.com" },  //From
        recipients,                        //Recipients
        2,                                 //Recipient count
        "",                                //Date
        "SMTP Client Demo",                //Subject
        "Hello World!"                     //Body
    };

       //Send mail
    error = smtpSendMail(&authInfo, &mail);

       //Return status code
    return error;
}
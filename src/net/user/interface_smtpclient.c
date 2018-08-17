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
static int is_smtp_initialized = 0;
error_t smtpClientTest(void)
{
    error_t error;
    int i;
    uint32_t value;

    RNG_HandleTypeDef hrng; 
    if (is_smtp_initialized == 0)
    {
        __HAL_RCC_RNG_CLK_ENABLE();
        hrng.Instance = RNG;
        HAL_RNG_Init(&hrng);
        //PRNG 初始化
        error = yarrowInit(&yarrowContext);
        if (error)
        {
            TRACE_ERROR("初始化PRNG失败!\r\n");
            return error;
        }
        is_smtp_initialized = 1;
    }
    //生成随机数种子
    for (i = 0; i < 32; i += 4)
    {
        while (HAL_RNG_GetState(&hrng) != HAL_RNG_STATE_READY)
            ;
        value = HAL_RNG_GetRandomNumber(&hrng);
        seed[i] = value & 0xFF;
        seed[i + 1] = (value >> 8) & 0xFF;
        seed[i + 2] = (value >> 16) & 0xFF;
        seed[i + 3] = (value >> 24) & 0xFF;
    }

    error = yarrowSeed(&yarrowContext, seed, sizeof(seed));
    if (error)
    {
        TRACE_ERROR("PRNG种子失败!\r\n");
        return error;
    }
    
    //认证信息
    static SmtpAuthInfo authInfo =
    {
        NULL,             //网络接口
        "smtp.qq.com", //SMTP 服务器
        25,               //SMTP 端口
        "rgw5267",       //用户名
        "13131685886a=",       //密码
        FALSE,            //Use STARTTLS rather than implicit TLS
        YARROW_PRNG_ALGO, //PRNG algorithm
        &yarrowContext    //PRNG context
    };

    //Recipients
    //    { "RGW", "renguangwei@dpc.com.cn", SMTP_RCPT_TYPE_TO }, //First recipient
    //    { "RGW", "rgw5267@gmail.com", SMTP_RCPT_TYPE_CC }      //Second recipient
    static SmtpMailAddr recipients[2] =
    {
        { "RGW", "rgw5267@163.com", SMTP_RCPT_TYPE_TO }
    };

    //Mail contents
    static SmtpMail mail =
    {
        { "RGW", "rgw5267@qq.com" },  //From
        recipients,                        //Recipients
        1,                                 //Recipient count
        "",                                //Date
        "SMTP Client Demo\0",                //Subject
        "Hello World!\0"                     //Body
    };

    //Send mail
    error = smtpSendMail(&authInfo, &mail);

    return error;
}
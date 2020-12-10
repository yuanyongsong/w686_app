#include "usr_main.h"

char Get0_result[10];
char Get2_result[10];
char Get3_result[10];
char GetALL_result[100];

TEST Test;

void Test_Init(void)
{
    Test.TestStep = 0xFF;
    Test.WaitEnterTest = 15;
    UART_Send(USART3,"Device can enter Test mode by send Test command in 10s\r\n",56);
}


void Test_Receive(void)
{   
    char *p0 = NULL;
    char *p1 = NULL;
    char temp_buf[20] = {0};
    char read_buf[20] = {0};
    char send_buf[150] = {0};

    if((Test.WaitEnterTest > 0) && (strstr(Uart3Buf, "AT^TST")))
	{
		Test.TestStep = 0;
        Test.InTesting = 1;
	}

    //外部IMEI设置：AT^SETIMEI=123456789012345\r\n
    if(strstr(Uart3Buf, "AT^SETIMEI="))    
    {
        p0 = strstr(Uart3Buf, "AT^SETIMEI=");
        p0 += 11;
        p1 = strstr(p0, "\r\n");
        if(p1 - p0 != 15) 
        {
            printf("The IMEI data format error\r\n");
            UART_Send(USART3,"The IMEI data format error\r\n",29); 
            return;
        }  
        strncpy(temp_buf,p0,p1 - p0);
        strcat(temp_buf,"#");     
        EXFLASH_EraseSector(IMEIADDR);
        EXFLASH_WriteBuffer((u8 *)temp_buf,IMEIADDR,strlen(temp_buf));
        EXFLASH_ReadBuffer((u8 *)read_buf,IMEIADDR,strlen(temp_buf));   

        if(memcmp(temp_buf,read_buf,strlen(temp_buf)) == 0)
        {
            p1 = strstr(temp_buf, "#");
            *p1 = 0;                                             //删除“#”
            memset(GetALL_result,0,sizeof(GetALL_result));
            EXFLASH_ReadBuffer((u8 *)GetALL_result,TESTRESULTADDR_4,sizeof(GetALL_result));
            p1 = strstr(GetALL_result, "\r\n");
            *p1 = 0; 
            sprintf(send_buf,"^DEV@SETIMEI=%s;%s\r\n",temp_buf,GetALL_result);
            UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));        
        }   
        else
        {
            UART_Send(USART3,"The IMEI Save Failed\r\n",23); 
        }
    }

    if(Test.TestStep == 0xFF)      
    {
        return;
    }

    if(strstr(Uart3Buf, "AT^SET0="))
    {
        p0 = strstr(Uart3Buf, "AT^SET0=");
        p0 += 8;
        p1 = strstr(p0, "\r\n");
        if(p1 - p0 < sizeof(temp_buf))
        {
            strncpy(temp_buf,p0,p1 - p0);
            strcat(temp_buf,"#");                   //添加一个#作为结束符
            EXFLASH_EraseSector(TESTRESULTADDR_0);
            EXFLASH_WriteBuffer((u8 *)temp_buf,TESTRESULTADDR_0,strlen(temp_buf));
            EXFLASH_ReadBuffer((u8 *)read_buf,TESTRESULTADDR_0,strlen(temp_buf));
            
            if(memcmp(temp_buf,read_buf,strlen(temp_buf)) == 0)
            {
                p1 = strstr(temp_buf, "#");
                *p1 = 0;
                sprintf(send_buf,"^DEV@SET0=%s\r\n",temp_buf);
                UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));        //长度减1是去掉“#”
            }
        }
    }
    else if(strstr(Uart3Buf, "AT^SET1="))
    {
        p0 = strstr(Uart3Buf, "AT^SET1=");
        p0 += 8;
        p1 = strstr(p0, "\r\n");
        if(p1 - p0 < sizeof(temp_buf))
        {
            strncpy(temp_buf,p0,p1 - p0);
            sprintf(send_buf,"^DEV@SET1=%s\r\n",temp_buf);
            UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));     
        }

        memset(GetALL_result,0,sizeof(GetALL_result));
        sprintf(GetALL_result,"IMEI=%s;ATI=%s;CCID=%s;GET0=%s;GET2=%s;GET3=%s;\r\n",IMEI_MODULE,GsmRev,CCID,Get0_result,Get2_result,Get3_result);
        EXFLASH_WriteBuffer((u8 *)GetALL_result,TESTRESULTADDR_4,strlen(GetALL_result));
        //测试完成，清除LED测试模式
        Clear_Leds();	

        if(*p0 == '0')
        {
            LED_NET_RED_ON;
            LED_SENSOR_RED_ON;
        }
        else if(*p0 == '1')
        {
            LED_NET_GREEN_ON;
            LED_SENSOR_GREEN_ON;
        }
        Test.ShowResultCnt = 200;     //通过LED显示测试结果持续200秒
        Test.TestOver = 1;
        Test.InTesting = 0;
    }
    else if(strstr(Uart3Buf, "AT^SET2="))
    {
        p0 = strstr(Uart3Buf, "AT^SET2=");
        p0 += 8;
        p1 = strstr(p0, "\r\n");
        if(p1 - p0 < sizeof(temp_buf))
        {
            strncpy(temp_buf,p0,p1 - p0);
            strcat(temp_buf,"#"); 
            EXFLASH_WriteBuffer((u8 *)temp_buf,TESTRESULTADDR_2,strlen(temp_buf));
            EXFLASH_ReadBuffer((u8 *)read_buf,TESTRESULTADDR_2,strlen(temp_buf));
            
            if(memcmp(temp_buf,read_buf,strlen(temp_buf)) == 0)
            {
                p1 = strstr(temp_buf, "#");
                *p1 = 0;
                sprintf(send_buf,"^DEV@SET2=%s\r\n",temp_buf);
                UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));
            }
        }
    }
    else if(strstr(Uart3Buf, "AT^SET3="))
    {
        p0 = strstr(Uart3Buf, "AT^SET3=");
        p0 += 8;
        p1 = strstr(p0, "\r\n");
        if(p1 - p0 < sizeof(temp_buf))
        {
            strncpy(temp_buf,p0,p1 - p0);
            strcat(temp_buf,"#"); 
            EXFLASH_WriteBuffer((u8 *)temp_buf,TESTRESULTADDR_3,strlen(temp_buf));
            EXFLASH_ReadBuffer((u8 *)read_buf,TESTRESULTADDR_3,strlen(temp_buf));
            
            if(memcmp(temp_buf,read_buf,strlen(temp_buf)) == 0)
            {
                p1 = strstr(temp_buf, "#");
                *p1 = 0;
                sprintf(send_buf,"^DEV@SET3=%s\r\n",temp_buf);
                UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));
            }
        }
    }
    else if(strstr(Uart3Buf, "AT^GET0="))
    {
        memset(Get0_result,0,sizeof(Get0_result));
        EXFLASH_ReadBuffer((u8 *)Get0_result,TESTRESULTADDR_0,sizeof(Get0_result));
        p0 = strstr(Get0_result,"#");
        *p0 = 0;
        sprintf(send_buf,"^DEV@GET0=%s\r\n",Get0_result);
        UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));
    }
    else if(strstr(Uart3Buf, "AT^GET1="))
    {
        EXFLASH_ReadBuffer((u8 *)read_buf,TESTRESULTADDR_1,sizeof(read_buf));
        p0 = strstr(read_buf,"#");
        *p0 = 0;
        sprintf(send_buf,"^DEV@GET1=%s\r\n",read_buf);
        UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));
    }
    else if(strstr(Uart3Buf, "AT^GET2="))
    {
        memset(Get2_result,0,sizeof(Get2_result));
        EXFLASH_ReadBuffer((u8 *)Get2_result,TESTRESULTADDR_2,sizeof(Get2_result));
        p0 = strstr(Get2_result,"#");
        *p0 = 0;
        sprintf(send_buf,"^DEV@GET2=%s\r\n",Get2_result);
        UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));
    }
    else if(strstr(Uart3Buf, "AT^GET3="))
    {
        memset(Get3_result,0,sizeof(Get3_result));
        EXFLASH_ReadBuffer((u8 *)Get3_result,TESTRESULTADDR_3,sizeof(Get3_result));
        p0 = strstr(Get3_result,"#");
        *p0 = 0;
        sprintf(send_buf,"^DEV@GET3=%s\r\n",Get3_result);
        UART_Send(USART3,(u8 *)send_buf,strlen(send_buf));
    }

    else if(strstr(Uart3Buf, "AT^NOTE="))
    {
        Test.WaitTestCnt = 0;
        p0 = strstr(Uart3Buf, "AT^NOTE=");
        p0 += 8;

        switch (*p0)
        {
         case '1':
            Test.TestStep = 1;
            break;
         case '2':
            Test.TestStep = 2;
            break;   
         case '3':
            Test.TestStep = 3;
            break;     
         case '4':
            Test.TestStep = 4;
            break;   
         case '5':
            Test.TestStep = 5;
            break;
         case '6':
            Test.TestStep = 6;
            break;   
         case '7':
            Test.TestStep = 7;
            break;     
         case '8':
            Test.TestStep = 8;
            break;
         case '9':
            Test.TestStep = 9;
            break;
        default:
            break;
        }
        
    }
}


void Test_Handle(void)
{
    char SendDataTemp[100] = {0};

    if((Test.WaitTestCnt > 0) || (Test.TestOver))
    {
        return;
    }
    Test.WaitTestCnt = 10;

    if(Test.TestOverStep != Test.TestStep)
    {
        switch (Test.TestStep)
        {
            case 1:                     //测试步骤1，输出软件版本及硬件版本    
                sprintf(SendDataTemp,"^TST@SW=%s;HW=%s\r\n",Edition_STD,HardWare_Edition);
                UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                Test.TestOverStep = Test.TestStep;
                
            break;

            case 2:                     //测试步骤2，输出模块版本及IMEI   
                if((Test.GetIMEI) && (Test.GetModuleAti))
                {
                    sprintf(SendDataTemp,"^DEV@IMEI=%s;Module ATI=%s\r\n",IMEI_MODULE,GsmRev);
                    UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                    Test.TestOverStep = Test.TestStep;
                }  
                else
                {
                    sprintf(SendDataTemp,"^NOTE@T0=%d\r\n",Test.TestStep);
                    UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));               /* code */
                }               
            break;

            case 3:                     //测试步骤3，外部flash测试结果
                if(Test.ExflashTestOver)
                {
                    if(Test.ExflashTestOk)
                    {
                        sprintf(SendDataTemp,"^EXFLASH@KEY=1\r\n");
                    }
                    else
                    {
                        sprintf(SendDataTemp,"^EXFLASH@KEY=0\r\n");
                    }
                    UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                    Test.TestOverStep = Test.TestStep;
                }      
            break;

            case 4:                     //测试步骤4，CCID读取
                if(Test.GetGsmCCID) 
                {
                    sprintf(SendDataTemp,"^GSM@CCID=%s\r\n",CCID);
                    UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                    Test.TestOverStep = Test.TestStep;
                }      
                else
                {
                    sprintf(SendDataTemp,"^NOTE@T0=%d\r\n",Test.TestStep);
                    UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));               /* code */
                }
            break;

            case 5:                     //测试步骤5，sensor测试结果,除RI外
                sprintf(SendDataTemp,"^SENSOR@CO2=%d;TEMP=%.1f*C;HUMI=%.1f%%\r\n",co2_module_value,temperature_value,humidity_value);
                UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                Test.TestOverStep = Test.TestStep;
            break;

            case 6:                     //测试步骤6，RI测试结果
                if(Test.GetRiAction) 
                {
                    sprintf(SendDataTemp,"^RI@KEY=1\r\n");
                }
                else
                {
                    sprintf(SendDataTemp,"^RI@KEY=0\r\n");
                }
                UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                Test.TestOverStep = Test.TestStep;
                    
            break;

            case 7:                     //测试步骤7,读取关键参数,平台地址和端口，APN相关信息
                memset(&Fs, 0, sizeof(Fs));
                STMFLASH_Read(FLASH_SAVE_ADDR, (u32 *)&Fs, (u16)(sizeof(Fs))/4);
                sprintf(SendDataTemp,"^APN@APN=%s;USRNAME=%s;PASSWORD=%s\r\n",Fs.ApnName,Fs.GprsUserName,Fs.GprsPassWord);
                UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                Test.TestOverStep = Test.TestStep;    
            break;

            case 8:                     //测试步骤8,读取电池电压
                sprintf(SendDataTemp,"^BAT@VOLTAGE=%d\r\n",BatVoltage_Adc);
                UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                Test.TestOverStep = Test.TestStep;  
            break;       

            case 9:                     //测试步骤9,读取GSM信号强度
                if(Test.GetGsmCsq) 
                {
                    sprintf(SendDataTemp,"^GSM@CSQ=%s\r\n",CsqValue);
                    UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));
                    Test.TestOverStep = Test.TestStep;
                }      
                else
                {
                    sprintf(SendDataTemp,"^NOTE@T0=%d\r\n",Test.TestStep);
                    UART_Send(USART3,(u8 *)SendDataTemp,strlen(SendDataTemp));               /* code */
                }
            break;   

            default:
                break;
        }
    }


    if(!Test.ExflashTestOver)
    {
        u8 test_buf[6] = {"Test"};
        u8 test_temp[6] = {0};
        
        Test.ExflashTestOver = 1;

        EXFLASH_EraseSector(0x00020000);
        EXFLASH_WriteBuffer(test_buf,0x00020000,4);
        EXFLASH_ReadBuffer(test_temp,0x00020000,4);
        
        if(memcmp(test_buf,test_temp,4) == 0)
        {
            Test.ExflashTestOk = 1;
        }
        EXFLASH_EraseSector(0x00020000);
        return;
    }

    if(Test.TestStep == 0)
	{
		UART_Send(USART3,"^NOTE@T0=?\r\n",12);
	}
}


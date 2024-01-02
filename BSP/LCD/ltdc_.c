/**
 ****************************************************************************************************
 * @file        ltdc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-4-20
 * @brief       LTDC ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ F429������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20220420
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "ltdc_.h"
#include "lcd.h"

LTDC_HandleTypeDef  g_ltdc_handle;       /* LTDC��� */
DMA2D_HandleTypeDef g_dma2d_handle;      /* DMA2D��� */

#if !(__ARMCC_VERSION >= 6010050)                                                    /* ����AC6����������ʹ��AC5������ʱ */

/* ���ݲ�ͬ����ɫ��ʽ,����֡�������� */
#if LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT == LCD_PIXFORMAT_RGB888
    uint32_t ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));   /* ����������ֱ���ʱ,LCD�����֡���������С */
#else
    uint16_t ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));   /* ����������ֱ���ʱ,LCD�����֡���������С */
    uint16_t ltdc_lcd_framebuf1[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR + 1280 * 800 * 2)));   /* ʹ��LTDC��2ʱʹ��(Ĭ��ʹ��LTDC��1) */
#endif

#else      /* ʹ��AC6������ʱ */

/* ���ݲ�ͬ����ɫ��ʽ,����֡�������� */
#if LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT == LCD_PIXFORMAT_RGB888
    uint32_t ltdc_lcd_framebuf[1280][800] __attribute__((section(".bss.ARM.__at_0XC0000000")));  /* ����������ֱ���ʱ,LCD�����֡���������С */
#else
    uint16_t ltdc_lcd_framebuf[1280][800] __attribute__((section(".bss.ARM.__at_0XC0000000")));  /* ����������ֱ���ʱ,LCD�����֡���������С */
#endif

#endif

uint32_t *ltdc_framebuf[2];                /* LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ����� */
_ltdc_dev lcdltdc;                         /* ����LCD LTDC����Ҫ���� */

/**
 * @brief       ��LCD����
 * @param       lcd_switch:1 ��,0���ر�
 * @retval      ��
 */
void ltdc_switch(uint8_t sw)
{
    if (sw == 1)
    {
        __HAL_LTDC_ENABLE(&g_ltdc_handle);
    }
    else if (sw == 0)
    {
        __HAL_LTDC_DISABLE(&g_ltdc_handle);
    }
}

/**
 * @brief       ����ָ����
 * @param       layerx:���,0,��һ��; 1,�ڶ���
 * @param       sw:1 ��;0�ر�
 * @retval      ��
 */
void ltdc_layer_switch(uint8_t layerx, uint8_t sw)
{
    if (sw == 1) 
    {
        __HAL_LTDC_LAYER_ENABLE(&g_ltdc_handle, layerx);
    }
    else if (sw == 0) 
    {
        __HAL_LTDC_LAYER_DISABLE(&g_ltdc_handle, layerx);
    }
    
    __HAL_LTDC_RELOAD_CONFIG(&g_ltdc_handle);
}

/**
 * @brief       ѡ���
 * @param       layerx:���;0,��һ��;1,�ڶ���;
 * @retval      ��
 */
void ltdc_select_layer(uint8_t layerx)
{
    lcdltdc.activelayer = layerx;
}

/**
 * @brief       ����LCD��ʾ����
 * @param       dir:0,������1,����
 * @retval      ��
 */
void ltdc_display_dir(uint8_t dir)
{
    lcdltdc.dir = dir;    /* ��ʾ���� */
    
    if (dir == 0)         /* ���� */
    {
        lcdltdc.width = lcdltdc.pheight;
        lcdltdc.height = lcdltdc.pwidth;
    }
    else if (dir == 1)    /* ���� */
    {
        lcdltdc.width = lcdltdc.pwidth;
        lcdltdc.height = lcdltdc.pheight;
    }
}

/**
 * @brief       ���㺯��
 * @param       x,y:д������
 * @param       color:��ɫֵ
 * @retval      ��
 */
void ltdc_draw_point(uint16_t x, uint16_t y, uint32_t color)
{ 
#if LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT == LCD_PIXFORMAT_RGB888
    if (lcdltdc.dir)   /* ���� */
    {
        *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
    else               /* ���� */
    {
        *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y)) = color; 
    }
#else
    if (lcdltdc.dir)   /* ���� */
    {
        *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x)) = color;
    }
    else              /* ���� */
    {
        *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)) = color; 
    }
#endif
}

/**
 * @brief       ���㺯��
 * @param       x,y:��ȡ�������
 * @param       ����ֵ:��ɫֵ
 * @retval      ��
 */
uint32_t ltdc_read_point(uint16_t x, uint16_t y)
{ 
#if LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT == LCD_PIXFORMAT_RGB888
    if (lcdltdc.dir)   /* ���� */
    {
        return *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
    else               /* ���� */
    {
        return *(uint32_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x) + y)); 
    }
#else
    if (lcdltdc.dir)   /* ���� */
    {
        return *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * y + x));
    }
    else               /* ���� */
    {
        return *(uint16_t *)((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * (lcdltdc.pheight - x - 1) + y)); 
    }
#endif 
}

/**
 * @brief       LTDC������,DMA2D���
 * @note        ��ʱ����ҪƵ���ĵ�����亯��������Ϊ���ٶȣ���亯�����üĴ����汾��
 * @note        ���������ж�Ӧ�Ŀ⺯���汾�Ĵ��롣
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
 * @param       color:Ҫ������ɫ
 * @retval      ��
 */
void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
{ 
    uint32_t psx, psy, pex, pey;   /* ��LCD���Ϊ��׼������ϵ,����������仯���仯 */
    uint32_t timeout = 0; 
    uint16_t offline;
    uint32_t addr; 

    /* ����ϵת�� */
    if (lcdltdc.dir)               /* ���� */
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else                          /* ���� */
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }

    offline = lcdltdc.pwidth - (pex - psx + 1);
    addr = ((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));

    __HAL_RCC_DMA2D_CLK_ENABLE();                              /* ʹ��DM2Dʱ�� */

    DMA2D->CR &= ~(DMA2D_CR_START);                            /* ��ֹͣDMA2D */
    DMA2D->CR = DMA2D_R2M;                                     /* �Ĵ������洢��ģʽ */
    DMA2D->OPFCCR = LCD_PIXFORMAT;                             /* ������ɫ��ʽ */
    DMA2D->OOR = offline;                                      /* ������ƫ��  */

    DMA2D->OMAR = addr;                                        /* ����洢����ַ */
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);    /* �趨�����Ĵ��� */
    DMA2D->OCOLR = color;                                      /* �趨�����ɫ�Ĵ��� */
    DMA2D->CR |= DMA2D_CR_START;                               /* ����DMA2D */

    while ((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)                /* �ȴ�������� */
    {
        timeout++;
        if (timeout > 0X1FFFFF)break;                          /* ��ʱ�˳� */
    } 
    DMA2D->IFCR |= DMA2D_FLAG_TC;                              /* ���������ɱ�־ */
}

///* ʹ��DMA2D��ص�HAL����ʹ��DMA2D����(���Ƽ�) */
///**
// * @brief       ��ָ����������䵥����ɫ
// * @param        (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
// * @param        color:Ҫ������ɫ
// * @retval      ��
// */
//void ltdc_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
//{
//    uint32_t psx, psy, pex, pey;   /* ��LCD���Ϊ��׼������ϵ,����������仯���仯 */
//    uint32_t timeout = 0; 
//    uint16_t offline;
//    uint32_t addr;
//    
//    if (ex >= lcdltdc.width)
//    {
//        ex = lcdltdc.width - 1;
//    }
//    
//    if (ey >= lcdltdc.height)
//    {
//        ey = lcdltdc.height - 1;
//    }
//    
//    /* ����ϵת�� */
//    if (lcdltdc.dir)   /* ���� */
//    {
//        psx = sx; 
//        psy = sy;
//        pex = ex;
//        pey = ey;
//    }
//    else             /* ���� */
//    {
//        psx = sy;
//        psy = lcdltdc.pheight - ex - 1;
//        pex = ey;
//        pey = lcdltdc.pheight - sx - 1;
//    }

//    offline = lcdltdc.pwidth - (pex - psx + 1);
//    addr = ((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));
//    
//    if (LCD_PIXFORMAT == LCD_PIXEL_FORMAT_RGB565)  /* �����RGB565��ʽ�Ļ���Ҫ������ɫת������16bitת��Ϊ32bit�� */
//    {
//        color = ((color & 0X0000F800) << 8) | ((color & 0X000007E0) << 5 ) | ((color & 0X0000001F) << 3);
//    }
//    
//    /* ����DMA2D��ģʽ */
//    g_dma2d_handle.Instance = DMA2D;
//    g_dma2d_handle.Init.Mode = DMA2D_R2M;               /* �ڴ浽�洢��ģʽ */
//    g_dma2d_handle.Init.ColorMode = LCD_PIXFORMAT;      /* ������ɫ��ʽ */
//    g_dma2d_handle.Init.OutputOffset = offline;         /* ���ƫ��  */
//    HAL_DMA2D_Init(&g_dma2d_handle);                    /* ��ʼ��DMA2D */
//    
//    HAL_DMA2D_ConfigLayer(&g_dma2d_handle, lcdltdc.activelayer);                            /* ������ */
//    HAL_DMA2D_Start(&g_dma2d_handle, color, (uint32_t)addr, pex - psx + 1, pey - psy + 1);  /* �������� */
//    HAL_DMA2D_PollForTransfer(&g_dma2d_handle, 1000);                                       /* �������� */
//    
//    while((__HAL_DMA2D_GET_FLAG(&g_dma2d_handle, DMA2D_FLAG_TC) == 0) && (timeout < 0X5000)) /* �ȴ�DMA2D��� */
//    {
//        timeout++;
//    }
//    __HAL_DMA2D_CLEAR_FLAG(&g_dma2d_handle,DMA2D_FLAG_TC);                                   /* ���������ɱ�־ */
//}

/**
 * @brief       ��ָ�����������ָ����ɫ��,DMA2D���
 * @param       �˺�����֧��u16,RGB565��ʽ����ɫ�������.
 * @param       (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
 * @param       ע��:sx,ex,���ܴ���lcddev.width-1;sy,ey,���ܴ���lcddev.height-1!!!
 * @param       color:Ҫ������ɫ�����׵�ַ
 * @retval      ��
 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint32_t psx, psy, pex, pey;   /* ��LCD���Ϊ��׼������ϵ,����������仯���仯 */
    uint32_t timeout = 0; 
    uint16_t offline;
    uint32_t addr;
  
    /* ����ϵת�� */
    if (lcdltdc.dir)               /* ���� */
    {
        psx = sx;
        psy = sy;
        pex = ex;
        pey = ey;
    }
    else                          /* ���� */
    {
        psx = sy;
        psy = lcdltdc.pheight - ex - 1;
        pex = ey;
        pey = lcdltdc.pheight - sx - 1;
    }
    
    offline = lcdltdc.pwidth - (pex - psx + 1);
    addr = ((uint32_t)ltdc_framebuf[lcdltdc.activelayer] + lcdltdc.pixsize * (lcdltdc.pwidth * psy + psx));

    __HAL_RCC_DMA2D_CLK_ENABLE();                             /* ʹ��DM2Dʱ�� */

    DMA2D->CR &= ~(DMA2D_CR_START);                           /* ��ֹͣDMA2D */
    DMA2D->CR = DMA2D_M2M;                                    /* �洢�����洢��ģʽ */
    DMA2D->FGPFCCR = LCD_PIXFORMAT;                           /* ������ɫ��ʽ */
    DMA2D->FGOR = 0;                                          /* ǰ������ƫ��Ϊ0 */
    DMA2D->OOR = offline;                                     /* ������ƫ�� */

    DMA2D->FGMAR = (uint32_t)color;                           /* Դ��ַ */
    DMA2D->OMAR = addr;                                       /* ����洢����ַ */
    DMA2D->NLR = (pey - psy + 1) | ((pex - psx + 1) << 16);   /* �趨�����Ĵ��� */
    DMA2D->CR |= DMA2D_CR_START;                              /* ����DMA2D */
    
    while((DMA2D->ISR & (DMA2D_FLAG_TC)) == 0)                /* �ȴ�������� */
    {
        timeout++;
        if (timeout > 0X1FFFFF)break;                         /* ��ʱ�˳� */
    } 
    DMA2D->IFCR |= DMA2D_FLAG_TC;                             /* ���������ɱ�־ */
}  

/**
 * @brief       LCD����
 * @param       color:��ɫֵ
 * @retval      ��
 */
void ltdc_clear(uint32_t color)
{
//    uint16_t x,y;
//    
//    for (y = 0; y < 800; y++)
//        for (x = 0; x < 480; x++)
//            ltdc_draw_point(x, y, color);
    
    ltdc_fill(0, 0, lcdltdc.width - 1, lcdltdc.height - 1, color);
}

/**
 * @brief       LTDCʱ��(Fdclk)���ú���
 * @param       Fvco=Fin*pllsain; 
 * @param       Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;
 *
 *              Fvco: VCOƵ��
 *              Fin:����ʱ��Ƶ��һ��Ϊ1Mhz(����ϵͳʱ��PLLM��Ƶ���ʱ��,��ʱ����ͼ)
 *              pllsain:SAIʱ�ӱ�Ƶϵ��N,ȡֵ��Χ:50~432.  
 *              pllsair:SAIʱ�ӵķ�Ƶϵ��R,ȡֵ��Χ:2~7
 *              pllsaidivr:LCDʱ�ӷ�Ƶϵ��,ȡֵ��Χ:RCC_PLLSAIDIVR_2/4/8/16,��Ӧ��Ƶ2~16 
 *              ����:�ⲿ����Ϊ25M,pllm=25��ʱ��,Fin=1Mhz.
 *              ����:Ҫ�õ�20M��LTDCʱ��,���������:pllsain=400,pllsair=5,pllsaidivr=RCC_PLLSAIDIVR_4
 *              Fdclk=1*400/5/4=400/20=20Mhz
 *              ����ֵ:0,�ɹ�;1,ʧ�ܡ�
*/
uint8_t ltdc_clk_set(uint32_t pllsain, uint32_t pllsair, uint32_t pllsaidivr)
{
//    RCC_PeriphCLKInitTypeDef periphclk_initure;

//    /* LTDC�������ʱ�ӣ���Ҫ�����Լ���ʹ�õ�LCD�����ֲ������ã� */
//    periphclk_initure.PeriphClockSelection = RCC_PERIPHCLK_LTDC;     /* LTDCʱ�� */
//    periphclk_initure.PLLSAI.PLLSAIN = pllsain;
//    periphclk_initure.PLLSAI.PLLSAIR = pllsair;
//    periphclk_initure.PLLSAIDivR = pllsaidivr;

//    if (HAL_RCCEx_PeriphCLKConfig(&periphclk_initure) == HAL_OK)     /* ��������ʱ�� */
//    {
        return 0;                                                    /* �ɹ� */
//    }
//    else return 1;                                                   /* ʧ�� */
}

/**
 * @brief       LTDC,�㴰������,������LCD�������ϵΪ��׼
 * @param       ע��:�˺���������LTDC_Layer_Parameter_Config֮��������.
 * @param       layerx:��ֵ,0/1.
 * @param       sx,sy:��ʼ����
 * @param       width,height:���Ⱥ͸߶�
 * @retval      ��
 */
void ltdc_layer_window_config(uint8_t layerx, uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    HAL_LTDC_SetWindowPosition(&g_ltdc_handle, sx, sy, layerx);   /* ���ô��ڵ�λ�� */
    HAL_LTDC_SetWindowSize(&g_ltdc_handle, width, height, layerx);/* ���ô��ڴ�С */
}

/**
 * @brief       LTDC,������������.
 * @note        �˺���,������LTDC_Layer_Window_Config֮ǰ����.
 * @param       layerx:��ֵ,0/1.
 * @param       bufaddr:����ɫ֡������ʼ��ַ
 * @param       pixformat:��ɫ��ʽ.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
 * @param       alpha:����ɫAlphaֵ,0,ȫ͸��;255,��͸��
 * @param       alpha0:Ĭ����ɫAlphaֵ,0,ȫ͸��;255,��͸��
 * @param       bfac1:���ϵ��1,4(100),�㶨��Alpha;6(101),����Alpha*�㶨Alpha
 * @param       bfac2:���ϵ��2,5(101),�㶨��Alpha;7(111),����Alpha*�㶨Alpha
 * @param       bkcolor:��Ĭ����ɫ,32λ,��24λ��Ч,RGB888��ʽ
 * @retval      ����ֵ:��
*/
void ltdc_layer_parameter_config(uint8_t layerx, uint32_t bufaddr, uint8_t pixformat, uint8_t alpha, uint8_t alpha0, uint8_t bfac1, uint8_t bfac2, uint32_t bkcolor)
{
    LTDC_LayerCfgTypeDef pLayerCfg;

    pLayerCfg.WindowX0 = 0;                                            /* ������ʼX���� */
    pLayerCfg.WindowY0 = 0;                                            /* ������ʼY���� */
    pLayerCfg.WindowX1 = lcdltdc.pwidth;                               /* ������ֹX���� */
    pLayerCfg.WindowY1 = lcdltdc.pheight;                              /* ������ֹY���� */
    pLayerCfg.PixelFormat = pixformat;                                 /* ���ظ�ʽ */
    pLayerCfg.Alpha = alpha;                                           /* Alphaֵ���ã�0~255,255Ϊ��ȫ��͸�� */
    pLayerCfg.Alpha0 = alpha0;                                         /* Ĭ��Alphaֵ */
    pLayerCfg.BlendingFactor1 = (uint32_t)bfac1 << 8;                  /* ���ò���ϵ�� */
    pLayerCfg.BlendingFactor2 = (uint32_t)bfac2 << 8;                  /* ���ò���ϵ�� */
    pLayerCfg.FBStartAdress = bufaddr;                                 /* ���ò���ɫ֡������ʼ��ַ */
    pLayerCfg.ImageWidth = lcdltdc.pwidth;                             /* ������ɫ֡�������Ŀ��� */
    pLayerCfg.ImageHeight = lcdltdc.pheight;                           /* ������ɫ֡�������ĸ߶� */
    pLayerCfg.Backcolor.Red = (uint8_t)(bkcolor & 0X00FF0000) >> 16;   /* ������ɫ��ɫ���� */
    pLayerCfg.Backcolor.Green = (uint8_t)(bkcolor & 0X0000FF00) >> 8;  /* ������ɫ��ɫ���� */
    pLayerCfg.Backcolor.Blue = (uint8_t)bkcolor & 0X000000FF;          /* ������ɫ��ɫ���� */
    HAL_LTDC_ConfigLayer(&g_ltdc_handle, &pLayerCfg, layerx);          /* ������ѡ�еĲ� */
}  

/**
 * @brief       ��ȡ������
 * @param        PG6=R7(M0);PI2=G7(M1);PI7=B7(M2);
 * @param        M2:M1:M0
 * @param        0 :0 :0    4.3�� 480*272 RGB��,ID=0X4342
 * @param        0 :0 :1    7  �� 800*480 RGB��,ID=0X7084
 * @param        0 :1 :0    7  ��1024*600 RGB��,ID=0X7016
 * @param        0 :1 :1    7  ��1280*800 RGB��,ID=0X7018
 * @param        1 :0 :0    4.3�� 800*480 RGB��,ID=0X4384
 * @param        1 :0 :1   10.1��1280*800 RGB��,ID=0X1018 
 * @param        ����ֵ:LCD ID:0,�Ƿ�;����ֵ,ID;
 * @retval      ��
 */
uint16_t ltdc_panelid_read(void)
{
//    uint8_t idx = 0;
//    GPIO_InitTypeDef gpio_init_struct;
//    
//    __HAL_RCC_GPIOG_CLK_ENABLE();                           /* ʹ��GPIOGʱ�� */
//    __HAL_RCC_GPIOI_CLK_ENABLE();                           /* ʹ��GPIOIʱ�� */
//    
//    gpio_init_struct.Pin = GPIO_PIN_6;                      /* PG6 */
//    gpio_init_struct.Mode = GPIO_MODE_INPUT;                /* ���� */
//    gpio_init_struct.Pull = GPIO_PULLUP;                    /* ���� */
//    gpio_init_struct.Speed = GPIO_SPEED_HIGH;               /* ���� */
//    HAL_GPIO_Init(GPIOG, &gpio_init_struct);                /* ��ʼ�� */
//    
//    gpio_init_struct.Pin = GPIO_PIN_2 | GPIO_PIN_7;         /* PI2,7 */
//    HAL_GPIO_Init(GPIOI, &gpio_init_struct);                /* ��ʼ�� */
//    
//    idx = (uint8_t)HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_6);     /* ��ȡM0 */
//    idx|= (uint8_t)HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_2) << 1;/* ��ȡM1 */
//    idx|= (uint8_t)HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) << 2;/* ��ȡM2 */
//    
//    switch (idx)
//    {
//        case 0 : 
//            return 0X4342;     /*  4.3����, 480*272�ֱ��� */

//        case 1 : 
//            return 0X7084;     /*  7  ����, 800*480�ֱ��� */

//        case 2 : 
//            return 0X7016;     /*  7  ����,1024*600�ֱ��� */

//        case 3 : 
//            return 0X7018;     /*  7  ����,1280*800�ֱ��� */

//        case 4 : 
//            return 0X4384;     /*  4.3����, 800*480�ֱ��� */

//        case 5 : 
//            return 0X1018;     /* 10.1����,1280*800�ֱ��� */

//        default : 
//            return 0;
//    }
return 0X4384;

}

/**
 * @brief       LCD��ʼ������
 * @param       ��
 * @retval      ��
 */
void ltdc_init(void)
{
    uint16_t lcdid = 0;

    lcdid = ltdc_panelid_read();                  /* ��ȡLCD���ID */
    
    if (lcdid == 0X4342)
    {
        lcdltdc.pwidth = 480;                     /* ������,��λ:���� */
        lcdltdc.pheight = 272;                    /* ���߶�,��λ:���� */
        lcdltdc.hsw = 1;                          /* ˮƽͬ������ */
        lcdltdc.vsw = 1;                          /* ��ֱͬ������ */
        lcdltdc.hbp = 40;                         /* ˮƽ���� */
        lcdltdc.vbp = 8;                          /* ��ֱ���� */
        lcdltdc.hfp = 5;                          /* ˮƽǰ�� */
        lcdltdc.vfp = 8;                          /* ��ֱǰ�� */
//        ltdc_clk_set(288, 4, RCC_PLLSAIDIVR_8);   /* ��������ʱ�� 9Mhz */
        /* ������������. */
    }
    else if (lcdid == 0X7084)
    {
        lcdltdc.pwidth = 800;                   /* ������,��λ:���� */
        lcdltdc.pheight = 480;                  /* ���߶�,��λ:���� */
        lcdltdc.hsw = 1;                        /* ˮƽͬ������ */
        lcdltdc.vsw = 1;                        /* ��ֱͬ������ */
        lcdltdc.hbp = 46;                       /* ˮƽ���� */
        lcdltdc.vbp = 23;                       /* ��ֱ���� */
        lcdltdc.hfp = 210;                      /* ˮƽǰ�� */
        lcdltdc.vfp = 22;                       /* ��ֱǰ�� */
//        ltdc_clk_set(396, 3, RCC_PLLSAIDIVR_4); /* ��������ʱ�� 33M(�����˫��,��Ҫ����DCLK��:18.75Mhz  300/4/4,�Ż�ȽϺ�) */
    }
    else if (lcdid == 0X7016)
    {
        lcdltdc.pwidth = 1024;                 /* ������,��λ:���� */
        lcdltdc.pheight = 600;                 /* ���߶�,��λ:���� */
        lcdltdc.hsw = 20;                      /* ˮƽͬ������ */
        lcdltdc.vsw = 3;                       /* ��ֱͬ������ */
        lcdltdc.hbp = 140;                     /* ˮƽ���� */
        lcdltdc.vbp = 20;                      /* ��ֱ���� */
        lcdltdc.hfp = 160;                     /* ˮƽǰ�� */
        lcdltdc.vfp = 12;                      /* ��ֱǰ�� */
//        ltdc_clk_set(360, 2, RCC_PLLSAIDIVR_4);/* ��������ʱ��  45Mhz  */
        /* ������������.*/
    }
    else if (lcdid == 0X7018)
    {
        lcdltdc.pwidth = 1280;                  /* ������,��λ:���� */
        lcdltdc.pheight = 800;                  /* ���߶�,��λ:���� */
        /* ������������.*/
    }
    else if (lcdid == 0X4384)
    {
        lcdltdc.pwidth = 800;                   /* ������,��λ:���� */
        lcdltdc.pheight = 480;                  /* ���߶�,��λ:���� */
        lcdltdc.hbp = 88;                       /* ˮƽ���� */
        lcdltdc.hfp = 40;                       /* ˮƽǰ�� */
        lcdltdc.hsw = 48;                       /* ˮƽͬ������ */
        lcdltdc.vbp = 32;                       /* ��ֱ���� */
        lcdltdc.vfp = 13;                       /* ��ֱǰ�� */
        lcdltdc.vsw = 3;                        /* ��ֱͬ������ */
			
			
			
//			  lcdltdc.pwidth = 800;                   /* ������,��λ:���� */
//        lcdltdc.pheight = 480;                  /* ���߶�,��λ:���� */
//        lcdltdc.hbp = 8;                       /* ˮƽ���� */
//        lcdltdc.hfp = 8;                       /* ˮƽǰ�� */
//        lcdltdc.hsw = 4;                       /* ˮƽͬ������ */
//        lcdltdc.vbp = 8;                       /* ��ֱ���� */
//        lcdltdc.vfp = 8;                       /* ��ֱǰ�� */
//        lcdltdc.vsw = 4;                        /* ��ֱͬ������ */
//        ltdc_clk_set(396, 3, RCC_PLLSAIDIVR_4); /* ��������ʱ�� 33M */
        /* ������������. */
    }
    else if (lcdid == 0X1018)                   /* 10.1��1280*800 RGB�� */
    {
        lcdltdc.pwidth = 1280;                 /* ������,��λ:���� */
        lcdltdc.pheight = 800;                 /* ���߶�,��λ:���� */
        lcdltdc.hbp = 140;                     /* ˮƽ���� */
        lcdltdc.hfp = 10;                      /* ˮƽǰ�� */
        lcdltdc.hsw = 10;                      /* ˮƽͬ������ */
        lcdltdc.vbp = 10;                      /* ��ֱ���� */
        lcdltdc.vfp = 10;                      /* ��ֱǰ�� */
        lcdltdc.vsw = 3;                       /* ��ֱͬ������ */
//        ltdc_clk_set(360, 2, RCC_PLLSAIDIVR_4);/* ��������ʱ��  45Mhz */
    } 

    lcddev.width = lcdltdc.pwidth;
    lcddev.height = lcdltdc.pheight;
    
#if LCD_PIXFORMAT == LCD_PIXFORMAT_ARGB8888 || LCD_PIXFORMAT == LCD_PIXFORMAT_RGB888 
    ltdc_framebuf[0] = (uint32_t*) &ltdc_lcd_framebuf;
    lcdltdc.pixsize = 4;                        /* ÿ������ռ4���ֽ� */
#else 
    lcdltdc.pixsize = 2;                        /* ÿ������ռ2���ֽ� */
    ltdc_framebuf[0] = (uint32_t*)&ltdc_lcd_framebuf;
//    ltdc_framebuf[1] = (uint32_t*)&ltdc_lcd_framebuf1;
#endif 
    /* LTDC���� */
    g_ltdc_handle.Instance = LTDC;
    g_ltdc_handle.Init.HSPolarity = LTDC_HSPOLARITY_AL;         /* ˮƽͬ������ */
    g_ltdc_handle.Init.VSPolarity = LTDC_VSPOLARITY_AL;         /* ��ֱͬ������ */
    g_ltdc_handle.Init.DEPolarity = LTDC_DEPOLARITY_AL;         /* ����ʹ�ܼ��� */
    
    if (lcdid == 0X1018)
    {
        g_ltdc_handle.Init.PCPolarity = LTDC_PCPOLARITY_IIPC;   /* ����ʱ�Ӽ��� */
    }
    else 
    {
        g_ltdc_handle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;    /* ����ʱ�Ӽ��� */
    }
//		
//		

//		
//    
    g_ltdc_handle.Init.HorizontalSync = lcdltdc.hsw - 1;                                            /* ˮƽͬ������ */
    g_ltdc_handle.Init.VerticalSync = lcdltdc.vsw - 1;                                              /* ��ֱͬ������ */
    g_ltdc_handle.Init.AccumulatedHBP = lcdltdc.hsw + lcdltdc.hbp - 1;                              /* ˮƽͬ�����ؿ��� */
    g_ltdc_handle.Init.AccumulatedVBP = lcdltdc.vsw + lcdltdc.vbp - 1;                              /* ��ֱͬ�����ظ߶� */
    g_ltdc_handle.Init.AccumulatedActiveW = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth - 1;         /* ��Ч���� */
    g_ltdc_handle.Init.AccumulatedActiveH = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight - 1;        /* ��Ч�߶� */
    g_ltdc_handle.Init.TotalWidth = lcdltdc.hsw + lcdltdc.hbp + lcdltdc.pwidth + lcdltdc.hfp - 1;   /* �ܿ��� */
    g_ltdc_handle.Init.TotalHeigh = lcdltdc.vsw + lcdltdc.vbp + lcdltdc.pheight + lcdltdc.vfp - 1;  /* �ܸ߶� */
		
    g_ltdc_handle.Init.Backcolor.Red = 0xFF;                                                           /* ��Ļ�������ɫ���� */
    g_ltdc_handle.Init.Backcolor.Green = 0;                                                         /* ��Ļ��������ɫ���� */
    g_ltdc_handle.Init.Backcolor.Blue = 0;                                                          /* ��Ļ����ɫ��ɫ���� */
    HAL_LTDC_Init(&g_ltdc_handle);

    /* ������ */
    ltdc_layer_parameter_config(0, (uint32_t)ltdc_framebuf[0], LCD_PIXFORMAT, 255, 0, 6, 7, 0X000000);/* ��һ��������� */
//    ltdc_layer_parameter_config(1, (uint32_t)ltdc_framebuf[1], LCD_PIXFORMAT, 127, 0, 6, 7, 0X000000);/* �ڶ���������� */

//    ltdc_display_dir(0);      /* Ĭ������,��lcd_init������������ */
    ltdc_select_layer(0);       /* ѡ���1�� */
//    LCD_BL(1);                  /* �������� */
    ltdc_clear(0Xbb);     /* ���� */
		
}

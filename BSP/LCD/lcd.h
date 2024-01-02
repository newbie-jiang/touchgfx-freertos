/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       2.8��/3.5��/4.3��/7�� TFTLCD(MCU��) ��������
 *              ֧������IC�ͺŰ���:ILI9341/NT35310/NT35510/SSD1963��
 *
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F429������
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

#ifndef __LCD_H
#define __LCD_H

#include "stdlib.h"
#include "stdint.h"


/******************************************************************************************/
/* LCD RST/WR/RD/BL/CS/RS ���� ���� 
 * LCD_D0~D15,��������̫��,�Ͳ������ﶨ����,ֱ����lcd_init�����޸�.��������ֲ��ʱ��,���˸�
 * ��6��IO��, ���ø�LCD_Init�����D0~D15���ڵ�IO��.
 */

/* RESET ��ϵͳ��λ�Ź��� �������ﲻ�ö��� RESET���� */
//#define LCD_RST_GPIO_PORT               GPIOx
//#define LCD_RST_GPIO_PIN                SYS_GPIO_PINx
//#define LCD_RST_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOx_CLK_ENABLE(); }while(0)   /* ����IO��ʱ��ʹ�� */

#define LCD_WR_GPIO_PORT                GPIOD
#define LCD_WR_GPIO_PIN                 GPIO_PIN_5
#define LCD_WR_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */

#define LCD_RD_GPIO_PORT                GPIOD
#define LCD_RD_GPIO_PIN                 GPIO_PIN_4
#define LCD_RD_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */

#define LCD_BL_GPIO_PORT                GPIOB
#define LCD_BL_GPIO_PIN                 GPIO_PIN_5
#define LCD_BL_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* ��������IO��ʱ��ʹ�� */

/* LCD_CS(��Ҫ����LCD_FSMC_NEX������ȷ��IO��) �� LCD_RS(��Ҫ����LCD_FSMC_AX������ȷ��IO��) ���� ���� */
#define LCD_CS_GPIO_PORT                GPIOD
#define LCD_CS_GPIO_PIN                 GPIO_PIN_7
#define LCD_CS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */

#define LCD_RS_GPIO_PORT                GPIOD
#define LCD_RS_GPIO_PIN                 GPIO_PIN_13
#define LCD_RS_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOD_CLK_ENABLE(); }while(0)     /* ����IO��ʱ��ʹ�� */

/* FMC��ز��� ���� 
 * ע��: ����Ĭ����ͨ��FMC��1������LCD, ��1��4��Ƭѡ: FMC_NE1~4
 *
 * �޸�LCD_FMC_NEX, ��Ӧ��LCD_CS_GPIO�������Ҳ�ø�
 * �޸�LCD_FMC_AX , ��Ӧ��LCD_RS_GPIO�������Ҳ�ø�
 */
#define LCD_FMC_NEX         1              /* ʹ��FMC_NE1��LCD_CS,ȡֵ��Χֻ����: 1~4 */
#define LCD_FMC_AX          18             /* ʹ��FMC_A18��LCD_RS,ȡֵ��Χ��: 0 ~ 25 */

/******************************************************************************************/

/* LCD��Ҫ������ */
typedef struct
{
    uint16_t width;     /* LCD ���� */
    uint16_t height;    /* LCD �߶� */
    uint16_t id;        /* LCD ID */
    uint8_t dir;        /* ���������������ƣ�0��������1�������� */
    uint16_t wramcmd;   /* ��ʼдgramָ�� */
    uint16_t setxcmd;   /* ����x����ָ�� */
    uint16_t setycmd;   /* ����y����ָ�� */
} _lcd_dev;

/* LCD���� */
extern _lcd_dev lcddev;             /* ����LCD��Ҫ���� */

/* LCD�Ļ�����ɫ�ͱ���ɫ */
extern uint32_t  g_point_color;     /* Ĭ�Ϻ�ɫ */
extern uint32_t  g_back_color;      /* ������ɫ.Ĭ��Ϊ��ɫ */

/* LCD������� */
#define LCD_BL(x)   do{ x ? \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LCD_BL_GPIO_PORT, LCD_BL_GPIO_PIN, GPIO_PIN_RESET); \
                     }while(0)

/* LCD��ַ�ṹ�� */
typedef struct
{
    volatile uint16_t LCD_REG;
    volatile uint16_t LCD_RAM;
} LCD_TypeDef;


/* LCD_BASE����ϸ���㷽��:
 * ����һ��ʹ��FMC�Ŀ�1(BANK1)������TFTLCDҺ����(MCU��), ��1��ַ��Χ�ܴ�СΪ256MB,���ֳ�4��:
 * �洢��1(FMC_NE1)��ַ��Χ: 0x6000 0000 ~ 0x63FF FFFF
 * �洢��2(FMC_NE2)��ַ��Χ: 0x6400 0000 ~ 0x67FF FFFF
 * �洢��3(FMC_NE3)��ַ��Χ: 0x6800 0000 ~ 0x6BFF FFFF
 * �洢��4(FMC_NE4)��ַ��Χ: 0x6C00 0000 ~ 0x6FFF FFFF
 *
 * ������Ҫ����Ӳ�����ӷ�ʽѡ����ʵ�Ƭѡ(����LCD_CS)�͵�ַ��(����LCD_RS)
 * ������F429������ʹ��FMC_NE1����LCD_CS, FMC_A18����LCD_RS ,16λ������,���㷽������:
 * ����FMC_NE1�Ļ���ַΪ: 0x6000 0000;     NEX�Ļ�ַΪ(x=1/2/3/4): 0x6000 0000 + (0x400 0000 * (x - 1))
 * FMC_A18��Ӧ��ֵַ: 2^18 * 2 = 0x80000;    FMC_Ay��Ӧ�ĵ�ַΪ(y = 0 ~ 25): 2^y * 2
 *
 * LCD->LCD_REG,��ӦLCD_RS = 0(LCD�Ĵ���); LCD->LCD_RAM,��ӦLCD_RS = 1(LCD����)
 * �� LCD->LCD_RAM�ĵ�ַΪ:  0x6000 0000 + 2^18 * 2 = 0x6008 0000
 *    LCD->LCD_REG�ĵ�ַ����Ϊ LCD->LCD_RAM֮��������ַ.
 * ��������ʹ�ýṹ�����LCD_REG �� LCD_RAM(REG��ǰ,RAM�ں�,��Ϊ16λ���ݿ���)
 * ��� �ṹ��Ļ���ַ(LCD_BASE) = LCD_RAM - 2 = 0x6008 0000 -2
 *
 * ����ͨ�õļ��㹫ʽΪ((Ƭѡ��FSMC_NEX)X=1/2/3/4, (RS�ӵ�ַ��FSMC_Ay)y=0~25):
 *          LCD_BASE = (0x6000 0000 + (0x400 0000 * (x - 1))) | (2^y * 2 -2)
 *          ��Ч��(ʹ����λ����)
 *          LCD_BASE = (0x6000 0000 + (0x400 0000 * (x - 1))) | ((1 << y) * 2 -2)
 */
#define LCD_BASE        (uint32_t)((0X60000000 + (0X4000000 * (LCD_FMC_NEX - 1))) | (((1 << LCD_FMC_AX) * 2) -2))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

/******************************************************************************************/
/* LCDɨ�跽�����ɫ ���� */

/* ɨ�跽���� */
#define L2R_U2D         0           /* ������,���ϵ��� */
#define L2R_D2U         1           /* ������,���µ��� */
#define R2L_U2D         2           /* ���ҵ���,���ϵ��� */
#define R2L_D2U         3           /* ���ҵ���,���µ��� */

#define U2D_L2R         4           /* ���ϵ���,������ */
#define U2D_R2L         5           /* ���ϵ���,���ҵ��� */
#define D2U_L2R         6           /* ���µ���,������ */
#define D2U_R2L         7           /* ���µ���,���ҵ��� */

#define DFT_SCAN_DIR    L2R_U2D     /* Ĭ�ϵ�ɨ�跽�� */

/* ���û�����ɫ */
#define WHITE           0xFFFF      /* ��ɫ */
#define BLACK           0x0000      /* ��ɫ */
#define RED             0xF800      /* ��ɫ */
#define GREEN           0x07E0      /* ��ɫ */
#define BLUE            0x001F      /* ��ɫ */ 
#define MAGENTA         0xF81F      /* Ʒ��ɫ/�Ϻ�ɫ = BLUE + RED */
#define YELLOW          0xFFE0      /* ��ɫ = GREEN + RED */
#define CYAN            0x07FF      /* ��ɫ = GREEN + BLUE */  

/* �ǳ�����ɫ */
#define BROWN           0xBC40      /* ��ɫ */
#define BRRED           0xFC07      /* �غ�ɫ */
#define GRAY            0x8430      /* ��ɫ */ 
#define DARKBLUE        0x01CF      /* ����ɫ */
#define LIGHTBLUE       0x7D7C      /* ǳ��ɫ */ 
#define GRAYBLUE        0x5458      /* ����ɫ */ 
#define LIGHTGREEN      0x841F      /* ǳ��ɫ */  
#define LGRAY           0xC618      /* ǳ��ɫ(PANNEL),���屳��ɫ */ 
#define LGRAYBLUE       0xA651      /* ǳ����ɫ(�м����ɫ) */ 
#define LBBLUE          0x2B12      /* ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */ 

/******************************************************************************************/
/* SSD1963������ò���(һ�㲻�ø�) */

/* LCD�ֱ������� */ 
#define SSD_HOR_RESOLUTION      800     /* LCDˮƽ�ֱ��� */ 
#define SSD_VER_RESOLUTION      480     /* LCD��ֱ�ֱ��� */ 

/* LCD������������ */ 
#define SSD_HOR_PULSE_WIDTH     1       /* ˮƽ���� */ 
#define SSD_HOR_BACK_PORCH      46      /* ˮƽǰ�� */ 
#define SSD_HOR_FRONT_PORCH     210     /* ˮƽ���� */ 

#define SSD_VER_PULSE_WIDTH     1       /* ��ֱ���� */ 
#define SSD_VER_BACK_PORCH      23      /* ��ֱǰ�� */ 
#define SSD_VER_FRONT_PORCH     22      /* ��ֱǰ�� */ 

/* ���¼����������Զ����� */ 
#define SSD_HT          (SSD_HOR_RESOLUTION + SSD_HOR_BACK_PORCH + SSD_HOR_FRONT_PORCH)
#define SSD_HPS         (SSD_HOR_BACK_PORCH)
#define SSD_VT          (SSD_VER_RESOLUTION + SSD_VER_BACK_PORCH + SSD_VER_FRONT_PORCH)
#define SSD_VPS         (SSD_VER_BACK_PORCH)
   
/******************************************************************************************/
/* �������� */

void lcd_wr_data(volatile uint16_t data);                                                   /* LCDд���� */
void lcd_wr_regno(volatile uint16_t regno);                                                 /* LCDд�Ĵ������/��ַ */
void lcd_write_reg(uint16_t regno, uint16_t data);                                          /* LCDд�Ĵ�����ֵ */

void lcd_init(void);                                                                        /* ��ʼ��LCD */ 
void lcd_display_on(void);                                                                  /* ����ʾ */ 
void lcd_display_off(void);                                                                 /* ����ʾ */
void lcd_scan_dir(uint8_t dir);                                                             /* ������ɨ�跽�� */ 
void lcd_display_dir(uint8_t dir);                                                          /* ������Ļ��ʾ���� */ 
void lcd_ssd_backlight_set(uint8_t pwm);                                                    /* SSD1963 ������� */ 

void lcd_write_ram_prepare(void);                                                           /* ׼��дGRAM */ 
void lcd_write_ram(uint16_t RGB_Code);                                                      /* LCDдGRAM */
void lcd_set_cursor(uint16_t x, uint16_t y);                                                /* ���ù�� */ 
uint32_t lcd_read_point(uint16_t x, uint16_t y);                                            /* ����(32λ��ɫ,����LTDC) */
void lcd_draw_point(uint16_t x, uint16_t y, uint32_t color);                                /* ����(32λ��ɫ,����LTDC) */

void lcd_clear(uint16_t color);                                                             /* LCD���� */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                   /* ���ʵ��Բ */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);                  /* ��Բ */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                  /* ��ˮƽ�� */
void lcd_set_window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);             /* ���ô��� */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color);          /* ��ɫ������(32λ��ɫ,����LTDC) */
void lcd_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color);   /* ��ɫ������ */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);     /* ��ֱ�� */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);/* ������ */

void lcd_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);



void LCD_DrawBitmap(int startX, int startY, int width, int height, const uint16_t* bitmap);/*ˢͼ����*/


#endif

















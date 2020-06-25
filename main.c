#include <CMSIS/stm32f10x.h>
#include <math.h>
// типовой вариант функции assert
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
    ;
  }
}
#define pi 3.1416

#define FrequencySin 880		//Частота синуса
#define Duration		10 		//Длительность в ms (only int)			500 < Duration < 1500

//Морзянка
#define DOT 1
#define DASH 3
#define pause 2
#define pause3 4
#define pause7 8
#define end 0

//Глобальные переменные

volatile uint16_t array_count = 0;
volatile uint16_t pause_flag = 0, sin_flag = 0;
const uint16_t T = ((Duration * FrequencySin) / 1000)  ;
int* buf = 0;

const int ArraySize = 220;
float sin_array[ArraySize] = { 0 };

const float period = (Duration / 1000) / FrequencySin;
const uint32_t FrequencyPWD = 200000; 
const uint16_t PeriodTim = 72000000 / FrequencyPWD;



const float delta = (2 * pi) / ArraySize ;

//Доступные символы

int n_array[5] = {DASH,pause,DOT,pause3,end};
int i_array[5] = {DOT,pause,DOT,pause3,end};
int k_array[7] = {DASH,pause,DOT,pause,DASH,pause3,end};
int t_array[3] = {DASH,pause,end};
int a_array[5] = {DOT,pause,DASH,pause3,end};
int h_array[9] = {DOT,pause,DOT,pause,DOT,pause,DOT,pause3,end};
int e_array[3] = {DOT,pause3,end};
int l_array[9] = {DOT,pause,DASH,pause,DOT,pause,DOT,pause3,end};
int o_array[7] = {DASH,pause,DASH,pause,DASH,pause3,end};
int w_array[7] = {DOT,pause,DASH,pause,DASH,pause3,end};
int r_array[7] = {DOT,pause,DASH,pause,DOT,pause3,end};
int d_array[7] = {DASH,pause,DOT,pause,DOT,pause3,end};
int space_array[2] = {pause7,end};

//int n_array[6] = {4,3,2,1,2,0};
//int i_array[6] = {4,1,2,1,2,0};
//int k_array[8] = {4,3,2,1,2,3,2,0};
//int t_array[4] = {4,3,2,0};
//int a_array[6] = {4,1,2,3,2,0};
//int h_array[10] = {4,1,2,1,2,1,2,1,2,0};
//int e_array[4] = {4,1,2,0};
//int l_array[10] = {4,1,2,3,2,1,2,1,2,0};
//int o_array[8] = {4,3,2,3,2,3,2,0};
//int w_array[8] = {4,1,2,3,2,3,2,0};
//int r_array[8] = {4,1,2,3,2,1,2,0};
//int d_array[8] = {4,3,2,1,2,1,2,0};
//int space_array[2] = {8,0};

//Прототипы функций
void translate (char symbol);
static void tim_init(void);
static void array_create(void);


//------------------------------------------------
//
//------------------------------------------------

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		//Если установлен флаг, то проигрываем паузу. Если нет, то генерируем сигнал.
		if (pause_flag > 0)
		{
			if (array_count <= ArraySize)
			{
				TIM_SetCompare2(TIM2, 100);
				array_count++;
				NVIC_DisableIRQ(TIM2_IRQn);
			}
			else
			{
				array_count = 0;
				pause_flag++;
			}
		}
		else
		{
			if (array_count <= ArraySize)
			{
				TIM_SetCompare2(TIM2, sin_array[array_count]);
				array_count++;
				NVIC_DisableIRQ(TIM2_IRQn);
			}
			else
			{
				array_count = 0;
				sin_flag++;
			}
		}
	}
}

//------------------------------------------------
//
//------------------------------------------------

int main(void)
{
	const uint8_t size = 255;
	
	char phrase[size] = {"hello world"};

	volatile uint8_t i = 0, j = 0, temp = 0, def = 0;
	
	array_create();
	tim_init();
		
  while(1)
	{
		//Проходим по массиву с фразой
		while (i < size)
		{
			//Считываем новый символ строки и переводим его в морзянку
			translate(phrase[i]);

			//Пробегаем по массиву конретной буквы
			while(*(buf+j) != 0)
			{
				temp = *(buf+j);
				switch(temp)
				{
					case 1:
									NVIC_EnableIRQ(TIM2_IRQn);
									if (sin_flag == T)
									{
										sin_flag = 0;
										array_count = 0;
										j++;
									}
									break;
									
					case 3:
									NVIC_EnableIRQ(TIM2_IRQn);
									if (sin_flag == 3 * T)
									{
										sin_flag = 0;
										array_count = 0;
										j++;
									}
									break;
					case 2:
									if (pause_flag == 0) pause_flag++;
									NVIC_EnableIRQ(TIM2_IRQn);
									if (pause_flag == T)
									{
										pause_flag = 0;
										array_count = 0;
										j++;
									}
									break;
								
					case 4:
									if (pause_flag == 0) pause_flag++;
									NVIC_EnableIRQ(TIM2_IRQn);
									if (pause_flag == 3 * T)
									{
										pause_flag = 0;
										array_count = 0;
										j++;
									}
									break;

					case 8:
									if (pause_flag == 0) pause_flag++;
									NVIC_EnableIRQ(TIM2_IRQn);
									if (pause_flag == 4 * T)
									{
										pause_flag = 0;
										array_count = 0;
										j++;
									}
									break;

					default:
										def++;
										i = size;
										break;
				}
			}
			NVIC_DisableIRQ(TIM2_IRQn);
			j = 0;
			i++;	//Переходим к следующему символу

			
		}
		break;
	}

  return 0;
}

//------------------------------------------------
//
//------------------------------------------------

void translate (char symbol)
{
	switch(symbol)
		{
			case 'n':
				buf = &(n_array[0]);
				break;

			case 'i':
				buf = i_array;
				break;

			case 'k':
				buf = k_array;
				break;

			case 't':
				buf = t_array;
				break;

			case 'a':
				buf = a_array;
				break;

			case 'h':
				buf = h_array;
				break;

			case 'e':
				buf = e_array;
				break;

			case 'l':
				buf = l_array;
				break;

			case 'o':
				buf = o_array;
				break;

			case 'w':
				buf = w_array;
				break;

			case 'r':
				buf = r_array;
				break;

			case 'd':
				buf = d_array;
				break;

			case ' ':
				buf = space_array;
				break;

			default:
				*buf = 0;
				break;
			

		}	
}

//------------------------------------------------
//
//------------------------------------------------

static void array_create(void)
{
	for (uint16_t i = 0; i <= ArraySize; i++)
	{
		sin_array[i] = 100 + ((period / 2) + 100 * sin(delta*i));
	}
}

//------------------------------------------------
//
//------------------------------------------------

static void tim_init(void)
{
	//тактируем ножку и таймер
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//настраиваем ножку
	GPIO_InitTypeDef pin;

	pin.GPIO_Mode = GPIO_Mode_AF_PP;
	pin.GPIO_Pin = GPIO_Pin_1;
	pin.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &pin);
	
	//настраиваем таймер
	TIM_TimeBaseInitTypeDef timer;

	timer.TIM_Prescaler = 0;
	timer.TIM_Period = PeriodTim;
	timer.TIM_ClockDivision = TIM_CKD_DIV1;
	timer.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM2, &timer);

	//настройка ШИМ
	TIM_OCInitTypeDef timerPWM;
	   
	timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
	timerPWM.TIM_OutputState = TIM_OutputState_Enable;
	timerPWM.TIM_Pulse = 0;
	timerPWM.TIM_OCPolarity = TIM_OCPolarity_High;
		
	TIM_OC2Init(TIM2, &timerPWM);

	TIM_ITConfig(TIM2, TIM_DIER_UIE, ENABLE);
	
	TIM_Cmd(TIM2, ENABLE);

}
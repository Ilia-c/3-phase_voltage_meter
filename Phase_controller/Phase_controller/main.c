#define F_CPU 16000000L
#include <xc.h>										// базовая библиотека с названиями регистров
#include <util/delay.h>								// библиотека для задержки
#include <avr/interrupt.h>
#include <math.h>



double Sum = 0;
double Buff = 0;

double U1dey = 0;
double U2dey = 0;
double U3dey = 0;

int16_t count_ADC_cycle = 0;

int8_t ADC_pr = 1;
int8_t ADC_second = 1;

int16_t pahse_num = 123;


ISR(TIMER1_COMPA_vect){
	//ADCSRA = (0<<ADEN)|(0<<ADSC)|(0<<ADATE)|(0<<ADCSRA)|(0<<ADIF)|(0<<ADIE);
	ADC_pr = 4;
	TCNT1 = 0;
}

ISR(ADC_vect){
	count_ADC_cycle++;
	Buff = ADC/204.8;
	Sum += Buff*Buff;
}


void Send_4bite(unsigned char bit_4){				// Функция записи 4х бит в LCD
	PORTD |= (1 << 5);								// E = 1 (начало записи команды)
	PORTD |= bit_4|(PIND&0b11000000);				// Передаем значение
	_delay_us(100);									// Задержка что-бы дисплей успел заметить измененное состояние
	PORTD &= ~((1 << 5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0));								// E = 0 (конец записи команды)
	_delay_us(100);									// Задержка что-бы дисплей успел заметить измененное состояние
	//PORTD = 0x00;									// Установка в 0 для корректной работы (без этого при следующем вызове фунция будет некорректно работать)
}
void Send_4bit_text(unsigned char text){			// Функция вывода текста на экран
	PORTD |= ((1 << 5)|(1<<4));						// E = 1 Rs = 1 (начало записи текста)
	PORTD |= text|(PIND&0b11000000);				// Передаем текст
	_delay_us(100);									// Задержка что-бы дисплей успел заметить измененное состояние
	PORTD &= ~((1 << 5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0));						// E = 0 Rs = 0 (начало записи текста)
	_delay_us(100);									// Задержка что-бы дисплей успел заметить измененное состояние
	//PORTD = 0x00;									// Установка в 0 для корректной работы (без этого при следующем вызове фунция будет некорректно работать)
}
void Send_text(unsigned char text){					// Функция отправляет 8 битный код символа, который необходимо вывести на дисплей
	Send_4bit_text(text>>4);						// Запись старшего полубайта
	Send_4bit_text(text & 0b00001111);				// Запись младшего полубайта
}
void initialization(){	
	_delay_ms(50);					// Функция инициализации дисплея
	// Установка четырехразрядного режима
	PORTD |= (1 << 1);
	PORTD &= ~(1 << 0);
	_delay_ms(50);
	// Активизация четырехразрядного режима
	PORTD |= (1 << 5);
	PORTD &= ~(1 << 5);
	_delay_ms(50);
	
	// Шина 4 бит, LCD - 2 строки
	Send_4bite(0b0010);
	Send_4bite(0b1000);
	_delay_ms(5);
	// Полное выключение дисплея
	Send_4bite(0b0000);
	Send_4bite(0b1000);
	_delay_ms(5);
	// Очистка дисплея
	Send_4bite(0b0000);
	Send_4bite(0b0001);
	
	_delay_ms(50);
	
	// Включение дисплея, курсор не видим
	Send_4bite(0b0000);
	Send_4bite(0b1100);
	
	_delay_ms(20);
	
	// Сдвиг курсора влево
	Send_4bite(0b1000);
	Send_4bite(0b0000);
	
}
void print_push(int16_t phase1, int16_t phase2, int16_t phase3, int16_t Pfase_n){			// Функция вывода сообщения "Нажата кнопка " и номер кнопки
	Send_4bite(0b0000);
	Send_4bite(0b0001);
	_delay_ms(5);
	Send_4bite(0b0000);
	Send_4bite(0b0010);
	_delay_ms(5);	
		
	unsigned char phase_count_1 = (phase1/100)+'0';
	unsigned char phase_count_2 = (phase1%100/10)+'0';
	unsigned char phase_count_3 = (phase1%10)+'0';
	unsigned char f1 = (Pfase_n/100)+'0';
	unsigned char f2 = (Pfase_n%100/10)+'0';
	unsigned char f3 = (Pfase_n%10)+'0';
	
	Send_4bite(0b1000);
	Send_4bite(0b0000);

	//Send_text(0x02);	// печать пустого символа
	Send_text(0b10101010);
	Send_text('1');
	Send_text(':');
	Send_text(phase_count_1);
	Send_text(phase_count_2);
	Send_text(phase_count_3);
	
	phase_count_1 = phase2/100+'0';
	phase_count_2 = phase2%100/10+'0';
	phase_count_3 = phase2%10+'0';	
	
	Send_text(' ');	// печать пустого символа
	Send_text(0b10101010);
	Send_text('2');
	Send_text(':');
	Send_text(phase_count_1);
	Send_text(phase_count_2);
	Send_text(phase_count_3);	
	
	Send_text(' ');
	Send_text(' ');
	if ((phase1>180) && (phase1<240) && (phase2>180) && (phase2<240) && (phase3>180) && (phase3<240)){
		Send_text('H');
	}
	else{
		if ((phase1>180) && (phase1<240)){
			Send_text(0b10100100);
		}
		else{
			Send_text('O');
		}
	}
	
	
	phase_count_1 = phase3/100+'0';
	phase_count_2 = phase3%100/10+'0';
	phase_count_3 = phase3%10+'0';
	
	Send_4bite(0b1100);
	Send_4bite(0b0000);
	
	Send_text(0b10101010);
	Send_text('3');
	Send_text(':');
	Send_text(phase_count_1);
	Send_text(phase_count_2);
	Send_text(phase_count_3);
	
	
	Send_text(' ');	// печать пустого символа	
	Send_text(0b10101000);
	//Send_text(':');
	Send_text(f1);
	Send_text(f2);
	Send_text(f3);
	
	Send_text(' ');
	Send_text(0b10100100);
	ADMUX = (0<<REFS1)|(1<<REFS0)|(1<<ADLAR)|(1<<MUX0)|(1<<MUX1)|(0<<MUX2)|(0<<MUX3);
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADCSRA)|(0<<ADIF)|(0<<ADIE);
	_delay_ms(5);
	int8_t buff = ADCH;
	float proc = buff/51*2;
	int8_t procent = 0;
	
	if (proc>=4.1){
		procent = 99;
	}
	else{
		if (proc>4){
			procent = 80;
		}
		else{
			if (proc>3.8){
				procent = 50;
			}
			else{
				if (proc>3.7){
					procent = 30;
				}
				else{
					if (proc>3.6){
						procent = 10;
					}
					else{
						if (proc>3.1){
							procent = 1;
						}
					}
				}
			}
		}
	}
	Send_text(((procent/10)+'0'));
	Send_text(((procent%10)+'0'));
	Send_text('%');
	Send_text((procent+'0'));
	
}
void print_start(){		
	
	Send_4bite(0b1000);
	Send_4bite(0b0000);
	
	Send_text(0b11100000);
	Send_text('o');
	Send_text(0b10110010);
	Send_text('p');
	Send_text('o');
	
	Send_text(' ');
	
	Send_text(0b10111110);
	Send_text('o');
	Send_text(0b10110110);
	Send_text('a');
	Send_text(0b10111011);
	Send_text('o');
	Send_text(0b10110011);
	Send_text('a');
	Send_text(0b10111111);
	Send_text(0b11000100);	
}
void phase_alternation(){
	while (ADC > 20){
		ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX0)|(0<<MUX1)|(0<<MUX2)|(0<<MUX3);
		ADCSRA = (1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADCSRA)|(0<<ADIF)|(0<<ADIE);
		_delay_us(10);
	}
	
	ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(1<<MUX0)|(0<<MUX1)|(0<<MUX2)|(0<<MUX3);
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADCSRA)|(0<<ADIF)|(0<<ADIE);
	_delay_us(10);
	int16_t phase2 = ADC;

	pahse_num = 100;
	
	if (phase2>30){
		pahse_num += 23;
	}
	else{
		pahse_num += 32;
	}
	
}


int main(void)
{
	Send_4bite(0b0000);
	Send_4bite(0b0001);
	_delay_ms(5);
	Send_4bite(0b0000);
	Send_4bite(0b0010);
	_delay_ms(10);
	//DDRB = 0xFF;
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	_delay_ms(1000);
	DDRC = 0x00;
	DDRB = 0x06;
	DDRD = 0xFF;	// на передачу PD0-PD7
	PORTD = 0x00;	// Порт D в 0
	//DDRB = 0xFF;
	PORTD |= (1<<6);
	PORTD |= (1<<7);
	_delay_ms(1000);
		
	initialization();
	print_start();
	TCCR1B = (0 << CS11)|(0 << CS10)|(1 << CS12); //Делитель на 64 и прерывание по совпадению в регистре OCR1A
	OCR1A = F_CPU/256;
	TIMSK1 = (1<<OCIE0A); //устанавливаем бит разрешения прерывания 1ого счетчика по совпадению с OCR1A(H и L)
	TCNT1 = 0;
	PORTD &= ~(1<<6);
	PORTD &= ~(1<<7);	
	ADC_pr = 1;
	ADC_second = 0;
	
	sei();
	//int16_t a = 0;
    while(1)
    {

		if (ADC_pr == 1){
			ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX0)|(0<<MUX1)|(0<<MUX2)|(0<<MUX3);
			ADCSRA = (1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADCSRA)|(1<<ADIF)|(1<<ADIE);
		}
		if (ADC_pr == 2){
			ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(1<<MUX0)|(0<<MUX1)|(0<<MUX2)|(0<<MUX3);
			ADCSRA = (1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADCSRA)|(1<<ADIF)|(1<<ADIE);
		}
		if (ADC_pr == 3){
			ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX0)|(1<<MUX1)|(0<<MUX2)|(0<<MUX3);
			ADCSRA = (1<<ADEN)|(1<<ADSC)|(0<<ADATE)|(0<<ADCSRA)|(1<<ADIF)|(1<<ADIE);
		}
		if (ADC_pr == 4)
		{
			cli();
			
			if (ADC_second == 0){
				U1dey = (sqrt(Sum*1/count_ADC_cycle)*2*2700/22*1.0917);
			}
			if (ADC_second == 1){
				U2dey = (sqrt(Sum*1/count_ADC_cycle)*2*2700/22*1.0917);
			}
			if (ADC_second == 2){
				U3dey = (sqrt(Sum*1/count_ADC_cycle)*2*2700/22*1.0917);
				if ((U1dey<50) || (U2dey<50) || (U3dey<50))
				{
					pahse_num = 0; 
				}
				else
				{
					phase_alternation();
				}
				print_push(U1dey, U2dey, U3dey, pahse_num); 
				ADC_second = 0;
				ADC_pr = 1;
				if ((U1dey>110) && (U1dey<238)){
					PORTB |= (1<<1);
					_delay_ms(500);
					PORTB |= (1<<2);
					PORTD |= (1<<6);
				}
				else{
					PORTB &= ~(1<<1);
					PORTB &= ~(1<<2);
					PORTD &= ~(1<<6);
				}
			}
			else{
				ADC_second++;
				ADC_pr = ADC_second+1;
			}
			count_ADC_cycle = 0;	
			Sum = 0;		
			TCNT1 = 0;
			sei();
		}
		
		
    }
}
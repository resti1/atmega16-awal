#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000L
#define BAUD 9600
// Deklarasi konstanta
#define THRESHOLD 100 // Ambang batas suara

// Fungsi untuk menginisialisasi pin dan pengaturan ADC
void initADC() {
	ADMUX |= (1 << REFS0); // Tegangan referensi internal AVCC
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Mengaktifkan ADC dengan preskaler 128
}

// Fungsi untuk membaca nilai ADC
uint16_t readADC(uint8_t channel) {
	ADMUX = (ADMUX & 0xF8) | (channel & 0x07); // Memilih channel ADC yang akan dibaca
	ADCSRA |= (1 << ADSC); // Memulai konversi ADC
	while (ADCSRA & (1 << ADSC)); // Menunggu konversi selesai
	return ADC;
}

// Fungsi untuk menginisialisasi pengaturan serial komunikasi (UART)
void initUART() {
	// Mengatur baud rate 9600 bps dengan preskaler 8MHz
	UBRRH = 0;
	UBRRL = 51;
	// Mengaktifkan transmit dan receive
	UCSRB = (1 << RXEN) | (1 << TXEN);
	// Mengatur format frame: 8 data, 1 stop bit
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void initPortD(){
	DDRD &=~(1<<PD2); // Set PD2 sebagai input
	PORTD |= (1<<PD2); // aktifkan pull-up 
}


// Fungsi untuk mengirim karakter melalui serial komunikasi
void sendChar(char data) {
	while (!(UCSRA & (1 << UDRE))); // Menunggu buffer transmit kosong
	UDR = data; // Mengirim karakter
}

// Fungsi untuk mengirim string melalui serial komunikasi
void sendString(char *string) {
	while (*string) {
		sendChar(*string);
		string++;
	}
}

int main(void) {
	DDRB |= (1 << PB0); // Mengatur PB0 sebagai output (LED)
	
	initPortD(); //inisialisasi portd D
	initADC(); // Inisialisasi ADC
	initUART(); // Inisialisasi serial komunikasi (UART)

	sei(); // Mengaktifkan interrupt global
	ADCSRA |= (1 << ADIE); // Mengaktifkan interrupt ADC

	while (1) {
		// Tidak ada yang perlu dilakukan di dalam loop utama
	}
}

ISR(ADC_vect) {
	uint16_t sensorValue = ADC; // Membaca nilai ADC dari sensor suara
	uint8_t inputStatus = PIND & (1<<PD2); // membaca status input pada pin PD2

	if (sensorValue > THRESHOLD) {
		PORTB |= (1 << PB0); // Mengaktifkan LED
		sendString("Saklar: ON\n"); // Mengirim status saklar melalui serial
		} else {
		PORTB &= ~(1 << PB0); // Mematikan LED
		sendString("Saklar: OFF\n"); // Mengirim status saklar melalui serial
		}
	}

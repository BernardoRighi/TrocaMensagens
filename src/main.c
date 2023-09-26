#include <sys/io.h>

#define Black 0x0
#define Light_Gray 0x7
#define Dark_Gray 0x8
#define Blue 0x1
#define White 0xF

//Initialization
#define PORT 0x3f8   /* COM1 */
 
void init_serial() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

//Receiving data
int serial_received() {
   return inb(PORT + 5) & 1;
}
 
char read_serial() {
   while (serial_received() == 0);
 
   return inb(PORT);
}
//Sending data
int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   outb(PORT,a);
}

unsigned short *video = (unsigned short *)0xB8000;

void printc(int x, int y, int fcolor, int bcolor, char c)
{
    video[x + y * 80] = (fcolor << 8) | (bcolor << 12) | c;
}

void prints(int x, int y, int fcolor, int bcolor, char *str)
{

    int inicio = x + y * 80;

    while (*str)
    {
        video[inicio] = (fcolor << 8) | (bcolor << 12) | *str;
        write_serial(*str);
        inicio++;
        str++;
    }
}

char mapeamento_teclado(int key)
{
    char key_map[150];

    for (int i = 0; i < 150; i++)
    {
        key_map[i] = '*';
    }

    key_map[0x02] = '1';
    key_map[0x03] = '2';
    key_map[0x04] = '3';
    key_map[0x05] = '4';
    key_map[0x06] = '5';
    key_map[0x07] = '6';
    key_map[0x08] = '7';
    key_map[0x09] = '8';
    key_map[0x0a] = '9';
    key_map[0x0b] = '0';
    key_map[0x1e] = 'A';
    key_map[0x30] = 'B';
    key_map[0x2e] = 'C';
    key_map[0x20] = 'D';
    key_map[0x12] = 'E';
    key_map[0x21] = 'F';
    key_map[0x22] = 'G';
    key_map[0x23] = 'H';
    key_map[0x17] = 'I';
    key_map[0x24] = 'J';
    key_map[0x25] = 'K';
    key_map[0x26] = 'L';
    key_map[0x32] = 'M';
    key_map[0x31] = 'N';
    key_map[0x18] = 'O';
    key_map[0x19] = 'P';
    key_map[0x10] = 'Q';
    key_map[0x13] = 'R';
    key_map[0x1f] = 'S';
    key_map[0x14] = 'T';
    key_map[0x16] = 'U';
    key_map[0x2f] = 'V';
    key_map[0x2d] = 'X';
    key_map[0x2c] = 'Z';
    key_map[0x15] = 'Y';
    key_map[0x11] = 'W';
    //barra de espaço
    key_map[0x39] = ' ';
    //enter
    key_map[0x1c] = 0x0A;
    //backspace
    key_map[0x0E] = ' ';

    return key_map[key];
}

void limpa_tela(void)
{
    int posicao_x = 0;
    for (posicao_x = 0; posicao_x < 1400; posicao_x++)
    {
        printc(posicao_x, 0, 0x00, 0x00, ' ');
    }
}

void limpa_chat(void)
{
    int posicao_x = 0;
    for (posicao_x = 0; posicao_x < 600; posicao_x++)
    {
        printc(posicao_x, 17, 0x00, 0x00, ' ');
    }
}

void tela(void)
{
    int pos_x = 0;
    int pos_y = 0;
    int fcolor2 = Blue;
    int bcolor2 = Blue;

    for (pos_x = 0; pos_x < 80; pos_x++)
    {
        printc(pos_x, 0, fcolor2, bcolor2, ' ');
    }
    for (pos_x = 0; pos_x < 80; pos_x++)
    {
        printc(pos_x, 17, fcolor2, bcolor2, ' ');
    }
    for (pos_x = 0; pos_x < 80; pos_x++)
    {
        printc(pos_x, 24, fcolor2, bcolor2, ' ');
    }

    for (pos_y = 0; pos_y < 24; pos_y++)
    {
        printc(0, pos_y, fcolor2, bcolor2, ' ');
    }

    for (pos_y = 0; pos_y < 24; pos_y++)
    {
        printc(79, pos_y, fcolor2, bcolor2, ' ');
    }
}

void dataehora(int linha, int coluna)
{
    //Dia
    outb(0x70, 0x07);
    int dia = inb(0x71);
    printc(0 + coluna, linha, White, Black, (dia >> 4) + '0');
    printc(1 + coluna, linha, White, Black, (0x0F & dia) + '0');
    printc(2 + coluna, linha, White, Black, '/');

    //Mes
    outb(0x70, 0x08);
    int mes = inb(0x71);
    printc(3 + coluna, linha, White, Black, (mes >> 4) + '0');
    printc(4 + coluna, linha, White, Black, (0x0F & mes) + '0');
    printc(5 + coluna, linha, White, Black, '/');

    //Ano
    outb(0x70, 0x09);
    int ano = inb(0x71);
    printc(6 + coluna, linha, White, Black, (ano >> 4) + '0');
    printc(7 + coluna, linha, White, Black, (0x0F & ano) + '0');

    //Horas
    outb(0x70, 0x04);
    int hora = inb(0x71);
    printc(8 + coluna, linha, White, Black, '-');
    printc(9 + coluna, linha, White, Black, (hora >> 4) + '0');
    printc(10 + coluna, linha, White, Black, (0x0F & hora) - 3 + '0');
    printc(11 + coluna, linha, White, Black, ':');

    //Minutos
    outb(0x70, 0x02);
    int min = inb(0x71);
    printc(12 + coluna, linha, White, Black, (min >> 4) + '0');
    printc(13 + coluna, linha, White, Black, (0x0F & min) + '0');
    printc(14 + coluna, linha, White, Black, ':');

    //Segundos
    outb(0x70, 0x00);
    int seg = inb(0x71);
    printc(15 + coluna, linha, White, Black, (seg >> 4) + '0');
    printc(16 + coluna, linha, White, Black, (0x0F & seg) + '0');
    printc(17 + coluna, linha, White, Black, ':');
}

int key = 0;
int key_temp = 0;
int chat_linha = 20;
int visualizar_linha = 20;
static char vetor_buffer[17][100];
int auxrow = 1;
int auxcol = 0;

int main(void)
{
    init_serial();
    limpa_tela();
    tela();

    while (1)
    {
        dataehora(19, 2);
        key = inb(0x60);

        if (serial_received()) {
            printc(visualizar_linha, 1, White, Black, read_serial());
        }

        if (mapeamento_teclado(key) != '*' && key < 150)
        {

            if (key_temp != key)
            {
                key_temp = key;
            
                if (mapeamento_teclado(key) != 0x0A) {

                    if (key == 0x0E) {
                        chat_linha--;
                        auxcol--;
                        printc(chat_linha, 22, White, Black, mapeamento_teclado(key)); //imprime linha do chat
                        vetor_buffer[auxrow][auxcol] = mapeamento_teclado(key);
                    }

                    printc(chat_linha, 19, White, Black, mapeamento_teclado(key)); //imprime linha do chat
                    vetor_buffer[auxrow][auxcol] = mapeamento_teclado(key);
                    chat_linha++;
                    auxcol++;
                }
                else // if (mapeamento_teclado(key) == 0x0A)
                {
                    vetor_buffer[auxrow][auxcol] = '\0';
                    dataehora(auxrow, 2);
                    visualizar_linha = 20;
                    prints(visualizar_linha, auxrow, White, Black, vetor_buffer[auxrow]);
                    auxcol = 0;
                    limpa_chat();
                    tela();
                    chat_linha = 20;
                    if(auxrow == 16){
                        limpa_tela();
                        tela();
                        prints(visualizar_linha, 1, White, Black, vetor_buffer[auxrow]);
                        dataehora(1, 2);
                        auxrow = 2;
                    } else {
                        auxrow++;
                    }
                }
            }
        }
    }
    return 0;
}
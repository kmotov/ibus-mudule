/*
 * ibus.c
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 

#include "ibus.h"

// debug
extern AVR_S16 nRxMaxSize;
extern AVR_S16 nTxMaxSize;
extern AVR_U16 nRxSkiped;
extern AVR_U16 nTxSkiped;
extern AVR_U8  dbg_mark5;
extern AVR_U8  dbg_mark6;

#define IBUS_MIN_TIME_BEETWEEN_CMDS_MS (100)

/* IBUS Buffer Defines */
#define IBUS_RX_BUFFER_SIZE 256     /* 2,4,8,16,32,64,128 or 256 bytes */
#define IBUS_TX_BUFFER_SIZE 256

#define IBUS_RX_BUFFER_MASK (IBUS_RX_BUFFER_SIZE - 1)
#if (IBUS_RX_BUFFER_SIZE & IBUS_RX_BUFFER_MASK)
    #error RX buffer size is not a power of 2
#endif

#define IBUS_TX_BUFFER_MASK (IBUS_TX_BUFFER_SIZE - 1)
#if (IBUS_TX_BUFFER_SIZE & IBUS_TX_BUFFER_MASK)
#error TX buffer size is not a power of 2
#endif

AVR_U8 dbg_mark4;

/* Static Variables */
AVR_U8 IBUS_RxBuf[IBUS_RX_BUFFER_SIZE];
volatile AVR_U16 IBUS_RxHead;
volatile AVR_U16 IBUS_RxTail;

AVR_U8 dbg_mark3;

AVR_U8 IBUS_TxBuf[IBUS_TX_BUFFER_SIZE];
volatile AVR_U16 IBUS_TxHead;
volatile AVR_U16 IBUS_TxTail;

AVR_U8 dbg_mark2;
AVR_U8 RetBuff[IBUS_CMD_MAX_SIZE + 1]; // + XOR

AVR_U8 SendBuff[IBUS_CMD_MAX_SIZE + 1]; // + XOR

AVR_U8 dbg_mark1;
AVR_U8 ReceiveBuff[IBUS_CMD_MAX_SIZE]; // do not store XOR
AVR_U8 dbg_mark0;
AVR_S16 ReceiveBuffSize;

AVR_S16 SendBuffCommandSize;
AVR_S16 SendBuffNextByteForSend; // used for send
AVR_S16 SendBuffNextByteForCheck; // used for check for collisions
AVR_S16 RemaindingBytes;
AVR_S16 CommandsForSend;
AVR_BOOL bCollision;

static AVR_S16 TIMER0_OVF_Count = 0;
static AVR_S16 TIMER0_TOT_Count = 0; // timeout
static AVR_U16 TimeLT = 0; // time last transaction
static AVR_U16 TimeCurr = 0; // current time

typedef enum {
    STATE_IDLE,
    STATE_GET_SIZE,
    STATE_GET_DATA,
    STATE_XOR
} StateT;

static StateT eState;

static void RunIBusTimer(void)
{
    TCCR0B  = (1 << CS01) | (1 << CS00); // Timer Clock = system clock / 64 (~1ms)
    TCNT0   = 0;             // reset Timer Count Register
    TIFR0   = (1 << TOV0);   // Clear TOV0 / Clear pending interrupts
    TIMSK0  = (1 << TOIE0);  // Enable Timer 0 Overflow Interrupt
}

void ibus_init(void)
{
    AVR_U16 ubrr;

    ubrr = (((((F_CPU * 10) / (16L * 9600)) + 5) / 10) - 1);

    // Set baud rate
    UBRR0H = (AVR_U8)(ubrr>>8);
    UBRR0L = (AVR_U8)ubrr;

    //Enable receiver, transmitter and RX INT
    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);

    // 8 bits, 1 stop bit, Even parity
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) | (1<<UPM01);

    // Flush receive buffer
    IBUS_RxTail = 0; // point first data
    IBUS_RxHead = 0; // point first free
    IBUS_TxTail = 0;
    IBUS_TxHead = 0;

    SendBuffCommandSize = 0;
    CommandsForSend = 0;
    SendBuffNextByteForSend = 0;
    bCollision = FALSE;

    RunIBusTimer();

    eState = STATE_IDLE;
}

void ibus_deinit(void)
{
    UCSR0A = 0;
    UCSR0B = 0;

    // stop timer :)
}

static void loadNextCmd(void)
{
    AVR_S16 i;
    AVR_S16 size;

    IBUS_TxTail = (IBUS_TxTail + 1) & IBUS_TX_BUFFER_MASK;
    SendBuff[0] = IBUS_TxBuf[IBUS_TxTail];
    IBUS_TxTail = (IBUS_TxTail + 1) & IBUS_TX_BUFFER_MASK;
    SendBuff[1] = IBUS_TxBuf[IBUS_TxTail];

    size = SendBuff[1] + 2;

    for(i = 2; i < size; i++) {
        IBUS_TxTail = (IBUS_TxTail + 1) & IBUS_TX_BUFFER_MASK;
        SendBuff[i] = IBUS_TxBuf[IBUS_TxTail];
    }

    CommandsForSend--;

    SendBuffNextByteForSend  = 0;
    SendBuffNextByteForCheck = size;
    SendBuffCommandSize      = size;

    return;
}

/****************************************************************/
/*                  Read and write functions                    */
/****************************************************************/
void ibus_send_msg( AVR_U8 *cmd )
{
    AVR_S16 i;
    AVR_S16 free_size;
    AVR_U8 XorByte = 0;
    AVR_U16 dTime = 0;

    if (IBUS_TxHead >= IBUS_TxTail) {
        free_size = IBUS_TX_BUFFER_SIZE - (IBUS_TxHead - IBUS_TxTail);
    } else {
        free_size = (IBUS_TxTail - IBUS_TxHead);
    }

#ifdef IBUS_DEBUG
    if (nTxMaxSize < (IBUS_TX_BUFFER_SIZE - free_size + cmd[1] + 2)) {
        nTxMaxSize = (IBUS_TX_BUFFER_SIZE - free_size + cmd[1] + 2);
    }
#endif

    if ((cmd[1] + 2) > IBUS_CMD_MAX_SIZE) {
        dbg_mark5 = 1;
        return; // todo return error
    }

    if (free_size < (cmd[1] + 2)) {
        // skip command because TX buffer is full
        nTxSkiped++;
        return; // todo return error
    }

    for (i = 0; i < (cmd[1] + 1); i++) {
        IBUS_TxHead = (IBUS_TxHead + 1) & IBUS_TX_BUFFER_MASK;
        IBUS_TxBuf[IBUS_TxHead] = cmd[i];
        XorByte ^= IBUS_TxBuf[IBUS_TxHead];
    }
    IBUS_TxHead = (IBUS_TxHead + 1) & IBUS_TX_BUFFER_MASK;
    IBUS_TxBuf[IBUS_TxHead] = XorByte;

    if (!TIMER0_OVF_Count && !CommandsForSend) {
        if (TimeCurr < TimeLT) {
            dTime = (0xFFFF - TimeLT) + TimeCurr + 1;
        } else {
            dTime = TimeCurr - TimeLT;
        }
		
        if (dTime >= IBUS_MIN_TIME_BEETWEEN_CMDS_MS) {
            TIMER0_OVF_Count = IBUS_MIN_TIME_BEETWEEN_CMDS_MS;
        } else {
            TIMER0_OVF_Count = dTime + 1;
        }
    }

    CommandsForSend++;
}

AVR_U8* ibus_get_msg( void )
{
    AVR_S16 i;

    if( IBUS_RxHead != IBUS_RxTail ) {
        IBUS_RxTail = (IBUS_RxTail + 1) & IBUS_RX_BUFFER_MASK;
        RetBuff[0] = IBUS_RxBuf[IBUS_RxTail];
        IBUS_RxTail = (IBUS_RxTail + 1) & IBUS_RX_BUFFER_MASK;
        RetBuff[1] = IBUS_RxBuf[IBUS_RxTail];
        for(i = 2; i < (RetBuff[1] + 1); i++) {
            IBUS_RxTail = (IBUS_RxTail + 1) & IBUS_RX_BUFFER_MASK;
            RetBuff[i] = IBUS_RxBuf[IBUS_RxTail];
        }
        return RetBuff;
    } else {
        return NULL;
    }
}

/****************************************************************/
/*                      Interrupt handlers                      */
/****************************************************************/
ISR(TIMER0_OVF_vect)
{
    TimeCurr++;
	TCNT0 = 0;

    if (TIMER0_OVF_Count && TIMER0_OVF_Count++ > IBUS_MIN_TIME_BEETWEEN_CMDS_MS) {
        TIMER0_OVF_Count = 0;

        if (bCollision) { // collision
            // Check for free bus
            if ((!(UCSR0A & (1<<UDRE0))) || (UCSR0A & (1<<RXC0))) {
                TIMER0_OVF_Count = 1; // Start send cmd counter
            } else {
                // Restart Sending
                SendBuffNextByteForSend  = 0;
                SendBuffNextByteForCheck = SendBuffCommandSize;
                bCollision               = FALSE;

                // Start TX (Enable UDRE interrupt)
                UCSR0B |= (1 << UDRIE0);
            }
        } else { // load next cmd
            loadNextCmd();

            // Start TX (Enable UDRE interrupt)
            UCSR0B |= (1 << UDRIE0);
        }
    }

    if (TIMER0_TOT_Count && TIMER0_TOT_Count++ > 4) { // 4ms
        TIMER0_TOT_Count = 0;
        eState = STATE_IDLE; // timeout occurs go to idle state
    }
}

ISR(USART_UDRE_vect)
{
    // Check if all data is transmitted
    if( SendBuffNextByteForSend < SendBuffCommandSize ) {
        UDR0 =  SendBuff[SendBuffNextByteForSend];
        SendBuffNextByteForSend++;
    } else {
        if (CommandsForSend) {
            TIMER0_OVF_Count = 1; // Start send cmd counter
        }
        // Disable UDRE interrupt
        UCSR0B &= ~(1<<UDRIE0);
    }
}

ISR(USART_RX_vect)
{
    AVR_S16 free_size;
    AVR_U8 i;
    AVR_U8 XorByte;
    AVR_U8 data;
    AVR_U8 myData;

    // Read the received data immediately
    data = UDR0;

    TimeLT = TimeCurr;

    if (SendBuffNextByteForCheck > 0) { // check for collision
        myData = SendBuff[SendBuffCommandSize - SendBuffNextByteForCheck];

        if (myData != data) {
            // Restart Sending
            SendBuffNextByteForCheck = 0;

            // Disable UDRE interrupt
            UCSR0B &= ~(1<<UDRIE0);

            bCollision = TRUE;
            //midLedOn();

            TIMER0_OVF_Count = 1; // Start send cmd counter
        } else {
            SendBuffNextByteForCheck--;
        }
        eState = STATE_IDLE;
    } else { // Receive Command
        TIMER0_TOT_Count = 1; // reset timeout counter

        switch (eState) {
            case STATE_IDLE :
                ReceiveBuff[0] = data;
                ReceiveBuffSize = 1;
                eState = STATE_GET_SIZE;
                break;

            case STATE_GET_SIZE :
                if (((data + 2) < IBUS_CMD_MIN_SIZE) || ((data + 2) >= IBUS_CMD_MAX_SIZE)) { // full msg size is data + 2
                    eState = STATE_IDLE;
                    TIMER0_TOT_Count = 0; // stop timeout counter
                    ReceiveBuffSize = 0;
                } else {
                    eState = STATE_GET_DATA;
                    ReceiveBuff[1] = data;
                    RemaindingBytes = data;
                    ReceiveBuffSize = 2;
                }
                break;

            case STATE_GET_DATA :
                if (ReceiveBuffSize >= IBUS_CMD_MAX_SIZE) {
                    eState = STATE_IDLE; // workaround
                    TIMER0_TOT_Count = 0; // stop timeout counter
                    dbg_mark6 = 0;
                } else {
                    ReceiveBuff[ReceiveBuffSize] = data;
                    ReceiveBuffSize++;
                    RemaindingBytes--;
                    if (RemaindingBytes == 1) {
                        eState = STATE_XOR;
                    }
                }
                break;

            case STATE_XOR :
                eState = STATE_IDLE;
                TIMER0_TOT_Count = 0; // stop timeout counter

                for (XorByte = 0, i = 0; i < (ReceiveBuff[1] + 1); i++) {
                    XorByte ^= ReceiveBuff[i];
                }

                if (XorByte == data) {
                    if (IBUS_RxHead >= IBUS_RxTail) {
                        free_size = IBUS_RX_BUFFER_SIZE - (IBUS_RxHead - IBUS_RxTail);
                    } else {
                        free_size = (IBUS_RxTail - IBUS_RxHead);
                    }

#ifdef IBUS_DEBUG
                    if (nRxMaxSize < (IBUS_RX_BUFFER_SIZE - free_size + ReceiveBuff[1] + 1)) {
                        nRxMaxSize = IBUS_RX_BUFFER_SIZE - free_size + ReceiveBuff[1] + 1;
                    }
#endif

                    if (free_size >= (ReceiveBuff[1] + 1)) {
                        // Great Success :)
                        for (XorByte = 0, i = 0; i < (ReceiveBuff[1] + 1); i++) {
                            IBUS_RxHead = ( IBUS_RxHead + 1 ) & IBUS_RX_BUFFER_MASK;
                            IBUS_RxBuf[IBUS_RxHead] = ReceiveBuff[i];
                        }
                    } else {
                        // skip because rx buffer is full
                        nRxSkiped++;
                    }
                }
                break;

            default:
                eState = STATE_IDLE;
                TIMER0_TOT_Count = 0; // stop timeout counter
                break;
        }
    }
}

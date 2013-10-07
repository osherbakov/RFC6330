#include "rfc6330_func.h"
#include "rfc6330_tasks.h"

unsigned char Source[source_bytes];
unsigned char Encoded[encoded_bytes];
unsigned int ESIs[num_generated_symbols];

// initialize semaphore as taken
BSEMAPHORE_DECL(txSem, 1);
BSEMAPHORE_DECL(encSem, 1);

//------------------------------------------------------------------------------
// handler task for Tx
static WORKING_AREA(waThdTx, 200);

msg_t task_tx(void *arg) {

	systime_t time = chTimeNow();
	while (1) {
    time += MS2ST(1);
	chThdSleepUntil(time);

  }
}

//------------------------------------------------------------------------------
// handler task for Encoding
static WORKING_AREA(waThdEnc, 600);

msg_t task_enc(void *arg) {
  while (1) {
    // wait for event
    chBSemWait(&encSem);
    // print elapsed time
    uint32_t t = micros();
    Serial.println(t);
  }
}

//------------------------------------------------------------------------------
void task_rf_send_setup() {
  // start tx task
  chThdCreateStatic(waThdTx, sizeof(waThdTx), NORMALPRIO + 1, task_tx, NULL);
  chThdCreateStatic(waThdEnc, sizeof(waThdEnc), NORMALPRIO + 1, task_enc, NULL);
}
//------------------------------------------------------------------------------

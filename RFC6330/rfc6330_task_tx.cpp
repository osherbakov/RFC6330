#include "rfc6330_func.h"
#include "rfc6330_tasks.h"

unsigned char Source[source_bytes];
unsigned char Encoded[encoded_bytes];
unsigned int ESIs[num_generated_symbols];

// initialize semaphore as taken
BSEMAPHORE_DECL(txSem, 1);

//------------------------------------------------------------------------------
// handler task for interrupt
static WORKING_AREA(waThdTx, 200);

msg_t task_tx(void *arg) {
  while (1) {
    // wait for event
    chBSemWait(&txSem);
    // print elapsed time
    uint32_t t = micros();
    Serial.println(t);
  }
}
//------------------------------------------------------------------------------
void task_rf_send_setup() {
  // start tx task
  chThdCreateStatic(waThdTx, sizeof(waThdTx), NORMALPRIO + 1, task_tx, NULL);
}
//------------------------------------------------------------------------------

#include "rfc6330_tasks.h"

// initialize semaphore as taken
BSEMAPHORE_DECL(isrSem, 1);

// ISR entry time
// volatile uint32_t tIsr = 0;
//------------------------------------------------------------------------------
// Fake ISR, normally
// void isrFcn() {
// would be replaced by somthing like
// CH_IRQ_HANDLER(INT0_vect) {
//
void isrFcn() {

  // on ARM CH_IRQ_PROLOGUE is void
  CH_IRQ_PROLOGUE();
  /* IRQ handling code, preemptable if the architecture supports it.*/

  // Only ISR processing is to save time
//  tIsr = micros();

  chSysLockFromIsr();
  /* Invocation of some I-Class system APIs, never preemptable.*/
  chBSemSignalI(&isrSem);  // signal handler task
  chSysUnlockFromIsr();

  /* More IRQ handling code, again preemptable.*/

  // Perform rescheduling if required.
  CH_IRQ_EPILOGUE();
}
//------------------------------------------------------------------------------
// handler task for interrupt
WORKING_AREA(waSample, 200);

msg_t sample_task(void *arg) {
  while (1) {
    // wait for event
    chBSemWait(&isrSem);
    Serial.println(SYST_CVR);
  }
}
//------------------------------------------------------------------------------
void sample_task_setup() {
  // setup and check pins
  pinMode(INPUT_PIN, INPUT);
  // start handler task
  chThdCreateStatic(waSample, sizeof(waSample), HIGHPRIO, sample_task, NULL);
  // attach interrupt function
  attachInterrupt(INPUT_PIN, isrFcn, FALLING);
}
//------------------------------------------------------------------------------

#ifndef USB_H
#define USB_H

void tud_task_();
void send_report(const uint8_t *report);
void send_report_size(const uint8_t *report, int size);
void queue_dequeue(void);
bool queue_is_empty(void);
#endif
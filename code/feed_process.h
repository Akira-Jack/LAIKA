#ifndef	__FEED_PROCESS_H
#define	__FEED_PROCESS_H

#include	"define.h"

extern u8 cancel_send_feed_error;
extern u8 rtc_date_change;

extern char feed_run(u8 dat, struct feed_infos * info, u8 feed_run_typ);
extern void feed_scan(struct feed_infos * info);
extern void send_feed_info(void);
extern void send_offline_buf(void);
extern void send_feed_state(u8 feed_state);

#endif

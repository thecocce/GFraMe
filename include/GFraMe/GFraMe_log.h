/**
 * @include/GFraMe/GFraMe_log.h
 */
#ifndef __GFRAME_LOG_H_
#define __GFRAME_LOG_H_

#define GFraMe_log_max_filename 512

void GFraMe_log_init(int append);
void GFraMe_log_close();
void GFraMe_log_private(char * fmt, ...);

#define GFraMe_new_log(message, ...) \
  GFraMe_log_private("@%s:%d - "message,__FILE__,__LINE__, ##__VA_ARGS__)

#endif

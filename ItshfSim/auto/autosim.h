#ifndef AUTOSIM_H
#define AUTOSIM_H

#include "type.h"

/* api */
extern const FreqRsp& recommender(int algId, const FreqReq& req);
extern void notification(int algId, bool flag, int glbChId, int snr);

#endif // AUTOSIM_H

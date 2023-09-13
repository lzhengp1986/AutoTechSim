#include "autosim.h"
#include "auto/basealg.h"
#include "link/linkdlg.h"

// 实例化
BaseAlg g_random_search;

// 根据索引号调用策略
const FreqRsp& recommender(int algId, const FreqReq& req)
{
    if (algId == LinkDlg::RANDOM_SEARCH) {
        return g_random_search.bandit(req);
    } else if (algId == LinkDlg::BISECTING_SEARCH) {
    }
}

// 信息知会
void notification(int algId, int glbChId, int snr)
{
    if (algId == LinkDlg::RANDOM_SEARCH) {
         g_random_search.notify(glbChId, snr);
    } else if (algId == LinkDlg::BISECTING_SEARCH) {
    }
}

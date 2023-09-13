#include "autosim.h"
#include "link/linkdlg.h"

// 实例化
BaseAlg g_random_search;

// 根据索引号调用策略
const FreqRsp& alg_sche(int algId, const FreqReq& req)
{
    if (algId == LinkDlg::RANDOM_SEARCH) {
        return g_random_search.sche(req);
    } else if (algId == LinkDlg::BISECTING_SEARCH) {
    }
}

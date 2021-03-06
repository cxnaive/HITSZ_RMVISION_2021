/*===========================================================================*/
/*                               使用本代码的兵种 */
/*===========================================================================*/
/*   _______________   _______________   _______________   _______________   */
/*  |     _____     | |     _  _      | |     ____      | |     _____     |  */
/*  ||   |___ /    || ||   | || |    || ||   | ___|    || ||   |___  |   ||  */
/*  ||     |_ \    || ||   | || |_   || ||   |___ \    || ||      / /    ||  */
/*  ||    ___) |   || ||   |__   _|  || ||    ___) |   || ||     / /     ||  */
/*  ||   |____/    || ||      |_|    || ||   |____/    || ||    /_/      ||  */
/*  |_______________| |_______________| |_______________| |_______________|  */
/*                                                                           */
/*===========================================================================*/

#define LOG_LEVEL LOG_NONE
#include <armor_finder/armor_finder.h>
#include <show_images/show_images.h>

#include <opencv2/highgui.hpp>

std::map<int, string> id2name = {  //装甲板id到名称的map
    {0, "OO"},   {-1, "NO"},  {1, "B1"},    {2, "B2"},  {3, "B3"},
    {4, "B4"},   {5, "B7"},   {6, "B11"},   {7, "B10"}, {8, "B10_B"},
    {9, "R1"},   {10, "R2"},  {11, "R3"},   {12, "R4"}, {13, "R7"},
    {14, "R11"}, {15, "R10"}, {16, "R10_B"}};

std::map<string, int> name2id = {  //装甲板名称到id的map
    {"OO", 0},   {"NO", -1},  {"B1", 1},    {"B2", 2},  {"B3", 3},
    {"B4", 4},   {"B7", 5},   {"B11", 6},   {"B10", 7}, {"B10_B", 8},
    {"R1", 9},   {"R2", 10},  {"R3", 11},   {"R4", 12}, {"R7", 13},
    {"R11", 14}, {"R10", 15}, {"R10_B", 16}};

std::map<string, int> prior_blue = {
    {"B8", 0}, {"B1", 1}, {"B3", 2}, {"B4", 2}, {"B5", 2},
    {"B7", 3}, {"B2", 4}, {"R8", 5}, {"R1", 6}, {"R3", 7},
    {"R4", 7}, {"R5", 7}, {"R7", 8}, {"R2", 9}, {"NO", 10},
};

std::map<string, int> prior_red = {
    {"R8", 0}, {"R1", 1}, {"R3", 2}, {"R4", 2}, {"R5", 2},
    {"R7", 3}, {"R2", 4}, {"B8", 5}, {"B1", 6}, {"B3", 7},
    {"B4", 7}, {"B5", 7}, {"B7", 8}, {"B2", 9}, {"NO", 10},
};

ArmorFinder::ArmorFinder(const int &color, RmSerial &u, const int &anti_top)
    : serial(u),
      enemy_color(color),
      is_anti_top(anti_top),   //反陀螺
      state(SEARCHING_STATE),  //默认为searching模式
      anti_switch_cnt(0),      // 防止乱切目标计数器
      classifier(),            // CNN分类器对象实例，用于数字识别,
      contour_area(
          0),  //装甲区域亮点个数，用于数字识别未启用时判断是否跟丢（已弃用）
      tracking_cnt(0) {  // 记录追踪帧数，用于定时退出追踪
}

void ArmorFinder::run(cv::Mat &src) {    // 自瞄主函数
    frame_time = rmTime.milliseconds();  //　获取当前帧时间(c++11 chrono)
    bool send = false;
    switch (state) {
        case SEARCHING_STATE:
            if (stateSearchingTarget(src)) {
                if ((target_box.rect & cv::Rect2d(0, 0, 640, 480)) ==
                    target_box.rect) {  // 判断装甲板区域是否脱离图像区域
                    tracker = TrackerToUse::
                        create();  // 成功搜寻到装甲板，创建tracker对象
                    tracker->init(src, target_box.rect);
                    send = true;
                    state = TRACKING_STATE;  // 自瞄状态对象实例
                    tracking_cnt = 0;  // 记录追踪帧数，用于定时退出追踪
                    LOG(INFO) << "into track!";
                }
            }
            break;
        case TRACKING_STATE:
            if (!stateTrackingTarget(src) ||
                ++tracking_cnt > 100) {  // 最多追踪100帧图像
                state = SEARCHING_STATE;
                LOG(INFO) << "into search!";
            } else
                send = true;
            break;
        case STANDBY_STATE:
        default:
            stateStandBy();  // currently meaningless
    }

    if (is_anti_top) {  // 判断当前是否为反陀螺模式
        antiTop();
    } else if (target_box.rect != cv::Rect2d()) {
        anti_top_cnt = 0;
        time_seq.clear();
        angle_seq.clear();
        if (send)
            sendBoxPosition(0);
        else
            sendBoxPosition(0, 0);
    }

    if (target_box.rect != cv::Rect2d()) {
        last_box = target_box;
    }

    if (config.show_armor_box &&
        target_box.rect != cv::Rect2d()) {  // 根据条件显示当前目标装甲板
        showArmorBox("box", src, target_box);
        cv::waitKey(1);
    }
}

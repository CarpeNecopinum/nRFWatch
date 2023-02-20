#include "AppSwitcher.hh"
#include "Task.hh"
#include "Inputs.hh"
#include "globals.hh"

IApp *AppSwitcher::mCurrentApp = nullptr;
// std::vector<std::unique_ptr<IApp>> AppSwitcher::mApps;
std::array<IApp *, 8> AppSwitcher::mApps;
uint32_t AppSwitcher::mNumApps = 0;
uint32_t AppSwitcher::mPid = -1;

// template <typename T>
// size_t index_of(std::vector<std::unique_ptr<T>> &vec, T *ptr)
// {
//     for (size_t i = 0; i < vec.size(); ++i)
//         if (vec[i].get() == ptr)
//             return i;
//     return -1;
// }

template <typename T, size_t N>
size_t index_of(std::array<T, N> const &arr, size_t size, T const &val)
{
    for (size_t i = 0; i < size; ++i)
        if (arr[i] == val)
            return i;
    return -1;
}

void AppSwitcher::init()
{
    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    mPid = register_task(([]() -> Task { //
        // wait for at least one app to be registered
        while (mApps.empty())
            co_yield Task::resume_asap();

        // start with the first app
        mCurrentApp = mApps[0];
        mCurrentApp->enable();

        static uint32_t num_resumes = 1;
        while (true)
        {
            co_yield Task::resume_on_input(InputFlag::SWIPE_UP | InputFlag::SWIPE_DOWN);
            const auto gesture = inputs.current() & (InputFlag::SWIPE_UP | InputFlag::SWIPE_DOWN);

            // display.fillRect(0, TOPBAR_SIZE, 176, 176 - TOPBAR_SIZE, COLOR_RED);
            // display.setCursor(0, 12);
            // display.setTextColor(COLOR_WHITE);
            // display.setTextSize(2);
            // display.print(gesture);
            // display.setCursor(0, 32);
            // display.print(num_resumes++);
            // display.setTextSize(1);
            // display.refresh();
            // delay(100);

            const auto current_index = index_of(mApps, mNumApps, mCurrentApp);

            if (gesture & InputFlag::SWIPE_UP)
            {
                mCurrentApp->disable();
                mCurrentApp = mApps[(current_index + 1) % mNumApps];
                mCurrentApp->enable();
            }
            else if (gesture & InputFlag::SWIPE_DOWN)
            {
                mCurrentApp->disable();
                mCurrentApp = mApps[(current_index + mNumApps - 1) % mNumApps];
                mCurrentApp->enable();
            }
        }
    })());
    enable();
}
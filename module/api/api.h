#ifndef API_H
#define API_H
#include <QDebug>
#include <thread>
#include "module/udp/udp_receiver.h"
#include "module/file_writer/file_writer.h"
#include <functional>
using CallbackFunction = std::function<void()>;
#define MAX_SAMP_FREQ 2000
#define MIN_SAMP_FREQ 1

class acqlib_api
{
    public:
        acqlib_api();
        ~acqlib_api();
        int acqlib_init();
        float acqlib_get_data(int row, int col);
        int acqlib_write_file();
        int acqlib_deinit();
        static int acqlib_active_receiver_thread(acqlib_api * api,CallbackFunction callback_func);
        int acqlib_start_receive();
        int acqlib_stop_receive();
        int acqlib_samp_freq(int freq);

    public:
        Udp_Receiver receiver;
        file_writer writer;

};

#endif // API_H
